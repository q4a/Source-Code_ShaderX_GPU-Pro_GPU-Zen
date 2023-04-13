@echo off

"..\Meshuggah Tools\zip.exe" -u -r -X data.zip InfiniteZoom Hair SoftObjects Beams DisplacementMap OceanScene VolumetricLight Shared Transitions Help Music -x *.scc
"..\Meshuggah Tools\Scramble.exe" data.zip data.cry