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
; Ambient color shader - Vertex shader
;
; Desc :
;   Renders ambient (and emisive) colored output
;   We pass ambient multiplier as texture coordinate from vertex shader,
;   because pixel shaders have limited precision for constants and
;   they are clamped to [-1,1]
;
; Part of Dimension3 Renderer
; (c)2002 Michal Valient
; akka em / STV3 akka  R.A.Y / EastW00D
;------------------------------
;
; Constant registers 
; c4-c7 world * view * proj
; c0 final ambient (and emisive) color multiplier
;
; Used input registers
; v0 - Position
;
; Output
; oT1 - ambient (and emisive) color multiplier
; oT0 - texture mapping coordinate
; oT1 - ambient color multiplier (rgb part)
; oT2 - ambient color multiplier (alpha part in r)
;------------------------------

vs.1.1

dcl_position v0
dcl_normal v1
dcl_texcoord v2
dcl_tangent v3


m4x4 oPos, v0, c4                   ;vertex clip position
mov oT0.xy, v2.xy                   ;Texture coordinates for color texture
mov oT1, c0                         ;Ambient color multiplier
mov oT2.x, c0.w                     ;Ambient color multiplier (alpha part)
