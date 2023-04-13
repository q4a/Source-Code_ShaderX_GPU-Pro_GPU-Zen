                  /*--.          .-"-.
                 /   ._.        / ´ ` \
                 \_  (__\       \_°v°_/
                 //   \\        //   \\
                ((     ))      ((     ))
 ¤¤¤¤¤¤¤¤¤¤¤¤¤¤--""---""--¤¤¤¤--""---""--¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
 ¤                 |||            |||                                               ¤
 ¤  Author's note:  |              |                                                ¤
 ¤                                                                                  ¤
 ¤  This file is part of work done by Emil Persson, aka Humus.                      ¤
 ¤  You're free to use the code in any way you like, modified, unmodified           ¤
 ¤  or cut'n'pasted into your own work. But for the good of the whole               ¤
 ¤  programming community I will ask you to as long as possible and to the          ¤
 ¤  extent you find suitable to follow these points:                                ¤
 ¤   * If you use this file and it's contents unmodified, please leave this note.   ¤
 ¤   * If you use a major part of the file in your app, please credit the author(s) ¤
 ¤     in one way or the other as you find suitable, for instance a small notice    ¤
 ¤     in the readme file may do, or perhaps a link to the authors site.            ¤
 ¤   * Share your work and ideas too as much as you can.                            ¤
 ¤    _______                                                                       ¤
 ¤  /` _____ `\;,    Humus                                                          ¤
 ¤ (__(^===^)__)';,  emiper-8@student.luth.se                         ___           ¤
 ¤   /  :::  \   ,;  ICQ #47010716                                  /^   ^\         ¤
 ¤  |   :::   | ,;'                                                ( Ö   Ö )        ¤
 ¤¤¤'._______.'`¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤--°oOo--(_)--oOo°--¤¤*/

#include "Direct3DRenderTarget.h"

Direct3DRenderTarget::Direct3DRenderTarget(LPDIRECT3DDEVICE9 d3ddevice){
	dev = d3ddevice;
}

bool Direct3DRenderTarget::bind(){
	return true;
}

bool Direct3DRenderTarget::unbind(){
	return true;
}

bool Direct3DRenderTarget::create(const int w, const int h, const bool mipmapped, const bool cubemap, RenderPixelFormat rPixelFormat){
	width = w;
	height = h;

	D3DFORMAT format;

	switch(rPixelFormat){
	case RPF_RGBA8:
		format = D3DFMT_A8R8G8B8;
		break;
	case RPF_RGBA16:
		format = D3DFMT_A16B16G16R16;
		break;
	case RPF_R16f:
		format = D3DFMT_R16F;
		break;
	case RPF_RG16f:
		format = D3DFMT_G16R16F;
		break;
	case RPF_RGBA16f:
		format = D3DFMT_A16B16G16R16F;
		break;
	case RPF_R32f:
		format = D3DFMT_R32F;
		break;
	case RPF_RG32f:
		format = D3DFMT_G32R32F;
		break;
	case RPF_RGBA32f:
		format = D3DFMT_A32B32G32R32F;
		break;
	}

	if (isCubeMap = cubemap){
		if (dev->CreateCubeTexture(w, mipmapped? 0 : 1, D3DUSAGE_RENDERTARGET, format, D3DPOOL_DEFAULT, (LPDIRECT3DCUBETEXTURE9 *) &texID, NULL) != D3D_OK) return false;
		for (unsigned int i = 0; i < 6; i++){
			((LPDIRECT3DCUBETEXTURE9) texID)->GetCubeMapSurface((D3DCUBEMAP_FACES) i, 0, &colorSurface[i]);
		}
	} else {
		if (dev->CreateTexture(w, h, mipmapped? 0 : 1, D3DUSAGE_RENDERTARGET, format, D3DPOOL_DEFAULT, (LPDIRECT3DTEXTURE9 *) &texID, NULL) != D3D_OK) return false;
	    ((LPDIRECT3DTEXTURE9) texID)->GetSurfaceLevel(0, &colorSurface[0]);
	}

	dev->CreateDepthStencilSurface(w, h, D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0, TRUE, &depthSurface, NULL);


	return true;
}

bool Direct3DRenderTarget::destroy(){
	depthSurface->Release();

	unsigned int i, n = isCubeMap? 6 : 1;
	for (i = 0; i < n; i++) colorSurface[i]->Release();

	return true;
}

bool Direct3DRenderTarget::setCurrent(){
    if (!isCubeMap) dev->SetRenderTarget(0, colorSurface[0]);
	dev->SetDepthStencilSurface(depthSurface);

	return true;
}

bool Direct3DRenderTarget::unsetCurrent(){
	return true;
}

bool Direct3DRenderTarget::isLost(){
	return false;
}

bool Direct3DRenderTarget::setRenderFace(const int face){
	if (isCubeMap){
		dev->SetRenderTarget(0, colorSurface[face]);

		return true;
	}
	return false;
}


