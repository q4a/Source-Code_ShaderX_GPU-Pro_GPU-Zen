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


#include "OpenGLRenderer.h"
#include "OpenGLRenderTarget.h"

// Blending constants
const int ZERO                 = GL_ZERO;
const int ONE                  = GL_ONE;
const int SRC_COLOR            = GL_SRC_COLOR;
const int ONE_MINUS_SRC_COLOR  = GL_ONE_MINUS_SRC_COLOR;
const int DST_COLOR            = GL_DST_COLOR;
const int ONE_MINUS_DST_COLOR  = GL_ONE_MINUS_DST_COLOR;
const int SRC_ALPHA            = GL_SRC_ALPHA;
const int ONE_MINUS_SRC_ALPHA  = GL_ONE_MINUS_SRC_ALPHA;
const int DST_ALPHA            = GL_DST_ALPHA;
const int ONE_MINUS_DST_ALPHA  = GL_ONE_MINUS_DST_ALPHA;
const int SRC_ALPHA_SATURATE   = GL_SRC_ALPHA_SATURATE;

const int BLENDMODE_ADD              = GL_FUNC_ADD_EXT;
const int BLENDMODE_SUBRACT          = GL_FUNC_SUBTRACT_EXT;
const int BLENDMODE_REVERSE_SUBTRACT = GL_FUNC_REVERSE_SUBTRACT_EXT;


GLenum OpenGLRenderer::targets[] = { GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_3D_EXT };

#if defined(WIN32)

OpenGLRenderer::OpenGLRenderer(HDC hDc, HGLRC hGlrc) : Renderer(){
	hdc = hDc;
	hglrc = hGlrc;
	init();
}

#elif defined(LINUX)

OpenGLRenderer::OpenGLRenderer(GLXContext glXc, Display *disp) : Renderer(){
	glxc = glXc;
	display = disp;
	init();
}

#endif

void OpenGLRenderer::init(){
	byteOrderRGBA = true;
	if (GL_ARB_multitexture){
		glGetIntegerv(GL_MAX_ACTIVE_TEXTURES_ARB, &nSimultaneousTextures);
		if (nSimultaneousTextures > MAX_TEXTURE) nSimultaneousTextures = MAX_TEXTURE;
	} else {
		nSimultaneousTextures = 1;
	}

	resetToDefaults();
}


void OpenGLRenderer::createTexture(Texture &texture){
	if (texture.isRenderTarget){
		texture.renderTarget = new OpenGLRenderTarget();
	} else {
		GLenum target = targets[texture.textureKind];

		glGenTextures(1, &texture.texID);
		glBindTexture(target, texture.texID);
	}	
}

void OpenGLRenderer::removeTexture(Texture &texture){
	if (texture.isRenderTarget){
		texture.renderTarget->destroy();
		delete texture.renderTarget;
	}
	glDeleteTextures(1, &texture.texID);
}

void OpenGLRenderer::resetTextureUnits(){
	for (int i = 0; i < nSimultaneousTextures; i++){
		changeTextureUnit(i);

		glBindTexture(GL_TEXTURE_1D, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		if (GL_EXT_texture3D) glBindTexture(GL_TEXTURE_3D_EXT, 0);
		if (GL_ARB_texture_cube_map) glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, 0);
	}
}

bool OpenGLRenderer::uploadTexture(Texture &texture){
	static int type[] = {0, GL_LUMINANCE, GL_DUDV_ATI, GL_RGB, GL_RGBA};
	static int internalFormats[] = 
	{	0,
		GL_INTENSITY,
		GL_RGB8,
		GL_RGBA8,
		GL_INTENSITY_FLOAT32_ATI,
		GL_RGB_FLOAT32_ATI,
		GL_RGBA_FLOAT32_ATI,
		0, // FORMAT_RGB565 currently unsupported
		GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
		GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
		GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
	};

	unsigned char *src;
	int internalFormat;

	GLenum target;
	int nImages = 1;

	switch(texture.textureKind){
	case TEXKIND_2D:
		target = GL_TEXTURE_2D;
		glTexParameteri(target, GL_TEXTURE_WRAP_S, texture.clampS? GL_CLAMP_TO_EDGE_EXT : GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, texture.clampT? GL_CLAMP_TO_EDGE_EXT : GL_REPEAT);
		break;
	case TEXKIND_CUBEMAP:
		target = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB;
		nImages = 6;
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
		break;
	case TEXKIND_3D:
		target = GL_TEXTURE_3D_EXT;
		glTexParameteri(target, GL_TEXTURE_WRAP_S, texture.clampS? GL_CLAMP_TO_EDGE_EXT : GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, texture.clampT? GL_CLAMP_TO_EDGE_EXT : GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_R, texture.clampR? GL_CLAMP_TO_EDGE_EXT : GL_REPEAT);
		break;
	default:
		return false;
	}

	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, texture.noLinearFilter? GL_NEAREST : GL_LINEAR);

	if (texture.textureKind == TEXKIND_3D){
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, texture.image3D->getNumberOfMipMaps() <= 1? (texture.noLinearFilter? GL_NEAREST : GL_LINEAR) : GL_LINEAR_MIPMAP_LINEAR);

		Image3D *img = texture.image3D;
		int nChannels = img->getChannels();
		internalFormat = (nChannels == 3)? GL_RGB8 : (nChannels == 4)? GL_RGBA8 : GL_INTENSITY;

		int mipMapLevel = 0;
		while ((src = img->getImage(mipMapLevel)) != NULL){
			glTexImage3DEXT(target, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), img->getDepth(mipMapLevel), 0, type[nChannels], GL_UNSIGNED_BYTE, src);
			mipMapLevel++;
		}

	} else {
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, texture.images[0]->getNumberOfMipMaps() <= 1? (texture.noLinearFilter? GL_NEAREST : GL_LINEAR) : GL_LINEAR_MIPMAP_LINEAR);

		for (int i = 0; i < nImages; i++){
			Image *img = texture.images[i];
			FORMAT format = img->getFormat();
			
			if (isCompressedFormat(format) && !(GL_ARB_texture_compression && GL_EXT_texture_compression_s3tc)){
				img->uncompressImage();
				format = img->getFormat();
			}
			
			if (colorMipMaps) img->colorMipMaps();
			
			if (isPlainFormat(format)){
				int nChannels = getChannels(format);
				if (texture.wantedBitDepth == 16) internalFormat = GL_RGB5; else internalFormat = internalFormats[format];

				int mipMapLevel = 0;
				while ((src = img->getImage(mipMapLevel)) != NULL){
					glTexImage2D(target, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, type[nChannels], GL_UNSIGNED_BYTE, src);
					mipMapLevel++;
				}
			} else {
				
				int mipMapLevel = 0;
				while ((src = img->getImage(mipMapLevel)) != NULL){
					glCompressedTexImage2DARB(target, mipMapLevel, internalFormats[format], img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, img->getMipMappedSize(mipMapLevel, 1), src);
					mipMapLevel++;
				}
			}
			target++;
		}
	}

	return true;
}

void OpenGLRenderer::createFragmentShader(FragmentShader &fragmentShader){
	if (fragmentShader.type == GL_FRAGMENT_SHADER_GL2){

	} else if (fragmentShader.type == GL_FRAGMENT_PROGRAM_ARB){
		glGenProgramsARB(1, &fragmentShader.shaderID);
		glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, fragmentShader.shaderID);
	} else if (fragmentShader.type == GL_FRAGMENT_SHADER_ATI){
		fragmentShader.shaderID = glGenFragmentShadersATI(1);
		glBindFragmentShaderATI(fragmentShader.shaderID);
	}
}

void OpenGLRenderer::removeFragmentShader(FragmentShader &fragmentShader){
	if (fragmentShader.type == GL_FRAGMENT_PROGRAM_ARB){
		glDeleteProgramsARB(1, &fragmentShader.shaderID);
	} else if (fragmentShader.type == GL_FRAGMENT_SHADER_ATI){
		glDeleteFragmentShaderATI(fragmentShader.shaderID);
	} else if (fragmentShader.type == GL_REGISTER_COMBINERS_NV){
		glDeleteLists(fragmentShader.shaderID, 1);
	}
}

void OpenGLRenderer::uploadFragmentShader(FragmentShader &fragmentShader){
	glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, fragmentShader.length, fragmentShader.program); 
}

void OpenGLRenderer::createVertexShader(VertexShader &vertexShader){
	glGenProgramsARB(1, &vertexShader.shaderID);
	glBindProgramARB(GL_VERTEX_PROGRAM_ARB, vertexShader.shaderID);
}

void OpenGLRenderer::removeVertexShader(VertexShader &vertexShader){
	glDeleteProgramsARB(1, &vertexShader.shaderID);
}

void OpenGLRenderer::uploadVertexShader(VertexShader &vertexShader){
	glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, vertexShader.length, vertexShader.program); 
}

void OpenGLRenderer::changeTextureUnit(const int textureUnit){
	if (textureUnit != currentTextureUnit){
		glActiveTexture(GL_TEXTURE0_ARB + (currentTextureUnit = textureUnit));
	}
}

void OpenGLRenderer::changeTexture(const TextureID textureID){
	TextureID currTex = currentTextures[currentTextureUnit];

	if (textureID != currTex){
		if (currTex != TEXTURE_NONE && textures[currTex].isRenderTarget) ((OpenGLRenderTarget *) textures[currTex].renderTarget)->unbind();
		if (textureID == TEXTURE_NONE){
			glDisable(targets[textures[currTex].textureKind]);
		} else {
			GLenum target = targets[textures[textureID].textureKind];

			if (currTex == TEXTURE_NONE){
				glEnable(target);
			} else {
				if (textures[textureID].textureKind != textures[currTex].textureKind){
					glDisable(targets[textures[currTex].textureKind]);
					glEnable(target);
				}
			}

			glBindTexture(target, textures[textureID].texID);
			if (textures[textureID].isRenderTarget) ((OpenGLRenderTarget *) textures[textureID].renderTarget)->bind();
		}
		currentTextures[currentTextureUnit] = textureID;
	}
}

void OpenGLRenderer::configureTexEnv(const TexEnv texEnv, bool isRGB){
	int combineMode, source0, source1, source2;
	int operand0, operand1, operand2, srcColor;
	if (isRGB){
		combineMode = GL_COMBINE_RGB_ARB;
		source0     = GL_SOURCE0_RGB_ARB;
		operand0    = GL_OPERAND0_RGB_ARB;
		srcColor    = GL_SRC_COLOR;
	} else {
		combineMode = GL_COMBINE_ALPHA_ARB;
		source0     = GL_SOURCE0_ALPHA_ARB;
		operand0    = GL_OPERAND0_ALPHA_ARB;
		srcColor    = GL_SRC_ALPHA;
	}
	source1  = source0  + 1;
	source2  = source1  + 1;
	operand1 = operand0 + 1;
	operand2 = operand1 + 1;

	switch(texEnv){
	case TEXENV_MODULATE:
		glTexEnvi(GL_TEXTURE_ENV, combineMode, GL_MODULATE);

		glTexEnvi(GL_TEXTURE_ENV, source0,  GL_PREVIOUS_ARB);
		glTexEnvi(GL_TEXTURE_ENV, operand0, srcColor);
		
		glTexEnvi(GL_TEXTURE_ENV, source1,  GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, operand1, srcColor);
		break;
	case TEXENV_DOT3:
		if (isRGB){
			glTexEnvi(GL_TEXTURE_ENV, combineMode, GL_DOT3_RGBA_ARB);
			
			glTexEnvi(GL_TEXTURE_ENV, source0,  GL_PREVIOUS_ARB);
			glTexEnvi(GL_TEXTURE_ENV, operand0, srcColor);
			
			glTexEnvi(GL_TEXTURE_ENV, source1,  GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, operand1, srcColor);
		}
		break;
	case TEXENV_DOT3_NEXT:
		if (isRGB){
			glTexEnvi(GL_TEXTURE_ENV, combineMode, GL_DOT3_RGBA_ARB);
			
			glTexEnvi(GL_TEXTURE_ENV, source0,  GL_TEXTURE0_ARB + currentTextureUnit);
			glTexEnvi(GL_TEXTURE_ENV, operand0, srcColor);
			
			glTexEnvi(GL_TEXTURE_ENV, source1,  GL_TEXTURE0_ARB + currentTextureUnit + 1);
			glTexEnvi(GL_TEXTURE_ENV, operand1, srcColor);
		}
		break;
	case TEXENV_ADD:
		glTexEnvi(GL_TEXTURE_ENV, combineMode, GL_ADD);

		glTexEnvi(GL_TEXTURE_ENV, source0,  GL_PREVIOUS_ARB);
		glTexEnvi(GL_TEXTURE_ENV, operand0, srcColor);
		
		glTexEnvi(GL_TEXTURE_ENV, source1,  GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, operand1, srcColor);
		break;
	case TEXENV_REPLACE:
		glTexEnvi(GL_TEXTURE_ENV, combineMode, GL_REPLACE);

		glTexEnvi(GL_TEXTURE_ENV, source0,  GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, operand0, srcColor);
		break;
	case TEXENV_PASSTHRU:
		glTexEnvi(GL_TEXTURE_ENV, combineMode, GL_REPLACE);

		glTexEnvi(GL_TEXTURE_ENV, source0,  GL_PREVIOUS_ARB);
		glTexEnvi(GL_TEXTURE_ENV, operand0, srcColor);
		break;
	case TEXENV_INTERPOLATE:
		glTexEnvi(GL_TEXTURE_ENV, combineMode, GL_INTERPOLATE_ARB);
		
		glTexEnvi(GL_TEXTURE_ENV, source0,  GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, operand0, srcColor);

		glTexEnvi(GL_TEXTURE_ENV, source1,  GL_PREVIOUS_ARB);
		glTexEnvi(GL_TEXTURE_ENV, operand1, srcColor);
		
		glTexEnvi(GL_TEXTURE_ENV, source2,  GL_PRIMARY_COLOR_ARB);
		glTexEnvi(GL_TEXTURE_ENV, operand2, srcColor);
		break;
	case TEXENV_INTERPOLATE_PRIMARY:
		glTexEnvi(GL_TEXTURE_ENV, combineMode, GL_INTERPOLATE_ARB);
		
		glTexEnvi(GL_TEXTURE_ENV, source0,  GL_PRIMARY_COLOR_ARB);
		glTexEnvi(GL_TEXTURE_ENV, operand0, srcColor);
		
		glTexEnvi(GL_TEXTURE_ENV, source1,  GL_PREVIOUS_ARB);
		glTexEnvi(GL_TEXTURE_ENV, operand1, srcColor);

		glTexEnvi(GL_TEXTURE_ENV, source2,  GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, operand2, srcColor);
		break;
	case TEXENV_INTERPOLATE_TEX_ALPHA:
		glTexEnvi(GL_TEXTURE_ENV, combineMode, GL_INTERPOLATE_ARB);
		
		glTexEnvi(GL_TEXTURE_ENV, source0,  GL_PREVIOUS_ARB);
		glTexEnvi(GL_TEXTURE_ENV, operand0, srcColor);
		
		glTexEnvi(GL_TEXTURE_ENV, source1,  GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, operand1, srcColor);

		glTexEnvi(GL_TEXTURE_ENV, source2,  GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, operand2, GL_SRC_ALPHA);
		break;
	default:
		break;
	}
}

void OpenGLRenderer::changeTexEnv(const TexEnv colorTexEnv, const TexEnv alphaTexEnv){
	if (colorTexEnv != currentColorTexEnvs[currentTextureUnit] || 
		alphaTexEnv != currentAlphaTexEnvs[currentTextureUnit]){

		bool handled;
		// Special case for compatibility
		if (handled = (colorTexEnv == alphaTexEnv)){
			switch(colorTexEnv){
			case TEXENV_MODULATE:
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				break;
			case TEXENV_ADD:
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
				break;
			case TEXENV_REPLACE:
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				break;
			default:
				handled = false;
			}
		}

		if (!handled){
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
			if (colorTexEnv != currentColorTexEnvs[currentTextureUnit]){
				configureTexEnv(colorTexEnv, true);
			}

			if (alphaTexEnv != currentAlphaTexEnvs[currentTextureUnit]){
				configureTexEnv(alphaTexEnv, false);
			}
		}

		currentColorTexEnvs[currentTextureUnit] = colorTexEnv;
		currentAlphaTexEnvs[currentTextureUnit] = alphaTexEnv;
	}
}

void OpenGLRenderer::changeTexGen(const TexGen texGen){
	if (texGen != currentTexGen[currentTextureUnit]){
		if (texGen == TEXGEN_NONE){
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			glDisable(GL_TEXTURE_GEN_R);
			glDisable(GL_TEXTURE_GEN_Q);
		} else {
			if (currentTexGen[currentTextureUnit] == TEXGEN_NONE){
				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);
				glEnable(GL_TEXTURE_GEN_R);
				glEnable(GL_TEXTURE_GEN_Q);
			}

			switch(texGen){
			case TEXGEN_OBJECT_LINEAR:
				glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
				glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
				glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
				glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
				break;
			case TEXGEN_EYE_LINEAR:
				glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
				glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
				glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
				glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
				break;
			default:
				break;
			};
		}

		currentTexGen[currentTextureUnit] = texGen;
	}
}

void OpenGLRenderer::changeFragmentShader(const ShaderID shader){
	if (currentFragmentShader != shader){
		if (shader == SHADER_NONE){
			glDisable(fragmentShaders[currentFragmentShader].type);
		} else {
			if (currentFragmentShader == SHADER_NONE){
				glEnable(fragmentShaders[shader].type);
			} else if (fragmentShaders[currentFragmentShader].type != fragmentShaders[shader].type){
				glDisable(fragmentShaders[currentFragmentShader].type);
				glEnable(fragmentShaders[shader].type);
			}

			if (fragmentShaders[shader].type == GL_FRAGMENT_PROGRAM_ARB){
				glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, fragmentShaders[shader].shaderID);
			} else if (fragmentShaders[shader].type == GL_FRAGMENT_SHADER_ATI){
				glBindFragmentShaderATI(fragmentShaders[shader].shaderID);
			} else if (fragmentShaders[shader].type == GL_REGISTER_COMBINERS_NV){
				glCallList(fragmentShaders[shader].shaderID);
			}
		}
		currentFragmentShader = shader;
	}
}

void OpenGLRenderer::changeVertexShader(const ShaderID shader){
	if (currentVertexShader != shader){
		if (shader == SHADER_NONE){
			glDisable(GL_VERTEX_PROGRAM_ARB);
		} else {
			if (currentVertexShader == SHADER_NONE){
				glEnable(GL_VERTEX_PROGRAM_ARB);
			}

			glBindProgramARB(GL_VERTEX_PROGRAM_ARB, vertexShaders[shader].shaderID);
		}
		currentVertexShader = shader;
	}
}

void OpenGLRenderer::changeBlending(const int srcFactor, const int destFactor, const int blendMode){
	if (selectedBlendingEnabled){
		if (srcFactor != currentSrcFactor || destFactor != currentDestFactor){
			glBlendFunc(currentSrcFactor = srcFactor, currentDestFactor = destFactor);
		}
		if (blendMode != currentBlendMode){
			glBlendEquationEXT(currentBlendMode = blendMode);
		}
		if (!currentBlendingEnabled){
			glEnable(GL_BLEND);
			currentBlendingEnabled = true;
		}
	} else {
		if (currentBlendingEnabled){
			glDisable(GL_BLEND);
			currentBlendingEnabled = false;
		}
	}
}

void OpenGLRenderer::changeVSync(const bool value){
#if defined(WIN32)
	if (WGL_EXT_swap_control) wglSwapIntervalEXT(value);
#elif defined(LINUX)

#endif
}

void OpenGLRenderer::changeAnisotropic(const TextureID textureID, const bool value){
	if (GL_EXT_texture_filter_anisotropic){
		if (textures[textureID].textureKind == TEXKIND_2D){
			changeTexture(textureID);
			
			int maxAnisotrophy = 1;
			if (value) glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotrophy);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotrophy);
		}
	}
}

bool OpenGLRenderer::changeToDefaultRenderTarget(){
#if defined(WIN32)
	wglMakeCurrent(hdc, hglrc);
#elif defined(LINUX)
	glXMakeCurrent(display, None, NULL);
#endif
	return true;
}

