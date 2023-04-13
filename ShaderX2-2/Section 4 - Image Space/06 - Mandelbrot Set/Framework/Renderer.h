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


#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "Util/Set.h"
#include "Imaging/Image.h"
#include "Imaging/Image3D.h"
#include "RenderTarget.h"

typedef int TextureID;
typedef int ShaderID;

#define TEXTURE_NONE  (-1)
#define BLENDING_NONE (-1)
#define SHADER_NONE   (-1)

#define TEXTURE_UNDEFINED 0
#define SHADER_UNDEFINED 0

#define MAX_TEXTURE 16

enum TEXKIND {
	TEXKIND_2D      =  0,
	TEXKIND_CUBEMAP =  1,
	TEXKIND_3D      =  2,
};

struct Texture {
	unsigned int texID;
	TEXKIND textureKind;
	union {
		Image *images[6];
		Image3D *image3D;
		RenderTarget *renderTarget;
	};
	int wantedBitDepth;
	int levelOfAnisotropy;
	bool clampS, clampT, clampR;
	bool noLinearFilter;
	bool isRenderTarget;
};

struct FragmentShader {
	unsigned int shaderID;
	char *program;
	unsigned int length;
	unsigned int type;
};

struct VertexShader {
	unsigned int shaderID;
	char *program;
	unsigned int length;
};

enum TexEnv {
	TEXENV_NONE,
	TEXENV_MODULATE,
	TEXENV_DOT3,
	TEXENV_DOT3_NEXT,
	TEXENV_ADD,
	TEXENV_REPLACE,
	TEXENV_PASSTHRU,
	TEXENV_INTERPOLATE,
	TEXENV_INTERPOLATE_PRIMARY,
	TEXENV_INTERPOLATE_TEX_ALPHA,
};

enum TexGen {
	TEXGEN_NONE,
	TEXGEN_OBJECT_LINEAR,
	TEXGEN_EYE_LINEAR,
};

struct Material {
	TextureID textures[MAX_TEXTURE];
	int nTextures;
	ShaderID vertexShader;
	ShaderID fragmentShader;

	Material(){}
	Material(const ShaderID vShader, const ShaderID fShader){
		vertexShader = vShader;
		fragmentShader = fShader;
	}

	inline void setTextures(const TextureID tex0){
		nTextures = 1;
		textures[0] = tex0;
	}
	inline void setTextures(const TextureID tex0, const TextureID tex1){
		nTextures = 2;
		textures[0] = tex0;
		textures[1] = tex1;
	}
	inline void setTextures(const TextureID tex0, const TextureID tex1, const TextureID tex2){
		nTextures = 3;
		textures[0] = tex0;
		textures[1] = tex1;
		textures[2] = tex2;
	}
};

// Blending constants
extern const int ZERO;
extern const int ONE;
extern const int SRC_COLOR;
extern const int ONE_MINUS_SRC_COLOR;
extern const int DST_COLOR;
extern const int ONE_MINUS_DST_COLOR;
extern const int SRC_ALPHA;
extern const int ONE_MINUS_SRC_ALPHA;
extern const int DST_ALPHA;
extern const int ONE_MINUS_DST_ALPHA;
extern const int SRC_ALPHA_SATURATE;

extern const int BLENDMODE_ADD;
extern const int BLENDMODE_SUBRACT;
extern const int BLENDMODE_REVERSE_SUBTRACT;

// Texture flags
#define TEX_NORMALMAP      0x1
#define TEX_NOMIPMAPPING   0x2
#define TEX_NOLINEARFILTER 0x4
#define TEX_ALPHAFADE      0x8
// Note: Don't change these values, they are purposedly set to their respective bitdepth value
#define TEX_USE_16BIT    0x10
#define TEX_USE_32BIT    0x20
#define TEX_USE_64BIT    0x40
#define TEX_USE_128BIT   0x80

#define TEX_CLAMP_S      0x100
#define TEX_CLAMP_T      0x200
#define TEX_CLAMP_R      0x400
#define TEX_CLAMP (TEX_CLAMP_S | TEX_CLAMP_T | TEX_CLAMP_R)

// Locking flags
#define LF_BLENDING         0x1
#define LF_FRAGMENT_SHADING 0x2
#define LF_VERTEX_SHADING   0x4


/* -------------------------------------------------------------------------------------------- */

class Renderer {
protected:
	Set <Texture> textures;
	int nSimultaneousTextures;
	int currentTextureUnit;
	int maxAnisotropic;
	TextureID selectedTextures[MAX_TEXTURE];
	TextureID currentTextures[MAX_TEXTURE];
	TexEnv selectedColorTexEnvs[MAX_TEXTURE];
	TexEnv currentColorTexEnvs[MAX_TEXTURE];
	TexEnv selectedAlphaTexEnvs[MAX_TEXTURE];
	TexEnv currentAlphaTexEnvs[MAX_TEXTURE];
	TexGen selectedTexGen[MAX_TEXTURE];
	TexGen currentTexGen[MAX_TEXTURE];

	Set <FragmentShader> fragmentShaders;
	ShaderID selectedFragmentShader;
	ShaderID currentFragmentShader;
	Set <VertexShader> vertexShaders;
	ShaderID selectedVertexShader;
	ShaderID currentVertexShader;

	TextureID currentRenderTarget;

	unsigned int lockFlags;

	int currentSrcFactor,  selectedSrcFactor;
	int currentDestFactor, selectedDestFactor;
	int currentBlendMode,  selectedBlendMode;
	bool currentBlendingEnabled, selectedBlendingEnabled;

	bool byteOrderRGBA;
	bool colorMipMaps;
	bool useAnisotropic;

	virtual void createTexture(Texture &texture) = 0;
	virtual void removeTexture(Texture &texture) = 0;
	virtual bool uploadTexture(Texture &texture) = 0;
	virtual void resetTextureUnits() = 0;

	virtual void createFragmentShader(FragmentShader &fragmentShader) = 0;
	virtual void removeFragmentShader(FragmentShader &fragmentShader) = 0;
	virtual void uploadFragmentShader(FragmentShader &fragmentShader) = 0;

	virtual void createVertexShader(VertexShader &VertexShader) = 0;
	virtual void removeVertexShader(VertexShader &VertexShader) = 0;
	virtual void uploadVertexShader(VertexShader &VertexShader) = 0;

	void resetToDefaults();
public:
	Renderer();
	virtual ~Renderer(){};

	void reset();
	void apply();

	void lock(const unsigned int flags);
	void unlock(const unsigned int flags = -1);

	int getSimultaneousTextures() const { return nSimultaneousTextures; }

	void setAnisotropic(const bool value){ useAnisotropic = value; }
	void setMipMapColoring(const bool value){ colorMipMaps = value; }

	void clear();
	TextureID addTexture(Image *img, unsigned int flags = 0);
	TextureID addTexture(const char *fileName, unsigned int flags = 0);
	TextureID addParticleTexture(const int size, unsigned int flags = 0);
	TextureID addEntropy2D(const int size, unsigned int flags = 0);

	TextureID addCubemap(const char *fileNegX, const char *filePosX, const char *fileNegY, const char *filePosY, const char *fileNegZ, const char *filePosZ);
	TextureID addNormalizingCubemap(const int size);

	TextureID addTexture3D(Image3D *img, unsigned int flags = 0);
	TextureID addAttenuationMap(const int size, unsigned int flags = 0);
	TextureID addDistanceMap(const int size, unsigned int flags = 0);
	TextureID addFogVolume(const int size, unsigned int flags = 0);
	TextureID addVectorField(const int size, unsigned int flags = 0);
	TextureID addEntropy3D(const int size, unsigned int flags = 0);

	TextureID addRenderTexture(const int width, const int height, bool mipmapped = true, const bool cubemap = false, RenderPixelFormat rPixelFormat = RPF_RGBA8);

	ShaderID addFragmentShader(unsigned int type, char *fileName = NULL);
	ShaderID addFragmentShader(unsigned int type, unsigned int shaderID);
	ShaderID addVertexShader(char *fileName);

protected:
	TextureID insertTexture(Texture &texture);
	void deleteTexture(const int index);

	ShaderID insertFragmentShader(FragmentShader &fragmentShader);
	void deleteFragmentShader(const int index);
	ShaderID insertVertexShader(VertexShader &VertexShader);
	void deleteVertexShader(const int index);

	virtual bool changeToDefaultRenderTarget() = 0;
public:
//	void deleteTexture(const TextureID texID);
//	int getNumberOfTextures();
	TextureID getNextTexture(TextureID currentTexture) const;

	bool changeRenderTarget(const TextureID renderTarget = TEXTURE_NONE);
	bool changeRenderFace(const TextureID renderTarget, const int renderFace);

	inline void setTextures(const TextureID tex0);
	inline void setTextures(const TextureID tex0, const TextureID tex1);
	inline void setTextures(const TextureID tex0, const TextureID tex1, const TextureID tex2);
	inline void setTextures(const TextureID tex0, const TextureID tex1, const TextureID tex2, const TextureID tex3);
	inline void setTextures(const TextureID tex0, const TextureID tex1, const TextureID tex2, const TextureID tex3, const TextureID tex4);
	inline void setTextures(const TextureID tex0, const TextureID tex1, const TextureID tex2, const TextureID tex3, const TextureID tex4, const TextureID tex5);

	inline void setTexEnvs(const TexEnv texEnv0);
	inline void setTexEnvs(const TexEnv texEnv0, const TexEnv texEnv1);
	inline void setTexEnvs(const TexEnv texEnv0, const TexEnv texEnv1, const TexEnv texEnv2);
	inline void setTexEnvs(const TexEnv texEnv0, const TexEnv texEnv1, const TexEnv texEnv2, const TexEnv texEnv3);
	inline void setTexEnvs(const TexEnv texEnv0, const TexEnv texEnv1, const TexEnv texEnv2, const TexEnv texEnv3, const TexEnv texEnv4);

	inline void setColorTexEnvs(const TexEnv texEnv0);
	inline void setColorTexEnvs(const TexEnv texEnv0, const TexEnv texEnv1);
	inline void setColorTexEnvs(const TexEnv texEnv0, const TexEnv texEnv1, const TexEnv texEnv2);
	inline void setColorTexEnvs(const TexEnv texEnv0, const TexEnv texEnv1, const TexEnv texEnv2, const TexEnv texEnv3);
	inline void setColorTexEnvs(const TexEnv texEnv0, const TexEnv texEnv1, const TexEnv texEnv2, const TexEnv texEnv3, const TexEnv texEnv4);

	inline void setAlphaTexEnvs(const TexEnv texEnv0);
	inline void setAlphaTexEnvs(const TexEnv texEnv0, const TexEnv texEnv1);
	inline void setAlphaTexEnvs(const TexEnv texEnv0, const TexEnv texEnv1, const TexEnv texEnv2);
	inline void setAlphaTexEnvs(const TexEnv texEnv0, const TexEnv texEnv1, const TexEnv texEnv2, const TexEnv texEnv3);
	inline void setAlphaTexEnvs(const TexEnv texEnv0, const TexEnv texEnv1, const TexEnv texEnv2, const TexEnv texEnv3, const TexEnv texEnv4);

	inline void setTexGen(const TexGen texGen0);
	inline void setTexGen(const TexGen texGen0, const TexGen texGen1);

	inline void setFragmentShader(const ShaderID shader);
	inline void setVertexShader(const ShaderID shader);

	inline void setBlending(const int srcFactor, const int destFactor, const int blendMode = BLENDMODE_ADD);

	void setMaterial(const Material &material);

	virtual void changeTextureUnit(const int textureUnit) = 0;
	virtual void changeTexture(const TextureID textureID) = 0;
	virtual void changeTexEnv(const TexEnv colorTexEnv, const TexEnv alphaTexEnv) = 0;
	virtual void changeTexGen(const TexGen texGen) = 0;
	virtual void changeFragmentShader(const ShaderID shader) = 0;
	virtual void changeVertexShader(const ShaderID shader) = 0;

	virtual void changeBlending(const int srcFactor, const int destFactor, const int blendMode = BLENDMODE_ADD) = 0;
	virtual void changeVSync(const bool value) = 0;
	virtual void changeAnisotropic(const TextureID textureID, const bool value) = 0;
};

/* -------------------------------------------------------------------------------------------- */

inline void Renderer::setTextures(const TextureID tex0){
	selectedTextures[0] = tex0;
}

inline void Renderer::setTextures(const TextureID tex0, const TextureID tex1){
	selectedTextures[0] = tex0;
	selectedTextures[1] = tex1;
}

inline void Renderer::setTextures(const TextureID tex0, const TextureID tex1, const TextureID tex2){
	selectedTextures[0] = tex0;
	selectedTextures[1] = tex1;
	selectedTextures[2] = tex2;
}

inline void Renderer::setTextures(const TextureID tex0, const TextureID tex1, const TextureID tex2, const TextureID tex3){
	selectedTextures[0] = tex0;
	selectedTextures[1] = tex1;
	selectedTextures[2] = tex2;
	selectedTextures[3] = tex3;
}

inline void Renderer::setTextures(const TextureID tex0, const TextureID tex1, const TextureID tex2, const TextureID tex3, const TextureID tex4){
	selectedTextures[0] = tex0;
	selectedTextures[1] = tex1;
	selectedTextures[2] = tex2;
	selectedTextures[3] = tex3;
	selectedTextures[4] = tex4;
}

inline void Renderer::setTextures(const TextureID tex0, const TextureID tex1, const TextureID tex2, const TextureID tex3, const TextureID tex4, const TextureID tex5){
	selectedTextures[0] = tex0;
	selectedTextures[1] = tex1;
	selectedTextures[2] = tex2;
	selectedTextures[3] = tex3;
	selectedTextures[4] = tex4;
	selectedTextures[5] = tex5;
}

inline void Renderer::setTexEnvs(const TexEnv texEnv0){
	selectedColorTexEnvs[0] = texEnv0;
	selectedAlphaTexEnvs[0] = texEnv0;
}

inline void Renderer::setColorTexEnvs(const TexEnv texEnv0){
	selectedColorTexEnvs[0] = texEnv0;
}

inline void Renderer::setAlphaTexEnvs(const TexEnv texEnv0){
	selectedAlphaTexEnvs[0] = texEnv0;
}


inline void Renderer::setTexEnvs(const TexEnv texEnv0, const TexEnv texEnv1){
	selectedColorTexEnvs[0] = texEnv0;
	selectedColorTexEnvs[1] = texEnv1;
	selectedAlphaTexEnvs[0] = texEnv0;
	selectedAlphaTexEnvs[1] = texEnv1;
}

inline void Renderer::setColorTexEnvs(const TexEnv texEnv0, const TexEnv texEnv1){
	selectedColorTexEnvs[0] = texEnv0;
	selectedColorTexEnvs[1] = texEnv1;
}

inline void Renderer::setAlphaTexEnvs(const TexEnv texEnv0, const TexEnv texEnv1){
	selectedAlphaTexEnvs[0] = texEnv0;
	selectedAlphaTexEnvs[1] = texEnv1;
}


inline void Renderer::setTexEnvs(const TexEnv texEnv0, const TexEnv texEnv1, const TexEnv texEnv2){
	selectedColorTexEnvs[0] = texEnv0;
	selectedColorTexEnvs[1] = texEnv1;
	selectedColorTexEnvs[2] = texEnv2;
	selectedAlphaTexEnvs[0] = texEnv0;
	selectedAlphaTexEnvs[1] = texEnv1;
	selectedAlphaTexEnvs[2] = texEnv2;
}

inline void Renderer::setColorTexEnvs(const TexEnv texEnv0, const TexEnv texEnv1, const TexEnv texEnv2){
	selectedColorTexEnvs[0] = texEnv0;
	selectedColorTexEnvs[1] = texEnv1;
	selectedColorTexEnvs[2] = texEnv2;
}

inline void Renderer::setAlphaTexEnvs(const TexEnv texEnv0, const TexEnv texEnv1, const TexEnv texEnv2){
	selectedAlphaTexEnvs[0] = texEnv0;
	selectedAlphaTexEnvs[1] = texEnv1;
	selectedAlphaTexEnvs[2] = texEnv2;
}


inline void Renderer::setTexEnvs(const TexEnv texEnv0, const TexEnv texEnv1, const TexEnv texEnv2, const TexEnv texEnv3){
	selectedColorTexEnvs[0] = texEnv0;
	selectedColorTexEnvs[1] = texEnv1;
	selectedColorTexEnvs[2] = texEnv2;
	selectedColorTexEnvs[3] = texEnv3;
	selectedAlphaTexEnvs[0] = texEnv0;
	selectedAlphaTexEnvs[1] = texEnv1;
	selectedAlphaTexEnvs[2] = texEnv2;
	selectedAlphaTexEnvs[3] = texEnv3;
}

inline void Renderer::setColorTexEnvs(const TexEnv texEnv0, const TexEnv texEnv1, const TexEnv texEnv2, const TexEnv texEnv3){
	selectedColorTexEnvs[0] = texEnv0;
	selectedColorTexEnvs[1] = texEnv1;
	selectedColorTexEnvs[2] = texEnv2;
	selectedColorTexEnvs[3] = texEnv3;
}

inline void Renderer::setAlphaTexEnvs(const TexEnv texEnv0, const TexEnv texEnv1, const TexEnv texEnv2, const TexEnv texEnv3){
	selectedAlphaTexEnvs[0] = texEnv0;
	selectedAlphaTexEnvs[1] = texEnv1;
	selectedAlphaTexEnvs[2] = texEnv2;
	selectedAlphaTexEnvs[3] = texEnv3;
}


inline void Renderer::setTexEnvs(const TexEnv texEnv0, const TexEnv texEnv1, const TexEnv texEnv2, const TexEnv texEnv3, const TexEnv texEnv4){
	selectedColorTexEnvs[0] = texEnv0;
	selectedColorTexEnvs[1] = texEnv1;
	selectedColorTexEnvs[2] = texEnv2;
	selectedColorTexEnvs[3] = texEnv3;
	selectedColorTexEnvs[4] = texEnv4;
	selectedAlphaTexEnvs[0] = texEnv0;
	selectedAlphaTexEnvs[1] = texEnv1;
	selectedAlphaTexEnvs[2] = texEnv2;
	selectedAlphaTexEnvs[3] = texEnv3;
	selectedAlphaTexEnvs[4] = texEnv4;
}

inline void Renderer::setColorTexEnvs(const TexEnv texEnv0, const TexEnv texEnv1, const TexEnv texEnv2, const TexEnv texEnv3, const TexEnv texEnv4){
	selectedColorTexEnvs[0] = texEnv0;
	selectedColorTexEnvs[1] = texEnv1;
	selectedColorTexEnvs[2] = texEnv2;
	selectedColorTexEnvs[3] = texEnv3;
	selectedColorTexEnvs[4] = texEnv4;
}

inline void Renderer::setAlphaTexEnvs(const TexEnv texEnv0, const TexEnv texEnv1, const TexEnv texEnv2, const TexEnv texEnv3, const TexEnv texEnv4){
	selectedAlphaTexEnvs[0] = texEnv0;
	selectedAlphaTexEnvs[1] = texEnv1;
	selectedAlphaTexEnvs[2] = texEnv2;
	selectedAlphaTexEnvs[3] = texEnv3;
	selectedAlphaTexEnvs[4] = texEnv4;
}

inline void Renderer::setTexGen(const TexGen texGen0){
	selectedTexGen[0] = texGen0;
}

inline void Renderer::setTexGen(const TexGen texGen0, const TexGen texGen1){
	selectedTexGen[0] = texGen0;
	selectedTexGen[1] = texGen1;
}

inline void Renderer::setFragmentShader(const ShaderID shader){
	selectedFragmentShader = shader;
}

inline void Renderer::setVertexShader(const ShaderID shader){
	selectedVertexShader = shader;
}

inline void Renderer::setBlending(const int srcFactor, const int destFactor, const int blendMode){
	selectedBlendingEnabled = true;
	selectedSrcFactor  = srcFactor;
	selectedDestFactor = destFactor;
	selectedBlendMode  = blendMode;
}

#endif // _RENDERER_H_
