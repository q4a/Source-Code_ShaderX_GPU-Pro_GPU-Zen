@REM Test compile for D3D9 pixel shaders
@REM
@REM calls:
@REM    SDK\TOOLS\bin\release\D3D9SDKAsm\psa.exe
@REM which is the assembler from the D3D9 SDK
@REM
@REM
@REM  Commands:  ..\CompilePsh.bat  "$(InputPath)"   $(InputName)
@REM  Output:    $(InputName).vso
@REM
@REM  /Fc generates text listing file of instructions & hex opcodes

ECHO Input file:          [%1]
ECHO Output Listing file: [%2.vls]

..\..\..\..\TOOLS\bin\Release\D3D9SDKAsm\psa.exe  /Fc%2.pso.txt  %1
