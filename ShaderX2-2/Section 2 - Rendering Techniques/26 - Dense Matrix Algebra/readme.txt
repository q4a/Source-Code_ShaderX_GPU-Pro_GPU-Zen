Linear Algebra on the GPU 

Implementation Program

created for ShaderX 2 by 
Adam Moravanszky
adam.moravanszky@novodex.com

v. 1.0 April 2003

Instructions
=========

This program is an interpreter for mathematics commands.  The math is 
performed on the GPU.  The commands are supplied in a script file 
specified on the command line.  (By dragging the  icon of the script 
file onto the executable icon in windows explorer.)

You can find the syntax for the script language documented in 
MatrixScriptInterpreter.cpp.  Some example scripts have been provided, 
they have the extension .mas.ods.  profile.mas.ods has the profiling 
code whose results for matrix size 1000 have been published in the 
paper. The load and save commands load/save matrices from plain text 
files compatible with MATLAB.  The files 9.mat and r16.mat are two 
examples.


Hardware Support
==============

* System requirements
	DirectX9 installed

* Reference Device 
The code works perfectly with the DirectX9 
reference rasterizer.  This device is only available if you have the 
DirectX9 SDK installed.  In this case you can control the use of this 
device in the control panel.

* ATI Radeon 9500 or better 
The public ATI drivers at the time of this 
writing still have a bug where the pixel shader clamps output to 
[0,1].  ATI has promised to fix this by the time this book is 
available.  Also, the Radeons we tested did not support 
D3DTADDRESS_BORDER.  Without support for this only matrices with rows 
and columns that are multiples of the MOPS (see article) will be 
multiplied correctly.  Thus you need to check what the MOPS are 
defined as for regular and transposed multiplication and pad your 
matrix.

* nVidia geForce FX 
Unfortunately the geForce FX does not appear to 
support 128 bit per pixel floating point pixelformats which this 
application depends on.

Compiling
=======

The source code builds on the DirectX9 appwizard generated code.   It 
has been tested with MS Visual C++ 6; a workspace is included.