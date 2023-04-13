//*****************************************************************************
//* This file is part of the Dimension3 renderer library                      *
//* Copyright (C) 2003  Michal Valient (valient@host.sk)                      *
//*                                                                           *
//* Dimension3 renderer can be found at http://www.dimension3.host.sk         *
//*                                                                           *
//* This program is free software; you can redistribute it and/or modify      *
//* it under the terms of the GNU General Public License as published by      *
//* the Free Software Foundation; either version 2 of the License, or         *
//* (at your option) any later version.                                       *
//*                                                                           *
//* This program is distributed in the hope that it will be useful,           *
//* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
//* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
//* GNU General Public License for more details.                              *
//*                                                                           *
//* You should have received a copy of the GNU General Public License         *
//* along with this program; if not, write to the Free Software               *
//* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA * 
//*****************************************************************************
//hdr_utility_remove_tag


//------------------------------
// Per pixel bump map reflection - Pixel shader
//
// Desc :
//   Renders cube map reflection
//
// Part of Dimension3 Renderer
// (c)2003 Michal Valient
// akka em / STV3 akka  R.A.Y / EastW00D
//------------------------------

//------------------------------
// Used input structure
//------------------------------
//
#pragma pack_matrix(col_major)
struct PS_INPUT {
    float2 tcCoord  : TEXCOORD0; //texture coordinates
    float3 vEye     : TEXCOORD1; //eye vector
    float3x3 mToWorld: TEXCOORD2; //from Tangent to World space 1
//    float3 mToWorld1: TEXCOORD2; //from Tangent to World space 1
//    float3 mToWorld2: TEXCOORD3; //from Tangent to World space 2
//    float3 mToWorld3: TEXCOORD4; //from Tangent to World space 3
    float3 vAdjust  : TEXCOORD5; //perspective spotlight coordinates
};

//------------------------------
// Used output structure
//------------------------------
//
struct PS_OUTPUT {
    float4 vColor  : COLOR0;    //render target 0
};

//------------------------------
// Used input texture samplers
//------------------------------
//
sampler smplTexture : register(ps,s0);  //decal texture
sampler smplNormal  : register(ps,s1);  //normal texture
sampler smplCube    : register(ps,s2);  //cube map texture

//#########################################################################
//##   CODE    ############################################################
//#########################################################################
// function    : main
// description : pixel shader function
// return      : PS_OUTPUT
// param: 
//    PS_INPUT input : pixel shader input - output from VS
//    float refindex: c0 - R(0) for Fresnel term
//
PS_OUTPUT main(const PS_INPUT input, 
               uniform float refindex : register(c0)) {
    PS_OUTPUT output;

    //load and normalize
    float3 vNormal = tex2D(smplNormal, input.tcCoord).xyz;  //load normal
    vNormal = 2.0 * vNormal - 1.0;                          //bias to [-1...1]
    vNormal = mul(vNormal, input.mToWorld);                 //to world space
    vNormal = normalize(vNormal);
    float3 vEye = normalize(input.vEye);
    
    //compute adjusted eye reflection vector
    float3 vEyeReflected = 2*dot(vEye, vNormal)*vNormal - vEye + input.vAdjust;
    float4 cube = texCUBE(smplCube, vEyeReflected);

    float4 color = tex2D(smplTexture, input.tcCoord);

    float Fresnel = lerp(pow(1 - dot(vNormal, vEye), 5), 1, refindex);
    Fresnel = Fresnel * color.a;                    //reflection only on shiny parts
    
    float4 diffuse = color * dot(vNormal, vEye);    //diffuse simulation
    output.vColor = lerp(diffuse, cube, Fresnel);
    return output;
}
