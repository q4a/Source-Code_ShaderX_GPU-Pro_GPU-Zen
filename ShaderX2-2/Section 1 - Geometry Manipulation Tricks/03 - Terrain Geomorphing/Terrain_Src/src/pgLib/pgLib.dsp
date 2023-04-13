# Microsoft Developer Studio Project File - Name="pgLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=pgLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pgLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pgLib.mak" CFG="pgLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pgLib - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pgLib - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pgLib - Win32 Win32 Release_Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pgLib - Win32 Release Intel" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pgLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Release/pgLib"
# PROP Intermediate_Dir "../Release/pgLib"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "pgLib_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I ".." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PGLIB_EXPORTS" /D "PGLIB_PROFILE" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc07 /d "NDEBUG"
# ADD RSC /l 0xc07 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ..\Libs\FMOD\Lib\fmodvc.lib winmm.lib d3d8.lib d3dx8.lib dinput8.lib dxguid.lib /nologo /dll /machine:I386 /out:"../Exec/pgLib.dll"
# SUBTRACT LINK32 /profile /debug

!ELSEIF  "$(CFG)" == "pgLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../Debug/pgLib"
# PROP Intermediate_Dir "../Debug/pgLib"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "pgLib_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W4 /Gm /GX /ZI /Od /I ".." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PGLIB_EXPORTS" /D "PGLIB_PROFILE" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc07 /d "_DEBUG"
# ADD RSC /l 0xc07 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib d3d8.lib d3dx8d.lib dinput8.lib dxguid.lib /nologo /dll /debug /machine:I386 /out:"../Exec/pgLibD.dll" /pdbtype:sept

!ELSEIF  "$(CFG)" == "pgLib - Win32 Win32 Release_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "pgLib___Win32_Win32_Release_Debug"
# PROP BASE Intermediate_Dir "pgLib___Win32_Win32_Release_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Release_Debug/pgLib"
# PROP Intermediate_Dir "../Release_Debug/pgLib"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /Z7 /O2 /I ".." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PGLIB_EXPORTS" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MT /W3 /GX /Z7 /O2 /I ".." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PGLIB_EXPORTS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc07 /d "NDEBUG"
# ADD RSC /l 0xc07 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ..\Libs\FMOD\Lib\fmodvc.lib winmm.lib d3d8.lib d3dx8.lib dinput8.lib dxguid.lib /nologo /dll /debug /machine:I386 /out:"../Exec/pgLib.dll"
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ..\Libs\FMOD\Lib\fmodvc.lib winmm.lib d3d8.lib d3dx8.lib dinput8.lib dxguid.lib /nologo /dll /debug /machine:I386 /out:"../Exec/pgLibRD.dll" /FIXED:NO
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "pgLib - Win32 Release Intel"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "pgLib___Win32_Release_Intel"
# PROP BASE Intermediate_Dir "pgLib___Win32_Release_Intel"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Release_Intel/pgLib"
# PROP Intermediate_Dir "../Release_Intel/pgLib"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I ".." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PGLIB_EXPORTS" /D "PGLIB_PROFILE" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MT /W3 /GX /O2 /I ".." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PGLIB_EXPORTS" /D "PGLIB_PROFILE" /D "_USE_INTEL_COMPILER" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc07 /d "NDEBUG"
# ADD RSC /l 0xc07 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ..\Libs\FMOD\Lib\fmodvc.lib winmm.lib d3d8.lib d3dx8.lib dinput8.lib dxguid.lib /nologo /dll /machine:I386 /out:"../Exec/pgLib.dll"
# SUBTRACT BASE LINK32 /profile /debug
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ..\Libs\FMOD\Lib\fmodvc.lib winmm.lib d3d8.lib d3dx8.lib dinput8.lib dxguid.lib /nologo /dll /machine:I386 /out:"../Exec/pgLibI.dll"
# SUBTRACT LINK32 /profile /debug

!ENDIF 

# Begin Target

# Name "pgLib - Win32 Release"
# Name "pgLib - Win32 Debug"
# Name "pgLib - Win32 Win32 Release_Debug"
# Name "pgLib - Win32 Release Intel"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Audio"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Audio\pgAudioFMOD.cpp
# End Source File
# Begin Source File

SOURCE=.\Audio\pgIAudio.cpp
# End Source File
# Begin Source File

SOURCE=.\Audio\pgSampleFMOD.cpp
# End Source File
# End Group
# Begin Group "Base"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Base\pgIDirectX.cpp
# End Source File
# Begin Source File

SOURCE=.\Base\pgIFileTool.cpp
# End Source File
# Begin Source File

SOURCE=.\Base\pgIImageTool.cpp
# End Source File
# Begin Source File

SOURCE=.\Base\pgImage.cpp
# End Source File
# Begin Source File

SOURCE=.\Base\pgInTextFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Base\pgIResourceManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Base\pgISettings.cpp
# End Source File
# Begin Source File

SOURCE=.\Base\pgIStringTool.cpp
# End Source File
# Begin Source File

SOURCE=.\Base\pgITime.cpp
# End Source File
# Begin Source File

SOURCE=.\Base\pgIXML.cpp
# End Source File
# Begin Source File

SOURCE=.\Base\pgLog.cpp
# End Source File
# Begin Source File

SOURCE=.\Base\pgOutTextFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Base\pgProfiler.cpp
# End Source File
# Begin Source File

SOURCE=.\Base\pgSettingsFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Base\pgString.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\Terrain\pgTerrainPatch.cpp
# End Source File
# End Group
# Begin Group "FrameWork"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FrameWork\d3dapp.cpp
# End Source File
# Begin Source File

SOURCE=.\FrameWork\d3dfont.cpp
# End Source File
# Begin Source File

SOURCE=.\FrameWork\d3dutil.cpp
# End Source File
# Begin Source File

SOURCE=.\FrameWork\dxutil.cpp
# End Source File
# Begin Source File

SOURCE=.\FrameWork\pgAppStub.cxx

!IF  "$(CFG)" == "pgLib - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pgLib - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pgLib - Win32 Win32 Release_Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pgLib - Win32 Release Intel"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Game"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Game\pgCharacter.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\pgIGame.cpp
# End Source File
# End Group
# Begin Group "Input"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Input\pgIInput.cpp
# End Source File
# Begin Source File

SOURCE=.\Input\pgInputDX.cpp
# End Source File
# Begin Source File

SOURCE=.\Input\pgSteering.cpp
# End Source File
# End Group
# Begin Group "Math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Math\pgIMathTool.cpp
# End Source File
# Begin Source File

SOURCE=.\Math\pgMatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\Math\pgPathLinear.cpp
# End Source File
# Begin Source File

SOURCE=.\Math\pgQuat.cpp
# End Source File
# Begin Source File

SOURCE=.\Math\pgTransform.cpp
# End Source File
# Begin Source File

SOURCE=.\Math\pgVec2.cpp
# End Source File
# Begin Source File

SOURCE=.\Math\pgVec3.cpp
# End Source File
# Begin Source File

SOURCE=.\Math\pgVec3n.cpp
# End Source File
# Begin Source File

SOURCE=.\Math\pgVec4.cpp
# End Source File
# End Group
# Begin Group "Render"

# PROP Default_Filter ""
# Begin Group "BSP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Render\BSP\pgBSPFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\BSP\pgBSPMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\BSP\pgBSPReader.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\BSP\pgBSPTools.cpp
# End Source File
# End Group
# Begin Group "Terrain"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Render\Terrain\pgTerrain.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\Terrain\pgTerrainPatchBase.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\Terrain\pgTerrainPatchMorphHW.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\Terrain\pgTerrainPatchMorphSW.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\Terrain\pgTerrainPVS.cpp
# End Source File
# End Group
# Begin Group "Tree"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Render\Tree\pgBlendTree.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\Tree\pgBlendWood.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\Tree\pgComplexTree.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\Tree\pgSimpleTree.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Render\pgAnimated.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\pgBaseMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\pgD3DObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\pgLensflare.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\pgLight.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\pgLighting.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\pgMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\pgMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\pgMeshUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\pgParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\pgSegment.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\pgSkyBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\pgTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\pgTextureStage.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\pgTransformable.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\pgXUtil.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\pbLib.rc
# End Source File
# Begin Source File

SOURCE=.\pgLib.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Audio_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Audio\pgAudioFMOD.h
# End Source File
# Begin Source File

SOURCE=.\Audio\pgIAudio.h
# End Source File
# Begin Source File

SOURCE=.\Audio\pgIAudioDevice.h
# End Source File
# Begin Source File

SOURCE=.\Audio\pgISample.h
# End Source File
# Begin Source File

SOURCE=.\Audio\pgSampleFMOD.h
# End Source File
# End Group
# Begin Group "Base_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Base\pgIDirectX.h
# End Source File
# Begin Source File

SOURCE=.\Base\pgIFileTool.h
# End Source File
# Begin Source File

SOURCE=.\Base\pgIImageTool.h
# End Source File
# Begin Source File

SOURCE=.\Base\pgImage.h
# End Source File
# Begin Source File

SOURCE=.\Base\pgInFile.h
# End Source File
# Begin Source File

SOURCE=.\Base\pgInTextFile.h
# End Source File
# Begin Source File

SOURCE=.\Base\pgIResourceManager.h
# End Source File
# Begin Source File

SOURCE=.\Base\pgISettings.h
# End Source File
# Begin Source File

SOURCE=.\Base\pgIStringTool.h
# End Source File
# Begin Source File

SOURCE=.\Base\pgITime.h
# End Source File
# Begin Source File

SOURCE=.\Base\pgIXML.h
# End Source File
# Begin Source File

SOURCE=.\Base\pgList.h
# End Source File
# Begin Source File

SOURCE=.\Base\pgLog.h
# End Source File
# Begin Source File

SOURCE=.\Base\pgOutFile.h
# End Source File
# Begin Source File

SOURCE=.\Base\pgOutTextFile.h
# End Source File
# Begin Source File

SOURCE=.\Base\pgPtrList.h
# End Source File
# Begin Source File

SOURCE=.\Base\pgSettingsFile.h
# End Source File
# Begin Source File

SOURCE=.\Base\pgString.h
# End Source File
# End Group
# Begin Group "FrameWork_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FrameWork\d3dapp.h
# End Source File
# Begin Source File

SOURCE=.\FrameWork\d3dfont.h
# End Source File
# Begin Source File

SOURCE=.\FrameWork\d3dres.h
# End Source File
# Begin Source File

SOURCE=.\FrameWork\d3dutil.h
# End Source File
# Begin Source File

SOURCE=.\FrameWork\dxutil.h
# End Source File
# End Group
# Begin Group "Game_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Game\pgCharacter.h
# End Source File
# Begin Source File

SOURCE=.\Game\pgIGame.h
# End Source File
# End Group
# Begin Group "Input_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Input\pgIInput.h
# End Source File
# Begin Source File

SOURCE=.\Input\pgInputDX.h
# End Source File
# Begin Source File

SOURCE=.\Input\pgSteering.h
# End Source File
# End Group
# Begin Group "Math_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Math\pgAABBox.h
# End Source File
# Begin Source File

SOURCE=.\Math\pgIMathTool.h
# End Source File
# Begin Source File

SOURCE=.\Math\pgMatrix.h
# End Source File
# Begin Source File

SOURCE=.\Math\pgPath.h
# End Source File
# Begin Source File

SOURCE=.\Math\pgPathLinear.h
# End Source File
# Begin Source File

SOURCE=.\Math\pgPlane.h
# End Source File
# Begin Source File

SOURCE=.\Math\pgQuat.h
# End Source File
# Begin Source File

SOURCE=.\Math\pgTransform.h
# End Source File
# Begin Source File

SOURCE=.\Math\pgVec2.h
# End Source File
# Begin Source File

SOURCE=.\Math\pgVec3.h
# End Source File
# Begin Source File

SOURCE=.\Math\pgVec3n.h
# End Source File
# Begin Source File

SOURCE=.\Math\pgVec4.h
# End Source File
# End Group
# Begin Group "Render_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Render\md2Normals.h
# End Source File
# Begin Source File

SOURCE=.\Render\pgAnimated.h
# End Source File
# Begin Source File

SOURCE=.\Render\pgBaseMesh.h
# End Source File
# Begin Source File

SOURCE=.\Render\pgD3DObject.h
# End Source File
# Begin Source File

SOURCE=.\Render\pgLensflare.h
# End Source File
# Begin Source File

SOURCE=.\Render\pgLight.h
# End Source File
# Begin Source File

SOURCE=.\Render\pgLighting.h
# End Source File
# Begin Source File

SOURCE=.\Render\pgMaterial.h
# End Source File
# Begin Source File

SOURCE=.\Render\pgMesh.h
# End Source File
# Begin Source File

SOURCE=.\Render\pgMeshUtil.h
# End Source File
# Begin Source File

SOURCE=.\Render\pgParticleSystem.h
# End Source File
# Begin Source File

SOURCE=.\Render\pgSegment.h
# End Source File
# Begin Source File

SOURCE=.\Render\pgSkyBox.h
# End Source File
# Begin Source File

SOURCE=.\Render\pgTexture.h
# End Source File
# Begin Source File

SOURCE=.\Render\pgTextureStage.h
# End Source File
# Begin Source File

SOURCE=.\Render\pgTransformable.h
# End Source File
# Begin Source File

SOURCE=.\Render\pgXUtil.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\pgLib.h
# End Source File
# Begin Source File

SOURCE=.\Render\Terrain\pgTerrainPatchMorphSW.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\todo.txt
# End Source File
# Begin Source File

SOURCE=..\Libs\FMOD\Lib\fmodvc.lib
# End Source File
# Begin Source File

SOURCE=..\Libs\MSXML\lib\msxml2.lib
# End Source File
# End Target
# End Project
