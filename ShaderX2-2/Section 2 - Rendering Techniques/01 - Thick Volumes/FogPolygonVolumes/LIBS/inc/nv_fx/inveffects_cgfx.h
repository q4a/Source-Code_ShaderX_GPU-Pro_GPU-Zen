/*********************************************************************NVMH4****
Path:  NVSDK\Common\include\nv_fx
File:  inveffects.h

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

#ifndef __INVEFFECTS_CGFX_H
#define __INVEFFECTS_CGFX_H

struct ICgFXEffect;
namespace nv_fx
{

class INVEffectDevice_CgFX : public INVEffectDevice
{
public:
	// INVEffectDevice_CgFX

};


class INVEffect_CgFX : public INVEffect
{
public:
	// INVEffect_CgFX
	virtual bool INTCALLTYPE SetEffectCgFX(ICgFXEffect* pCgFXEffect) = 0;
};

}; // namespace nv_fx

// {1417C44B-1DE5-423c-B1FC-1B56235B1AD4}
static const nv_sys::NVGUID IID_INVEffectDevice_CgFX = 
{ 0x1417c44b, 0x1de5, 0x423c, { 0xb1, 0xfc, 0x1b, 0x56, 0x23, 0x5b, 0x1a, 0xd4 } };

// {C686CE7C-97F6-4f5c-AA33-946BBB781629}
static const nv_sys::NVGUID IID_INVEffect_CgFX = 
{ 0xc686ce7c, 0x97f6, 0x4f5c, { 0xaa, 0x33, 0x94, 0x6b, 0xbb, 0x78, 0x16, 0x29 } };

#endif // __INVEFFECTS_CGFX_H
