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
// Cook-Torrance lighting and shading - HLSL Pixel Shader
//
// Desc :
//   Cook-Torrance lighting and shading
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
    float3 vHalf   : TEXCOORD3; //eye vector
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

//#########################################################################
//##   CODE    ############################################################
//#########################################################################
// function    : main
// description : pixel shader function
// return      : PS_OUTPUT
// param: 
//    PS_INPUT input : pixel shader input - output from VS
//    float3 roughness : c2 - roughness
//    float3 refindex  : c3 - R(0) for Fresnel term
//
PS_OUTPUT main(const PS_INPUT input, 
               uniform float roughness : register(c2), 
               uniform float refindex : register(c3)) {

    PS_OUTPUT output;

    //------------------------------
    // Load and normalize input vectors
    //------------------------------
    //
    float3 vNormal = tex2D(smplNormal, input.tcCoord).xyz;//sample normal
    vNormal = normalize(2.0f * vNormal - 1.0f);           //bias to range [-1...1] and normalize
    float3 vLight = normalize(input.vLight);              //normalize light
    float3 vEye = normalize(input.vEye);                  //normalize eye vector
    float3 vHalf = normalize(input.vHalf);                //normalize half vector

    //------------------------------
    // Beckmann's distribution function D
    //------------------------------
    //
    float normalDotHalf = dot(vNormal, vHalf);
    float normalDotHalf2 = normalDotHalf * normalDotHalf;
    float roughness2 = roughness * roughness;
    float exponent = - (1.0f - normalDotHalf2) / (normalDotHalf2 * roughness2);
    float e = 2.718281828459045235360287471352662497757247093699959574966967628f;
    float D = pow(e, exponent) / (roughness2 * normalDotHalf2 * normalDotHalf2);

    //------------------------------
    // Compute Fresnel term F
    //------------------------------
    //
    float normalDotEye = dot(vNormal, vEye);
    float F = lerp(pow(1 - normalDotEye, 5), 1, refindex);

    //------------------------------
    // Compute self shadowing term G
    //------------------------------
    //
    float normalDotLight = dot(vNormal, vLight);
    float X = 2.0f * normalDotHalf / dot(vEye, vHalf);
    float G = min(1, min(X * normalDotLight, X * normalDotEye));

    //------------------------------
    // Compute final Cook-Torrance specular term
    // Load texture, compute diffuse part and combine it all to output
    //------------------------------
    //
    float pi = 3.141592653589793238462643383279502884197169399375105820974944592f;
    float CookTorrance = (D*F*G) / (normalDotEye * pi);

    float4 color = tex2D(smplTexture, input.tcCoord);
    output.vColor = color * (max(0.0f, CookTorrance) * color.a + max(0.0f, normalDotLight));
    return output;
}
