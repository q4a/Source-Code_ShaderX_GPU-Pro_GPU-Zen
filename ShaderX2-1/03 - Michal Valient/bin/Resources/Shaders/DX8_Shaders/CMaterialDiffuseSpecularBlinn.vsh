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


;------------------------------
; Per pixel diffuse and specular - Vertex shader
;
; Desc :
;   Renders diffuse and specular part for one light per pixel using Blinn's equation
;
; Part of Dimension3 Renderer
; (c)2002 Michal Valient
; akka em / STV3 akka  R.A.Y / EastW00D
;------------------------------
;
; Constant registers 
; c0-c3     - world space transposed
; c4-c7     - world * view * proj
; c8        - Light position (In World Space)
; c9        - Eye position (In World Space)
;
; Input registers
; v0 - Position
; v1 - Normal
; v2 - Texture 
; v3 - Tangent T
;
; Fixed temporary registers
; r9, r10, r11 - tangent space basis
; r8 - vertex world position
; r7 - light vector
; r6 - half vector
;
; Output
; oT0 - tex coord
; oT1 - Light vector (in tangent space)
; oT2 - half vector (in tangent space)
;------------------------------

vs.1.1

dcl_position v0
dcl_normal v1
dcl_texcoord v2
dcl_tangent v3

;------------------------------
;Following code output position and texture coordinates
;------------------------------
;
m4x4 oPos, v0, c4                   ;vertex clip position
mov oT0.xy, v2.xy                   ;Texture coordinates for color texture

;------------------------------
;Following code generates Tangent space base vectors
;------------------------------
;
m3x3 r11, v1, c0                    ;use normal as third parameter of texture space
mov r11.w, v1.w
m3x3 r9, v3, c0                     ;generate tangent SxN
mov r9.w, v3.w

mul r0,r9.zxyw,r11.yzxw             ;The cross product to compute binormal NxT
mad r10,r9.yzxw,r11.zxyw,-r0        ;The cross product to compute binormal NxT

m4x4 r8, v0, c0                     ;Transform vertex into world position
add r0, c8, -r8                     ;Build the light vector from light source to vertex

;------------------------------
;Following code computes light vector in texture space
;for pixel shader
;------------------------------
;
                                    ;normalize the light vector (d stands for distance)
dp3 r1.w, r0, r0                    ;r1.w = r0 * r0 = (x*x) + (y*y) + (z*z)
rsq r1.w, r1.w	                    ;r1.w = 1/d = 1/||V|| = 1/sqrt(r1.w)
mul r7, r0, r1.w                    ;r7 = r0 * (1/d) = r0/d

dp3 oT1.x, r9, r7                   ;transform the light vector with N, T, NxT
dp3 oT1.y, r10, r7                  ;into texture coordinate space
dp3 oT1.z, r11, r7                  ;for per pixel diffuse lighting

;------------------------------
;Following code computes specular lighting parts
;for pixel shader (half vector)
;------------------------------
;
add r0, c9, -r8                     ;build the eye vector from vertex to camera source

dp3 r1.w, r0, r0                    ;normalize the eye vector
rsq r1.w, r1.w
mul r6, r0, r1.w

add r0, r6, r7                      ;build half vector
dp3 r1.w, r0, r0                    ;normalize the half vector
rsq r1.w, r1.w
mul r6, r0, r1.w

dp3 oT2.x, r9, r6                   ;transform the eye vector with S, T, SxT
dp3 oT2.y, r10, r6
dp3 oT2.z, r11, r6
