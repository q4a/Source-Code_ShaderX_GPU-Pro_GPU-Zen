# Microsoft Developer Studio Generated NMAKE File, Based on D3DCloth.dsp
!IF "$(CFG)" == ""
CFG=D3DCloth - Win32 Debug
!MESSAGE No configuration specified. Defaulting to D3DCloth - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "D3DCloth - Win32 Release" && "$(CFG)" != "D3DCloth - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "D3DCloth - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\D3DCloth.exe"


CLEAN :
	-@erase "$(INTDIR)\D3DCloth.obj"
	-@erase "$(INTDIR)\D3DCloth.res"
	-@erase "$(INTDIR)\D3DShell.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\D3DCloth.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\D3DCloth.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\D3DCloth.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\D3DCloth.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib D3d9.lib d3dx9.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\D3DCloth.pdb" /machine:I386 /out:"$(OUTDIR)\D3DCloth.exe" 
LINK32_OBJS= \
	"$(INTDIR)\D3DCloth.obj" \
	"$(INTDIR)\D3DShell.obj" \
	"$(INTDIR)\D3DCloth.res" \
	".\D3DTools.lib"

"$(OUTDIR)\D3DCloth.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "D3DCloth - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\D3DCloth.exe"


CLEAN :
	-@erase "$(INTDIR)\D3DCloth.obj"
	-@erase "$(INTDIR)\D3DCloth.res"
	-@erase "$(INTDIR)\D3DShell.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\D3DCloth.exe"
	-@erase "$(OUTDIR)\D3DCloth.ilk"
	-@erase "$(OUTDIR)\D3DCloth.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\D3DCloth.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\D3DCloth.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\D3DCloth.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib D3d9.lib d3dx9d.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\D3DCloth.pdb" /debug /machine:I386 /out:"$(OUTDIR)\D3DCloth.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\D3DCloth.obj" \
	"$(INTDIR)\D3DShell.obj" \
	"$(INTDIR)\D3DCloth.res" \
	".\D3DTools.lib"

"$(OUTDIR)\D3DCloth.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("D3DCloth.dep")
!INCLUDE "D3DCloth.dep"
!ELSE 
!MESSAGE Warning: cannot find "D3DCloth.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "D3DCloth - Win32 Release" || "$(CFG)" == "D3DCloth - Win32 Debug"
SOURCE=.\D3DCloth.cpp

"$(INTDIR)\D3DCloth.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\D3DCloth.rc

"$(INTDIR)\D3DCloth.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\D3DShell.cpp

"$(INTDIR)\D3DShell.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

