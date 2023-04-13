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
// Per pixel bump map reflection - HLSL Vertex shader
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
    float4 vClipPos  : POSITION;  //Clipping space position
    float2 tcCoord   : TEXCOORD0; //texture coordinates
    float3 vEye      : TEXCOORD1; //eye vector
    float3x3 mToWorld: TEXCOORD2; //from Tangent to World space
    float3 vAdjust   : TEXCOORD5; //perspective spotlight coordinates
};

//------------------------------
// Constant registers 
//------------------------------
//
float4x4 mToCube  : register(c0);   //cubemap space transposed
float4x4 mToClip  : register(c4);   //world * view * proj
float4 pEye       : register(c8);   //Eye position (cube Space)
float  Adjustment : register(c9);   //Adjustment factor for cube map

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
    float4 pVertexCube = mul(input.vPosition, mToCube); //Transform vertex into cube space
    output.vEye = normalize(pEye - (float3)pVertexCube);

    //------------------------------
    //Create tangent space basis and transformation matrix from tangent to world space
    //------------------------------
    //
    float3x3 mToTangent;
    mToTangent[0] = input.vTangent;
    mToTangent[2] = input.vNormal;
    mToTangent[1] = cross(mToTangent[0], mToTangent[2]); //The cross product to compute binormal NxT
    output.mToWorld = mul(mToTangent, mToCube);
    output.vAdjust = pVertexCube * Adjustment;  //vector adjustment for pixel shader.
    return output;
}
