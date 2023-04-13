'Advanced lighting and shading with Direct3D'

and

'Shadow mapping with Direct3D'

examples

by 

Michal Valient

e-mail: valient@host.sk
project pages: http://www.dimension3.host.sk

-------------------------------
Intro

This package contains examples for articles mentioned above.
Examples were tested on ATI Radeon 9700 and 8500 and DX9 reference rasterizer.
to compile, expat libraries are required - http://www.libexpat.org/
To compile MAX's .V2 exporter, Flexporter package is needed - http://www.codercorner.com/Flexporter.html

-------------------------------
Licence

This program is released under GPL licence. For details see file 'Licence'

-------------------------------
Command lines

Usage: app.exe -i <filename> -m <material> [-o <filename>] [-s <speed>]
Example: app.exe -m spotlight -s 3 -o spot

Parameters:
-i <filename> plays specified XML scene file
   ! do not specify any path, just filename and extension !

-m <material> sets specified material
   Materials are:
     spotlight      Phong lighting with spotlight - PS.1.4
     spotlight2     Phong lighting with spotlight - PS.2.0
     spotlight3     Phong lighting with spotlight - PS.3.0
     cooktorrance   Cook-Torrance lighting - PS.1.4
     cooktorrance2  Cook-Torrance lighting - PS.2.0
     orennayar      Oren-Nayar lighting - PS.2.0
     envmap         Environmental mapping - PS.1.4
     envmap2        Environmental mapping - PS.2.0
     shadow         Phong lighting with spotlight and shadows - PS.2.0

-o <filename> saves each screen into file '<filename>_nnnn.bmp'
   nnnn is frame number
   ! do not specify any path, just filename without extension!
-s <speed> alters playing speed - any positive number
   Speed modifier:
     1              original speed
     2              half speed
     n              1/n of original speed (n times slower)

-------------------------------
Package hierarchy:

./bin
	- precompiled examples
               app.exe is release build
               app_debug.exe is debug build and has extended output to log file.
	- Dimension3Exporter.flx - Dimension3 3DS MAX4 exporter

./bin/Resources/Shaders
	- DX shaders, usable with Direct3D9

./bin/Resources/Shaders/DX8_Shaders
        - Optimized Phong shaders for DX8 class of hardware

./bin/Resources/Shaders/Hlsl
        - HLSL versions of many shaders.

./bin/Resources/Required
        - Required textures and lookup tables

./bin/Resources/Scenes
        - Scenes for your pleasure

./Include 
./Source
        - source code of the renderer.

./Source/Utils/MaxExporter
        - MAX exporter source

./Source/Utils/MaxExporter/Flexporter
        - Here comes flexporter SDK

./Source/Utils/Tester02
        - Your place to start - an application executable project ;-)

