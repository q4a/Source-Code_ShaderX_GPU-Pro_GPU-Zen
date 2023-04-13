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
// Per pixel diffuse and specular - HLSL Pixel shader
//
// Desc :
//   Renders diffuse and specular part for one light per pixel using Phong's equation
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
    float4 tcpSpot : TEXCOORD3; //perspective spotlight coordinates
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
sampler smplSpot    : register(ps,s3);  //spotlight texture

//#########################################################################
//##   CODE    ############################################################
//#########################################################################
// function    : main
// description : pixel shader function
// return      : PS_OUTPUT
// param: 
//    PS_INPUT input : pixel shader input - output from VS
//    float3 colDiff : c0 - diffuse texture multiplier (premultiplied with light color)
//    float3 colSpec : c1 - specular texture multiplier (premultiplied with light const)
//    float shininess: c2 - specular shininess
//
PS_OUTPUT main(const PS_INPUT input, 
               uniform float3 colDiff : c0, 
               uniform float3 colSpec : c1, 
               uniform float shininess : c2) {
    PS_OUTPUT output;

    //------------------------------
    // Load and normalize input vectors
    //------------------------------
    //
    float3 vNormal = tex2D(smplNormal, input.tcCoord).xyz;//sample normal
    vNormal = normalize(2.0 * vNormal - 1.0);             //bias to range [-1...1] and normalize
    float3 vLight = normalize(input.vLight);              //normalize light
    float3 vEye = normalize(input.vEye);                  //normalize eye vector

    //------------------------------
    // Compute diffuse and specular intensities
    //------------------------------
    //
    float normalDotLight = dot(vNormal, vLight);
    float3 vLightReflect = 2.0*normalDotLight*vNormal - vLight;
    float eyeDotReflect = saturate(dot(vEye, vLightReflect));
    float specularIntensity = pow(eyeDotReflect, shininess);
    float4 tmpOutput = {0.0f, 0.0f, 0.0f, 1.0f};
    if (normalDotLight>0.0f) {
        float4 tDecal = tex2D(smplTexture, input.tcCoord);
        float4 tSpot = tex2Dproj(smplSpot, input.tcpSpot);
        
        float3 diffuse = normalDotLight * colDiff * tDecal;
        float3 specular = specularIntensity * colSpec * tDecal.a;
        
        tmpOutput.xyz = tSpot * (diffuse + specular);
    }
    output.vColor = tmpOutput;
    return output;
}
