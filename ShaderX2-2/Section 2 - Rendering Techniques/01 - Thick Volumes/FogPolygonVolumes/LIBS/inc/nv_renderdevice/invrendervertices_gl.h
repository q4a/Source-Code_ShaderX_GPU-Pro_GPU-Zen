/*********************************************************************NVMH4****
Path:  NVSDK\Common\include\nv_renderdevice
File:  invrendervertices_gl.h

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


Gl render vertices interface


******************************************************************************/

#ifndef __INVRENDERVERTICES_GL_H
#define __INVRENDERVERTICES_GL_H

namespace nv_renderdevice
{

class INVRenderVertices_GL : public INVRenderVertices
{
public:
	virtual bool INTCALLTYPE GetData(void** pData) = 0;
	virtual bool INTCALLTYPE SetData(void* pData) = 0;

};

}; // nv_renderdevice

// {D87DA5DB-7C95-4184-89D5-D819F4885871}
static const nv_sys::NVGUID IID_INVRenderVertices_GL = 
{ 0xd87da5db, 0x7c95, 0x4184, { 0x89, 0xd5, 0xd8, 0x19, 0xf4, 0x88, 0x58, 0x71 } };

#endif //__INVRENDERVERTICES_GL_H