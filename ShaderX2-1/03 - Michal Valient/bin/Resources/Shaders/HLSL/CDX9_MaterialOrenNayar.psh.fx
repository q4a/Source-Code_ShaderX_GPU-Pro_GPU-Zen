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
// Oren-Nayar lighting - HLSH - pixel part
//
// Desc :
//   Oren-Nayar lighting and shading
//
// Part of Dimension3 Renderer
// (c)2003 Michal Valient
// akka em / STV3 akka  R.A.Y / EastW00D
//------------------------------

//------------------------------
// Used input structure
//------------------------------
//
struct PS_INPUT {
    float2 tcCoord : TEXCOORD0; //input texture coordinates
    float3 vLight  : TEXCOORD1; //light vectpor
    float3 vEye    : TEXCOORD2; //eye vector
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
sampler smplLookUp  : register(ps,s2);  //lookup texture for sin.tan

//#########################################################################
//##   CODE    ############################################################
//#########################################################################
//
// function    : main
// description : pixel shader function
// return      : PS_OUTPUT
// param: 
//    PS_INPUT input : pixel shader input - output from VS
//    float roughness : c0 - roughness of a surface
//
PS_OUTPUT main(const PS_INPUT input, uniform float roughness : c0) {
    PS_OUTPUT output;

    //------------------------------
    // Load and normalize input vectors
    //------------------------------
    //
    float3 vNormal = tex2D(smplNormal, input.tcCoord).xyz;//sample normal
    vNormal = normalize(2.0*vNormal - 1.0);               //bias to range [-1...1] and normalize
    float3 vLight = normalize(input.vLight);              //normalize light
    float3 vEye = normalize(input.vEye);                  //normalize eye vector

    //------------------------------
    // A = 1 - 0.5 * R^2 / (R^2 + 0.33)
    // B = 0.45 * R^2 / (R^2 + 0.09)    
    //------------------------------
    //
    float roughness2 = roughness*roughness;
    float A = 1.0f - 0.5f * roughness2 / (roughness2 + 0.33f);
    float B = 0.45f * roughness2 / (roughness2 + 0.09f);
    
    //------------------------------
    // CX = Max(0, cos (l',v'))
    //------------------------------
    //
    float  normalDotLight  = dot(vNormal, vLight);
    float3 vLightProjected = normalize(vLight - vNormal*normalDotLight);
    
    float  normalDotEye  = dot(vNormal, vEye);
    float3 vEyeProjected = normalize(vEye - vNormal*normalDotEye);
    
    float CX = saturate(dot(vLightProjected, vEyeProjected));
    
    //------------------------------
    // DX = texture lookup for sin(a)*tan(b); a=max(O-r,O-i); b=min(O-r,O-i)
    //------------------------------
    //
    float2 tcLookup = {normalDotLight, normalDotEye};
    float DX = tex2D(smplLookUp, tcLookup);
    
    //------------------------------
    // completize it - (n.l)*texture*(A + B*CX*DX)
    //------------------------------
    //
    output.vColor = saturate(normalDotLight)*tex2D(smplTexture, input.tcCoord)*(A+B*CX*DX);;
    return output;
}
