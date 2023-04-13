# Microsoft Developer Studio Project File - Name="D3DCloth" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=D3DCloth - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "D3DCloth.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "D3DCloth.mak" CFG="D3DCloth - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "D3DCloth - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "D3DCloth - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "D3DCloth - Win32 Release"

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
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib D3d9.lib d3dx9.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "D3DCloth - Win32 Debug"

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
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib D3d9.lib d3dx9d.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "D3DCloth - Win32 Release"
# Name "D3DCloth - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\D3DCloth.cpp
# End Source File
# Begin Source File

SOURCE=.\D3DCloth.rc
# End Source File
# Begin Source File

SOURCE=.\D3DShell.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\D3DShell.h
# End Source File
# Begin Source File

SOURCE=.\D3DTools.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Scene.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\PowerVR.ico
# End Source File
# End Group
# Begin Group "Libraries"

# PROP Default_Filter "lib"
# Begin Source File

SOURCE=.\D3DTools.lib
# End Source File
# End Group
# Begin Group "Pixel Shaders"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Shaders\PSBasic.txt
# End Source File
# Begin Source File

SOURCE=".\Shaders\PSClothPhase-04OPT.txt"
# End Source File
# Begin Source File

SOURCE=".\Shaders\PSClothPhase-08OPT.txt"
# End Source File
# Begin Source File

SOURCE=".\Shaders\PSClothPhase-12OPT.txt"
# End Source File
# Begin Source File

SOURCE=".\Shaders\PSClothPhase-16OPT.txt"
# End Source File
# Begin Source File

SOURCE=".\Shaders\PSClothPhase-20OPT.txt"
# End Source File
# Begin Source File

SOURCE=".\Shaders\PSClothPhase-24OPT.txt"
# End Source File
# Begin Source File

SOURCE=.\Shaders\PSConstraintPhase.txt
# End Source File
# Begin Source File

SOURCE=.\Shaders\PSDebugDEPTH.txt
# End Source File
# Begin Source File

SOURCE=.\Shaders\PSDebugGEO.txt
# End Source File
# Begin Source File

SOURCE=.\Shaders\PSDebugNRM.txt
# End Source File
# Begin Source File

SOURCE=.\Shaders\PSDepthPhase.txt
# End Source File
# Begin Source File

SOURCE=.\Shaders\PSDisplayPhaseA.txt
# End Source File
# Begin Source File

SOURCE=.\Shaders\PSDisplayPhaseB.txt
# End Source File
# Begin Source File

SOURCE=.\Shaders\PSInitPhase.txt
# End Source File
# Begin Source File

SOURCE=.\Shaders\PSNormalMapPhase.txt
# End Source File
# End Group
# Begin Group "Vertex Shaders"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Shaders\VSBasic.txt
# End Source File
# Begin Source File

SOURCE=.\Shaders\VSClothPhase.txt
# End Source File
# Begin Source File

SOURCE=.\Shaders\VSConstraintPhase.txt
# End Source File
# Begin Source File

SOURCE=.\Shaders\VSDebug.txt
# End Source File
# Begin Source File

SOURCE=.\Shaders\VSDepthPhase.txt
# End Source File
# Begin Source File

SOURCE=.\Shaders\VSDisplayPhaseA.txt
# End Source File
# Begin Source File

SOURCE=.\Shaders\VSDisplayPhaseB.txt
# End Source File
# Begin Source File

SOURCE=.\Shaders\VSInitPhase.txt
# End Source File
# Begin Source File

SOURCE=.\Shaders\VSNormalMapPhase.txt
# End Source File
# End Group
# End Target
# End Project
