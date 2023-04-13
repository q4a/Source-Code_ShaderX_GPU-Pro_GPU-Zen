# Microsoft Developer Studio Project File - Name="SurfaceBasis" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SurfaceBasis - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SurfaceBasis.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SurfaceBasis.mak" CFG="SurfaceBasis - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SurfaceBasis - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "SurfaceBasis - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SurfaceBasis - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /ZI /W4 /Od /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Gm /Gy /GZ /c /GX 
# ADD CPP /nologo /MT /ZI /W4 /Od /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Gm /Gy /GZ /c /GX 
# ADD BASE MTL /nologo /win32 
# ADD MTL /nologo /win32 
# ADD BASE RSC /l 1033 
# ADD RSC /l 1033 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo 
# ADD BSC32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dsound.lib dinput8.lib dxerr9.lib d3dx9d.lib d3d9.lib d3dxof.lib dxguid.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /debug /pdbtype:sept /subsystem:windows 
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dsound.lib dinput8.lib dxerr9.lib d3dx9d.lib d3d9.lib d3dxof.lib dxguid.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /debug /pdbtype:sept /subsystem:windows 

!ELSEIF  "$(CFG)" == "SurfaceBasis - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /Zi /W4 /O2 /Ob1 /Oy /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /GF /Gy /c /GX 
# ADD CPP /nologo /MT /Zi /W4 /O2 /Ob1 /Oy /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /GF /Gy /c /GX 
# ADD BASE MTL /nologo /win32 
# ADD MTL /nologo /win32 
# ADD BASE RSC /l 1033 
# ADD RSC /l 1033 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo 
# ADD BSC32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dsound.lib dinput8.lib dxerr9.lib d3dx9.lib d3d9.lib d3dxof.lib dxguid.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /incremental:no /pdbtype:sept /subsystem:windows 
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dsound.lib dinput8.lib dxerr9.lib d3dx9.lib d3d9.lib d3dxof.lib dxguid.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /incremental:no /pdbtype:sept /subsystem:windows 

!ENDIF

# Begin Target

# Name "SurfaceBasis - Win32 Debug"
# Name "SurfaceBasis - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=DirectX.ico
# End Source File
# Begin Source File

SOURCE=SurfaceBasis.rc
# End Source File
# Begin Source File

SOURCE=d3dapp.cpp
# End Source File
# Begin Source File

SOURCE=d3dapp.h
# End Source File
# Begin Source File

SOURCE=d3denumeration.cpp
# End Source File
# Begin Source File

SOURCE=d3denumeration.h
# End Source File
# Begin Source File

SOURCE=d3dfile.cpp
# End Source File
# Begin Source File

SOURCE=d3dfile.h
# End Source File
# Begin Source File

SOURCE=d3dfont.cpp
# End Source File
# Begin Source File

SOURCE=d3dfont.h
# End Source File
# Begin Source File

SOURCE=d3dsettings.cpp
# End Source File
# Begin Source File

SOURCE=d3dsettings.h
# End Source File
# Begin Source File

SOURCE=d3dutil.cpp
# End Source File
# Begin Source File

SOURCE=d3dutil.h
# End Source File
# Begin Source File

SOURCE=dxutil.cpp
# End Source File
# Begin Source File

SOURCE=dxutil.h
# End Source File
# Begin Source File

SOURCE=resource.h
# End Source File
# End Group
# Begin Source File

SOURCE=ReadMe.txt
# End Source File
# Begin Source File

SOURCE=SurfaceBasis.cpp
# End Source File
# Begin Source File

SOURCE=SurfaceBasis.h
# End Source File
# Begin Source File

SOURCE=bary.fx
# End Source File
# Begin Source File

SOURCE=control.fx
# End Source File
# Begin Source File

SOURCE=linear.fx
# End Source File
# Begin Source File

SOURCE=npatch.fx
# End Source File
# End Target
# End Project

