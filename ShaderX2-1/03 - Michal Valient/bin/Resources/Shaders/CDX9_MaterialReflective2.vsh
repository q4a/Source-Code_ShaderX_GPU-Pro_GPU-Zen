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
// Per pixel bump map reflection - Vertex shader
//
// Desc :
//   Renders cube map reflection
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
// c0-c3     - cube map space transposed (might be identical to world space)
// c4-c7     - world * view * proj
// c8        - Eye position (In cube Space)
// c9        - Adjustment factor for cube map
//

//------------------------------
// Input registers
//------------------------------
//
dcl_position v0
dcl_normal v1
dcl_texcoord v2
dcl_tangent v3

//------------------------------
// Output
//------------------------------
//
// oT0 - tex coord
// oT1 - eye vector in world space
// oT2 - 1st row of tangent-to-world matrix
// oT3 - 2nd row of tangent-to-world matrix
// oT4 - 3rd row of tangent-to-world matrix
// oT5 - vertex modifier
//------------------------------


//#########################################################################
//##   CODE    ############################################################
//#########################################################################
//

//------------------------------
//Output clip space position, texture coordinates.
//Compute eye vector in cube space
//------------------------------
//
m4x4    oPos, v0, c4                //vertex clip position
mov     oT0.xy, v2.xy               //Texture coordinates for color texture

m4x4    r8, v0, c0                  //Transform vertex into cube map space
add     r0, c8, -r8                 //Eye vector - from vertex to camera source
nrm     r1.xyz, r0                  //Normalize it
mov     oT1.xyz, r1                 //Output it

//------------------------------
//Create tangent space basis and transformation matrix from tangent to world space
//------------------------------
//
mov     r9, v3                      //Copy tangent, then
crs     r10.xyz, r9, v1             //do cross product to compute binormal B = NxT and 
mov     r11, v1                     //then copy normal - T,B,N matrix in r9,r10,r11

m3x3    oT2.xyz, c0, r9             //Create transformation matrix (transposed)
m3x3    oT3.xyz, c1, r9
m3x3    oT4.xyz, c2, r9

mul oT5, r8, c9.xxxx                //reflection vector adjustment in pixel shader.
