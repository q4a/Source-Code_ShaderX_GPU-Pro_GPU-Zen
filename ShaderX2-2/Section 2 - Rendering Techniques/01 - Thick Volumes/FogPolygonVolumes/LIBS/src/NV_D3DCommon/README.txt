

NV_D3DCommon

A set of D3D utility classes which can be compiled for either D3D8 or D3D9.

These include geometry classes for system memory geometry or D3D vertex buffer geometry, a shader manager for loading and compiling shaders, a base class that includes texture render targets, matrix tree and scene factory code, and other useful classes.


The choice of D3D8 or D3D9 is made by #defining either:

#define NV_USE_D3D8
or
#define NV_USE_D3D9

Including "NVD3DCommonSwitch.h" then selects the appropriate types and code.

Currently, the files #include "NVD3DVers.h"  The idea is for each project using the files to create its own version of this file and define either NV_USE_D3D8 or NV_USE_D3D9 in it.  This could be changed by instead  specifying the #defines in the project build settings. 


-GJ


