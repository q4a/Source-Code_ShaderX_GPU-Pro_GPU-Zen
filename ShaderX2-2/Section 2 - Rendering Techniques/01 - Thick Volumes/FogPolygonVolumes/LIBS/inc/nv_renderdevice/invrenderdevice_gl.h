/*********************************************************************NVMH4****
Path:  NVSDK\Common\include\nv_renderdevice
File:  invrenderdevice_d3d.h

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

INVRenderDevice_gl.


******************************************************************************/

#ifndef __INVRENDERDEVICE_GL_H
#define __INVRENDERDEVICE_GL_H

namespace nv_renderdevice
{

class INVRenderDevice_GL : public INVRenderDevice
{
public:
	// INVRenderDevice_GL
	virtual bool INTCALLTYPE GetPixelFormat(PIXELFORMATDESCRIPTOR* ppFD) = 0;
	virtual bool INTCALLTYPE GetDeviceContext(HGLRC* phglrc) = 0;
};

}; // nv_renderdevice namespace

// {6061CEAE-B5BA-4ade-B767-50F8018D71F1}
static const nv_sys::NVGUID IID_INVRenderDevice_GL = 
{ 0x6061ceae, 0xb5ba, 0x4ade, { 0xb7, 0x67, 0x50, 0xf8, 0x1, 0x8d, 0x71, 0xf1 } };

#endif //__INVRENDERDEVICE_GL_H