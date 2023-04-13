@REM  Argument 1:
@REM	-d      : copy debug libs
@REM	-r	: copy release libs
@echo off
@ECHO Executing CopyDLLs.bat for this project
SET FILES=(nv_nvb_mtdll.dll nv_core_mtdll.dll nv_sys_mtdll.dll )

if "%1" == "-d"  GOTO DEBUG
if "%1" == "-r"  GOTO RELEASE
echo HERE
echo Usage:  CopyLibs -d,-r
echo         -d  copies debug dlls
echo         -r  copies release dlls
GOTO END

:DEBUG
attrib ..\..\dll\debug\*.* -r
FOR %%a IN %FILES% DO copy ..\..\..\..\LIBS\dll\debug\%%a  ..\..\dll\debug\%%a

GOTO END
:RELEASE
attrib ..\..\dll\release\*.* -r
FOR %%a IN %FILES% DO copy ..\..\..\..\LIBS\dll\release\%%a  ..\..\dll\release\%%a

GOTO END
:END
 
 