/*********************************************************************NVMH4****
Path:  NVSDK\Common\include\nv_fx
File:  nv_fx.h

Copyright NVIDIA Corporation 2002
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
*AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.



Comments:





******************************************************************************/

#ifndef __NV_FX_H
#define __NV_FX_H

#include "nv_graphics\nv_graphics.h"
#include "nv_renderdevice\nv_renderdevice.h"

#include "inveffects.h"

// {53DD20BC-A8F0-44dc-9C35-04B17A2AD467}
static const nv_sys::NVGUID CLSID_NVEffect_CgFX = 
{ 0x53dd20bc, 0xa8f0, 0x44dc, { 0x9c, 0x35, 0x4, 0xb1, 0x7a, 0x2a, 0xd4, 0x67 } };

// {05A2C789-A08D-41ad-8AB0-393F196B56A7}
static const nv_sys::NVGUID CLSID_NVEffectDevice_CgFX = 
{ 0x5a2c789, 0xa08d, 0x41ad, { 0x8a, 0xb0, 0x39, 0x3f, 0x19, 0x6b, 0x56, 0xa7 } };


#endif