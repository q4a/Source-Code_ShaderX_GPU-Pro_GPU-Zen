# Microsoft Developer Studio Project File - Name="PhongIllumination" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PhongIllumination - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PhongIllumination.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PhongIllumination.mak" CFG="PhongIllumination - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PhongIllumination - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "PhongIllumination - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PhongIllumination - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41d /d "NDEBUG"
# ADD RSC /l 0x41d /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc" /out:"PhongIllumination.exe"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "PhongIllumination - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41d /d "_DEBUG"
# ADD RSC /l 0x41d /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc" /out:"PhongIllumination.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "PhongIllumination - Win32 Release"
# Name "PhongIllumination - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Framework\Windows\ControlsDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\Framework\CPU.cpp
# End Source File
# Begin Source File

SOURCE=..\Framework\Util\DemoLooper.cpp
# End Source File
# Begin Source File

SOURCE=..\Framework\Windows\Direct3DApp.cpp
# End Source File
# Begin Source File

SOURCE=..\Framework\Direct3D\Direct3DRenderer.cpp
# End Source File
# Begin Source File

SOURCE=..\Framework\Direct3D\Direct3DRenderTarget.cpp
# End Source File
# Begin Source File

SOURCE=..\Framework\DisplayMode.cpp
# End Source File
# Begin Source File

SOURCE=..\Framework\Imaging\Image.cpp
# End Source File
# Begin Source File

SOURCE=..\Framework\Imaging\Image3D.cpp
# End Source File
# Begin Source File

SOURCE=.\Main.cpp
# End Source File
# Begin Source File

SOURCE=..\Framework\Math\Matrix.cpp
# End Source File
# Begin Source File

SOURCE=..\Framework\Math\Plane.cpp
# End Source File
# Begin Source File

SOURCE=..\Framework\Util\Polygon.cpp
# End Source File
# Begin Source File

SOURCE=..\Framework\Windows\RenderApp.cpp
# End Source File
# Begin Source File

SOURCE=..\Framework\Renderer.cpp
# End Source File
# Begin Source File

SOURCE=..\Framework\Windows\RenderWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\Framework\Windows\RenderWindowResources.rc
# End Source File
# Begin Source File

SOURCE=..\Framework\Util\String.cpp
# End Source File
# Begin Source File

SOURCE=..\Framework\Util\T3dLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\Framework\Util\Tokenizer.cpp
# End Source File
# Begin Source File

SOURCE=..\Framework\Math\Vertex.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Framework\Windows\ControlsDialog.h
# End Source File
# Begin Source File

SOURCE=..\Framework\CPU.h
# End Source File
# Begin Source File

SOURCE=..\Framework\Util\DemoLooper.h
# End Source File
# Begin Source File

SOURCE=..\Framework\Windows\Direct3DApp.h
# End Source File
# Begin Source File

SOURCE=..\Framework\Direct3D\Direct3DRenderer.h
# End Source File
# Begin Source File

SOURCE=..\Framework\Direct3D\Direct3DRenderTarget.h
# End Source File
# Begin Source File

SOURCE=..\Framework\DisplayMode.h
# End Source File
# Begin Source File

SOURCE=..\Framework\Framework.h
# End Source File
# Begin Source File

SOURCE=..\Framework\Imaging\Image.h
# End Source File
# Begin Source File

SOURCE=..\Framework\Imaging\Image3D.h
# End Source File
# Begin Source File

SOURCE=..\Framework\Math\Matrix.h
# End Source File
# Begin Source File

SOURCE=..\Framework\PixelFormat.h
# End Source File
# Begin Source File

SOURCE=..\Framework\Math\Plane.h
# End Source File
# Begin Source File

SOURCE=..\Framework\Platform.h
# End Source File
# Begin Source File

SOURCE=..\Framework\Util\Polygon.h
# End Source File
# Begin Source File

SOURCE=..\Framework\Windows\RenderApp.h
# End Source File
# Begin Source File

SOURCE=..\Framework\Renderer.h
# End Source File
# Begin Source File

SOURCE=..\Framework\RenderTarget.h
# End Source File
# Begin Source File

SOURCE=..\Framework\Windows\RenderWindow.h
# End Source File
# Begin Source File

SOURCE=..\Framework\Util\String.h
# End Source File
# Begin Source File

SOURCE=..\Framework\Util\T3dLoader.h
# End Source File
# Begin Source File

SOURCE=..\Framework\Util\Tokenizer.h
# End Source File
# Begin Source File

SOURCE=..\Framework\Math\Vertex.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\Framework\Windows\mainicon.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\distance.fsh
# End Source File
# Begin Source File

SOURCE=.\distance.vsh
# End Source File
# Begin Source File

SOURCE=.\lighting.fsh
# End Source File
# Begin Source File

SOURCE=.\lighting.vsh
# End Source File
# End Target
# End Project
