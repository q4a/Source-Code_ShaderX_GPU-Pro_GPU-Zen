//-----------------------------------------------------------------------------
// File: TextureUtils.h
//
// Desc: 
//-----------------------------------------------------------------------------
#ifndef TEXTUREUTILS_H
#define TEXTUREUTILS_H
#include <tchar.h>
#include <d3d9.h>
#include <d3dx9.h>





//-----------------------------------------------------------------------------
// Name: class TextureUtils
// Desc: 
//-----------------------------------------------------------------------------
class TextureUtils
{
public:
	static LPDIRECT3DCUBETEXTURE9 GetNormalisationCubemap(LPDIRECT3DDEVICE9 pd3dDevice);
	static LPDIRECT3DCUBETEXTURE9 GetSpecularCubemap(LPDIRECT3DDEVICE9 pd3dDevice);

	static LPDIRECT3DTEXTURE9 CreateTextureAlpha(
									TCHAR* name, 
									TCHAR* alphaName, 
									LPDIRECT3DDEVICE9 pd3dDevice);

	static LPDIRECT3DTEXTURE9 CreateNormalMapAlpha(
									FLOAT bumpScale, 
									TCHAR* name, 
									TCHAR* alphaName, 
									LPDIRECT3DDEVICE9 pd3dDevice);

	static void Cleanup();

private:
    TextureUtils();
    ~TextureUtils();

	static LPDIRECT3DCUBETEXTURE9		m_pNormalisationCubemap;
	static LPDIRECT3DCUBETEXTURE9		m_pSpecularCubemap;
};

#endif



