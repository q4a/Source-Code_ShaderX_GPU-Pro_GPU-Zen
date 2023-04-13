# Microsoft Developer Studio Project File - Name="RevExtShadows" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=RevExtShadows - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "RevExtShadows.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RevExtShadows.mak" CFG="RevExtShadows - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RevExtShadows - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "RevExtShadows - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "RevExtShadows - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x427 /d "NDEBUG"
# ADD RSC /l 0x427 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib d3d8.lib d3dx8.lib winmm.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "RevExtShadows - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x427 /d "_DEBUG"
# ADD RSC /l 0x427 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib d3d8.lib d3dx8.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "RevExtShadows - Win32 Release"
# Name "RevExtShadows - Win32 Debug"
# Begin Group "src"

# PROP Default_Filter "c;cpp;h"
# Begin Group "dxutils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\dxutils\d3dapp.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dxutils\d3dapp.h
# End Source File
# Begin Source File

SOURCE=.\src\dxutils\d3dfont.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dxutils\d3dfont.h
# End Source File
# Begin Source File

SOURCE=.\src\dxutils\d3dres.h
# End Source File
# Begin Source File

SOURCE=.\src\dxutils\d3dutil.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dxutils\d3dutil.h
# End Source File
# Begin Source File

SOURCE=.\src\dxutils\dxutil.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dxutils\dxutil.h
# End Source File
# End Group
# Begin Group "utils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\utils\Camera.cpp
# End Source File
# Begin Source File

SOURCE=.\src\utils\Camera.h
# End Source File
# Begin Source File

SOURCE=.\src\utils\DXHelpers.cpp
# End Source File
# Begin Source File

SOURCE=.\src\utils\DXHelpers.h
# End Source File
# Begin Source File

SOURCE=.\src\utils\MathUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\src\utils\MathUtil.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\DemoApp.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DemoApp.h
# End Source File
# Begin Source File

SOURCE=.\src\DXObject.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DXObject.h
# End Source File
# Begin Source File

SOURCE=.\src\main.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Shadows.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Shadows.h
# End Source File
# Begin Source File

SOURCE=.\src\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\src\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\src\Vertices.h
# End Source File
# End Group
# Begin Group "efx"

# PROP Default_Filter "sha;efx;fx"
# Begin Source File

SOURCE=.\efx\Caster.sha
# End Source File
# Begin Source File

SOURCE=.\efx\Receiver.sha
# End Source File
# End Group
# End Target
# End Project
