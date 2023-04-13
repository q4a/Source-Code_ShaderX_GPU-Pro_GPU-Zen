# Microsoft Developer Studio Generated NMAKE File, Based on ShaderX2.dsp
!IF "$(CFG)" == ""
CFG=ShaderX2 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ShaderX2 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ShaderX2 - Win32 Release" && "$(CFG)" != "ShaderX2 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ShaderX2.mak" CFG="ShaderX2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ShaderX2 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ShaderX2 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "ShaderX2 - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\ShaderX2.exe"


CLEAN :
	-@erase "$(INTDIR)\D3DShell.obj"
	-@erase "$(INTDIR)\ShaderX2MRT.obj"
	-@erase "$(INTDIR)\ShaderX2MRT.res"
	-@erase "$(INTDIR)\Shadow.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ShaderX2.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\ShaderX2.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\ShaderX2MRT.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ShaderX2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3d9.lib d3dx9.lib d3dtools.lib dinput8.lib dxguid.lib winmm.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\ShaderX2.pdb" /machine:I386 /out:"$(OUTDIR)\ShaderX2.exe" 
LINK32_OBJS= \
	"$(INTDIR)\D3DShell.obj" \
	"$(INTDIR)\ShaderX2MRT.obj" \
	"$(INTDIR)\Shadow.obj" \
	"$(INTDIR)\ShaderX2MRT.res"

"$(OUTDIR)\ShaderX2.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ShaderX2 - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\ShaderX2.exe"


CLEAN :
	-@erase "$(INTDIR)\D3DShell.obj"
	-@erase "$(INTDIR)\ShaderX2MRT.obj"
	-@erase "$(INTDIR)\ShaderX2MRT.res"
	-@erase "$(INTDIR)\Shadow.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ShaderX2.exe"
	-@erase "$(OUTDIR)\ShaderX2.ilk"
	-@erase "$(OUTDIR)\ShaderX2.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\ShaderX2.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\ShaderX2MRT.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ShaderX2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3d9.lib d3dx9d.lib d3dtools.lib dinput8.lib dxguid.lib winmm.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\ShaderX2.pdb" /debug /machine:I386 /out:"$(OUTDIR)\ShaderX2.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\D3DShell.obj" \
	"$(INTDIR)\ShaderX2MRT.obj" \
	"$(INTDIR)\Shadow.obj" \
	"$(INTDIR)\ShaderX2MRT.res"

"$(OUTDIR)\ShaderX2.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("ShaderX2.dep")
!INCLUDE "ShaderX2.dep"
!ELSE 
!MESSAGE Warning: cannot find "ShaderX2.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ShaderX2 - Win32 Release" || "$(CFG)" == "ShaderX2 - Win32 Debug"
SOURCE=.\D3DShell.cpp

"$(INTDIR)\D3DShell.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ShaderX2MRT.cpp

"$(INTDIR)\ShaderX2MRT.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Shadow.cpp

"$(INTDIR)\Shadow.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ShaderX2MRT.rc

"$(INTDIR)\ShaderX2MRT.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

