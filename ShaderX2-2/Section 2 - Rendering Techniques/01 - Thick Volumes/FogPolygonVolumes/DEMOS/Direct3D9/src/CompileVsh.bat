@REM Test compile for D3D9 vertex shaders
@REM
@REM calls:
@REM    SDK\TOOLS\bin\release\D3D9SDKAsm\vsa.exe
@REM which is the assembler from the D3D9 SDK
@REM
@REM 
@REM   //////////////////////////////////
@REM   // OLD VERSION
@REM  ..\CompileVsh.bat  "$(InputDir)"   $(InputName)
@REM  $(InputDir)\$(InputName).vso
@REM  ECHO Compiling:  [%1\%2.vsh]
@REM  ECHO To:         [%1\%2.vso]
@REM  ..\..\..\..\TOOLS\bin\Release\D3D9SDKAsm\vsa.exe /Fo%1\%2.vso /Fc%1\%2.vls  %1\%2.vsh
@REM   //////////////////////////////////
@REM
@REM   //////////////////////////////////
@REM   // GOOD VERSION:
@REM
@REM  Commands:  ..\CompileVsh.bat  "$(InputPath)"   $(InputName)
@REM  Output:    $(InputName).vso
@REM
@REM  /Fc generates text listing file of instructions & hex opcodes

ECHO Input file:          [%1]
ECHO Output Listing file: [%2.vls]

..\..\..\..\TOOLS\bin\Release\D3D9SDKAsm\vsa.exe  /Fc%2.vso.txt  %1
