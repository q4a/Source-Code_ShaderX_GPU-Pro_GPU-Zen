@echo off

"..\Meshuggah Tools\zip.exe" -u -r -X dlls.zip Bass -x *.scc
"..\Meshuggah Tools\Scramble.exe" dlls.zip dlls.cry