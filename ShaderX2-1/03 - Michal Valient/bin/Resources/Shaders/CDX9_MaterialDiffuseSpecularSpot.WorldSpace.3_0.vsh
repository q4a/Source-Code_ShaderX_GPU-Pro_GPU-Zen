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
// Per pixel diffuse and specular - Vertex shader
//
// Desc :
//   Renders diffuse and specular part for one light per pixel using Phong's equation
//   This one is prepared if Constant relative addressing will be available in PS!!!
//
// Part of Dimension3 Renderer
// (c)2003 Michal Valient
// akka em / STV3 akka  R.A.Y / EastW00D
//------------------------------
//
// Constant registers 
// c0-c3     - world space transposed
// c4-c7     - world * view * proj
// c8        - Eye position (In World Space)
//
// Input registers
// v0 - Position
// v1 - Normal
// v2 - Texture 
// v3 - Tangent T
//
// Fixed temporary registers
// r9, r10, r11 - tangent space basis
// r8 - vertex world position
// r7 - light vector
// r6 - eye vector
//
// Output
// oT0 - tex coord
// oT1 - Light vector (in tangent space)
// oT2 - eye vector (in tangent space)
// oT3 - projective spotlight texture coordinates
//------------------------------

vs_3_0

dcl_position v0
dcl_normal v1
dcl_texcoord v2
dcl_tangent v3

dcl_position0 o0                    //clip space coordinates
dcl_texcoord0 o1.xy                 //texture coordinates
dcl_position1 o2                    //world space coordinates
dcl_position2 o3.xyz                //eye vector
dcl_position3 o4                    //Tangent_To_World matrix 1
dcl_position4 o5                    //Tangent_To_World matrix 2
dcl_position5 o6                    //Tangent_To_World matrix 1

//------------------------------
//Following code output position and texture coordinates
//------------------------------
//
m4x4 o0, v0, c4                     //vertex clip position
mov o1.xy, v2.xy                    //Texture coordinates for color texture

//------------------------------
//Following code generates Tangent space base vectors
//------------------------------
//
mov r11, v1
mov r9, v3

mul r0,r9.zxyw,r11.yzxw             //The cross product to compute binormal NxT
mad r10,r9.yzxw,r11.zxyw,-r0        //The cross product to compute binormal NxT

m4x4 r8, v0, c0                     //Transform vertex into world position
mov o2, r8                          //output to pixel shader

//------------------------------
//Following code computes transformation matrix
//from tangent space to world space for pixel shader 
//(Used to transform pixel normal into world space)
//------------------------------
//
dp3		o4.x, c0, r9
dp3		o4.y, c0, r10
dp3		o4.z, c0, r11

dp3		o5.x, c1, r9
dp3		o5.y, c1, r10
dp3		o5.z, c1, r11

dp3		o6.x, c2, r9
dp3		o6.y, c2, r10
dp3		o6.z, c2, r11

//------------------------------
//Following code computes specular lighting parts
//for pixel shader (Eye vector in world space)
//------------------------------
//
add r0, c8, -r8                     //build the eye vector from vertex to camera source
nrm r1, r0                          //normalize the eye vector
mov o3.xyz, r1

