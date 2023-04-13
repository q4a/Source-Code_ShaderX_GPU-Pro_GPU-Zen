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


#ifndef _OPENGLRENDERER_H_
#define _OPENGLRENDERER_H_

#include "../Renderer.h"
#include "glExtensions.h"

class OpenGLRenderer : public Renderer {
public:

#if defined(WIN32)
	HDC hdc;
	HGLRC hglrc;
	OpenGLRenderer(HDC hDc, HGLRC hGlrc);

#elif defined(LINUX)

    GLXContext glxc;
	Display *display;
	OpenGLRenderer(GLXContext glXc, Display *disp);
#endif

protected:
	void init();
	static GLenum targets[];

	void createTexture(Texture &texture);
	void removeTexture(Texture &texture);
	bool uploadTexture(Texture &texture);
	void resetTextureUnits();

	void createFragmentShader(FragmentShader &fragmentShader);
	void removeFragmentShader(FragmentShader &fragmentShader);
	void uploadFragmentShader(FragmentShader &fragmentShader);

	void createVertexShader(VertexShader &vertexShader);
	void removeVertexShader(VertexShader &vertexShader);
	void uploadVertexShader(VertexShader &vertexShader);

	void configureTexEnv(const TexEnv texEnv, bool isRGB);
public:

	bool changeToDefaultRenderTarget();

	void changeTextureUnit(const int textureUnit);
	void changeTexture(const TextureID textureID);
	void changeTexEnv(const TexEnv colorTexEnv, const TexEnv alphaTexEnv);
	void changeTexGen(const TexGen texGen);
	void changeFragmentShader(const ShaderID shader);
	void changeVertexShader(const ShaderID shader);

	void changeBlending(const int srcFactor, const int destFactor, const int blendMode);
	void changeVSync(const bool value);
	void changeAnisotropic(const TextureID textureID, const bool value);
};


#endif // _OPENGLRENDERER_H_
