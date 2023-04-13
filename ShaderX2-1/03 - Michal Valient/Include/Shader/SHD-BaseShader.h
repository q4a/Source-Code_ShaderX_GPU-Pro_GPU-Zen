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


#pragma once

#include "nommgr.h"
#include "d3d9.h"
#include "mmgr.h"

#include "eng-baseresource.h"

class CShaderManager;

class CBaseShader : public CBaseResource {
protected:
    ///Shader specific flags
    DWORD ShaderFlags;

    virtual ~CBaseShader(void);
public:
    static const DWORD Shader_Flag_Set         = 1;

    ///returns IsSet state
    ///If IsSet is true, shader is set as current
    D3_INLINE bool GetIsSet() { 
        return (ShaderFlags & Shader_Flag_Set) ? true : false;
    }
    ///Sets IsSetStatus.
    D3_INLINE void SetIsSet(bool Status) {
        if (Status) ShaderFlags = ShaderFlags | Shader_Flag_Set;
        else ShaderFlags = ShaderFlags & (~Shader_Flag_Set);
    }

    ///Set this sader as current
    virtual bool Set(CShaderManager *ShaderManager, bool VertexShader, CRenderSettings &Settings) = 0;
    
    ///Const, dest & more
    CBaseShader(char *ObjName,CEngine *OwnerEngine);

    MAKE_CLSNAME("CBaseShader");
};
