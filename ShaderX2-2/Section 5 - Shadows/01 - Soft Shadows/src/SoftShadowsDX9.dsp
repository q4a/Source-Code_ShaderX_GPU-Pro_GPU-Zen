# Microsoft Developer Studio Project File - Name="SoftShadowsDX9" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SoftShadowsDX9 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SoftShadowsDX9.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SoftShadowsDX9.mak" CFG="SoftShadowsDX9 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SoftShadowsDX9 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SoftShadowsDX9 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SoftShadowsDX9 - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x80c /d "NDEBUG"
# ADD RSC /l 0x80c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3d9.lib d3dx9.lib winmm.lib ijl15.lib /nologo /subsystem:windows /machine:I386 /out:"H:\PERSO1\PROJECTS\DX9-SHADOWS\SoftShadowsDX9.exe"

!ELSEIF  "$(CFG)" == "SoftShadowsDX9 - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x80c /d "_DEBUG"
# ADD RSC /l 0x80c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3d9.lib d3dx9.lib winmm.lib ijl15.lib /nologo /subsystem:windows /debug /machine:I386 /out:"D:\PERSO1\PROJECTS\DX9-SHADOWS\SoftShadowsDX9.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "SoftShadowsDX9 - Win32 Release"
# Name "SoftShadowsDX9 - Win32 Debug"
# Begin Source File

SOURCE=.\CASEFile.cpp
# End Source File
# Begin Source File

SOURCE=.\CASEFile.h
# End Source File
# Begin Source File

SOURCE=.\CEntity.cpp
# End Source File
# Begin Source File

SOURCE=.\CEntity.h
# End Source File
# Begin Source File

SOURCE=.\CObject.cpp
# End Source File
# Begin Source File

SOURCE=.\CObject.h
# End Source File
# Begin Source File

SOURCE=.\CObjMd3.cpp
# End Source File
# Begin Source File

SOURCE=.\CObjMd3.h
# End Source File
# Begin Source File

SOURCE=.\CScene.cpp
# End Source File
# Begin Source File

SOURCE=.\CScene.h
# End Source File
# Begin Source File

SOURCE=.\CSoftShadows.cpp
# End Source File
# Begin Source File

SOURCE=.\CSoftShadows.h
# End Source File
# Begin Source File

SOURCE=.\CTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\CTexture.h
# End Source File
# Begin Source File

SOURCE=.\CUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\CUtils.h
# End Source File
# Begin Source File

SOURCE=.\ijl.h
# End Source File
# End Target
# End Project
