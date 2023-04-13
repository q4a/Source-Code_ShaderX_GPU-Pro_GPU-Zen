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


#include "Direct3DRenderer.h"
#include "Direct3DRenderTarget.h"

// Blending constants
const int ZERO                 = D3DBLEND_ZERO;
const int ONE                  = D3DBLEND_ONE;
const int SRC_COLOR            = D3DBLEND_SRCCOLOR;
const int ONE_MINUS_SRC_COLOR  = D3DBLEND_INVSRCCOLOR;
const int DST_COLOR            = D3DBLEND_DESTCOLOR;
const int ONE_MINUS_DST_COLOR  = D3DBLEND_INVDESTCOLOR;
const int SRC_ALPHA            = D3DBLEND_SRCALPHA;
const int ONE_MINUS_SRC_ALPHA  = D3DBLEND_INVSRCALPHA;
const int DST_ALPHA            = D3DBLEND_DESTALPHA;
const int ONE_MINUS_DST_ALPHA  = D3DBLEND_INVDESTALPHA;
const int SRC_ALPHA_SATURATE   = D3DBLEND_SRCALPHASAT;

const int BLENDMODE_ADD              = D3DBLENDOP_ADD;
const int BLENDMODE_SUBRACT          = D3DBLENDOP_SUBTRACT;
const int BLENDMODE_REVERSE_SUBTRACT = D3DBLENDOP_REVSUBTRACT;



Direct3DRenderer::Direct3DRenderer(const D3DCAPS9 &d3dcaps, const LPDIRECT3DDEVICE9 d3ddev) : Renderer(){
	dev = d3ddev;
	byteOrderRGBA = false;

	nSimultaneousTextures = d3dcaps.MaxSimultaneousTextures;
	maxAnisotropic = d3dcaps.MaxAnisotropy;

	resetToDefaults();

    dev->GetRenderTarget(0, &mainColorSurface);
    dev->GetDepthStencilSurface(&mainDepthSurface);
}

Direct3DRenderer::~Direct3DRenderer(){
    if (mainColorSurface != NULL) mainColorSurface->Release();
    if (mainDepthSurface != NULL) mainDepthSurface->Release();
}

void Direct3DRenderer::createTexture(Texture &texture){
	static D3DFORMAT internalFormats[] = 
	{	D3DFMT_UNKNOWN,
		D3DFMT_L8,
		D3DFMT_X8R8G8B8,
		D3DFMT_A8R8G8B8,
		D3DFMT_UNKNOWN,//GL_INTENSITY_FLOAT32_ATI,
		D3DFMT_UNKNOWN,//GL_RGB_FLOAT32_ATI,
		D3DFMT_UNKNOWN,//GL_RGBA_FLOAT32_ATI,
		D3DFMT_R5G6B5,
		D3DFMT_DXT1,
		D3DFMT_DXT3,
		D3DFMT_DXT5,
	};

	FORMAT format;
	/*if (isCompressedFormat(format) && !(GL_ARB_texture_compression && GL_EXT_texture_compression_s3tc)){
		img->uncompressImage();
		format = img->getFormat();
	}
	
	*/

	if (texture.isRenderTarget){
		texture.renderTarget = new Direct3DRenderTarget(dev);
	} else {

		switch(texture.textureKind){
		case TEXKIND_2D:
			format = texture.images[0]->getFormat();
			dev->CreateTexture(texture.images[0]->getWidth(), texture.images[0]->getHeight(), 0, 0, internalFormats[format], D3DPOOL_MANAGED, (LPDIRECT3DTEXTURE9 *) &texture.texID, NULL);
			break;
		case TEXKIND_CUBEMAP:
			break;
		case TEXKIND_3D:
			break;
		}
	}
}

void Direct3DRenderer::removeTexture(Texture &texture){
	if (texture.isRenderTarget){
		texture.renderTarget->destroy();
		delete texture.renderTarget;
	}
	((LPDIRECT3DTEXTURE9) texture.texID)->Release();
	
}

void Direct3DRenderer::resetTextureUnits(){
	for (int i = 0; i < nSimultaneousTextures; i++){
		dev->SetTexture(i, NULL);
	}
}

bool Direct3DRenderer::uploadTexture(Texture &texture){

	unsigned char *src;
	int nImages = 1;

	switch(texture.textureKind){
	case TEXKIND_2D:
		break;
	case TEXKIND_CUBEMAP:
		nImages = 6;
		break;
	case TEXKIND_3D:
		break;
	default:
		return false;
	}


	D3DLOCKED_RECT rect;

	if (texture.textureKind == TEXKIND_3D){

	} else {

		for (int i = 0; i < nImages; i++){
			Image *img = texture.images[i];
			FORMAT format = img->getFormat();
			if (format == FORMAT_RGB8) img->convert(FORMAT_RGBA8);
			
			if (colorMipMaps) img->colorMipMaps();

			int mipMapLevel = 0;
			while ((src = img->getImage(mipMapLevel)) != NULL){

				((LPDIRECT3DTEXTURE9) texture.texID)->LockRect(mipMapLevel, &rect, NULL, 0);
				memcpy(rect.pBits, src, img->getMipMappedSize(mipMapLevel, 1));
				((LPDIRECT3DTEXTURE9) texture.texID)->UnlockRect(mipMapLevel);

				mipMapLevel++;
			}
		}
	}

	return true;
}

void Direct3DRenderer::createFragmentShader(FragmentShader &fragmentShader){
}

void Direct3DRenderer::removeFragmentShader(FragmentShader &fragmentShader){
	((LPDIRECT3DPIXELSHADER9) fragmentShader.shaderID)->Release();
}

void Direct3DRenderer::uploadFragmentShader(FragmentShader &fragmentShader){
	LPD3DXBUFFER shader = NULL, errors = NULL;

	fragmentShader.shaderID = SHADER_UNDEFINED;
	if (D3DXAssembleShader(fragmentShader.program, fragmentShader.length, NULL, NULL, D3DXSHADER_DEBUG, &shader, &errors) == D3D_OK){
		dev->CreatePixelShader((DWORD *) shader->GetBufferPointer(), (LPDIRECT3DPIXELSHADER9 *) &fragmentShader.shaderID);
	} else {
		MessageBox(NULL, (const char *) errors->GetBufferPointer(), "Fragment shader error", MB_OK);
	}

	if (shader != NULL) shader->Release();
	if (errors != NULL) errors->Release();
}

void Direct3DRenderer::createVertexShader(VertexShader &vertexShader){
}

void Direct3DRenderer::removeVertexShader(VertexShader &vertexShader){
	((LPDIRECT3DVERTEXSHADER9) vertexShader.shaderID)->Release();
}

void Direct3DRenderer::uploadVertexShader(VertexShader &vertexShader){
	LPD3DXBUFFER shader = NULL, errors = NULL;

	vertexShader.shaderID = SHADER_UNDEFINED;
	if (D3DXAssembleShader(vertexShader.program, vertexShader.length, NULL, NULL, D3DXSHADER_DEBUG, &shader, &errors) == D3D_OK){
		dev->CreateVertexShader((DWORD *) shader->GetBufferPointer(), (LPDIRECT3DVERTEXSHADER9 *) &vertexShader.shaderID);
	} else {
		MessageBox(NULL, (const char *) errors->GetBufferPointer(), "Vertex shader error", MB_OK);
	}

	if (shader != NULL) shader->Release();
	if (errors != NULL) errors->Release();
}

void Direct3DRenderer::changeTextureUnit(const int textureUnit){
	currentTextureUnit = textureUnit;
}

void Direct3DRenderer::changeTexture(const TextureID textureID){
	TextureID currTex = currentTextures[currentTextureUnit];

	if (textureID != currTex){
		if (textureID == TEXTURE_NONE){
			dev->SetTexture(currentTextureUnit, NULL);
		} else {
			dev->SetTexture(currentTextureUnit, (LPDIRECT3DTEXTURE9) (textures[textureID].texID));

			dev->SetSamplerState(currentTextureUnit, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
			if (textures[textureID].levelOfAnisotropy > 1){
				dev->SetSamplerState(currentTextureUnit, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
				dev->SetSamplerState(currentTextureUnit, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
				dev->SetSamplerState(currentTextureUnit, D3DSAMP_MAXANISOTROPY, textures[textureID].levelOfAnisotropy);
			} else {
				dev->SetSamplerState(currentTextureUnit, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
				dev->SetSamplerState(currentTextureUnit, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			}

			dev->SetSamplerState(currentTextureUnit, D3DSAMP_ADDRESSU, textures[textureID].clampS? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP);
			dev->SetSamplerState(currentTextureUnit, D3DSAMP_ADDRESSV, textures[textureID].clampT? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP);
			if (textures[textureID].textureKind != TEXKIND_2D){
				dev->SetSamplerState(currentTextureUnit, D3DSAMP_ADDRESSW, textures[textureID].clampR? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP);
			}
		}
		currentTextures[currentTextureUnit] = textureID;
	}
}

void Direct3DRenderer::configureTexEnv(const TexEnv texEnv, bool isRGB){
	D3DTEXTURESTAGESTATETYPE combineMode, source0, source1, source2;
	if (isRGB){
		combineMode = D3DTSS_COLOROP;
		source0     = D3DTSS_COLORARG1;
		source1     = D3DTSS_COLORARG2;
		source2     = D3DTSS_COLORARG0;
	} else {
		combineMode = D3DTSS_ALPHAOP;
		source0     = D3DTSS_ALPHAARG1;
		source1     = D3DTSS_ALPHAARG2;
		source2     = D3DTSS_ALPHAARG0;
	}

	switch(texEnv){
	case TEXENV_MODULATE:
		dev->SetTextureStageState(currentTextureUnit, combineMode, D3DTOP_MODULATE);
		dev->SetTextureStageState(currentTextureUnit, source0,     D3DTA_CURRENT);
		dev->SetTextureStageState(currentTextureUnit, source1,     D3DTA_TEXTURE);
		break;
	case TEXENV_DOT3:
		if (isRGB){
			dev->SetTextureStageState(currentTextureUnit, combineMode, D3DTOP_DOTPRODUCT3);
			dev->SetTextureStageState(currentTextureUnit, source0,     D3DTA_CURRENT);
			dev->SetTextureStageState(currentTextureUnit, source1,     D3DTA_TEXTURE);
		}
		break;
/*	case TEXENV_DOT3_NEXT:
		if (isRGB){
			glTexEnvi(GL_TEXTURE_ENV, combineMode, GL_DOT3_RGBA_ARB);
			
			glTexEnvi(GL_TEXTURE_ENV, source0,  GL_TEXTURE0_ARB + currentTextureUnit);
			glTexEnvi(GL_TEXTURE_ENV, operand0, srcColor);
			
			glTexEnvi(GL_TEXTURE_ENV, source1,  GL_TEXTURE0_ARB + currentTextureUnit + 1);
			glTexEnvi(GL_TEXTURE_ENV, operand1, srcColor);
		}
		break;
		*/
	case TEXENV_ADD:
		dev->SetTextureStageState(currentTextureUnit, combineMode, D3DTOP_ADD);
		dev->SetTextureStageState(currentTextureUnit, source0,     D3DTA_CURRENT);
		dev->SetTextureStageState(currentTextureUnit, source1,     D3DTA_TEXTURE);
		break;
	case TEXENV_REPLACE:
		dev->SetTextureStageState(currentTextureUnit, combineMode, D3DTOP_SELECTARG1);
		dev->SetTextureStageState(currentTextureUnit, source0,     D3DTA_TEXTURE);
		break;
	case TEXENV_PASSTHRU:
		dev->SetTextureStageState(currentTextureUnit, combineMode, D3DTOP_SELECTARG1);
		dev->SetTextureStageState(currentTextureUnit, source0,     D3DTA_CURRENT);
		break;
	case TEXENV_INTERPOLATE:
		dev->SetTextureStageState(currentTextureUnit, combineMode, D3DTOP_LERP);
		dev->SetTextureStageState(currentTextureUnit, source0,     D3DTA_TEXTURE);
		dev->SetTextureStageState(currentTextureUnit, source1,     D3DTA_CURRENT);
		dev->SetTextureStageState(currentTextureUnit, source2,     D3DTA_DIFFUSE);
		break;
	case TEXENV_INTERPOLATE_PRIMARY:
		dev->SetTextureStageState(currentTextureUnit, combineMode, D3DTOP_LERP);
		dev->SetTextureStageState(currentTextureUnit, source0,     D3DTA_DIFFUSE);
		dev->SetTextureStageState(currentTextureUnit, source1,     D3DTA_CURRENT);
		dev->SetTextureStageState(currentTextureUnit, source2,     D3DTA_TEXTURE);
		break;
	case TEXENV_INTERPOLATE_TEX_ALPHA:
		dev->SetTextureStageState(currentTextureUnit, combineMode, D3DTOP_BLENDTEXTUREALPHA);
		dev->SetTextureStageState(currentTextureUnit, source0,     D3DTA_CURRENT);
		dev->SetTextureStageState(currentTextureUnit, source1,     D3DTA_TEXTURE);
		break;
	default:
		break;
	}

}

void Direct3DRenderer::changeTexEnv(const TexEnv colorTexEnv, const TexEnv alphaTexEnv){
	if (colorTexEnv != currentColorTexEnvs[currentTextureUnit] || 
		alphaTexEnv != currentAlphaTexEnvs[currentTextureUnit]){

		if (colorTexEnv != currentColorTexEnvs[currentTextureUnit]){
			configureTexEnv(colorTexEnv, true);
		}

		if (alphaTexEnv != currentAlphaTexEnvs[currentTextureUnit]){
			configureTexEnv(alphaTexEnv, false);
		}

		currentColorTexEnvs[currentTextureUnit] = colorTexEnv;
		currentAlphaTexEnvs[currentTextureUnit] = alphaTexEnv;
	}
}

void Direct3DRenderer::changeTexGen(const TexGen texGen){
/*	if (texGen != currentTexGen[currentTextureUnit]){
		currentTexGen[currentTextureUnit] = texGen;
	}*/
}

void Direct3DRenderer::changeFragmentShader(const ShaderID shader){
	if (currentFragmentShader != shader){
		if (shader == SHADER_NONE){
			dev->SetPixelShader(NULL);
		} else {
			dev->SetPixelShader((LPDIRECT3DPIXELSHADER9) fragmentShaders[shader].shaderID);
		}
		currentFragmentShader = shader;
	}
}

void Direct3DRenderer::changeVertexShader(const ShaderID shader){
	if (currentVertexShader != shader){
		if (shader == SHADER_NONE){
			dev->SetVertexShader(NULL);
		} else {
			dev->SetVertexShader((LPDIRECT3DVERTEXSHADER9) vertexShaders[shader].shaderID);
		}
		currentVertexShader = shader;
	}
}

void Direct3DRenderer::changeBlending(const int srcFactor, const int destFactor, const int blendMode){
	if (selectedBlendingEnabled){
		if (srcFactor != currentSrcFactor){
			dev->SetRenderState(D3DRS_SRCBLEND, currentSrcFactor = srcFactor);
		}
		if (destFactor != currentDestFactor){
			dev->SetRenderState(D3DRS_DESTBLEND, currentDestFactor = destFactor);
		}
		if (blendMode != currentBlendMode){
			dev->SetRenderState(D3DRS_BLENDOP, currentBlendMode = blendMode);
		}
		if (!currentBlendingEnabled){
			dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			currentBlendingEnabled = true;
		}
	} else {
		if (currentBlendingEnabled){
			dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			currentBlendingEnabled = false;
		}
	}
}

void Direct3DRenderer::changeVSync(const bool value){
/*#if defined(WIN32)
	if (WGL_EXT_swap_control) wglSwapIntervalEXT(value);
#elif defined(LINUX)

#endif*/
}

void Direct3DRenderer::changeAnisotropic(const TextureID textureID, const bool value){
	if (textures[textureID].textureKind == TEXKIND_2D){
		textures[textureID].levelOfAnisotropy = value? maxAnisotropic : 1;
	}
}

bool Direct3DRenderer::changeToDefaultRenderTarget(){
    dev->SetRenderTarget(0, mainColorSurface);
    dev->SetDepthStencilSurface(mainDepthSurface);
	return true;
}

