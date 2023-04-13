# Microsoft Developer Studio Project File - Name="dow" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=dow - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dow.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dow.mak" CFG="dow - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dow - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "dow - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dow - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Ob1 /Gy /I "..\..\..\common\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /GF /c
# ADD CPP /nologo /W3 /GX /Ob1 /Gy /I "C:\DXSDK\Samples\C++\Common\Include" /I "C:\Program Files\NVIDIA Corporation\Cg\include" /I "C:\DXSDK\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /YX /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3dx9.lib d3d9.lib d3dxof.lib comctl32.lib winmm.lib dxguid.lib /nologo /stack:0x200000,0x200000 /subsystem:windows /machine:I386 /pdbtype:sept /libpath:"..\..\framework\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 d3dx9.lib vfw32.lib dxguid.lib d3d9.lib d3dxof.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib winmm.lib /nologo /stack:0x200000,0x200000 /subsystem:windows /incremental:yes /machine:I386 /out:"..\dow.exe" /pdbtype:sept /libpath:"C:\DXSDK\Lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "dow - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /I "..\..\..\common\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /GZ /c
# ADD CPP /nologo /W3 /GX /Zi /Od /I "C:\DXSDK\Samples\C++\Common\Include" /I "C:\Program Files\NVIDIA Corporation\Cg\include" /I "C:\DXSDK\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3dx9dt.lib d3d9.lib d3dxof.lib comctl32.lib winmm.lib dxguid.lib /nologo /stack:0x200000,0x200000 /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\..\framework\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 d3dx9dt.lib vfw32.lib dxguid.lib d3d9.lib d3dxof.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib winmm.lib /nologo /stack:0x200000,0x200000 /subsystem:windows /debug /machine:I386 /out:"..\dow.exe" /pdbtype:sept /libpath:"C:\DXSDK\Lib"
# SUBTRACT LINK32 /pdb:none /incremental:no

!ENDIF 

# Begin Target

# Name "dow - Win32 Release"
# Name "dow - Win32 Debug"
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=c:\common\directx.ico
# End Source File
# Begin Source File

SOURCE="c:\DXSDK\Samples\C++\Common\directx.ico"
# End Source File
# Begin Source File

SOURCE=.\dow.manifest
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\winmain.rc
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE="c:\DXSDK\Samples\C++\Common\Src\d3dapp.cpp"
NODEP_CPP_D3DAP=\
	"..\..\..\..\DXSDK\Include\D3D9.h"\
	"..\..\..\..\DXSDK\Include\d3d9caps.h"\
	"..\..\..\..\DXSDK\Include\d3d9types.h"\
	"..\..\..\..\DXSDK\Include\d3dx9.h"\
	"..\..\..\..\DXSDK\Include\d3dx9anim.h"\
	"..\..\..\..\DXSDK\Include\d3dx9core.h"\
	"..\..\..\..\DXSDK\Include\d3dx9effect.h"\
	"..\..\..\..\DXSDK\Include\D3DX9Math.h"\
	"..\..\..\..\DXSDK\Include\d3dx9math.inl"\
	"..\..\..\..\DXSDK\Include\d3dx9mesh.h"\
	"..\..\..\..\DXSDK\Include\d3dx9shader.h"\
	"..\..\..\..\DXSDK\Include\d3dx9shape.h"\
	"..\..\..\..\DXSDK\Include\d3dx9tex.h"\
	"..\..\..\..\DXSDK\Include\dxfile.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dapp.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3denumeration.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\D3DRes.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dsettings.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dutil.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\dxutil.h"\
	
# End Source File
# Begin Source File

SOURCE="c:\DXSDK\Samples\C++\Common\Include\d3dapp.h"
# End Source File
# Begin Source File

SOURCE="c:\DXSDK\Samples\C++\Common\Src\d3denumeration.cpp"
NODEP_CPP_D3DEN=\
	"..\..\..\..\DXSDK\Include\D3D9.h"\
	"..\..\..\..\DXSDK\Include\d3d9caps.h"\
	"..\..\..\..\DXSDK\Include\d3d9types.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3denumeration.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\dxutil.h"\
	
# End Source File
# Begin Source File

SOURCE="c:\DXSDK\Samples\C++\Common\Include\d3denumeration.h"
# End Source File
# Begin Source File

SOURCE="c:\DXSDK\Samples\C++\Common\Src\d3dfile.cpp"
NODEP_CPP_D3DFI=\
	"..\..\..\..\DXSDK\Include\D3D9.h"\
	"..\..\..\..\DXSDK\Include\d3d9caps.h"\
	"..\..\..\..\DXSDK\Include\d3d9types.h"\
	"..\..\..\..\DXSDK\Include\d3dx9.h"\
	"..\..\..\..\DXSDK\Include\d3dx9anim.h"\
	"..\..\..\..\DXSDK\Include\d3dx9core.h"\
	"..\..\..\..\DXSDK\Include\d3dx9effect.h"\
	"..\..\..\..\DXSDK\Include\D3DX9Math.h"\
	"..\..\..\..\DXSDK\Include\d3dx9math.inl"\
	"..\..\..\..\DXSDK\Include\d3dx9mesh.h"\
	"..\..\..\..\DXSDK\Include\d3dx9shader.h"\
	"..\..\..\..\DXSDK\Include\d3dx9shape.h"\
	"..\..\..\..\DXSDK\Include\d3dx9tex.h"\
	"..\..\..\..\DXSDK\Include\dxfile.h"\
	"..\..\..\..\DXSDK\Include\rmxfguid.h"\
	"..\..\..\..\DXSDK\Include\rmxftmpl.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dfile.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\dxutil.h"\
	
# End Source File
# Begin Source File

SOURCE="c:\DXSDK\Samples\C++\Common\Include\d3dfile.h"
# End Source File
# Begin Source File

SOURCE="c:\DXSDK\Samples\C++\Common\Src\d3dfont.cpp"
NODEP_CPP_D3DFO=\
	"..\..\..\..\DXSDK\Include\D3D9.h"\
	"..\..\..\..\DXSDK\Include\d3d9caps.h"\
	"..\..\..\..\DXSDK\Include\d3d9types.h"\
	"..\..\..\..\DXSDK\Include\d3dx9.h"\
	"..\..\..\..\DXSDK\Include\d3dx9anim.h"\
	"..\..\..\..\DXSDK\Include\d3dx9core.h"\
	"..\..\..\..\DXSDK\Include\d3dx9effect.h"\
	"..\..\..\..\DXSDK\Include\D3DX9Math.h"\
	"..\..\..\..\DXSDK\Include\d3dx9math.inl"\
	"..\..\..\..\DXSDK\Include\d3dx9mesh.h"\
	"..\..\..\..\DXSDK\Include\d3dx9shader.h"\
	"..\..\..\..\DXSDK\Include\d3dx9shape.h"\
	"..\..\..\..\DXSDK\Include\d3dx9tex.h"\
	"..\..\..\..\DXSDK\Include\dxfile.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dfont.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dutil.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\dxutil.h"\
	
# End Source File
# Begin Source File

SOURCE="c:\DXSDK\Samples\C++\Common\Include\d3dfont.h"
# End Source File
# Begin Source File

SOURCE="c:\DXSDK\Samples\C++\Common\Src\d3dsettings.cpp"
NODEP_CPP_D3DSE=\
	"..\..\..\..\DXSDK\Include\D3D9.h"\
	"..\..\..\..\DXSDK\Include\d3d9caps.h"\
	"..\..\..\..\DXSDK\Include\d3d9types.h"\
	"..\..\..\..\DXSDK\Include\d3dx9.h"\
	"..\..\..\..\DXSDK\Include\d3dx9anim.h"\
	"..\..\..\..\DXSDK\Include\d3dx9core.h"\
	"..\..\..\..\DXSDK\Include\d3dx9effect.h"\
	"..\..\..\..\DXSDK\Include\D3DX9Math.h"\
	"..\..\..\..\DXSDK\Include\d3dx9math.inl"\
	"..\..\..\..\DXSDK\Include\d3dx9mesh.h"\
	"..\..\..\..\DXSDK\Include\d3dx9shader.h"\
	"..\..\..\..\DXSDK\Include\d3dx9shape.h"\
	"..\..\..\..\DXSDK\Include\d3dx9tex.h"\
	"..\..\..\..\DXSDK\Include\dxfile.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3denumeration.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\D3DRes.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dsettings.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dutil.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\dxutil.h"\
	
# End Source File
# Begin Source File

SOURCE="c:\DXSDK\Samples\C++\Common\Include\d3dsettings.h"
# End Source File
# Begin Source File

SOURCE="c:\DXSDK\Samples\C++\Common\Src\d3dutil.cpp"
NODEP_CPP_D3DUT=\
	"..\..\..\..\DXSDK\Include\D3D9.h"\
	"..\..\..\..\DXSDK\Include\d3d9caps.h"\
	"..\..\..\..\DXSDK\Include\d3d9types.h"\
	"..\..\..\..\DXSDK\Include\d3dx9.h"\
	"..\..\..\..\DXSDK\Include\d3dx9anim.h"\
	"..\..\..\..\DXSDK\Include\d3dx9core.h"\
	"..\..\..\..\DXSDK\Include\d3dx9effect.h"\
	"..\..\..\..\DXSDK\Include\D3DX9Math.h"\
	"..\..\..\..\DXSDK\Include\d3dx9math.inl"\
	"..\..\..\..\DXSDK\Include\d3dx9mesh.h"\
	"..\..\..\..\DXSDK\Include\d3dx9shader.h"\
	"..\..\..\..\DXSDK\Include\d3dx9shape.h"\
	"..\..\..\..\DXSDK\Include\d3dx9tex.h"\
	"..\..\..\..\DXSDK\Include\dxfile.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dutil.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\dxutil.h"\
	
# End Source File
# Begin Source File

SOURCE="c:\DXSDK\Samples\C++\Common\Include\d3dutil.h"
# End Source File
# Begin Source File

SOURCE="c:\DXSDK\Samples\C++\Common\Src\dxutil.cpp"
NODEP_CPP_DXUTI=\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\dxutil.h"\
	
# End Source File
# Begin Source File

SOURCE="c:\DXSDK\Samples\C++\Common\Include\dxutil.h"
# End Source File
# End Group
# Begin Group "cg"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\..\fp_dow_layer.cg

!IF  "$(CFG)" == "dow - Win32 Release"

!ELSEIF  "$(CFG)" == "dow - Win32 Debug"

# Begin Custom Build
InputPath=.\..\fp_dow_layer.cg

"Debug/fp_dow_layer.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"C:\Program Files\NVIDIA Corporation\Cg\bin\cgc.exe" "$(InputPath)" -profile ps_2_x -o Debug/fp_dow_layer.txt

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\..\fp_dow_surface.cg

!IF  "$(CFG)" == "dow - Win32 Release"

!ELSEIF  "$(CFG)" == "dow - Win32 Debug"

# Begin Custom Build
InputPath=.\..\fp_dow_surface.cg

"Debug/fp_dow_surface.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"C:\Program Files\NVIDIA Corporation\Cg\bin\cgc.exe" "$(InputPath)" -profile ps_2_x -o Debug/fp_dow_surface.txt

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\..\fp_dow_wet_areas.cg

!IF  "$(CFG)" == "dow - Win32 Release"

!ELSEIF  "$(CFG)" == "dow - Win32 Debug"

# Begin Custom Build
InputPath=.\..\fp_dow_wet_areas.cg

"Debug/fp_dow_wet_areas.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"C:\Program Files\NVIDIA Corporation\Cg\bin\cgc.exe" "$(InputPath)" -profile ps_2_x -o Debug/fp_dow_wet_areas.txt

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\..\vp_dow.cg

!IF  "$(CFG)" == "dow - Win32 Release"

!ELSEIF  "$(CFG)" == "dow - Win32 Debug"

# Begin Custom Build
InputPath=.\..\vp_dow.cg

"Debug/vp_dow.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"C:\Program Files\NVIDIA Corporation\Cg\bin\cgc.exe" "$(InputPath)" -profile vs_2_x -o Debug/vp_dow.txt

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Tools"

# PROP Default_Filter ""
# Begin Source File

SOURCE=libtexture\d3dtex.h
# End Source File
# Begin Source File

SOURCE=c:\common\include\dxutil.h
# End Source File
# Begin Source File

SOURCE=.\MovieMaker.cpp
DEP_CPP_MOVIE=\
	".\MovieMaker.h"\
	
# End Source File
# Begin Source File

SOURCE=.\MovieMaker.h
# End Source File
# Begin Source File

SOURCE=.\ScreenCapture.cpp
DEP_CPP_SCREE=\
	".\ScreenCapture.h"\
	
# End Source File
# Begin Source File

SOURCE=.\ScreenCapture.h
# End Source File
# End Group
# Begin Group "Libs"

# PROP Default_Filter ""
# Begin Source File

SOURCE="c:\..\..\Program Files\NVIDIA Corporation\Cg\lib\cg.lib"
# End Source File
# Begin Source File

SOURCE="c:\..\..\Program Files\NVIDIA Corporation\Cg\lib\cgD3D9.lib"
# End Source File
# Begin Source File

SOURCE=.\libtexture\libtexture.lib
# End Source File
# End Group
# Begin Group "Animators"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CDARain.cpp
DEP_CPP_CDARA=\
	"..\..\..\..\DXSDK\Include\D3D9.h"\
	"..\..\..\..\DXSDK\Include\d3d9caps.h"\
	"..\..\..\..\DXSDK\Include\d3d9types.h"\
	"..\..\..\..\DXSDK\Include\d3dx9.h"\
	"..\..\..\..\DXSDK\Include\d3dx9anim.h"\
	"..\..\..\..\DXSDK\Include\d3dx9core.h"\
	"..\..\..\..\DXSDK\Include\d3dx9effect.h"\
	"..\..\..\..\DXSDK\Include\D3DX9Math.h"\
	"..\..\..\..\DXSDK\Include\d3dx9math.inl"\
	"..\..\..\..\DXSDK\Include\d3dx9mesh.h"\
	"..\..\..\..\DXSDK\Include\d3dx9shader.h"\
	"..\..\..\..\DXSDK\Include\d3dx9shape.h"\
	"..\..\..\..\DXSDK\Include\d3dx9tex.h"\
	"..\..\..\..\DXSDK\Include\dxfile.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dapp.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3denumeration.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dfile.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dfont.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dsettings.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dutil.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\dxutil.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\Cg.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_bindlocations.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_datatypes.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_deprecated_api.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_errors.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_profiles.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\CgD3D9.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cgGL_profiles.h"\
	".\CD3DCylinder.h"\
	".\CD3DQuad.h"\
	".\CDARain.h"\
	".\CDropAnimator.h"\
	".\dow.h"\
	".\drop.h"\
	".\libtexture\CException.h"\
	".\libtexture\CTexture.h"\
	".\libtexture\d3dtex.h"\
	".\MovieMaker.h"\
	
# End Source File
# Begin Source File

SOURCE=.\CDARain.h
# End Source File
# Begin Source File

SOURCE=.\CDAShower.cpp
DEP_CPP_CDASH=\
	"..\..\..\..\DXSDK\Include\D3D9.h"\
	"..\..\..\..\DXSDK\Include\d3d9caps.h"\
	"..\..\..\..\DXSDK\Include\d3d9types.h"\
	"..\..\..\..\DXSDK\Include\d3dx9.h"\
	"..\..\..\..\DXSDK\Include\d3dx9anim.h"\
	"..\..\..\..\DXSDK\Include\d3dx9core.h"\
	"..\..\..\..\DXSDK\Include\d3dx9effect.h"\
	"..\..\..\..\DXSDK\Include\D3DX9Math.h"\
	"..\..\..\..\DXSDK\Include\d3dx9math.inl"\
	"..\..\..\..\DXSDK\Include\d3dx9mesh.h"\
	"..\..\..\..\DXSDK\Include\d3dx9shader.h"\
	"..\..\..\..\DXSDK\Include\d3dx9shape.h"\
	"..\..\..\..\DXSDK\Include\d3dx9tex.h"\
	"..\..\..\..\DXSDK\Include\dxfile.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dapp.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3denumeration.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dfile.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dfont.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dsettings.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dutil.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\dxutil.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\Cg.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_bindlocations.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_datatypes.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_deprecated_api.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_errors.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_profiles.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\CgD3D9.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cgGL_profiles.h"\
	".\CD3DCylinder.h"\
	".\CD3DQuad.h"\
	".\CDAShower.h"\
	".\CDropAnimator.h"\
	".\dow.h"\
	".\drop.h"\
	".\libtexture\CException.h"\
	".\libtexture\CTexture.h"\
	".\libtexture\d3dtex.h"\
	".\MovieMaker.h"\
	
# End Source File
# Begin Source File

SOURCE=.\CDAShower.h
# End Source File
# Begin Source File

SOURCE=.\CDASplatch.cpp
DEP_CPP_CDASP=\
	"..\..\..\..\DXSDK\Include\D3D9.h"\
	"..\..\..\..\DXSDK\Include\d3d9caps.h"\
	"..\..\..\..\DXSDK\Include\d3d9types.h"\
	"..\..\..\..\DXSDK\Include\d3dx9.h"\
	"..\..\..\..\DXSDK\Include\d3dx9anim.h"\
	"..\..\..\..\DXSDK\Include\d3dx9core.h"\
	"..\..\..\..\DXSDK\Include\d3dx9effect.h"\
	"..\..\..\..\DXSDK\Include\D3DX9Math.h"\
	"..\..\..\..\DXSDK\Include\d3dx9math.inl"\
	"..\..\..\..\DXSDK\Include\d3dx9mesh.h"\
	"..\..\..\..\DXSDK\Include\d3dx9shader.h"\
	"..\..\..\..\DXSDK\Include\d3dx9shape.h"\
	"..\..\..\..\DXSDK\Include\d3dx9tex.h"\
	"..\..\..\..\DXSDK\Include\dxfile.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dapp.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3denumeration.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dfile.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dfont.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dsettings.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dutil.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\dxutil.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\Cg.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_bindlocations.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_datatypes.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_deprecated_api.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_errors.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_profiles.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\CgD3D9.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cgGL_profiles.h"\
	".\CD3DCylinder.h"\
	".\CD3DQuad.h"\
	".\CDASplatch.h"\
	".\CDropAnimator.h"\
	".\dow.h"\
	".\drop.h"\
	".\libtexture\CException.h"\
	".\libtexture\CTexture.h"\
	".\libtexture\d3dtex.h"\
	".\MovieMaker.h"\
	
# End Source File
# Begin Source File

SOURCE=.\CDASplatch.h
# End Source File
# Begin Source File

SOURCE=.\CDropAnimator.cpp
DEP_CPP_CDROP=\
	"..\..\..\..\DXSDK\Include\D3D9.h"\
	"..\..\..\..\DXSDK\Include\d3d9caps.h"\
	"..\..\..\..\DXSDK\Include\d3d9types.h"\
	"..\..\..\..\DXSDK\Include\d3dx9.h"\
	"..\..\..\..\DXSDK\Include\d3dx9anim.h"\
	"..\..\..\..\DXSDK\Include\d3dx9core.h"\
	"..\..\..\..\DXSDK\Include\d3dx9effect.h"\
	"..\..\..\..\DXSDK\Include\D3DX9Math.h"\
	"..\..\..\..\DXSDK\Include\d3dx9math.inl"\
	"..\..\..\..\DXSDK\Include\d3dx9mesh.h"\
	"..\..\..\..\DXSDK\Include\d3dx9shader.h"\
	"..\..\..\..\DXSDK\Include\d3dx9shape.h"\
	"..\..\..\..\DXSDK\Include\d3dx9tex.h"\
	"..\..\..\..\DXSDK\Include\dxfile.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dapp.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3denumeration.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dfile.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dfont.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dsettings.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dutil.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\dxutil.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\Cg.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_bindlocations.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_datatypes.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_deprecated_api.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_errors.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_profiles.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\CgD3D9.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cgGL_profiles.h"\
	".\CD3DCylinder.h"\
	".\CD3DQuad.h"\
	".\CDropAnimator.h"\
	".\dow.h"\
	".\drop.h"\
	".\libtexture\CException.h"\
	".\libtexture\CTexture.h"\
	".\libtexture\d3dtex.h"\
	".\MovieMaker.h"\
	
# End Source File
# Begin Source File

SOURCE=.\CDropAnimator.h
# End Source File
# End Group
# Begin Group "3d"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CD3DCylinder.cpp
DEP_CPP_CD3DC=\
	"..\..\..\..\DXSDK\Include\D3D9.h"\
	"..\..\..\..\DXSDK\Include\d3d9caps.h"\
	"..\..\..\..\DXSDK\Include\d3d9types.h"\
	"..\..\..\..\DXSDK\Include\d3dx9.h"\
	"..\..\..\..\DXSDK\Include\d3dx9anim.h"\
	"..\..\..\..\DXSDK\Include\d3dx9core.h"\
	"..\..\..\..\DXSDK\Include\d3dx9effect.h"\
	"..\..\..\..\DXSDK\Include\D3DX9Math.h"\
	"..\..\..\..\DXSDK\Include\d3dx9math.inl"\
	"..\..\..\..\DXSDK\Include\d3dx9mesh.h"\
	"..\..\..\..\DXSDK\Include\d3dx9shader.h"\
	"..\..\..\..\DXSDK\Include\d3dx9shape.h"\
	"..\..\..\..\DXSDK\Include\d3dx9tex.h"\
	"..\..\..\..\DXSDK\Include\dxfile.h"\
	".\CD3DCylinder.h"\
	
# End Source File
# Begin Source File

SOURCE=.\CD3DCylinder.h
# End Source File
# Begin Source File

SOURCE=.\CD3DQuad.cpp
DEP_CPP_CD3DQ=\
	"..\..\..\..\DXSDK\Include\D3D9.h"\
	"..\..\..\..\DXSDK\Include\d3d9caps.h"\
	"..\..\..\..\DXSDK\Include\d3d9types.h"\
	"..\..\..\..\DXSDK\Include\d3dx9.h"\
	"..\..\..\..\DXSDK\Include\d3dx9anim.h"\
	"..\..\..\..\DXSDK\Include\d3dx9core.h"\
	"..\..\..\..\DXSDK\Include\d3dx9effect.h"\
	"..\..\..\..\DXSDK\Include\D3DX9Math.h"\
	"..\..\..\..\DXSDK\Include\d3dx9math.inl"\
	"..\..\..\..\DXSDK\Include\d3dx9mesh.h"\
	"..\..\..\..\DXSDK\Include\d3dx9shader.h"\
	"..\..\..\..\DXSDK\Include\d3dx9shape.h"\
	"..\..\..\..\DXSDK\Include\d3dx9tex.h"\
	"..\..\..\..\DXSDK\Include\dxfile.h"\
	".\CD3DQuad.h"\
	
# End Source File
# Begin Source File

SOURCE=.\CD3DQuad.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\dow.cpp
DEP_CPP_DOW_C=\
	"..\..\..\..\DXSDK\Include\D3D9.h"\
	"..\..\..\..\DXSDK\Include\d3d9caps.h"\
	"..\..\..\..\DXSDK\Include\d3d9types.h"\
	"..\..\..\..\DXSDK\Include\d3dx9.h"\
	"..\..\..\..\DXSDK\Include\d3dx9anim.h"\
	"..\..\..\..\DXSDK\Include\d3dx9core.h"\
	"..\..\..\..\DXSDK\Include\d3dx9effect.h"\
	"..\..\..\..\DXSDK\Include\D3DX9Math.h"\
	"..\..\..\..\DXSDK\Include\d3dx9math.inl"\
	"..\..\..\..\DXSDK\Include\d3dx9mesh.h"\
	"..\..\..\..\DXSDK\Include\d3dx9shader.h"\
	"..\..\..\..\DXSDK\Include\d3dx9shape.h"\
	"..\..\..\..\DXSDK\Include\d3dx9tex.h"\
	"..\..\..\..\DXSDK\Include\dxfile.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dapp.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3denumeration.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dfile.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dfont.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dsettings.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\d3dutil.h"\
	"..\..\..\..\DXSDK\Samples\C++\Common\Include\dxutil.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\Cg.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_bindlocations.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_datatypes.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_deprecated_api.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_errors.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cg_profiles.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\CgD3D9.h"\
	"..\..\..\..\Program Files\NVIDIA Corporation\Cg\include\Cg\cgGL_profiles.h"\
	".\CD3DCylinder.h"\
	".\CD3DQuad.h"\
	".\CDARain.h"\
	".\CDAShower.h"\
	".\CDASplatch.h"\
	".\CDropAnimator.h"\
	".\dow.h"\
	".\drop.h"\
	".\libtexture\CException.h"\
	".\libtexture\CTexture.h"\
	".\libtexture\d3dtex.h"\
	".\MovieMaker.h"\
	
# End Source File
# Begin Source File

SOURCE=.\dow.h
# End Source File
# Begin Source File

SOURCE=.\drop.h
# End Source File
# End Target
# End Project
