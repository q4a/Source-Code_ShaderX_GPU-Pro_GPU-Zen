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
#include "rootclass.h"
#include "d3d9.h"

//############################################################################
//Forward declarations


class CMeshPart : public CRootClass {
protected:
    //Index buffer part
    IDirect3DIndexBuffer9  *IndexBuffer;
    DWORD                   IndexLockCount;
    //Index buffer properties
    UINT                    IndexLength;
    DWORD                   IndexUsage;
    D3DFORMAT               IndexFormat;
    D3DPOOL                 IndexPool;
    //Vertex buffer part
    IDirect3DVertexBuffer9  *VertexBuffer;
    DWORD                   VertexLockCount;
    //Vertex buffer properties
    UINT                    VertexLength;
    DWORD                   VertexUsage;
    D3DPOOL                 VertexPool;
    DWORD                   VertexFVF;

    //For SetStreamSource
    UINT                    StrideSize;
    //For SetIncides
    UINT                    BaseVertexIndex;
    //For DrawIndexedPrimitive
    D3DPRIMITIVETYPE        PrimitiveType;
    UINT                    MinIndex;
    UINT                    NumVerticles;
    UINT                    StartIndex;
    UINT                    PrimitiveCount;

    int                     AppData;        //Application specific data (i.e. MeshD3F loader stores here default 
                                            //material ID for scene loader (ya I know it is a 
                                            //non-systematic hack :-)))
public:
    D3_INLINE IDirect3DIndexBuffer9  *GetIndexBuffer() { return IndexBuffer; }
    D3_INLINE DWORD GetIndexLockCount() { return IndexLockCount; }

    D3_INLINE UINT GetIndexLength() { return IndexLength; }
    D3_INLINE DWORD GetIndexUsage() { return IndexUsage; }
    D3_INLINE D3DFORMAT GetIndexFormat() { return IndexFormat; }
    D3_INLINE D3DPOOL GetIndexPool() { return IndexPool; }

    D3_INLINE IDirect3DVertexBuffer9  *GetVertexBuffer() { return VertexBuffer; }
    D3_INLINE DWORD GetVertexLockCount() { return VertexLockCount; }

    D3_INLINE UINT GetVertexLength() { return VertexLength; }
    D3_INLINE DWORD GetVertexUsage() { return VertexUsage; }
    D3_INLINE D3DPOOL GetVertexPool() { return VertexPool; }
    D3_INLINE DWORD GetVertexFVF() { return VertexFVF; }

    D3_INLINE UINT GetStrideSize() { return StrideSize; }
    D3_INLINE void SetStrideSize(UINT StrideSize) { this->StrideSize = StrideSize; }
    D3_INLINE UINT GetBaseVertexIndex() { return BaseVertexIndex; }
    D3_INLINE void SetBaseVertexIndex(UINT BaseVertexIndex) { this->BaseVertexIndex = BaseVertexIndex; }
    D3_INLINE D3DPRIMITIVETYPE GetPrimitiveType() { return PrimitiveType; }
    D3_INLINE void SetPrimitiveType(D3DPRIMITIVETYPE PrimitiveType) { this->PrimitiveType = PrimitiveType; }
    D3_INLINE UINT GetMinIndex() { return MinIndex; }
    D3_INLINE void SetMinIndex(UINT MinIndex) { this->MinIndex = MinIndex; }
    D3_INLINE UINT GetNumVerticles() { return NumVerticles; }
    D3_INLINE void SetNumVerticles(UINT NumVerticles) { this->NumVerticles = NumVerticles; }
    D3_INLINE UINT GetStartIndex() { return StartIndex; }
    D3_INLINE void SetStartIndex(UINT StartIndex) { this->StartIndex = StartIndex; }
    D3_INLINE UINT GetPrimitiveCount() { return PrimitiveCount; }
    D3_INLINE void SetPrimitiveCount(UINT PrimitiveCount) { this->PrimitiveCount = PrimitiveCount; }
    D3_INLINE int GetAppData() { return AppData; }
    D3_INLINE void SetAppData(int AppData) { this->AppData = AppData; }

    char *IndexLock(UINT OffsetToLock, UINT SizeToLock, DWORD Flags);
    void IndexUnlock(bool ForceAll = FALSE);
    bool IndexBufferCreate(IDirect3DDevice9 *Device, UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool);
    void IndexDelete();
    void IndexUseExisting(CMeshPart &Part);

    char *VertexLock(UINT OffsetToLock, UINT SizeToLock, DWORD Flags);
    void VertexUnlock(bool ForceAll = FALSE);
    bool VertexBufferCreate(IDirect3DDevice9 *Device, UINT Length, DWORD Usage, UINT FVF, D3DPOOL Pool);
    void VertexDelete();
    void VertexUseExisting(CMeshPart &Part);

    CMeshPart(void);
    virtual ~CMeshPart(void);
};
