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

#include "eng-baseresource.h"

class CBase3DObject;

class CBaseMaterialContext : public CRootClass {
    public:
        CBase3DObject   *Object;
};

class CBaseMaterial : public CBaseResource {
protected:
    ///Material specific flags
    DWORD MatFlags;

    virtual ~CBaseMaterial(void);
public:
    static const DWORD Mat_Flag_Set            = 1;     
    static const DWORD Mat_Flag_Context        = 2;     

    ///returns IsSet state
    ///If IsSet is true, material is set as current material
    D3_INLINE bool GetIsSet() { 
        return (MatFlags & Mat_Flag_Set) ? true : false;
    }
    ///Sets IsSetStatus.
    D3_INLINE void SetIsSet(bool Status) {
        if (Status) MatFlags = MatFlags | Mat_Flag_Set;
        else MatFlags = MatFlags & (~Mat_Flag_Set);
    }

    ///returns Context state
    ///If HasContext is true, material can be customized for each object
    ///each object can call CreateContext/SetContext/GetContext method to
    ///Context is material specific and can contain texture ID's, shader constants and so.
    ///instruct material to use settings from that context.
    ///Each method, that stores it's data into context is documented in that way (depending on context).
    D3_INLINE bool GetHasContext() { return (MatFlags & Mat_Flag_Context)? true : false; }
    ///Sets HasContext.
    D3_INLINE void SetHasContext(bool Status) {
        if (Status) MatFlags = MatFlags | Mat_Flag_Context;
        else Flags = MatFlags & (~Mat_Flag_Context);
    }

    ///returns render pass count for this material
    ///This method is defined as possible depending on context
    virtual DWORD GetPassCount() = 0;
    ///Sets PassCount.
    ///This method is defined as possible depending on context
    virtual void SetPassCount(DWORD PassCount) = 0;

    ///This method creates empty context for material
    virtual CBaseMaterialContext *CreateContext(CBase3DObject *Obj) = 0;

    ///This method erases context
    ///Default implementation does 
    virtual void DeleteContext(CBaseMaterialContext *Context) = 0;

    ///This method sets current context
    ///NULL as parameter means to set default context (the initial state)
    ///Return false on error.
    ///Material MUST accept NULL context parameter even if it is not context-able.
    virtual bool SetContext(CBaseMaterialContext *Context) =0;

    ///Sets this material as current.
    ///Return false on error
    virtual bool Set(CRenderSettings &RenderSettings) = 0;

    ///Prepares material for rendering specified pass
    ///Return false on error
    virtual bool SetPass(CRenderSettings &RenderSettings, DWORD PassNumber) = 0;

    ///Returns stages set by material to previous state
    ///Return false on error
    virtual bool UnSet(CRenderSettings &RenderSettings) = 0;

    ///Const, dest & more
    CBaseMaterial(char *ObjName,CEngine *OwnerEngine);

    MAKE_CLSNAME("CBaseMaterial");
};
