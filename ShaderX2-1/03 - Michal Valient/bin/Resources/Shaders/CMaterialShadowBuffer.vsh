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
// Shader for depth shadow map
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
// c0-c3     - world * view * proj - this time for light
// c4-c7     - world space transposed
// c8        - Light position (In World Space)
// c9        - Z ranges
def c10, 0.5f, 0.0f, 0.0f, 0.0f

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
// oT0 - Z value

//#########################################################################
//##   CODE    ############################################################
//#########################################################################
//

//------------------------------
//Following code outputs position and texture coordinates
//------------------------------
//
m4x4 oPos, v0, c0                   //vertex clip position
m4x4 r8, v0, c4                     //Transform vertex into world position

//------------------------------
//Compute distance from light and normalize it ot [0...1]
//------------------------------
//
sub r0, c8, r8                      //Build the light vector from light source to vertex
dp3 r1.x, r0, r0                    //length of vector^2
pow r2.x, r1.x, c10.x               //sqrt(length^2)
sub r3.x, r2.x, c9.x                //Dst - ZNear
mul r4.x, r3.x, c9.z                //(Dst - ZNear)/(ZFar - ZNear) - normalized position
add r4.x, r4.x, c9.w                //add depth bias
mov oT0, r4.x                       //Output it
