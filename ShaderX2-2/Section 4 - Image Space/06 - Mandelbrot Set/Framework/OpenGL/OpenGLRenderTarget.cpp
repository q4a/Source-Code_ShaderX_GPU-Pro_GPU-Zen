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

#include "OpenGLRenderTarget.h"

#ifdef WIN32

bool OpenGLRenderTarget::bind(){
	return (wglBindTexImageARB(hPBuffer, WGL_FRONT_LEFT_ARB) == GL_TRUE);
}

bool OpenGLRenderTarget::unbind(){
	return (wglReleaseTexImageARB(hPBuffer, WGL_FRONT_LEFT_ARB) == GL_TRUE);
}

bool OpenGLRenderTarget::create(const int w, const int h, const bool mipmapped, const bool cubemap, RenderPixelFormat rPixelFormat){
	width  = w;
	height = h;

	int nBitsPerChannel = getChannelBits(rPixelFormat);

	HDC hdc = wglGetCurrentDC();
	HGLRC hglrc = wglGetCurrentContext();

    int pixelFormat;
	unsigned int nMatchingPixelFormats;

	int iAttributeIList[] = {
		WGL_SUPPORT_OPENGL_ARB,       GL_TRUE, 
		

		WGL_DRAW_TO_PBUFFER_ARB,      GL_TRUE,
		WGL_BIND_TO_TEXTURE_RGBA_ARB, GL_TRUE,


		WGL_PIXEL_TYPE_ARB,           isFloatPixelFormat(rPixelFormat)? WGL_TYPE_RGBA_FLOAT_ATI : WGL_TYPE_RGBA_ARB,
		WGL_ACCELERATION_ARB,         WGL_FULL_ACCELERATION_ARB,
		
		WGL_RED_BITS_ARB,             nBitsPerChannel,
		WGL_GREEN_BITS_ARB,           nBitsPerChannel,
		WGL_BLUE_BITS_ARB,            nBitsPerChannel,
		WGL_ALPHA_BITS_ARB,           nBitsPerChannel,

		WGL_DEPTH_BITS_ARB,           24,
		WGL_STENCIL_BITS_ARB,         0,
		WGL_DOUBLE_BUFFER_ARB,        GL_FALSE,

		0, 0 // End of array
	};

	wglChoosePixelFormatARB(hdc, iAttributeIList, NULL, 1, &pixelFormat, &nMatchingPixelFormats);

/*	if (nMatchingPixelFormats <= 0){
		::MessageBox(NULL, "No matching format", "Error", MB_OK);
	}*/

	
	int iPBufferAttributes[] = {
		WGL_PBUFFER_LARGEST_ARB,   GL_TRUE,
		WGL_TEXTURE_FORMAT_ARB,    WGL_TEXTURE_RGBA_ARB,
		WGL_TEXTURE_TARGET_ARB,    cubemap? WGL_TEXTURE_CUBE_MAP_ARB : WGL_TEXTURE_2D_ARB,
		WGL_MIPMAP_TEXTURE_ARB,    mipmapped? GL_TRUE : GL_FALSE,
		0, 0,
	};


	glGenTextures(1, &texID);
	glBindTexture(cubemap? GL_TEXTURE_CUBE_MAP_ARB : GL_TEXTURE_2D, texID);

	hPBuffer = wglCreatePbufferARB(hdc, pixelFormat, width, height, iPBufferAttributes);

	hPdc = wglGetPbufferDCARB(hPBuffer);
	hPrc = wglCreateContext(hPdc);

	wglShareLists(hglrc, hPrc);

	GLenum target = cubemap? GL_TEXTURE_CUBE_MAP_ARB : GL_TEXTURE_2D;
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, mipmapped? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	if (mipmapped) glTexParameteri(target, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
	
//	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0, width, height, 0);



	return true;
}

bool OpenGLRenderTarget::destroy(){
	wglDeleteContext(hPrc);
	wglReleasePbufferDCARB(hPBuffer, hPdc);
	wglDestroyPbufferARB(hPBuffer);
	return true;
}

bool OpenGLRenderTarget::setCurrent(){
    wglMakeCurrent(hPdc, hPrc);
	return true;
}

bool OpenGLRenderTarget::unsetCurrent(){
	//glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, 192, width, height, 0);
	//glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 192, width, 64);
	return true;
}

bool OpenGLRenderTarget::isLost(){
	int lost = 1;

	wglQueryPbufferARB(hPBuffer, WGL_PBUFFER_LOST_ARB, &lost);
	
	return (lost != 0);
}

bool OpenGLRenderTarget::setRenderFace(const int face){
	int iPBufferAttributes[] = {
		WGL_CUBE_MAP_FACE_ARB, WGL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + face,
		0, 0
	};

    return (wglSetPbufferAttribARB(hPBuffer, iPBufferAttributes) == TRUE);
}

#endif // WIN32







#ifdef LINUX

bool OpenGLRenderTarget::bind(){
	return true;
}

bool OpenGLRenderTarget::unbind(){
	return true;
}

bool OpenGLRenderTarget::create(const int w, const int h, const bool mipmapped, const bool cubemap, RenderPixelFormat rPixelFormat){
	//GLXPbuffer pBuffer = glXCreatePbuffer(Display *dpy, GLXFBConfig config, const int *attrib_list);

	return true;
}

bool OpenGLRenderTarget::destroy(){
	//glXDestroyPbuffer(Display *dpy, GLXPbuffer pbuf);

	return true;
}

bool OpenGLRenderTarget::setCurrent(){
	return true;
}

bool OpenGLRenderTarget::unsetCurrent(){
	return true;
}

bool OpenGLRenderTarget::isLost(){
	return false;
}

bool OpenGLRenderTarget::setRenderFace(const int face){
    return true;
}

#endif // LINUX

