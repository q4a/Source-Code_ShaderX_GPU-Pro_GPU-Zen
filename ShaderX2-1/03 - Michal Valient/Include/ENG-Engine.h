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


//############################################################################
//# Engine - Manages whole thing
//############################################################################

#pragma once

#include "global.h"

//############################################################################
//Forward declarations
class CWindowManager;
class CDirectGraphicsManager;
class CRenderSettings;
class CMeshManager;
class CFileManager;
class CTextureManager;
class CEngineDefaults;
class CRuntimeCreator;
class CMaterialManager;
class CShaderManager;

class CEngine {
private:
    HINSTANCE               AppInstance;
    CWindowManager          *Window;
    CDirectGraphicsManager  *Gfx;
    CRenderSettings         *RenderSettings;
    CMeshManager            *MeshManager;
    CFileManager            *FileManager;
    CTextureManager         *TextureManager;
    CEngineDefaults         *EngineDefaults;
    CRuntimeCreator         *RuntimeCreator;
    CMaterialManager        *MaterialManager;
    CShaderManager          *ShaderManager;

    DWORD                   LastTemporaryNumber;
protected:
    CEngine();

    D3_INLINE void SetWindowManager(CWindowManager &WindowManager) {
        Window = &WindowManager;
    }

    D3_INLINE void SetGraphicsManager(CDirectGraphicsManager &GfxManager) {
        Gfx = &GfxManager;
    }

    D3_INLINE void SetRenderSettings(CRenderSettings &Settings) {
        RenderSettings = &Settings;
    }
public:
	void Restore();
	void DeviceLost();
	bool InitEngineFromINI(char *IniFileName);
    static CEngine *CreateEngine(char *sLogFile);
    virtual ~CEngine();

    //This functions returns unique temporary number for resource managers
    DWORD GetUniqueTemporaryNumber();

    //Application instance settings
    D3_INLINE HINSTANCE GetAppInstance() { return AppInstance; }
    //Window manager manipulator
    D3_INLINE CWindowManager *GetWindowManager() { return Window; }
    //Direct graphics manager manipulator
    D3_INLINE CDirectGraphicsManager *GetGraphicsManager() { return Gfx; }
    //Render settings manipulator
    D3_INLINE CRenderSettings *GetRenderSettings() { return RenderSettings; }
    //Mesh manager manipulator
    D3_INLINE CMeshManager *GetMeshManager() { return MeshManager; }
    //Texture manager manipulator
    D3_INLINE CTextureManager *GetTextureManager() { return TextureManager; }
    //Texture manager manipulator
    D3_INLINE CFileManager *GetFileManager() { return FileManager; }
    //Engine defaults
    D3_INLINE CEngineDefaults *GetEngineDefaults() { return EngineDefaults; }
    //runtime creator
    D3_INLINE CRuntimeCreator *GetRuntimeCreator() { return RuntimeCreator; }
    //MaterialManager
    D3_INLINE CMaterialManager *GetMaterialManager() { return MaterialManager; }
    //ShaderManager
    D3_INLINE CShaderManager *GetShaderManager() { return ShaderManager; }
    // This method registers all classes derived from CBaseClass that engine knows for dynamic creation
    void RegisterRuntimeClasses(void);
};

