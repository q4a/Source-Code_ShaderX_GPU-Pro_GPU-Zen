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


// Tester02.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <D3dx9.h>
#include "global.h"
#include "EngineVersion.h"
#include "ENG-engine.h"
#include "eng-rendersettings.h"

//Managers
#include "MGR-MemoryManager.h"
#include "MGR-DirectGraphicsManager.h"
#include "WND-WindowManager.h"

//Files
#include "FIL-FileManager.h"
#include "FIL-DirTreeDirectory.h"

//System classes
#include "SC-Timer.h"
#include "SC-MString.h"
#include "eng-framerate.h"
#include "SC-HashTable2.h"
#include "sc-xmlparser.h"

//Scene
#include "eng-simplescene.h"
#include "eng-rendergroup.h"

//Materials
#include "Material\MAT-MaterialManager.h"
#include "material\MAT-DX9DiffuseSpecularSpot.h"
#include "material\MAT-DX9CookTorrance.h"
#include "material\MAT-DX9OrenNayar.h"
#include "material\MAT-DX9Reflective.h"
#include "material\MAT-DX9ShadowPhong.h"

//Texture samplers
#include "texturesampler\txs-texture2dsampler_specularpower.h"
#include "texturesampler\txs-texture2dsampler_specularmaps.h"
#include "texturesampler\txs-texture2dsampler_fresnel.h"
#include "texturesampler\TXS-Texture2DSampler_BeckmanDistribution.h"
#include "texturesampler\TXS-Texture2DSampler_Rcp.h"
#include "texturesampler\TXS-Texture2DSampler_OrenNayar.h"

CEngine *eng;

void RunSamplers() {
/*
    //Texture sampler to create full specular lightmap
    CTexture2DSampler_SpecularPower *TexSampler = new CTexture2DSampler_SpecularPower("Sampler", eng);
    TexSampler->FillTextureFile("C:\\D3F_LightMap.dds", 128,128,1);
    delete TexSampler;
//*/
/*
    //Texture sampler to create partial lightmaps
    CTexture2DSampler_SpecularMaps *TexSampler2 = new CTexture2DSampler_SpecularMaps("Sampler", eng);
    TexSampler2->FillTextureFile("C:\\D3F_LightMap", 512,8,1);
    //TexSampler2->FillTextureFile("C:\\D3F_LightMap", 1024,64,1);
    delete TexSampler2;
//*/
/*
    //Texture sampler to create fresnel term texture
    CTexture2DSampler_Fresnel *TexSampler3 = new CTexture2DSampler_Fresnel("Sampler", eng);
    TexSampler3->FillTextureFile("C:\\D3F_Fresnel.dds", 512,8,1);
    TexSampler3->FillTextureFile("C:\\D3F_Fresnel_NotInterpolated.dds", 512,1,1);
    delete TexSampler3;
//*/
/*
    //Texture sampler to create Beckman Distribution - cook torrance
    CTexture2DSampler_BeckmanDistribution *TexSampler4 = new CTexture2DSampler_BeckmanDistribution("Sampler", eng);
    TexSampler4->FillTextureFile("C:\\D3F_BeckMan512.dds", 512,512,1);
    TexSampler4->FillTextureFile("C:\\D3F_BeckMan256.dds", 256,256,1);
    TexSampler4->FillTextureFile("C:\\D3F_BeckMan128.dds", 128,128,1);
    delete TexSampler4;
//*/
/*
    //Texture sampler to reciprocal part of CookTorrance lighting
    CTexture2DSampler_Rcp *TexSampler5 = new CTexture2DSampler_Rcp("Sampler", eng);
    TexSampler5->FillTextureFile("C:\\D3F_Rcp512.dds", 512,512,1);
    TexSampler5->FillTextureFile("C:\\D3F_Rcp256.dds", 256,256,1);
    TexSampler5->FillTextureFile("C:\\D3F_Rcp128.dds", 128,128,1);
    delete TexSampler5;
//*/
/*
    //Texture sampler for part of Oren-Nayar lighting
    CTexture2DSampler_OrenNayar *TexSampler6 = new CTexture2DSampler_OrenNayar("Sampler", eng);
    TexSampler6->FillTextureFile("C:\\D3F_OrenNayar512.dds", 512,512,1);
    TexSampler6->FillTextureFile("C:\\D3F_OrenNayar256.dds", 256,256,1);
    TexSampler6->FillTextureFile("C:\\D3F_OrenNayar128.dds", 128,128,1);
    delete TexSampler6;
//*/
}

int work(int argc, char* argv[]) {
    bool cont=TRUE;
    printf("\n\nStarting the magic\n\n");

    printf("Parameter parsing\n");
    //parse parameters
    char tempparam[1024];
    char nextparam[1024];
    CMString InputFile;
    CMString OutputFile;
    CMString Speed;
    CMString Material;
    for (int i=1; i<argc; i++) {
        strcpy(tempparam, argv[i]);
        strupr(tempparam);
        if (i+1<argc) {
            strcpy(nextparam, argv[i+1]);
            strupr(nextparam);
        } else {
            strcpy(nextparam, "");
        }
        if (strcmp(tempparam, "-I")==0) { //input
            InputFile = nextparam;
        } else if (strcmp(tempparam, "-O")==0) { //output
            OutputFile = nextparam;
        } else if (strcmp(tempparam, "-S")==0) { //speed
            Speed = nextparam;
        } else if (strcmp(tempparam, "-M")==0) { //material
            Material = nextparam;
        }
    }
    //Load scene
    if (InputFile.Length()==0) {
        printf("Error: No input file specified!\n");
        return -1;
    }

    //Init engine from config file
    eng->InitEngineFromINI(".\\engine.ini");
    //eng->InitEngineFromINI("D:\\Diplomovka\\Source\\engine.ini");

    //SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
    //SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
    //SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
    //SetProcessPriorityBoost(GetCurrentProcess(),TRUE);

    RunSamplers();

    _LOG("Engine",NULL,D3_DV_GRP0_LEV0,"------------------------------------------------------------------------");
    _LOG("Engine",NULL,D3_DV_GRP0_LEV0,"-[Scene init]-----------------------------------------------------------");
    _LOG("Engine",NULL,D3_DV_GRP0_LEV0,"------------------------------------------------------------------------");

    //rendering speed
    float renderSpeed = 1.0f;
    if (Speed.Length()>0) {
        float tmpSpeed = (float)atof(Speed.c_str());
        if (tmpSpeed>0) renderSpeed = 1.0f / tmpSpeed;
    }
    printf("Playing speed - %f\n", renderSpeed);

    //material prepare
    if (Material.Length()==0) Material="DEFAULT";
    ///Create one material with ID D3_GRI_Mat_PixelPhong
    CBaseMaterial *MPhong = (CBaseMaterial *)eng->GetMaterialManager()->Query(D3_GRI_Mat_PixelPhong);
    if (MPhong == NULL) {
        if (strcmp(Material.c_str(), "SPOTLIGHT") == 0) {
            MPhong = new CDX9_MaterialDiffuseSpecularSpot("DX9_Spot",eng);
            ((CDX9_MaterialDiffuseSpecularSpot *)MPhong)->SetShaderVersion(1);
        } else if (strcmp(Material.c_str(), "SPOTLIGHT2") == 0) {
            MPhong = new CDX9_MaterialDiffuseSpecularSpot("DX9_Spot2",eng);
            ((CDX9_MaterialDiffuseSpecularSpot *)MPhong)->SetShaderVersion(2);
        } else if (strcmp(Material.c_str(), "SPOTLIGHT3") == 0) {
            MPhong = new CDX9_MaterialDiffuseSpecularSpot("DX9_Spot3",eng);
            ((CDX9_MaterialDiffuseSpecularSpot *)MPhong)->SetShaderVersion(3);
        } else if (strcmp(Material.c_str(), "COOKTORRANCE") == 0) {
            MPhong = new CDX9_MaterialCookTorrance("DX9_CookTorrance",eng);
            ((CDX9_MaterialCookTorrance *)MPhong)->SetShaderVersion(1);
        } else if (strcmp(Material.c_str(), "COOKTORRANCE2") == 0) {
            MPhong = new CDX9_MaterialCookTorrance("DX9_CookTorrance_PS2",eng);
            ((CDX9_MaterialCookTorrance *)MPhong)->SetShaderVersion(2);
        } else if (strcmp(Material.c_str(), "ORENNAYAR") == 0) {
            MPhong = new CDX9_MaterialOrenNayar("DX9_OrenNayar",eng);
        } else if (strcmp(Material.c_str(), "ENVMAP") == 0) {
            MPhong = new CDX9_MaterialReflective("DX9_Reflective",eng);
            ((CDX9_MaterialReflective *)MPhong)->SetShaderVersion(1);
        } else if (strcmp(Material.c_str(), "ENVMAP2") == 0) {
            MPhong = new CDX9_MaterialReflective("DX9_Reflective_PS2",eng);
            ((CDX9_MaterialReflective *)MPhong)->SetShaderVersion(2);
        } else if (strcmp(Material.c_str(), "SHADOW") == 0) {
            MPhong = new CDX9_MaterialShadowPhong("DX9_ShadowPhong",eng);
            eng->GetRenderSettings()->Flags |= D3RS_SHADOWSENABLED;
        } else {
            MPhong = new CDX9_MaterialDiffuseSpecularSpot("DX9_Spot",eng);
            ((CDX9_MaterialDiffuseSpecularSpot *)MPhong)->SetShaderVersion(1);
            printf("Warning: material not recognised, selecting default.\n");
        }
        MPhong->SetResourceID(D3_GRI_Mat_PixelPhong);
        eng->GetMaterialManager()->Add(MPhong->GetResourceID(),*MPhong);
    }
    printf("Material - %s\n", MPhong->GetObjName());
    //Initialize scene
    CSimpleScene *Scene = new CSimpleScene("Scene",eng);
    //Add render groups
    CRenderGroup *RenderGroup = new CRenderGroup("Group",eng);
    Scene->AddGroupByID(0,*RenderGroup);
    //Load scene
    if (InputFile.Length()!=0) {
        printf("Scene - '%s'\n", InputFile.c_str());
        int resindex = eng->GetFileManager()->Find(InputFile);
        if (resindex>=0) {
            bool loadresult = Scene->Load(resindex, *eng->GetRenderSettings());
            if (!loadresult) {
                printf("Cannot load '%s'\n", InputFile.c_str());
                cont = false;
            }
        } else {
            printf("Cannot load '%s'\n", InputFile.c_str());
            cont = false;
        }
    } else {
        printf("Error: No input file specified!\n");
        cont = false;
    }

    _LOG("Engine",NULL,D3_DV_GRP0_LEV0,"------------------------------------------------------------------------");
    _LOG("Engine",NULL,D3_DV_GRP0_LEV0,"-[Rendering]------------------------------------------------------------");
    _LOG("Engine",NULL,D3_DV_GRP0_LEV0,"------------------------------------------------------------------------");

    //debug 
    MSG pok;
    
    CTimer *Timer = new CTimer("Timer",eng);
    CFrameRate *FPS_Counter = new CFrameRate("FPS_Counter",eng);

    Timer->ResumeTimer();
    eng->GetRenderSettings()->RenderTime = Timer->GetTimerSecond();
    eng->GetRenderSettings()->PrevRenderTime = eng->GetRenderSettings()->RenderTime;
    bool    WasDeactivated = FALSE;
    bool    DeviceIsLost = FALSE;
    double realTime = eng->GetRenderSettings()->RenderTime;

/*
    D3DVIEWPORT9    ViewPort;
    ViewPort.X = ViewPort.Y = 0;
    ViewPort.Height = ViewPort.Width = 256;
    ViewPort.MinZ = 0.0f;
    ViewPort.MaxZ = 1.0f;

    eng->GetGraphicsManager()->Direct3DDevice->SetViewport(&ViewPort);
*/

    while (cont) {
        if (PeekMessage(&pok,NULL,0,0,PM_NOREMOVE)) {
            if (GetMessage(&pok,NULL,0,0)<=0) cont=FALSE;   //shutdown application...
            TranslateMessage( &pok );
            DispatchMessage( &pok );
            if (pok.message == MSG_WND_CLOSED) {    //in this case, we will quit application
                PostQuitMessage(0);
            }
        } else {    //iddle code...
            if (1) {
//            if (eng->GetWindowManager()->m_bActive) {
                if (WasDeactivated || DeviceIsLost) {
                    HRESULT res = eng->GetGraphicsManager()->Direct3DDevice->TestCooperativeLevel();
                    if (res == D3DERR_DEVICENOTRESET) {
                        eng->DeviceLost();
                        Scene->DeviceLost(*eng->GetRenderSettings());
                        eng->GetGraphicsManager()->Direct3DDevice->Reset(&eng->GetGraphicsManager()->StartPresentationParameters);
                        eng->Restore();
                        Scene->Restore(*eng->GetRenderSettings());
                        DeviceIsLost = FALSE;
                    } else if (res == D3DERR_DEVICELOST) {
                        DeviceIsLost = TRUE;
                    }
                    WasDeactivated = FALSE;
                    Timer->ResumeTimer();
                }
                eng->GetRenderSettings()->PrevRenderTime = eng->GetRenderSettings()->RenderTime;
                //eng->GetRenderSettings()->RenderTime = eng->GetRenderSettings()->RenderTime + 0.010f;
                eng->GetRenderSettings()->RenderTime = eng->GetRenderSettings()->RenderTime + renderSpeed;
                realTime = Timer->GetTimerSecond();
                //eng->GetRenderSettings()->RenderTime = Timer->GetTimerSecond();
                eng->GetRenderSettings()->TimesDelta = eng->GetRenderSettings()->RenderTime - eng->GetRenderSettings()->PrevRenderTime;
                //Set correct render ID
                eng->GetRenderSettings()->RenderID++;
                if (eng->GetRenderSettings()->RenderID == 0) eng->GetRenderSettings()->RenderID++;
                //timer
                //timer overleapping
                if (eng->GetRenderSettings()->RenderTime<eng->GetRenderSettings()->PrevRenderTime) {
                    eng->GetRenderSettings()->Flags &= D3RS_TIMEOVERLEAPPED;
                    _WARN("EXE","EXE", ErrMgr->TSystem, "Time overtalpped");
                } else {
                    eng->GetRenderSettings()->Flags = eng->GetRenderSettings()->Flags & (~D3RS_TIMEOVERLEAPPED);
                }
                //if (eng->GetRenderSettings()->RenderID>0) {
                if (eng->GetRenderSettings()->RenderID>0) {
                    //eng->GetGraphicsManager()->Direct3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255,0,255), 1.0f, 0 );
                    //eng->GetGraphicsManager()->Direct3DDevice->BeginScene();

                    Scene->Render(*eng->GetRenderSettings());

                    //eng->GetGraphicsManager()->Direct3DDevice->EndScene();

                    //screenshot
                    if (OutputFile.Length()>0) {
                        IDirect3DSurface9 *OutSurface;

                        HRESULT res2 = eng->GetGraphicsManager()->Direct3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &OutSurface);
                        if (FAILED(res2)) {
                            res2 = 0;
                        }

                        CMString OutFileName;
                        OutFileName.Format(".\\%s_%06i.bmp", OutputFile.c_str(), eng->GetRenderSettings()->RenderID);

                        res2 = D3DXSaveSurfaceToFile(OutFileName.c_str(), D3DXIFF_BMP, OutSurface, NULL, NULL);
                        if (FAILED(res2)) {
                            res2 = 0;
                        }

                        OutSurface->Release();
                    }

                    HRESULT res = eng->GetGraphicsManager()->Direct3DDevice->Present( NULL, NULL, NULL, NULL );
                    if (res == D3DERR_DEVICELOST) {
                        DeviceIsLost = TRUE;
                    }
                }

                double OldRT = eng->GetRenderSettings()->RenderTime;
                eng->GetRenderSettings()->RenderTime = realTime;

                if (eng->GetRenderSettings()->RenderTime - FPS_Counter->GetLastApplicationTime()>= 1.0f) {
                    double fps = FPS_Counter->GetFPS(*eng->GetRenderSettings());
                    char tmp[256];
                    sprintf(tmp,"FPS : %0.4f | RenderID - %u", fps, eng->GetRenderSettings()->RenderID);
                    SetWindowText(eng->GetWindowManager()->m_hWnd, tmp);
                }

                eng->GetRenderSettings()->RenderTime = OldRT;
            } else {
                Timer->PauseTimer();
                WasDeactivated = TRUE;
            }
        }
    }

    _LOG("Engine",NULL,D3_DV_GRP0_LEV0,"------------------------------------------------------------------------");
    _LOG("Engine",NULL,D3_DV_GRP0_LEV0,"-[Scene destroy]--------------------------------------------------------");
    _LOG("Engine",NULL,D3_DV_GRP0_LEV0,"------------------------------------------------------------------------");
    delete Scene;
    eng->GetMaterialManager()->Delete(MPhong->GetResourceID());
    delete Timer;
    delete FPS_Counter;
    _LOG("Engine",NULL,D3_DV_GRP0_LEV0,"------------------------------------------------------------------------");
    _LOG("Engine",NULL,D3_DV_GRP0_LEV0,"-[Work end]-------------------------------------------------------------");
    _LOG("Engine",NULL,D3_DV_GRP0_LEV0,"------------------------------------------------------------------------");
    return 0;
}

void help() {
    printf("%s renderer example - DirectX 9 version\n", ProjectNameString);
    printf("   Version %s\n", ProjectVersionString);
    printf("   Copyright (C) 2000,2001,2002,2003 Michal Valient\n");
    printf("   http://www.dimension3.host.sk\n");
    printf("\n");
    printf("This example shows advanced lighting and shading methods.\n");
    printf("Pixel shader requirements are specified for each example in form PS.X.X.\n");
    printf("You can switch to reference rasterizer in engine.ini file.\n");
    printf("\n");
    printf("Usage: app.exe -i <filename> -m <material> [-o <filename>] [-s <speed>]\n");
    printf("Example: app.exe -m spotlight -s 3 -o spot\n");
    printf("\n");
    printf("Parameters:\n");
    printf("-i <filename> plays specified XML scene file\n");
    printf("   ! do not specify any path, just filename and extension !\n");
    printf("\n");
    printf("-m <material> sets specified material\n");
    printf("   Materials are:\n");
    printf("     spotlight      Phong lighting with spotlight - PS.1.4\n");
    printf("     spotlight2     Phong lighting with spotlight - PS.2.0\n");
    printf("     spotlight3     Phong lighting with spotlight - PS.3.0\n");
    printf("     cooktorrance   Cook-Torrance lighting - PS.1.4\n");
    printf("     cooktorrance2  Cook-Torrance lighting - PS.2.0\n");
    printf("     orennayar      Oren-Nayar lighting - PS.2.0\n");
    printf("     envmap         Environmental mapping - PS.1.4\n");
    printf("     envmap2        Environmental mapping - PS.2.0\n");
    printf("     shadow         Phong lighting with spotlight and shadows - PS.2.0\n");
    printf("\n");
    printf("-o <filename> saves each screen into file '<filename>_nnnn.bmp'\n");
    printf("   nnnn is frame number\n");
    printf("   ! do not specify any path, just filename without extension!\n");
    printf("-s <speed> alters playing speed - any positive number\n");
    printf("   Speed modifier:\n");
    printf("     1              original speed\n");
    printf("     2              half speed\n");
    printf("     n              1/n of original speed (n times slower)\n");
    printf("\n");
}

__cdecl main(int argc, char* argv[]) {
    help();
    eng = CEngine::CreateEngine(".\\engine.log");
    work(argc,argv);
    delete eng;
    return 0;
}
