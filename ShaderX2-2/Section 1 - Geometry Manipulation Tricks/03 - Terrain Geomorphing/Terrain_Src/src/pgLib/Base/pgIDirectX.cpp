///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgIDirectX
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgIDirectX.cpp,v 1.10 2003/01/06 12:51:11 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgIDirectX.h"
#include <pgLib/Base/pgLog.h>
#include <pgLib/Base/pgList.h>
#include <pgLib/Math/pgMatrix.h>
#include <pgLib/Math/pgPlane.h>
#include <pgLib/Math/pgVec3.h>
#include <pgLib/Math/pgVec4.h>



static pgMatrix				viewMatrix;
static pgMatrix				cameraMatrix;
static pgMatrix				projMatrix;

static pgList<pgMatrix>		matrixStack;


static float				nearPlane = 100.0f,
							farPlane = 10000.0f;

static float				aspectRatio = 4.0f/3.0f;

static pgVec3				camPos;

static pgVec4				clearColor;
static float				clearZ = 1.0f;

static bool					updateVisibility = true;

static float				fov = D3DX_PI/4;

static int					maxTextureBlendStages = 0;
static bool					textureTempArgSupported = false;

static bool					disableWriteOnlyBuffers = false;
static pgIDirectX::POOLTYPE	d3dMemoryPool = pgIDirectX::POOL_DEFAULT;
static int					d3dFillType = D3DFILL_SOLID;
static int					d3dDynanimBuffer = D3DUSAGE_DYNAMIC;

static float				skyBoxHeightFactor = 0.015f;

static LPDIRECT3DDEVICE8	pd3dDevice = NULL;
static LPDIRECT3D8			pD3D = NULL;

static int					numLevelTris = 0;
static int					numTerrainTris = 0;
static int					numNonLevelTris = 0;
static int					numParticles = 0;



void pgIDirectX::init(LPDIRECT3DDEVICE8 nDevice, LPDIRECT3D8 nD3D)
{
	pd3dDevice = nDevice;
	assert(pd3dDevice);

	viewMatrix.makeIdent();
	pd3dDevice->SetTransform(D3DTS_VIEW, viewMatrix.getD3DMatrix());

	cameraMatrix.makeIdent();

	projMatrix.makeProjection(fov, aspectRatio, nearPlane, farPlane);
	pd3dDevice->SetTransform(D3DTS_PROJECTION, projMatrix.getD3DMatrix());

	pd3dDevice = nDevice;
	assert(pd3dDevice);

	pD3D = nD3D;
	assert(pD3D);

	D3DCAPS8 caps;
	if(pd3dDevice->GetDeviceCaps(&caps) == D3D_OK)
	{
		maxTextureBlendStages = caps.MaxTextureBlendStages;
		textureTempArgSupported = (caps.PrimitiveMiscCaps & D3DPMISCCAPS_TSSARGTEMP) != 0;
		pgString alphaStr;
		alphaStr += caps.AlphaCmpCaps&D3DPCMPCAPS_LESS ? "<, " : "";
		alphaStr += caps.AlphaCmpCaps&D3DPCMPCAPS_LESSEQUAL ? "<=, " : "";
		alphaStr += caps.AlphaCmpCaps&D3DPCMPCAPS_EQUAL ? "==, " : "";
		alphaStr += caps.AlphaCmpCaps&D3DPCMPCAPS_GREATEREQUAL ? ">=, " : "";
		alphaStr += caps.AlphaCmpCaps&D3DPCMPCAPS_GREATER ? ">, " : "";
		alphaStr += caps.AlphaCmpCaps&D3DPCMPCAPS_NOTEQUAL ? "!=" : "";

		pgLog::info("**********************************************************************");
		pgLog::info("");
		pgLog::info("     Video Card Information:");
		pgLog::info("");
		pgLog::info("                    execute buffers from system memory: %s", caps.DevCaps&D3DDEVCAPS_EXECUTESYSTEMMEMORY ? "yes" : "no");
		pgLog::info("                     execute buffers from video memory: %s", caps.DevCaps&D3DDEVCAPS_EXECUTEVIDEOMEMORY ? "yes" : "no");
		pgLog::info("                 execute TL buffers from system memory: %s", caps.DevCaps&D3DDEVCAPS_TLVERTEXSYSTEMMEMORY ? "yes" : "no");
		pgLog::info("                  execute TL buffers from video memory: %s", caps.DevCaps&D3DDEVCAPS_TLVERTEXVIDEOMEMORY ? "yes" : "no");
		pgLog::info("       max number of primitives per DrawPrimitive call: %d", caps.MaxPrimitiveCount);
		pgLog::info("                                 vertex shader version: %d.%d", (caps.VertexShaderVersion>>8)&0xff, caps.VertexShaderVersion&0xff);
		pgLog::info("            number of vertex shader constant registers: %d", caps.MaxVertexShaderConst);
		pgLog::info("                                  pixel shader version: %d.%d", (caps.PixelShaderVersion>>8)&0xff, caps.PixelShaderVersion&0xff);
		pgLog::info("             number of pixel shader constant registers: %d", caps.MaxPixelShaderValue);
		pgLog::info("");
		pgLog::info("                                   free texture memory: %d MB", getAvailableTextureMemory());
		pgLog::info("                              max texture blend stages: %d", maxTextureBlendStages);
		pgLog::info("                             max simultaneous textures: %d", caps.MaxSimultaneousTextures);
		pgLog::info("                              temp arg in renderstages: %s", textureTempArgSupported ? "yes" : "no");
		pgLog::info("                            texture from system memory: %s", caps.DevCaps&D3DDEVCAPS_TEXTURESYSTEMMEMORY ? "yes" : "no");
		pgLog::info("                             texture from video memory: %s", caps.DevCaps&D3DDEVCAPS_TEXTUREVIDEOMEMORY ? "yes" : "no");
		pgLog::info("                    texture from nonlocal video memory: %s", caps.DevCaps&D3DDEVCAPS_TEXTURENONLOCALVIDMEM ? "yes" : "no");
		pgLog::info("                    texture from separate memory pools: %s", caps.DevCaps&D3DDEVCAPS_SEPARATETEXTUREMEMORIES ? "yes" : "no");
		pgLog::info("  tex blt from system memory to non-local video memory: %s", caps.DevCaps&D3DDEVCAPS_CANBLTSYSTONONLOCAL ? "yes" : "no");
		pgLog::info("           power-of-2 texture (2d) dimensions required: %s", caps.TextureCaps&D3DPTEXTURECAPS_POW2 ? "yes" : "no");
		pgLog::info("                              square textures required: %s", caps.TextureCaps&D3DPTEXTURECAPS_SQUAREONLY ? "yes" : "no");
		pgLog::info("                     textures with alpha map supported: %s", caps.TextureCaps&D3DPTEXTURECAPS_ALPHA ? "yes" : "no");
		pgLog::info("                                 alpha tests supported: %s", alphaStr);
		pgLog::info("                                      cubemap textures: %s", caps.TextureCaps&D3DPTEXTURECAPS_CUBEMAP ? "yes" : "no");
		pgLog::info("                                       volume textures: %s", caps.TextureCaps&D3DPTEXTURECAPS_VOLUMEMAP ? "yes" : "no");
		pgLog::info("                can use compressed texture format DXT1: %s", canUseCompressedTextureFormat(pgImage::DXT1) ? "yes" : "no");
		pgLog::info("                can use compressed texture format DXT2: %s", canUseCompressedTextureFormat(pgImage::DXT2) ? "yes" : "no");
		pgLog::info("                can use compressed texture format DXT3: %s", canUseCompressedTextureFormat(pgImage::DXT3) ? "yes" : "no");
		pgLog::info("                can use compressed texture format DXT4: %s", canUseCompressedTextureFormat(pgImage::DXT4) ? "yes" : "no");
		pgLog::info("                can use compressed texture format DXT5: %s", canUseCompressedTextureFormat(pgImage::DXT5) ? "yes" : "no");
		pgLog::info("");
		pgLog::info("           render without waiting for flip to complete: %s", caps.DevCaps&D3DDEVCAPS_CANRENDERAFTERFLIP ? "yes" : "no");
		pgLog::info("                                       T&L in hardware: %s", caps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT ? "yes" : "no");
		pgLog::info("                          quintic beziers and bsplines: %s", caps.DevCaps&D3DDEVCAPS_QUINTICRTPATCHES ? "yes" : "no");
		pgLog::info("                                  rect and tri patches: %s", caps.DevCaps&D3DDEVCAPS_RTPATCHES ? "yes" : "no");
		pgLog::info("");
		pgLog::info("**********************************************************************");
		pgLog::info("");
	}
	else
		pgLog::error("retrieving video card information failed");
}


void
pgIDirectX::setClearColor(const pgColor& nColor)
{
	clearColor = nColor;
}

void
pgIDirectX::setClearZ(float nZ)
{
	clearZ = nZ;
}


pgVec4
pgIDirectX::getClearColor()
{
	return clearColor;
}


float
pgIDirectX::getClearZ()
{
	return clearZ;
}


bool
pgIDirectX::renderBegin(int nFlags)
{
	pd3dDevice->SetTransform(D3DTS_VIEW, viewMatrix.getD3DMatrix());

	DWORD	flags = 0,
			color = D3DCOLOR_RGBA((int)(255*clearColor[0]),
								  (int)(255*clearColor[1]),
								  (int)(255*clearColor[2]),
								  (int)(255*clearColor[3]));

	if(nFlags&CLEAR_Z)
		flags |= D3DCLEAR_ZBUFFER;

	if(nFlags&CLEAR_COLOR)
		flags |= D3DCLEAR_TARGET;

	pd3dDevice->Clear(0L, NULL, flags, color, clearZ, 0L);

    return !FAILED(pd3dDevice->BeginScene());
}

void
pgIDirectX::renderEnd()
{
	pd3dDevice->EndScene();
}


void
pgIDirectX::setProjectionMatrix(float nAspectRatio, float nFOV, float nNear, float nFar)
{
	aspectRatio = nAspectRatio;
	fov = nFOV;
	nearPlane = nNear;
	farPlane = nFar;
	projMatrix.makeProjection(fov, aspectRatio, nearPlane, farPlane);
	pd3dDevice->SetTransform(D3DTS_PROJECTION, projMatrix.getD3DMatrix());
}


void
pgIDirectX::setAspectRatio(float nAspectRatio)
{
	aspectRatio = nAspectRatio;
	projMatrix.makeProjection(fov, aspectRatio, nearPlane, farPlane);
	pd3dDevice->SetTransform(D3DTS_PROJECTION, projMatrix.getD3DMatrix());
}


float
pgIDirectX::getAspectRatio()
{
	return aspectRatio;
}


float
pgIDirectX::getFOVY()
{
	return fov;
}


float
pgIDirectX::getFOVX()
{
	return fov * aspectRatio;
}


float
pgIDirectX::getNearPlane()
{
	return nearPlane;
}


float
pgIDirectX::getFarPlane()
{
	return farPlane;
}


void
pgIDirectX::setViewMatrix(const pgMatrix& nMatrix)
{
	viewMatrix = nMatrix;
}


const pgMatrix&
pgIDirectX::getViewMatrix()
{
	return viewMatrix;
}


const D3DXMATRIX*
pgIDirectX::getD3DViewMatrix()
{
	return reinterpret_cast<D3DXMATRIX*>(&viewMatrix);
}


void
pgIDirectX::setCameraMatrix(const pgMatrix& nMatrix)
{
	cameraMatrix = nMatrix;
}


const pgMatrix&
pgIDirectX::getCameraMatrix()
{
	return cameraMatrix;
}


const pgMatrix&
pgIDirectX::getProjectionMatrix()
{
	return projMatrix;
}


void
pgIDirectX::setSkyBoxHeightFactor(float nFactor)
{
	skyBoxHeightFactor = nFactor;
}


float
pgIDirectX::getSkyBoxHeightFactor()
{
	return skyBoxHeightFactor;
}


void
pgIDirectX::switchFillOperation()
{
	if(d3dFillType==D3DFILL_POINT)
		d3dFillType = D3DFILL_WIREFRAME;
	else
	if(d3dFillType==D3DFILL_WIREFRAME)
		d3dFillType = D3DFILL_SOLID;
	else
		d3dFillType = D3DFILL_POINT;
}


int
pgIDirectX::getFillFlag()
{
	return d3dFillType;
}


bool
pgIDirectX::isFillSolid()
{
	return d3dFillType == D3DFILL_SOLID;
}

int
pgIDirectX::getCullFlag(int nCullOp)
{
	return d3dFillType == D3DFILL_SOLID ? nCullOp : D3DCULL_NONE;
}



void
pgIDirectX::setPoolType(POOLTYPE nPool)
{
	d3dMemoryPool = nPool;
}


D3DPOOL
pgIDirectX::getPoolType()
{
	return (D3DPOOL)d3dMemoryPool;
}


void
pgIDirectX::switchUpdateVisibility()
{
	updateVisibility = !updateVisibility;
}


bool
pgIDirectX::getUpdateVisibility()
{
	return updateVisibility;
}


void
pgIDirectX::setCameraPos(const pgVec3& nPos)
{
	camPos = nPos;
}


pgVec3&
pgIDirectX::getCameraPos()
{
	return camPos;
}



void
pgIDirectX::createClippingPlanes(pgPlane nPlanes[6])
{
	float nx,ny,nz;

	// near plane
	nPlanes[0].set(0.0f, 0.0f, -1.0f, -nearPlane);

	// far plane
	nPlanes[1].set(0.0f, 0.0f, 1.0f, farPlane);


	nx = (float)cos(getFOVX()/2.0f);
	nz = (float)sin(getFOVX()/2.0f);

	// right plane
	nPlanes[2].set(nx, 0.0f, -nz, 0.0f);

	// left plane
	nPlanes[3].set(-nx, 0.0f, -nz, 0.0f);


	ny = (float)cos(getFOVY()/2.0f);
	nz = (float)sin(getFOVY()/2.0f);

	// up plane
	nPlanes[4].set(0.0f, ny, -nz, 0.0f);

	// down plane
	nPlanes[5].set(0.0f, -ny, -nz, 0.0f);
}


LPDIRECT3DDEVICE8
pgIDirectX::getDevice()
{
	return pd3dDevice;
}


LPDIRECT3D8
pgIDirectX::getD3D()
{
	return pD3D;
}


void
pgIDirectX::setColor(D3DCOLORVALUE& nDst, const pgVec4& nSrc)
{
	nDst.r = nSrc[0];
	nDst.g = nSrc[1];
	nDst.b = nSrc[2];
	nDst.a = nSrc[3];
}


void
pgIDirectX::setD3DVecFromVec3(D3DVECTOR& nDst, const pgVec3& nSrc)
{
	nDst.x = nSrc.x;
	nDst.y = nSrc.y;
	nDst.z = nSrc.z;
}


void
pgIDirectX::setVec3FromVecD3D(pgVec3& nDst, const D3DVECTOR& nSrc)
{
	nDst.x = nSrc.x;
	nDst.y = nSrc.y;
	nDst.z = nSrc.z;
}


void
pgIDirectX::setVec3FromVecD3D(pgVec3& nDst, const D3DXVECTOR4& nSrc)
{
	nDst.x = nSrc.x;
	nDst.y = nSrc.y;
	nDst.z = nSrc.z;
}

void
pgIDirectX::resetStats()
{
	numLevelTris = numTerrainTris = numNonLevelTris = numParticles = 0;
}


void
pgIDirectX::addLevelTris(int nNum)
{
	numLevelTris += nNum;
}


void
pgIDirectX::addTerrainTris(int nNum)
{
	numTerrainTris += nNum;
}


void
pgIDirectX::addNonLevelTris(int nNum)
{
	numNonLevelTris += nNum;
}


void
pgIDirectX::addParticles(int nNum)
{
	numParticles += nNum;
}


int
pgIDirectX::getNumLevelTris()
{
	return numLevelTris;
}


int
pgIDirectX::getNumTerrainTris()
{
	return numTerrainTris;
}


int
pgIDirectX::getNumNonLevelTris()
{
	return numNonLevelTris;
}


int
pgIDirectX::getNumParticles()
{
	return numParticles;
}


int
pgIDirectX::getAvailableTextureMemory()
{
	return pd3dDevice->GetAvailableTextureMem()/(1024*1024);
}


int
pgIDirectX::getNumTextureBlendStages()
{
	D3DCAPS8 caps;
	if(pd3dDevice->GetDeviceCaps(&caps) != D3D_OK)
		return -1;

	return caps.MaxTextureBlendStages;
}


bool
pgIDirectX::canUseCompressedTextureFormat(pgImage::FORMAT nFormat)
{
    HRESULT hr = pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT,
                                          D3DDEVTYPE_HAL,
										  pgImage::getD3DFormat(nFormat),
                                          0,
                                          D3DRTYPE_TEXTURE,
                                          pgImage::getD3DFormat(nFormat));

    return SUCCEEDED(hr) == D3D_OK;
}


void
pgIDirectX::setDisableWriteOnlyBuffers(bool nSet)
{
	disableWriteOnlyBuffers = nSet;
}


bool
pgIDirectX::getDisableWriteOnlyBuffers()
{
	return disableWriteOnlyBuffers;
}


void
pgIDirectX::pushMatrix(const pgMatrix& nMatrix)
{
	matrixStack.addTail(nMatrix);
}


const pgMatrix&
pgIDirectX::popMatrix()
{
	const pgMatrix& mat = matrixStack.getTail();
	matrixStack.removeTail();
	return mat;	
}
