//-----------------------------------------------------------------------------
// File: TextureUtils.cpp
//
// Desc: 
//-----------------------------------------------------------------------------
#include <tchar.h>
#include <stdio.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxfile.h>
#include "TextureUtils.h"
#include "DXUtil.h"


LPDIRECT3DCUBETEXTURE9 TextureUtils::m_pNormalisationCubemap = NULL;
LPDIRECT3DCUBETEXTURE9 TextureUtils::m_pSpecularCubemap = NULL;

VOID WINAPI FillNormalisationCubemap(
				D3DXVECTOR4* pOut, 
				CONST D3DXVECTOR3* pTexCoord, 
				CONST D3DXVECTOR3* pTexelSize, 
				LPVOID pData)
{
	D3DXVECTOR3 norm(pTexCoord->x, pTexCoord->y, pTexCoord->z);
	D3DXVec3Normalize(&norm, &norm);
	*pOut = D3DXVECTOR4(
				(norm.x + 1.0f) * 0.5f, 
				(norm.y + 1.0f) * 0.5f, 
				(norm.z + 1.0f) * 0.5f, 
				0.0f);
}

VOID WINAPI FillSpecularCubemap(
				D3DXVECTOR4* pOut, 
				CONST D3DXVECTOR3* pTexCoord, 
				CONST D3DXVECTOR3* pTexelSize, 
				LPVOID pData)
{
	static const FLOAT pow0 = 2.0f;
	static const FLOAT pow1 = 12.0f;
	static const FLOAT pow2 = 50.0f;
	static const FLOAT pow3 = 400.0f;
	static const D3DXVECTOR3 lightDir(0.0f, 1.0f, 0.0f);

	D3DXVECTOR3 dir(pTexCoord->x, pTexCoord->y, pTexCoord->z);
	D3DXVec3Normalize(&dir, &dir);

	FLOAT dot = D3DXVec3Dot(&dir, &lightDir);
	if (dot<0.0f) dot = 0.0f;

	*pOut = D3DXVECTOR4(
				powf(dot, pow0), 
				powf(dot, pow1), 
				powf(dot, pow2), 
				powf(dot, pow3));
}

//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
TextureUtils::TextureUtils()
{
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
TextureUtils::~TextureUtils()
{
}




LPDIRECT3DCUBETEXTURE9 TextureUtils::GetNormalisationCubemap(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if (m_pNormalisationCubemap == NULL)
	{
		// Need to generate it.
		D3DXCreateCubeTexture(pd3dDevice, 128, D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8,  D3DPOOL_MANAGED, &m_pNormalisationCubemap);
		D3DXFillCubeTexture(m_pNormalisationCubemap, &FillNormalisationCubemap,  NULL);
	}
	return m_pNormalisationCubemap;
}

LPDIRECT3DCUBETEXTURE9 TextureUtils::GetSpecularCubemap(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if (m_pSpecularCubemap == NULL)
	{
		// Need to generate it.
		D3DXCreateCubeTexture(pd3dDevice, 128, D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8,  D3DPOOL_MANAGED, &m_pSpecularCubemap);
		D3DXFillCubeTexture(m_pSpecularCubemap, &FillSpecularCubemap,  NULL);
	}
	return m_pSpecularCubemap;
}

LPDIRECT3DTEXTURE9 TextureUtils::CreateTextureAlpha(
										TCHAR* name, 
										TCHAR* alphaName, 
										LPDIRECT3DDEVICE9 pd3dDevice)
{
    HRESULT hr;
    TCHAR tStrPath[MAX_PATH];
    TCHAR aStrPath[MAX_PATH];

	LPDIRECT3DTEXTURE9 pTexture;
	LPDIRECT3DTEXTURE9 pAlphaTexture;

	D3DPOOL pool = D3DPOOL_SYSTEMMEM;
	DWORD filter = D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR;

    // Get the path to the texture
    if( FAILED( hr = DXUtil_FindMediaFileCb( tStrPath, sizeof(tStrPath), name ) ) )
        return NULL;

    // Get the path to the texture
    if( FAILED( hr = DXUtil_FindMediaFileCb( aStrPath, sizeof(aStrPath), alphaName ) ) )
        return NULL;

	D3DXIMAGE_INFO tInfo;
	D3DXIMAGE_INFO aInfo;
	D3DXGetImageInfoFromFile( tStrPath, &tInfo );
	D3DXGetImageInfoFromFile( aStrPath, &aInfo );

	// Work out size
	UINT width = tInfo.Width;
	UINT height = tInfo.Height;
	if (aInfo.Width > width) width = aInfo.Width;
	if (aInfo.Height > height) height = aInfo.Height;

    // Create the texture using D3DX
    D3DXCreateTextureFromFileEx( pd3dDevice, tStrPath, 
                width, height, D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8, 
                pool, filter, 
                D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0, NULL, NULL, &pTexture );

    // Create the texture using D3DX
    D3DXCreateTextureFromFileEx( pd3dDevice, aStrPath, 
                width, height, D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8, 
                pool, filter, 
                D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0, NULL, NULL, &pAlphaTexture );

	// Combine pTexture and pAlphaTexture
	LPDIRECT3DTEXTURE9 pCombined;
	D3DXCreateTexture( pd3dDevice, width, height, D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pCombined );

	D3DLOCKED_RECT tLRect;
	D3DLOCKED_RECT aLRect;
	D3DLOCKED_RECT cLRect;
	pTexture->LockRect(0, &tLRect, NULL, D3DLOCK_READONLY);
	pAlphaTexture->LockRect(0, &aLRect, NULL, D3DLOCK_READONLY);
	pCombined->LockRect(0, &cLRect, NULL, 0);

	// Now fill in cLRect with tLRect and aLRect
	BYTE* tData = (BYTE*)(tLRect.pBits);
	BYTE* aData = (BYTE*)(aLRect.pBits);
	BYTE* cData = (BYTE*)(cLRect.pBits);
	UINT u, v;
	for (v=0;v<height;v++)
	{
		for (u=0;u<width;u++)
		{
			BYTE* cPixel = cData + (v*cLRect.Pitch) + (u*4);
			BYTE* tPixel = tData + (v*tLRect.Pitch) + (u*4);
			BYTE* aPixel = aData + (v*aLRect.Pitch) + (u*4);
			cPixel[3] = aPixel[0];		// c.alpha = a.red
			cPixel[0] = tPixel[0];		// c.red = t.red
			cPixel[1] = tPixel[1];		// c.green = t.green
			cPixel[2] = tPixel[2];		// c.blue = t.blue
		}
	}

	pTexture->UnlockRect(0);
	pAlphaTexture->UnlockRect(0);
	pCombined->UnlockRect(0);

	SAFE_RELEASE(pTexture);
	SAFE_RELEASE(pAlphaTexture);

	D3DXFilterTexture(pCombined, NULL, D3DX_DEFAULT, D3DX_DEFAULT);

	return pCombined;
}

LPDIRECT3DTEXTURE9 TextureUtils::CreateNormalMapAlpha(
										FLOAT bumpScale, 
										TCHAR* name, 
										TCHAR* alphaName, 
										LPDIRECT3DDEVICE9 pd3dDevice)
{
    HRESULT hr;
    TCHAR tStrPath[MAX_PATH];
    TCHAR aStrPath[MAX_PATH];

	LPDIRECT3DTEXTURE9 pSrcTexture;
	LPDIRECT3DTEXTURE9 pTexture;
	LPDIRECT3DTEXTURE9 pAlphaTexture;

	D3DPOOL pool = D3DPOOL_SYSTEMMEM;
	DWORD filter = D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR|D3DX_FILTER_DITHER;
	//DWORD filter = D3DX_FILTER_LINEAR|D3DX_FILTER_MIRROR|D3DX_FILTER_DITHER;

    // Get the path to the texture
    if( FAILED( hr = DXUtil_FindMediaFileCb( tStrPath, sizeof(tStrPath), name ) ) )
        return NULL;

    // Get the path to the texture
    if( FAILED( hr = DXUtil_FindMediaFileCb( aStrPath, sizeof(aStrPath), alphaName ) ) )
        return NULL;

	D3DXIMAGE_INFO tInfo;
	D3DXIMAGE_INFO aInfo;
	D3DXGetImageInfoFromFile( tStrPath, &tInfo );
	D3DXGetImageInfoFromFile( aStrPath, &aInfo );

	// Work out size
	UINT width = tInfo.Width;
	UINT height = tInfo.Height;
	if (aInfo.Width > width) width = aInfo.Width;
	if (aInfo.Height > height) height = aInfo.Height;

    // Create the texture using D3DX
    D3DXCreateTextureFromFileEx( pd3dDevice, tStrPath, 
                width, height, D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8, 
                pool, filter, 
                D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0, NULL, NULL, &pSrcTexture );

	{
		if( FAILED( hr = D3DXCreateTexture( pd3dDevice, width, height, 
			pSrcTexture->GetLevelCount(), 0, 
			D3DFMT_A8R8G8B8, pool, &pTexture ) ) )
		{
			return NULL;
		}

		if( FAILED( hr = D3DXComputeNormalMap( pTexture, pSrcTexture,
			NULL, 0, D3DX_CHANNEL_RED, bumpScale ) ) )
		{
			return NULL;
		}

		SAFE_RELEASE(pSrcTexture);
	}

    // Create the texture using D3DX
    D3DXCreateTextureFromFileEx( pd3dDevice, aStrPath, 
                width, height, D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8, 
                pool, filter, 
                D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0, NULL, NULL, &pAlphaTexture );

	// Combine pTexture and pAlphaTexture
	LPDIRECT3DTEXTURE9 pCombined;
	D3DXCreateTexture( pd3dDevice, width, height, D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pCombined );

	D3DLOCKED_RECT tLRect;
	D3DLOCKED_RECT aLRect;
	D3DLOCKED_RECT cLRect;
	pTexture->LockRect(0, &tLRect, NULL, D3DLOCK_READONLY);
	pAlphaTexture->LockRect(0, &aLRect, NULL, D3DLOCK_READONLY);
	pCombined->LockRect(0, &cLRect, NULL, 0);

	// Now fill in cLRect with tLRect and aLRect
	BYTE* tData = (BYTE*)(tLRect.pBits);
	BYTE* aData = (BYTE*)(aLRect.pBits);
	BYTE* cData = (BYTE*)(cLRect.pBits);
	UINT u, v;
	for (v=0;v<height;v++)
	{
		for (u=0;u<width;u++)
		{
			BYTE* cPixel = cData + (v*cLRect.Pitch) + (u*4);
			BYTE* tPixel = tData + (v*tLRect.Pitch) + (u*4);
			BYTE* aPixel = aData + (v*aLRect.Pitch) + (u*4);
			cPixel[3] = aPixel[0];		// c.alpha = a.red
			cPixel[0] = tPixel[0];		// c.red = t.red
			cPixel[1] = tPixel[1];		// c.green = t.green
			cPixel[2] = tPixel[2];		// c.blue = t.blue
		}
	}

	pTexture->UnlockRect(0);
	pAlphaTexture->UnlockRect(0);
	pCombined->UnlockRect(0);

	SAFE_RELEASE(pTexture);
	SAFE_RELEASE(pAlphaTexture);

	D3DXFilterTexture(pCombined, NULL, D3DX_DEFAULT, D3DX_DEFAULT);

	return pCombined;
}

void TextureUtils::Cleanup()
{
	SAFE_RELEASE(m_pNormalisationCubemap);
	SAFE_RELEASE(m_pSpecularCubemap);
}