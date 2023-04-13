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
; Per vertex phong - Pixel shader
;
; Desc :
;   Renders diffuse and specular part for one light per vertex
;
; Part of Dimension3 Renderer
; (c)2002 Michal Valient
; akka em / STV3 akka  R.A.Y / EastW00D
;------------------------------
;
; Constant registers 
; c0-c3     - world * view * proj matrix transposed
; c4-c7     - world matrix inverted
; c8        - world matrix transposed
; c12       - Light position
; c13       - Eye position
; c14       - Material diffuse color
; c15       - Light diffuse color
; c16       - Light Attenuation parameters
; c17       - Specular power (shininess)
;
; Input registers
; v0 - Position
; v3 - Normal
; v7 - texture coordinates
;
; Output
; oT0 - tex coord
; oD0 - diffuse color
; oD1 - specular color
;------------------------------

vs.1.1                          ;shader version specification

dcl_position v0
dcl_normal v1
dcl_texcoord v2
dcl_tangent v3


m4x4 oPos, v0, c0               ;transform vertex to clip space. 
                                ;c0 holds transposed clip matrix
mov oT0, v2                     ;output texture coordinates

m3x3 r1, v3, c4                 ;transform normal. c4-c7 holds 
                                ;inverse world transformation matrix.

dp3 r1.w, r1, r1                ;renormalize normal
rsq r1.w, r1.w
mul r1, r1, r1.w

m4x4 r7, v0, c8                 ;transform vertex to world space.
                                ;c8-c11 holds transposed world 
                                ;transformation matrix

add r10.xyz, c12, -r7.xyz       ;calculate vector from vertex position to
                                ;light position (both in world space)

dp3 r10.w, r10, r10             ;renormalize light vector
rsq r11.w, r10.w	                    
mul r11, r10, r11.w

dst r4, r10.w, r11.w            ;compute the distance attenuation
dp3 r7.x, r4, c16               ;in c16 we have modifiers of attenuation
rcp r7.x, r7.x                  ;reciprocal of attenuation

add r2, c13, r11                ;compute half vector H = L + V

dp3 r2.w, r2, r2                ;renormalize H
rsq r2.w, r2.w	
mul r2, r2, r2.w

dp3 r0.y, r1, r2                ;N dot H
dp3 r0.x, r1, r11               ;N dot L
mov r0.w, c17.x                 ;get shining strength from c17
lit r4, r0

mul r4, r4, r7.xxxx             ;Scale the diffuse and specular intensity 
                                ;by the attenuation
mul oD0, c14, r4.y              ;diffuse color * diffuse intensity (r4.y)
mul oD1, c15, r4.z              ;specular color * specular intensity (r4.z)
