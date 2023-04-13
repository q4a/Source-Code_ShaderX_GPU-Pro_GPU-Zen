@echo off

set PATH="C:\Programme\HTML Help Workshop";"%PATH%"

rem "================================================================================"
rem "delete old documentation"
rem "================================================================================"

rem "delete old documentation directory and re-create it"
rmdir /s /q Doc
mkdir Doc

rem "re-create temp directory"
rmdir /s /q Temp
mkdir Temp

rem "================================================================================"
rem "create new documentation"
rem "================================================================================"

rem "create new documentation"
doxygen Makefiles\Doxyfile_Meshuggah

rem "create compressed html help"
cd Temp\html
hhc index.hhp

rem "move help"
copy index.chm ..\..\Doc\MeshuggahSourceDoc.chm

cd ..\..

rem "remove and recreate temp directory"
rmdir /s /q Temp

rem "================================================================================"
rem "Done"
rem "================================================================================"

