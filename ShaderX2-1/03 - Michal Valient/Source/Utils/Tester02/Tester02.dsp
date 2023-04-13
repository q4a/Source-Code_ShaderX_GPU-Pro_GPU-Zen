# Microsoft Developer Studio Project File - Name="Tester02" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Tester02 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Tester02.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Tester02.mak" CFG="Tester02 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Tester02 - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Tester02 - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Tester02 - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x41b /d "NDEBUG"
# ADD RSC /l 0x41b /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "Tester02 - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W4 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x41b /d "_DEBUG"
# ADD RSC /l 0x41b /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /debug /machine:I386

!ENDIF 

# Begin Target

# Name "Tester02 - Win32 Release"
# Name "Tester02 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\BaseClass.cpp
# End Source File
# Begin Source File

SOURCE="..\..\Managers\DirectGraphicsManager\DG-DirectGraphicsDeviceInfo.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Engine\ENG-Base3DObject.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Engine\ENG-BaseScene.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Engine\ENG-BaseVisibilityDeterminer.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Engine\ENG-Engine.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Engine\ENG-Mesh.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Engine\ENG-MeshManager.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Engine\ENG-RenderSettings.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Engine\ENG-TestCube.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Engine\ENG-Texture2D.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Engine\ENG-TextureManager.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Engine\ENG-Types.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Engine\ENG-VirtualIndexBuffer.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Engine\ENG-VirtualVertexBuffer.cpp"
# End Source File
# Begin Source File

SOURCE=..\..\Resource\EngineResources.rc
# End Source File
# Begin Source File

SOURCE="..\..\Files\FIL-BaseDirectory.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Files\FIL-DirTreeDirectory.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Files\FIL-FileManager.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Managers\MGR-BaseManager.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Managers\DirectGraphicsManager\MGR-DirectGraphicsManager.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Managers\MGR-ErrorManager.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Managers\MGR-MemoryManager.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Engine\MeshLoaders\MSHIMP-ImportInterface.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\Engine\MeshLoaders\MSHIMP-MilkShape3D.cpp"
# End Source File
# Begin Source File

SOURCE=..\..\RootClass.cpp
# End Source File
# Begin Source File

SOURCE="..\..\SysClasses\SC-DynamicArray.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\SysClasses\SC-HashTable.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\SysClasses\SC-IniFile.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\SysClasses\SC-MString.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\SysClasses\SC-SyntaxScanner.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\SysClasses\SC-Timer.cpp"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE="..\..\SysUtils\SU-StrUtils.cpp"
# End Source File
# Begin Source File

SOURCE=.\Tester02.cpp
# End Source File
# Begin Source File

SOURCE="..\..\Managers\WindowManager\WND-WindowManager.cpp"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\Include\BaseClass.h
# End Source File
# Begin Source File

SOURCE="C:\Program Files\DirectX 8 SDK\include\d3d8.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\DirectX 8 SDK\include\d3d8types.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\DirectX 8 SDK\include\d3dx8.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\DirectX 8 SDK\include\d3dx8math.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\DG-DirectGraphicsDeviceInfo.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\ENG-Base3DObject.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\ENG-BaseScene.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\ENG-BaseVisibilityDeterminer.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\ENG-Engine.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\ENG-Mesh.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\ENG-MeshManager.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\ENG-RenderSettings.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\ENG-TestCube.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\ENG-Texture2D.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\ENG-TextureManager.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\ENG-Types.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\ENG-VirtualIndexBuffer.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\ENG-VirtualVertexBuffer.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\FIL-BaseDirectory.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\FIL-DirTreeDirectory.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\FIL-FileManager.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\Global.h
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\MGR-BaseManager.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\MGR-DirectGraphicsManager.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\MGR-ErrorManager.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\MGR-MemoryManager.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\MSHIMP-ImportInterface.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\MSHIMP-MilkShape3D.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\RootClass.h
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\SC-DynamicArray.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\SC-HashTable.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\SC-IniFile.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\SC-MString.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\SC-SyntaxScanner.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\SC-Timer.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\SU-StrUtils.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\Version.h
# End Source File
# Begin Source File

SOURCE="..\..\..\Include\WND-WindowManager.h"
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\Resource\DefWindowIcon.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft SDK\lib\WinMM.Lib"
# End Source File
# Begin Source File

SOURCE=C:\DXSDK\lib\d3dx8.lib
# End Source File
# Begin Source File

SOURCE=C:\DXSDK\lib\d3d8.lib
# End Source File
# End Target
# End Project
