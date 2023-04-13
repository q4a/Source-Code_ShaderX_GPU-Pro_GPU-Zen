# Microsoft Developer Studio Project File - Name="FogPolygonVolumes" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=FogPolygonVolumes - Win32 Browser Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FogPolygonVolumes.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FogPolygonVolumes.mak" CFG="FogPolygonVolumes - Win32 Browser Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FogPolygonVolumes - Win32 Browser Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FogPolygonVolumes - Win32 Browser Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "FogPolygonVolumes"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FogPolygonVolumes - Win32 Browser Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FogPolygonVolumes_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "./" /I "../../inc" /I "../shared" /I "../../../../LIBS/inc" /I "../../../../LIBS/src" /I "../../../common/inc" /I "../../../common/inc/shared" /I "../../../../inc" /I "$(CG_INC_PATH)" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FogPolygonVolumes_EXPORTS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 d3dx9dt.lib nv_math.lib d3d9.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../../dll/release/D3D9_FogPolygonVolume.bfx" /libpath:"../../../../LIBS/lib/release" /libpath:"$(CG_LIB_PATH)" /libpath:"../../../../LIBS/implib/release"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=CopyDLLs.bat -r	ECHO	ECHO ***********************************************	ECHO This project code is in a bad state.	ECHO There is much to be cleaned up and renamed.	ECHO ***********************************************
# End Special Build Tool

!ELSEIF  "$(CFG)" == "FogPolygonVolumes - Win32 Browser Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FogPolygonVolumes_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./" /I "../../inc" /I "../shared" /I "../../../../LIBS/inc" /I "../../../../LIBS/src" /I "../../../common/inc" /I "../../../common/inc/shared" /I "../../../../inc" /I "$(CG_INC_PATH)" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FogPolygonVolumes_EXPORTS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 d3dx9dt.lib d3d9.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /debug /machine:I386 /out:"../../dll/debug/D3D9_FogPolygonVolume.bfx" /pdbtype:sept /libpath:"../../../../LIBS/lib/debug" /libpath:"$(CG_LIB_PATH)" /libpath:"../../../../LIBS/implib/debug"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=CopyDLLs.bat -d	ECHO	ECHO ***********************************************	ECHO This project code is in a bad state.	ECHO There is much to be cleaned up and renamed.	ECHO ***********************************************
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "FogPolygonVolumes - Win32 Browser Release"
# Name "FogPolygonVolumes - Win32 Browser Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\D3DDeviceAndHWInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\NVEffectsBrowser\eb_property.cpp
# End Source File
# Begin Source File

SOURCE=.\Effect_FogPolygonVolumes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\MatrixNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\NoiseGrid3D.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\inc\shared\NV_StringFuncs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\NVBModelLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\NVBScene9.cpp
# End Source File
# Begin Source File

SOURCE=.\PolygonalVolumeFog.cpp
# End Source File
# Begin Source File

SOURCE=.\PolygonalVolumeFogScene.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\RenderToTextureBase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\SceneFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\ShaderManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\ShadowVolumeObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\ShadowVolumeVBObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\SimpleObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\SimpleVBObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\TextureDisplay.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\WingedEdgeObject.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\Constants.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\D3DDeviceAndHWInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\inc\NVEffectsBrowser\eb_effect9.h
# End Source File
# Begin Source File

SOURCE=.\Effect_FogPolygonVolumes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\MatrixNode.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\shared\MouseUI9.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\NoiseGrid3D.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\inc\shared\NV_Common.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\inc\shared\NV_Error.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\inc\shared\NV_StringFuncs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\NVBModelLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\shared\NVBScene9.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\inc\NV_D3DCommon\NVD3DCommonSwitch.h
# End Source File
# Begin Source File

SOURCE=.\NVD3DVers.h
# End Source File
# Begin Source File

SOURCE=.\PolygonalVolumeFog.h
# End Source File
# Begin Source File

SOURCE=.\PolygonalVolumeFogObjects.h
# End Source File
# Begin Source File

SOURCE=.\PolygonalVolumeFogScene.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\RenderToTextureBase.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\SceneFactory.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\ShaderManager.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\ShadowVolumeObject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\ShadowVolumeVBObject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\SimpleObject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\SimpleObjectVertex.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\SimpleVBObject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\TextureDisplay.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\LIBS\src\NV_D3DCommon\WingedEdgeObject.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Vertex Shaders"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\DepthToTexcrd_ObjectExtruder.nvv

!IF  "$(CFG)" == "FogPolygonVolumes - Win32 Browser Release"

# Begin Custom Build
InputPath=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\DepthToTexcrd_ObjectExtruder.nvv
InputName=DepthToTexcrd_ObjectExtruder

"$(InputName).vso.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\CompileVsh.bat  "$(InputPath)"   $(InputName)

# End Custom Build

!ELSEIF  "$(CFG)" == "FogPolygonVolumes - Win32 Browser Debug"

# Begin Custom Build
InputPath=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\DepthToTexcrd_ObjectExtruder.nvv
InputName=DepthToTexcrd_ObjectExtruder

"$(InputName).vso.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\CompileVsh.bat  "$(InputPath)"   $(InputName)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\DepthToTexcrdForRGB.nvv

!IF  "$(CFG)" == "FogPolygonVolumes - Win32 Browser Release"

# Begin Custom Build
InputPath=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\DepthToTexcrdForRGB.nvv
InputName=DepthToTexcrdForRGB

"$(InputName).vso.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\CompileVsh.bat  "$(InputPath)"   $(InputName)

# End Custom Build

!ELSEIF  "$(CFG)" == "FogPolygonVolumes - Win32 Browser Debug"

# Begin Custom Build
InputPath=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\DepthToTexcrdForRGB.nvv
InputName=DepthToTexcrdForRGB

"$(InputName).vso.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\CompileVsh.bat  "$(InputPath)"   $(InputName)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\DepthToTexcrdForRGB_TC4Proj.nvv

!IF  "$(CFG)" == "FogPolygonVolumes - Win32 Browser Release"

# Begin Custom Build
InputPath=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\DepthToTexcrdForRGB_TC4Proj.nvv
InputName=DepthToTexcrdForRGB_TC4Proj

"$(InputName).vso.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\CompileVsh.bat  "$(InputPath)"   $(InputName)

# End Custom Build

!ELSEIF  "$(CFG)" == "FogPolygonVolumes - Win32 Browser Debug"

# Begin Custom Build
InputPath=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\DepthToTexcrdForRGB_TC4Proj.nvv
InputName=DepthToTexcrdForRGB_TC4Proj

"$(InputName).vso.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\CompileVsh.bat  "$(InputPath)"   $(InputName)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\DiffuseDirectional.nvv

!IF  "$(CFG)" == "FogPolygonVolumes - Win32 Browser Release"

# Begin Custom Build
InputPath=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\DiffuseDirectional.nvv
InputName=DiffuseDirectional

"$(InputName).vso.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\CompileVsh.bat  "$(InputPath)"   $(InputName)

# End Custom Build

!ELSEIF  "$(CFG)" == "FogPolygonVolumes - Win32 Browser Debug"

# Begin Custom Build
InputPath=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\DiffuseDirectional.nvv
InputName=DiffuseDirectional

"$(InputName).vso.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\CompileVsh.bat  "$(InputPath)"   $(InputName)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\RGBDifferenceToFogColor_11.nvv

!IF  "$(CFG)" == "FogPolygonVolumes - Win32 Browser Release"

# Begin Custom Build
InputPath=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\RGBDifferenceToFogColor_11.nvv
InputName=RGBDifferenceToFogColor_11

"$(InputName).vso.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\CompileVsh.bat  "$(InputPath)"   $(InputName)

# End Custom Build

!ELSEIF  "$(CFG)" == "FogPolygonVolumes - Win32 Browser Debug"

# Begin Custom Build
InputPath=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\RGBDifferenceToFogColor_11.nvv
InputName=RGBDifferenceToFogColor_11

"$(InputName).vso.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\CompileVsh.bat  "$(InputPath)"   $(InputName)

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Pixel Shaders"

# PROP Default_Filter ".nvp"
# Begin Source File

SOURCE=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\DepthToRGBAndCompare_13.nvp

!IF  "$(CFG)" == "FogPolygonVolumes - Win32 Browser Release"

# Begin Custom Build
InputPath=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\DepthToRGBAndCompare_13.nvp
InputName=DepthToRGBAndCompare_13

"$(InputName).pso.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\CompilePsh.bat  "$(InputPath)"   $(InputName)

# End Custom Build

!ELSEIF  "$(CFG)" == "FogPolygonVolumes - Win32 Browser Debug"

# Begin Custom Build
InputPath=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\DepthToRGBAndCompare_13.nvp
InputName=DepthToRGBAndCompare_13

"$(InputName).pso.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\CompilePsh.bat  "$(InputPath)"   $(InputName)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\DepthToRGBAndCompare_20.psh

!IF  "$(CFG)" == "FogPolygonVolumes - Win32 Browser Release"

# Begin Custom Build
InputPath=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\DepthToRGBAndCompare_20.psh
InputName=DepthToRGBAndCompare_20

"$(InputName).pso.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\CompilePsh.bat  "$(InputPath)"   $(InputName)

# End Custom Build

!ELSEIF  "$(CFG)" == "FogPolygonVolumes - Win32 Browser Debug"

# Begin Custom Build
InputPath=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\DepthToRGBAndCompare_20.psh
InputName=DepthToRGBAndCompare_20

"$(InputName).pso.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\CompilePsh.bat  "$(InputPath)"   $(InputName)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\DepthToRGBEncode.nvp

!IF  "$(CFG)" == "FogPolygonVolumes - Win32 Browser Release"

# Begin Custom Build
InputPath=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\DepthToRGBEncode.nvp
InputName=DepthToRGBEncode

"$(InputName).pso.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\CompilePsh.bat  "$(InputPath)"   $(InputName)

# End Custom Build

!ELSEIF  "$(CFG)" == "FogPolygonVolumes - Win32 Browser Debug"

# Begin Custom Build
InputPath=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\DepthToRGBEncode.nvp
InputName=DepthToRGBEncode

"$(InputName).pso.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\CompilePsh.bat  "$(InputPath)"   $(InputName)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\RGBDifferenceToFogColor_13.nvp

!IF  "$(CFG)" == "FogPolygonVolumes - Win32 Browser Release"

# Begin Custom Build
InputPath=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\RGBDifferenceToFogColor_13.nvp
InputName=RGBDifferenceToFogColor_13

"$(InputName).pso.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\CompilePsh.bat  "$(InputPath)"   $(InputName)

# End Custom Build

!ELSEIF  "$(CFG)" == "FogPolygonVolumes - Win32 Browser Debug"

# Begin Custom Build
InputPath=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\RGBDifferenceToFogColor_13.nvp
InputName=RGBDifferenceToFogColor_13

"$(InputName).pso.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\CompilePsh.bat  "$(InputPath)"   $(InputName)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\SubtractRGBAndGetFogColor_20.psh

!IF  "$(CFG)" == "FogPolygonVolumes - Win32 Browser Release"

# Begin Custom Build
InputPath=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\SubtractRGBAndGetFogColor_20.psh
InputName=SubtractRGBAndGetFogColor_20

"$(InputName).pso.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\CompilePsh.bat  "$(InputPath)"   $(InputName)

# End Custom Build

!ELSEIF  "$(CFG)" == "FogPolygonVolumes - Win32 Browser Debug"

# Begin Custom Build
InputPath=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\SubtractRGBAndGetFogColor_20.psh
InputName=SubtractRGBAndGetFogColor_20

"$(InputName).pso.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\CompilePsh.bat  "$(InputPath)"   $(InputName)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\SubtractRGBEncodedDepth_13.nvp

!IF  "$(CFG)" == "FogPolygonVolumes - Win32 Browser Release"

# Begin Custom Build
InputPath=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\SubtractRGBEncodedDepth_13.nvp
InputName=SubtractRGBEncodedDepth_13

"$(InputName).pso.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\CompilePsh.bat  "$(InputPath)"   $(InputName)

# End Custom Build

!ELSEIF  "$(CFG)" == "FogPolygonVolumes - Win32 Browser Debug"

# Begin Custom Build
InputPath=..\..\..\..\MEDIA\programs\D3D9_FogPolygonVolumes\SubtractRGBEncodedDepth_13.nvp
InputName=SubtractRGBEncodedDepth_13

"$(InputName).pso.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\CompilePsh.bat  "$(InputPath)"   $(InputName)

# End Custom Build

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
