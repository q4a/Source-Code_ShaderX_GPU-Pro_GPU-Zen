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


// ENG-Types.h: interface for the CTypes class.
// Simple types for Engine.
//
//////////////////////////////////////////////////////////////////////

#pragma once

//Global resource ID's (do not use)
static const DWORD D3_GRI_GlobalStart               = 0xF0000000;
static const DWORD D3_GRI_TemporaryStart            = 0xF2000000;

//Textures
static const DWORD D3_GRI_Tex_Black                 = D3_GRI_GlobalStart + 1;
static const DWORD D3_GRI_Tex_White                 = D3_GRI_GlobalStart + 2;
static const DWORD D3_GRI_Tex_Gray                  = D3_GRI_GlobalStart + 3;
static const DWORD D3_GRI_Tex_ShininessLightMap     = D3_GRI_GlobalStart + 4;
static const DWORD D3_GRI_Tex_DefaultSpotLight      = D3_GRI_GlobalStart + 5;
static const DWORD D3_GRI_Tex_DefaultNotmalMap      = D3_GRI_GlobalStart + 6;
static const DWORD D3_GRI_Tex_DefaultEnvMap         = D3_GRI_GlobalStart + 7;
static const DWORD D3_GRI_Tex_FresnelMap            = D3_GRI_GlobalStart + 8;

static const DWORD D3_GRI_Tex_ShininessStart        = D3_GRI_GlobalStart + 9;   //100 textures from this point are reserved for shininess maps
#define GetShininessID(n) (DWORD)(D3_GRI_Tex_ShininessStart + (DWORD)n)   //Use this macro to obtain correct shininess level

//Materials
static const DWORD D3_GRI_Mat_PixelPhong            = D3_GRI_GlobalStart + 1;
static const DWORD D3_GRI_Mat_PixelNoOutput         = D3_GRI_GlobalStart + 2;
static const DWORD D3_GRI_Mat_ShadowBuffer          = D3_GRI_GlobalStart + 3;

//############################################################################
//Forward declarations


