# Microsoft Developer Studio Project File - Name="IGN_shared" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=IGN_shared - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "IGN_shared.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "IGN_shared.mak" CFG="IGN_shared - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "IGN_shared - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "IGN_shared - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "IGN_shared - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "I:\Projects\Ignition\Temp\Release"
# PROP Intermediate_Dir "I:\Projects\Ignition\Temp\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"I:\Projects\Ignition\Binaries\Engine\Release/IGN_shared.lib"

!ELSEIF  "$(CFG)" == "IGN_shared - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "I:\Projects\Ignition\Temp\Debug"
# PROP Intermediate_Dir "I:\Projects\Ignition\Temp\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"I:\Projects\Ignition\Binaries\Engine\Debug/IGN_shared.lib"

!ENDIF 

# Begin Target

# Name "IGN_shared - Win32 Release"
# Name "IGN_shared - Win32 Debug"
# Begin Group "Code"

# PROP Default_Filter ""
# Begin Group "Types"

# PROP Default_Filter ""
# Begin Group "Lists"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Types\Lists\ign_pointerlist_shared.h
# End Source File
# Begin Source File

SOURCE=.\Types\Lists\ign_valuelist_shared.h
# End Source File
# End Group
# Begin Group "Math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Types\Math\ign_color_shared.h
# End Source File
# Begin Source File

SOURCE=.\Types\Math\ign_matrix_shared.h
# End Source File
# Begin Source File

SOURCE=.\Types\Math\ign_quaternion_shared.h
# End Source File
# Begin Source File

SOURCE=.\Types\Math\ign_vector_shared.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Types\ign_types_shared.h
# End Source File
# End Group
# Begin Group "Exceptions"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Exceptions\ign_exceptions.h
# End Source File
# Begin Source File

SOURCE=.\Exceptions\ign_exceptionsdummy.h
# End Source File
# End Group
# Begin Group "Modules"

# PROP Default_Filter ""
# Begin Group "Subsystems"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Modules\Subsystems\ign_graphic_sub_shared.h
# End Source File
# Begin Source File

SOURCE=.\Modules\Subsystems\ign_input_sub_shared.h
# End Source File
# Begin Source File

SOURCE=.\Modules\Subsystems\ign_timer_sub_shared.h
# End Source File
# End Group
# Begin Group "Supersystems"

# PROP Default_Filter ""
# End Group
# Begin Group "Systems"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Modules\Systems\ign_camera_sys_shared.h
# End Source File
# End Group
# End Group
# End Group
# End Target
# End Project
