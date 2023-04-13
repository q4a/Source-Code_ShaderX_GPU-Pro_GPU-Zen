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
//   Renders diffuse and specular spotlight for 4 lights per pixel using Phong's equation
//
// Part of Dimension3 Renderer
// (c)2003 Michal Valient
// akka em / STV3 akka  R.A.Y / EastW00D
//------------------------------

vs_3_0

//------------------------------
// Constants
//------------------------------
//
// i0        - light iteration loop data
// c0-c3     - world space transposed
// c4-c7     - world * view * proj
// c8        - Eye position (In World Space)
// c9 >>     - start of lighting data : 
//              VECTOR4     LightPos;
//              MATRIX4x4   Projector;
//              COLOR4      LightColor (x,y,z) and Light multiplier (w); Colors are expected to be < 1.0f
def c255, 6.0f, 0.0f, 1.0f, 9.0f    //Loop helpers are here -   c255.x is size of light structure
                                    //                          c255.y is used for reseting counter
                                    //                          c255.z is used for incrementing counter
                                    //                          c255.w is used as starting constant index
//------------------------------
// Input
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
dcl_position0 o0                    //clip space coordinates
dcl_texcoord0 o1.xy                 //texture coordinates
dcl_texcoord1 o2.xyz                //eye vector
dcl_texcoord2 o3                    //Light vector 1
dcl_texcoord3 o4                    //Projector texture coordinates 1
dcl_texcoord4 o5                    //Light vector 2
dcl_texcoord5 o6                    //Projector texture coordinates 2
dcl_texcoord6 o7                    //Light vector 3
dcl_texcoord7 o8                    //Projector texture coordinates 3
dcl_texcoord8 o9                    //Light vector 4
dcl_texcoord9 o10                   //Projector texture coordinates 4

//------------------------------
// Fixed temporary registers (used anywhere in shader)
//------------------------------
//
// r9, r10, r11 - tangent space basis
// r8 - vertex in world position
// r0 - constant addressing register

//#########################################################################
//##   CODE    ############################################################
//#########################################################################
//
//------------------------------
//Following code output position and texture coordinates
//------------------------------
//
m4x4 o0, v0, c4                     //vertex clip position
mov o1.xy, v2.xy                    //Texture coordinates for color texture
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
//Following code computes normalized eye vector and transforms it to tangent space
//------------------------------
//
add r0, c8, -r8                     //build the eye vector from vertex to camera source
nrm r6, r0                          //normalize vector
m3x3 o2.xyz, r6, r9                 //transform the eye vector with N, T, NxT

//------------------------------
//In following loop we are computing normalized light vectors and transforming them to tangent space
//------------------------------
//
mov r0.y, c255.y                    //reset constant addressing counter to zero
loop aL, i0                         //Loop for lights
    mad r0.x, r0.y, c255.x, c255.w  //Compute light data index : I = Count*DataSize + DataStart
    mova a0, r0.x                   //fill address register

    add r1, c[a0.x], -r8            //Build the light vector from light source to vertex
    nrm r6, r1                      //normalize vector
    m3x3 o3[aL].xyz, r6, r9         //transform the light vector with N, T, NxT

    m4x4 o3[aL+1], v0, c[a0.x+1]    //transform vertex with light projection matrix 
                                    //(get projector texture coordinates)

    add r0.y, r0.y, c255.z          //Increment constant addressing counter. 
endloop
