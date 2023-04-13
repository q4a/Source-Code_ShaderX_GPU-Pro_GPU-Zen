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


/*
    Uses two NVIDIA Effect's browser methods for tangent space calculation.
*/
#pragma once
#include "eng-mesh.h"
#include "nommgr.h"
#include "D3dx9.h"
#include "mmgr.h"

class CMeshD3F : public CMesh {
    //My directX vertex bufffer struct.
    // Vertex format for Dimension^3 mesh.
    struct D3F_VERTEX {
        D3DXVECTOR3 Position;
	    D3DXVECTOR3 Normal;
	    D3DXVECTOR2 Texture;
	    D3DXVECTOR3 S;
    };
public:
    virtual bool Load(DWORD LoadIndex, CRenderSettings &RenderSettings);
    virtual bool Update(CRenderSettings &RenderSettings);

    HRESULT FindAndFixDegenerateVertexBasis(	D3F_VERTEX	* pVertices,
											DWORD		  dwNumVertices,
											void		* pIndices,
											DWORD		  dwNumIndices,
											bool bUnifyNormals,
											bool is32BitIndices);
    HRESULT CreateBasisVectors(LPDIRECT3DVERTEXBUFFER9 pVertexBuffer, LPDIRECT3DINDEXBUFFER9 pIndexBuffer);

    CMeshD3F(char *ObjName, CEngine *OwnerEngine);
    MAKE_CLSCREATE(CMeshD3F);
    MAKE_CLSNAME("CMeshD3F");
    MAKE_DEFAULTGARBAGE()
};
