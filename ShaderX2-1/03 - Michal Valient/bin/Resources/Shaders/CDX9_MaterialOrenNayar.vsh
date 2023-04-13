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
// Oren-Nayar lighting - Vertex shader
//
// Desc :
//   Oren-Nayar lighting and shading
//
// Part of Dimension3 Renderer
// (c)2003 Michal Valient
// akka em / STV3 akka  R.A.Y / EastW00D
//------------------------------

vs_2_0

//------------------------------
// Constant registers 
//------------------------------
//
// c0-c3     - world space transposed
// c4-c7     - world * view * proj
// c8        - Light position (In World Space)
// c9        - Eye position (In World Space)
// c10-c13   - Spotlight projection matrix

//------------------------------
// Input registers
//------------------------------
//
dcl_position v0
dcl_normal v1
dcl_texcoord v2
dcl_tangent v3

//------------------------------
// Fixed temporary registers
//------------------------------
// r9, r10, r11 - tangent space basis
// r8 - vertex world position

//------------------------------
// Output
//------------------------------
// oT0 - texture coordinates
// oT1 - Light vector (in tangent space)
// oT2 - eye vector (in tangent space)
// oT3 - projective spotlight texture coordinates

//#########################################################################
//##   CODE    ############################################################
//#########################################################################
//

//------------------------------
//Following code will output position and texture coordinates
//------------------------------
//
m4x4 oPos, v0, c4                   //vertex clip position
mov oT0.xy, v2.xy                   //Texture coordinates for color texture
m4x4 r8, v0, c0                     //Transform vertex into world position

//------------------------------
//Following code generates Tangent space base vectors
//------------------------------
//
m3x3 r11.xyz, v1, c0                //transform normal N to world space
mov r11.w, v1.w
m3x3 r9.xyz, v3, c0                 //transform tangent T to world space
mov r9.w, v3.w
crs r10.xyz, r9, r11                //The cross product to compute binormal NxT

//------------------------------
//Computes light and eye vectors
//------------------------------
//
add r0, c8, -r8                     //Build the light vector from light source to vertex
nrm r6, r0                          //normalize vector
m3x3 oT1.xyz, r6, r9                //transform vector with N, T, NxT into tangent space

add r0, c9, -r8                     //Build the eye vector from vertex to camera source
nrm r7, r0                          //normalize vector
m3x3 oT2.xyz, r7, r9                //transform vector with N, T, NxT into tangent space

m4x4 oT3.xyzw, v0, c10              //compute projector texture coordinates from vertex position
