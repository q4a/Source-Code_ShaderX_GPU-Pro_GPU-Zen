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

#include "shader\shd-baseshader.h"

class CPixelShader : public CBaseShader {
protected:
    bool                    TryLoadBinaryFirst;     ///try to load binary version first (default is false)

    IDirect3DPixelShader9   *Shader;                ///Shader object

    virtual ~CPixelShader(void);                    ///destroyer
    ///load binary version of shader
    IDirect3DPixelShader9 *Load_Bin(void *Buffer, int BufferSize, CRenderSettings &RenderSettings);
    ///load and compile source version of shader
    IDirect3DPixelShader9 *Load_Src(void *Buffer, int BufferSize, CRenderSettings &RenderSettings);
public:
    ///set value of TryLoadBinaryFirst
    void SetLoadBinaryFirst(bool TryLoadBinaryFirst) {
        this->TryLoadBinaryFirst = TryLoadBinaryFirst;
    }
    ///get value of TryLoadBinaryFirst
    bool GetLoadBinaryFirst() {
        return this->TryLoadBinaryFirst;
    }

    virtual bool Load(DWORD LoadIndex, CRenderSettings &RenderSettings);
    virtual bool Update(CRenderSettings &RenderSettings);
    virtual void DeviceLost(CRenderSettings &Settings);
    virtual void DoCacheOut(CRenderSettings &RenderSettings);

    virtual bool Set(CShaderManager *ShaderManager, bool VertexShader, CRenderSettings &Settings);

    CPixelShader(char *ObjName,CEngine *OwnerEngine);

    MAKE_CLSCREATE(CPixelShader);
    MAKE_CLSNAME("CPixelShader");
    MAKE_DEFAULTGARBAGE()
};
