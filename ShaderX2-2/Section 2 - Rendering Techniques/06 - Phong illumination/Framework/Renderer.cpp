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


#include "Renderer.h"
#include "Math/Vertex.h"
#include <stdio.h>

void Renderer::resetToDefaults(){
	for (int i = 0; i < nSimultaneousTextures; i++){
		currentTextures[i] = TEXTURE_NONE;
		currentColorTexEnvs[i] = TEXENV_MODULATE;
		currentAlphaTexEnvs[i] = TEXENV_MODULATE;
		currentTexGen[i] = TEXGEN_NONE;
	}
	currentTextureUnit = 0;
	currentSrcFactor  = BLENDING_NONE;
	currentDestFactor = BLENDING_NONE;
	currentBlendMode  = BLENDMODE_ADD;
	currentBlendingEnabled = false;
	currentFragmentShader  = SHADER_NONE;
	currentVertexShader    = SHADER_NONE;
	currentRenderTarget    = TEXTURE_NONE;

	reset();
}

Renderer::Renderer(){
	colorMipMaps = false;
	useAnisotropic = false;

	lockFlags = 0xFFFFFFFF;
}

void Renderer::reset(){
	for (int i = 0; i < nSimultaneousTextures; i++){
		selectedTextures[i] = TEXTURE_NONE;
		selectedColorTexEnvs[i] = TEXENV_NONE;
		selectedAlphaTexEnvs[i] = TEXENV_NONE;
		selectedTexGen[i] = TEXGEN_NONE;
	}
	selectedBlendingEnabled = false;
	selectedFragmentShader = SHADER_NONE;
	selectedVertexShader   = SHADER_NONE;
}

void Renderer::apply(){
	for (int textureUnit = 0; textureUnit < nSimultaneousTextures; textureUnit++){
		changeTextureUnit(textureUnit);
		changeTexture(selectedTextures[textureUnit]);
		changeTexGen(selectedTexGen[textureUnit]);
		if ((lockFlags & LF_FRAGMENT_SHADING) && (selectedFragmentShader == SHADER_NONE)) changeTexEnv(selectedColorTexEnvs[textureUnit], selectedAlphaTexEnvs[textureUnit]);
	}

	if (lockFlags & LF_FRAGMENT_SHADING) changeFragmentShader(selectedFragmentShader);
	if (lockFlags & LF_VERTEX_SHADING) changeVertexShader(selectedVertexShader);
	if (lockFlags & LF_BLENDING) changeBlending(selectedSrcFactor, selectedDestFactor, selectedBlendMode);

	reset();
}

void Renderer::lock(const unsigned int flags){
	lockFlags &= ~flags;

	if (flags & LF_FRAGMENT_SHADING) changeFragmentShader(selectedFragmentShader);
	if (flags & LF_VERTEX_SHADING) changeVertexShader(selectedVertexShader);
	if (flags & LF_BLENDING) changeBlending(selectedSrcFactor, selectedDestFactor);
}

void Renderer::unlock(const unsigned int flags){
	lockFlags |= flags;
}

void Renderer::clear(){
	int index;

	reset();
	apply();

	resetTextureUnits();

	index = textures.getSize();
	while (index--)	deleteTexture(index);

	index = fragmentShaders.getSize();
	while (index--)	deleteFragmentShader(index);

	index = vertexShaders.getSize();
	while (index--)	deleteVertexShader(index);
}

TextureID Renderer::addTexture(Image *img, unsigned int flags){
	Texture texture;
	
	texture.wantedBitDepth = 0;
	for (int i = TEX_USE_16BIT; i <= TEX_USE_128BIT; i <<= 1){
		if (flags & i) texture.wantedBitDepth = i;
	}

	texture.clampS = ((flags & TEX_CLAMP_S) != 0);
	texture.clampT = ((flags & TEX_CLAMP_T) != 0);
	texture.noLinearFilter = ((flags & TEX_NOLINEARFILTER) != 0);
	
	if (flags & TEX_NORMALMAP){
		if (img->getFormat() != FORMAT_I8) img->toGrayScale(byteOrderRGBA, false);
		img->toNormalMap(byteOrderRGBA);
		img->createMipMaps();
	}

	if (flags & TEX_ALPHAFADE) img->alphaFade();
	
	texture.textureKind = TEXKIND_2D;
	texture.isRenderTarget = false;
	texture.images[0] = img;
	texture.levelOfAnisotropy = 1;
	createTexture(texture);
	
	if (uploadTexture(texture)){
		return insertTexture(texture);
	} else {
		removeTexture(texture);
		delete img;
		
		return TEXTURE_NONE;
	}
}


TextureID Renderer::addTexture(const char *fileName, unsigned int flags){
	Image *img = new Image();

	if (img->loadImage(fileName, byteOrderRGBA, (flags & TEX_NORMALMAP)? MIPMAP_ALLOCATE : MIPMAP_CREATE)){
		return addTexture(img, flags);
	}

	delete img;

	return TEXTURE_NONE;
}

TextureID Renderer::addParticleTexture(const int size, unsigned int flags){
	float mid = 0.5f * (size - 1);
	float invSize = 1.0f / mid;
	unsigned char *dest, *mem = new unsigned char[size * size];

	dest = mem;
	for (int y = 0; y < size; y++){
		for (int x = 0; x < size; x++){
			float len = 1.0f - sqrtf(sqrf(mid - x) + sqrf(mid - y)) * invSize;
			if (len < 0) len = 0;

			*dest++ = (unsigned char) (255 * len);
		}
	}

	Image *img = new Image();
	img->loadFromMemory(mem, size, size, FORMAT_I8, true, false);
	if (!(flags & TEX_NOMIPMAPPING)) img->createMipMaps();

	return addTexture(img, flags);
}

TextureID Renderer::addEntropy2D(const int size, unsigned int flags){
	unsigned char *mem = new unsigned char[4 * size * size];
	int x,y;

	for (y = 0; y < size; y++){
		for (x = 0; x < size; x++){
			mem[(y * size + x) * 4] = rand();
		}
	}
	for (y = 0; y < size; y++){
		for (x = 0; x < size; x++){
			int base = (y * size + x) * 4;
			mem[base + 1] = mem[(( y            ) * size + ((x + 1) % size)) * 4];
			mem[base + 2] = mem[(((y + 1) % size) * size +   x             ) * 4];
			mem[base + 3] = mem[(((y + 1) % size) * size + ((x + 1) % size)) * 4];
		}
	}

	Image *img = new Image();
	img->loadFromMemory(mem, size, size, FORMAT_RGBA8, true, false);
	if (!(flags & TEX_NOMIPMAPPING)) img->createMipMaps();

	return addTexture(img, flags);
}

TextureID Renderer::addCubemap(const char *fileNegX, const char *filePosX, const char *fileNegY, const char *filePosY, const char *fileNegZ, const char *filePosZ){
	Texture texture;
	const char *fileNames[] = {filePosX, fileNegX, filePosY, fileNegY, filePosZ, fileNegZ};
	
	for (int i = 0; i < 6; i++){
		texture.images[i] = new Image();

		if (!texture.images[i]->loadImage(fileNames[i], byteOrderRGBA, MIPMAP_CREATE)){
			do {
				delete texture.images[i];
			} while (i--);
			
			return TEXTURE_NONE;
		}
	}

	texture.textureKind = TEXKIND_CUBEMAP;
	texture.isRenderTarget = false;
	texture.noLinearFilter = false;
	texture.levelOfAnisotropy = 1;
	createTexture(texture);
	
	if (uploadTexture(texture)){
		return insertTexture(texture);
	} else {
		removeTexture(texture);
		return TEXTURE_NONE;
	}
}

inline unsigned char PackFloatInByte(const float x){
    return (unsigned char) (127.5f * (x + 1));
}


TextureID Renderer::addNormalizingCubemap(const int size){
	Texture texture;
	unsigned char *imgNeg, *imgPos, *destNeg, *destPos;
	Vertex v;
	int i,x,y,z;
	float mid = 0.5f * (size - 1);

	for (i = 0; i < 6; i++) texture.images[i] = new Image();

	destPos = imgPos = new unsigned char[size * size * 3];
	destNeg = imgNeg = new unsigned char[size * size * 3];
	for (y = 0; y < size; y++){
		for (z = 0; z < size; z++){
			v.x = mid;
			v.y = mid - y;
			v.z = mid - z;
			v.fastNormalize();

			*destPos++ = PackFloatInByte( v.x);	*destNeg++ = PackFloatInByte(-v.x);
			*destPos++ =                        *destNeg++ = PackFloatInByte( v.y);
			*destPos++ = PackFloatInByte( v.z);	*destNeg++ = PackFloatInByte(-v.z);
		}
	}
	texture.images[0]->loadFromMemory(imgPos, size, size, FORMAT_RGB8, true, false);
	texture.images[1]->loadFromMemory(imgNeg, size, size, FORMAT_RGB8, true, false);

	destPos = imgPos = new unsigned char[size * size * 3];
	destNeg = imgNeg = new unsigned char[size * size * 3];
	for (z = 0; z < size; z++){
		for (x = 0; x < size; x++){
			v.x = x - mid;
			v.y = mid;
			v.z = z - mid;
			v.fastNormalize();

			*destPos++ = *destNeg++ = PackFloatInByte(v.x);
			*destPos++ = PackFloatInByte( v.y);
			*destNeg++ = PackFloatInByte(-v.y);
			*destPos++ = PackFloatInByte( v.z);
			*destNeg++ = PackFloatInByte(-v.z);
		}
	}
	texture.images[2]->loadFromMemory(imgPos, size, size, FORMAT_RGB8, true, false);
	texture.images[3]->loadFromMemory(imgNeg, size, size, FORMAT_RGB8, true, false);

	destPos = imgPos = new unsigned char[size * size * 3];
	destNeg = imgNeg = new unsigned char[size * size * 3];
	for (y = 0; y < size; y++){
		for (x = 0; x < size; x++){
			v.x = x - mid;
			v.y = mid - y;
			v.z = mid;
			v.fastNormalize();

			*destPos++ = PackFloatInByte( v.x);
			*destNeg++ = PackFloatInByte(-v.x);
			*destNeg++ = *destPos++ = PackFloatInByte(v.y);
			*destPos++ = PackFloatInByte( v.z);
			*destNeg++ = PackFloatInByte(-v.z);
		}
	}
	texture.images[4]->loadFromMemory(imgPos, size, size, FORMAT_RGB8, true, false);
	texture.images[5]->loadFromMemory(imgNeg, size, size, FORMAT_RGB8, true, false);

	for (i = 0; i < 6; i++) texture.images[i]->createMipMaps();


	texture.textureKind = TEXKIND_CUBEMAP;
	texture.isRenderTarget = false;
	texture.noLinearFilter = false;
	createTexture(texture);
	
	if (uploadTexture(texture)){
		return insertTexture(texture);
	} else {
		removeTexture(texture);
		return TEXTURE_NONE;
	}
}

TextureID Renderer::addTexture3D(Image3D *img, unsigned int flags){
	if (!(flags & TEX_NOMIPMAPPING)) img->createMipMaps();

	Texture texture;
	texture.textureKind = TEXKIND_3D;
	texture.isRenderTarget = false;
	texture.noLinearFilter = ((flags & TEX_NOLINEARFILTER) != 0);
	texture.clampS = ((flags & TEX_CLAMP_S) != 0);
	texture.clampT = ((flags & TEX_CLAMP_T) != 0);
	texture.clampR = ((flags & TEX_CLAMP_R) != 0);
	texture.levelOfAnisotropy = 1;
	createTexture(texture);

	texture.image3D = img;

	if (uploadTexture(texture)){
		return insertTexture(texture);
	} else {
		removeTexture(texture);
		delete img;

		return TEXTURE_NONE;
	}
}

TextureID Renderer::addTexture3D(const char *fileName, unsigned int flags){
	Image3D *img = new Image3D();

	if (img->loadFromFile(fileName)){
		return addTexture3D(img, flags);
	}

	delete img;
	return TEXTURE_NONE;
}

TextureID Renderer::addAttenuationMap(const int size, unsigned int flags){
	float mid = 0.5f * (size - 1);
	float invSize = 1.0f / mid;
	unsigned char *dest, *mem = new unsigned char[size * size * size];

	dest = mem;
	for (int z = 0; z < size; z++){
		for (int y = 0; y < size; y++){
			for (int x = 0; x < size; x++){
				Vertex v(mid - x, mid - y, mid - z);
				float len = 1.0f - fastLength(v) * invSize;
				if (len < 0) len = 0;

				*dest++ = (unsigned char) (255 * len);
			}
		}
	}

	Image3D *img = new Image3D();
	img->loadFromMemory(mem, size, size, size, 1, true, false);

	return addTexture3D(img, flags | TEX_CLAMP);
}

TextureID Renderer::addDistanceMap(const int size, unsigned int flags){
	float mid = 0.5f * (size - 1);
	float invSize = 1.0f / mid;
	unsigned char *dest, *mem = new unsigned char[size * size * size];

	dest = mem;
	for (int z = 0; z < size; z++){
		for (int y = 0; y < size; y++){
			for (int x = 0; x < size; x++){
				Vertex v(mid - x, mid - y, mid - z);
				float len = fastLength(v) * invSize;
				if (len > 1) len = 1;

				*dest++ = (unsigned char) (255 * len);
			}
		}
	}

	Image3D *img = new Image3D();
	img->loadFromMemory(mem, size, size, size, 1, true, false);

	return addTexture3D(img, flags | TEX_CLAMP);
}

TextureID Renderer::addFogVolume(const int size, unsigned int flags){
	float invSize = 2.0f / (size - 1.0f);
	unsigned char *dest, *mem = new unsigned char[size * size * size];

	dest = mem;
	for (int z = 0; z < size; z++){
		float fz = z * invSize - 1.0f;
		for (int y = 0; y < size; y++){
			float fy = y * invSize - 1.0f;
			for (int x = 0; x < size; x++){
				float fx = x * invSize - 1.0f;
				float dz = 1.0f - fx * fx - fy * fy;
				if (dz < 0) dz = 0; else dz = sqrtf(dz);

				float len;
				if (fz > 0){
					len = (fz < dz)? 0.5f * (dz - fz) : 0;
				} else {
					len = (fz < -dz)? dz : 0.5f * (dz - fz);
				}

				//if (len > 1.0f) len = 1.0f;
				//if (len < 0.0f) len = 0.0f;

				*dest++ = (unsigned char) (255 * len);
			}
		}
	}

	Image3D *img = new Image3D();
	img->loadFromMemory(mem, size, size, size, 1, true, false);
	return addTexture3D(img, flags | TEX_CLAMP);
}

TextureID Renderer::addVectorField(const int size, unsigned int flags){
	float mid = 0.5f * (size - 1);
	float invSize = 1.0f / mid;
	unsigned char *dest, *mem = new unsigned char[4 * size * size * size];

	int red, blue;
	if (byteOrderRGBA){
		red  = 0;
		blue = 2;
	} else {
		red  = 2;
		blue = 0;
	}

	dest = mem;
	for (int z = 0; z < size; z++){
		for (int y = 0; y < size; y++){
			for (int x = 0; x < size; x++){
				Vertex v(x - mid, y - mid, z - mid);

				float lsqr = lengthSqr(v);
				float rsqr = rsqrtf(lsqr);

				float len = 1.0f - lsqr * rsqr * invSize;
				if (len < 0) len = 0;
				v *= len * rsqr;

				dest[red]  = PackFloatInByte(v.x);
				dest[1]    = PackFloatInByte(v.y);
				dest[blue] = PackFloatInByte(v.z);
				dest[3]    = PackFloatInByte(len);
				dest += 4;
			}
		}
	}

	Image3D *img = new Image3D();
	img->loadFromMemory(mem, size, size, size, 4, true, false);
	return addTexture3D(img, flags | TEX_CLAMP);
}

TextureID Renderer::addEntropy3D(const int size, unsigned int flags){
	unsigned char *mem = new unsigned char[4 * size * size * size];
	int x,y,z;

	for (z = 0; z < size; z++){
		for (y = 0; y < size; y++){
			for (x = 0; x < size; x++){
				mem[((z * size + y) * size + x) * 4] = rand();
			}
		}
	}
	for (z = 0; z < size; z++){
		for (y = 0; y < size; y++){
			for (x = 0; x < size; x++){
				int base = ((z * size + y) * size + x) * 4;
				mem[base + 1] = mem[((z * size +   y             ) * size + ((x + 1) % size)) * 4];
				mem[base + 2] = mem[((z * size + ((y + 1) % size)) * size +   x             ) * 4];
				mem[base + 3] = mem[((z * size + ((y + 1) % size)) * size + ((x + 1) % size)) * 4];
			}
		}
	}

	Image3D *img = new Image3D();
	img->loadFromMemory(mem, size, size, size, 4, true, false);
	return addTexture3D(img, flags);
}


TextureID Renderer::addRenderTexture(const int width, const int height, bool mipmapped, const bool cubemap, RenderPixelFormat rPixelFormat){
	Texture texture;

	texture.textureKind = cubemap? TEXKIND_CUBEMAP : TEXKIND_2D;
	texture.isRenderTarget = true;
	createTexture(texture);

	if (texture.renderTarget->create(width, height, mipmapped, cubemap, rPixelFormat)){
		texture.texID = texture.renderTarget->getTexID();
		return insertTexture(texture);
	} else {
		return TEXTURE_NONE;
	}
}


ShaderID Renderer::addFragmentShader(unsigned int type, char *fileName){
	FragmentShader fragmentShader;
	fragmentShader.type = type;

	createFragmentShader(fragmentShader);
	if (fileName != NULL){
		FILE *file = fopen(fileName, "rb");

		if (file == NULL) return SHADER_NONE;
		
		// Find file size
		fseek(file,  0, SEEK_END);
		fragmentShader.length = ftell(file);
		fseek(file,  0, SEEK_SET);

		fragmentShader.program = new char[fragmentShader.length];
		fread(fragmentShader.program, fragmentShader.length, 1, file);
		fclose(file);
		
		uploadFragmentShader(fragmentShader);
		delete fragmentShader.program;
	}

	return insertFragmentShader(fragmentShader);
}

ShaderID Renderer::addFragmentShader(unsigned int type, unsigned int shaderID){
	FragmentShader fragmentShader;
	fragmentShader.type = type;
	fragmentShader.shaderID = shaderID;

	return insertFragmentShader(fragmentShader);
}


ShaderID Renderer::addVertexShader(char *fileName){
	VertexShader vertexShader;

	createVertexShader(vertexShader);

	FILE *file;
	if ((file = fopen(fileName, "rb")) != NULL){
		// Find file size
		fseek(file,  0, SEEK_END);
		vertexShader.length = ftell(file);
		fseek(file,  0, SEEK_SET);

		vertexShader.program = new char[vertexShader.length];
		fread(vertexShader.program, vertexShader.length, 1, file);
		fclose(file);
		
		uploadVertexShader(vertexShader);
		delete vertexShader.program;

		return insertVertexShader(vertexShader);
	}

	return SHADER_NONE;
}

TextureID Renderer::insertTexture(Texture &texture){
	int i, len = textures.getSize();
	for (i = 0; i < len; i++){
		if (textures[i].texID == TEXTURE_UNDEFINED){
			textures[i] = texture;
			break;
		}
	}

	if (i == len) i = textures.add(texture);

	changeAnisotropic(i, useAnisotropic);

	return i;
}

void Renderer::deleteTexture(const int index){
	if (textures[index].texID != TEXTURE_UNDEFINED){
		removeTexture(textures[index]);

		if (!textures[index].isRenderTarget){
			switch(textures[index].textureKind){
			case TEXKIND_2D:
				delete textures[index].images[0];
				break;
			case TEXKIND_CUBEMAP:
				int i;
				for (i = 0; i < 6; i++) delete textures[index].images[i];
				break;
			case TEXKIND_3D:
				delete textures[index].image3D;
				break;
			}
		}
		
		textures[index].texID = TEXTURE_UNDEFINED;
	}
}

ShaderID Renderer::insertFragmentShader(FragmentShader &fragmentShader){
	int i, len = fragmentShaders.getSize();
	for (i = 0; i < len; i++){
		if (fragmentShaders[i].shaderID == SHADER_UNDEFINED){
			fragmentShaders[i] = fragmentShader;
			return i;
		}
	}

	return fragmentShaders.add(fragmentShader);
}

void Renderer::deleteFragmentShader(const int index){
	if (fragmentShaders[index].shaderID != SHADER_UNDEFINED){
		removeFragmentShader(fragmentShaders[index]);
		fragmentShaders[index].shaderID = SHADER_UNDEFINED;
	}
}

ShaderID Renderer::insertVertexShader(VertexShader &vertexShader){
	int i, len = vertexShaders.getSize();
	for (i = 0; i < len; i++){
		if (vertexShaders[i].shaderID == SHADER_UNDEFINED){
			vertexShaders[i] = vertexShader;
			return i;
		}
	}

	return vertexShaders.add(vertexShader);
}

void Renderer::deleteVertexShader(const int index){
	if (vertexShaders[index].shaderID != SHADER_UNDEFINED){
		removeVertexShader(vertexShaders[index]);
		vertexShaders[index].shaderID = SHADER_UNDEFINED;
	}
}

TextureID Renderer::getNextTexture(TextureID currentTexture) const {
	int len = textures.getSize();

	do {
		currentTexture++;
		if (currentTexture == len) return TEXTURE_NONE;
	} while (textures[currentTexture].texID == TEXTURE_UNDEFINED);
	
	return currentTexture;
}

bool Renderer::changeRenderTarget(const TextureID renderTarget){
	bool result = false;

	if (renderTarget != currentRenderTarget){
		if (currentRenderTarget != TEXTURE_NONE){
			changeTextureUnit(0);
			changeTexture(currentRenderTarget);
			
			textures[currentRenderTarget].renderTarget->unsetCurrent();
		}
		reset();
		apply();

		changeTextureUnit(0);

		if (renderTarget == TEXTURE_NONE){
			result = changeToDefaultRenderTarget();
		} else {
			result = textures[renderTarget].renderTarget->setCurrent();	
		}
		currentRenderTarget = renderTarget;
	}
	return result;
}

bool Renderer::changeRenderFace(const TextureID renderTarget, const int renderFace){
	if (textures[renderTarget].isRenderTarget){
		return textures[renderTarget].renderTarget->setRenderFace(renderFace);
	}
	return false;
}

void Renderer::setMaterial(const Material &material){
	for (int i = 0; i < material.nTextures; i++)
		selectedTextures[i] = material.textures[i];		

	selectedFragmentShader = material.fragmentShader;
	selectedVertexShader   = material.vertexShader;
}

