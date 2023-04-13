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
// Per pixel diffuse and specular - HLSL vertex shader
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
struct VS_INPUT {
    float4 vPosition : POSITION; //position in object space
    float3 vNormal   : NORMAL;   //normal
    float2 tcCoord   : TEXCOORD; //texture coordinates
    float3 vTangent  : TANGENT;  //tangent
};

//------------------------------
// Used output structure
//------------------------------
//
struct VS_OUTPUT {
    float4 vClipPos: POSITION;  //Clipping space position
    float2 tcCoord : TEXCOORD0; //texture coordinates
    float3 vLight  : TEXCOORD1; //light vector
    float3 vEye    : TEXCOORD2; //eye vector
    float4 tcpSpot : TEXCOORD3; //perspective spotlight coordinates
};

//------------------------------
// Constant registers 
//------------------------------
//
col_major float4x4 mToWorld : register(c0);   //world space transposed
col_major float4x4 mToClip  : register(c4);   //world * view * proj
float4 pLight               : register(c8);   //Light position (World Space)
float4 pEye                 : register(c9);   //Eye position (World Space)
col_major float4x4 mSpot    : register(c10);  //Spotlight projection matrix

//#########################################################################
//##   CODE    ############################################################
//#########################################################################
// function    : main
// description : vertex shader function
// return      : VS_OUTPUT
// param: 
//    VS_INPUT input : vertex shader input
//
VS_OUTPUT main(const VS_INPUT input) {
    VS_OUTPUT output;

    //------------------------------
    //Following code outputs position and texture coordinates
    //------------------------------
    //
    output.vClipPos = mul(input.vPosition, mToClip); //vertex clip position
    output.tcCoord = input.tcCoord; //Texture coordinates for color texture
    float4 pVertexWorld = mul(input.vPosition, mToWorld); //Transform vertex into world position

    //------------------------------
    //Following code generates Tangent space base vectors
    //------------------------------
    //
    float3x3 mToTangent;
    mToTangent[0] = mul(input.vTangent, (float3x3)mToWorld); //transform tangent T to world space
    mToTangent[2] = mul(input.vNormal, (float3x3)mToWorld);  //transform normal N to world space
    mToTangent[1] = cross(mToTangent[0], mToTangent[2]);     //The cross product to compute binormal NxT

    //------------------------------
    //Compute light and eye vectors and projector's texture coordinates
    //------------------------------
    //
    float3 vToLight = normalize(pLight - pVertexWorld);
    output.vLight = mul(vToLight, mToTangent);
    
    float3 vToEye = normalize(pEye - pVertexWorld);
    output.vEye = mul(vToEye, mToTangent);

    output.tcpSpot = mul(input.vPosition, mSpot);
    return output;
}
