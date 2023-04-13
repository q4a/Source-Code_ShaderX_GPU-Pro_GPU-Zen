# Microsoft Developer Studio Project File - Name="WaterDemo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=WaterDemo - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "WaterDemo.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "WaterDemo.mak" CFG="WaterDemo - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "WaterDemo - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "WaterDemo - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WaterDemo - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3dx8.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "WaterDemo - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3dx8.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "WaterDemo - Win32 Release"
# Name "WaterDemo - Win32 Debug"
# Begin Group "Code"

# PROP Default_Filter ""
# Begin Group "WinMain"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Code\WinMain\Frame.h
# End Source File
# Begin Source File

SOURCE=.\Code\WinMain\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\Code\WinMain\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Code\WinMain\WinMain.cpp
# End Source File
# End Group
# Begin Group "Demo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Code\Demo\Camera.cpp
# End Source File
# Begin Source File

SOURCE=.\Code\Demo\Camera.h
# End Source File
# Begin Source File

SOURCE=.\Code\Demo\DemoFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\Code\Demo\DemoFrame.h
# End Source File
# Begin Source File

SOURCE=.\Code\Demo\Moon.cpp
# End Source File
# Begin Source File

SOURCE=.\Code\Demo\Moon.h
# End Source File
# Begin Source File

SOURCE=.\Code\Demo\Skydome.cpp
# End Source File
# Begin Source File

SOURCE=.\Code\Demo\Skydome.h
# End Source File
# Begin Source File

SOURCE=.\Code\Demo\SkyObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Code\Demo\SkyObject.h
# End Source File
# Begin Source File

SOURCE=.\Code\Demo\Spline.h
# End Source File
# Begin Source File

SOURCE=.\Code\Demo\Sun.cpp
# End Source File
# Begin Source File

SOURCE=.\Code\Demo\Sun.h
# End Source File
# Begin Source File

SOURCE=.\Code\Demo\Terrain.cpp
# End Source File
# Begin Source File

SOURCE=.\Code\Demo\Terrain.h
# End Source File
# Begin Source File

SOURCE=.\Code\Demo\Water.cpp
# End Source File
# Begin Source File

SOURCE=.\Code\Demo\Water.h
# End Source File
# End Group
# End Group
# End Target
# End Project
