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

#include "OpenGLApp.h"

#pragma comment (lib, "opengl32.lib")

String OpenGLApp::getErrorString(){
	String error;
	GLenum err;
	bool first = true;

	while ((err = glGetError()) != GL_NO_ERROR){
		if (first){
			first = false;
		} else {
			error += ',';
		}

		switch(err){
		case GL_INVALID_ENUM:
			error += "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error += "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			error += "GL_INVALID_OPERATION";
			break;
		case GL_STACK_OVERFLOW:
			error += "GL_STACK_OVERFLOW";
			break;
		case GL_STACK_UNDERFLOW:
			error += "GL_STACK_UNDERFLOW";
			break;
		case GL_OUT_OF_MEMORY:
			error += "GL_OUT_OF_MEMORY";
			break;
		default:
			error.sprintf("Unknown error: 0x%X", err);
		}
	}

	return error;
}

bool OpenGLApp::processCommand(const char *cmd, String &response){
	char str[256];

	Tokenizer tok;
	tok.setString(cmd);
	tok.next(str);

	if (stricmp(str, "extensions") == 0){
		response = (char *) glGetString(GL_EXTENSIONS);
		response.replace(" ", "\r\n");
	} else if (stricmp(str, "vendor") == 0){
		response = (char *) glGetString(GL_VENDOR);
	} else if (stricmp(str, "renderer") == 0){
		response = (char *) glGetString(GL_RENDERER);
	} else if (stricmp(str, "version") == 0){
		response = (char *) glGetString(GL_VERSION);
	} else {
		return RenderApp::processCommand(cmd, response);
	}

	return true;
}


class PFormatWnd : public CFrameWnd {
public:
	PFormatWnd(){
		Create(NULL, "PFormat", WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_DISABLED, CRect(0,0,10,10));
	}

	void initEntryPoints(const PIXELFORMATDESCRIPTOR &pfd){
		HDC hdc = ::GetDC(m_hWnd);
	
		int nPixelFormat = ChoosePixelFormat(hdc, &pfd);
		SetPixelFormat(hdc, nPixelFormat, &pfd);
		
		HGLRC hglrc = wglCreateContext(hdc);
		wglMakeCurrent(hdc, hglrc);

		initExtensions(hdc);
		
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(hglrc);
		::ReleaseDC(m_hWnd, hdc);
	}
};

bool OpenGLApp::initAPI(HWND hwnd, int fsaaLevel, bool fullscreen, unsigned int refreshRate){
	PixelFormat pf;

	selectPixelFormat(pf);

	int bpp = pf.alphaBits > 0? 32 : 24;

    static PIXELFORMATDESCRIPTOR pfd = {
        sizeof (PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		bpp,
		0, 0, 0, 0, 0, 0,
		0, 0,
		0, 0, 0, 0, 0,
		pf.depthBits,
		pf.stencilBits,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
    };



	PFormatWnd *pFormatWnd = new PFormatWnd();
	pFormatWnd->initEntryPoints(pfd);
	pFormatWnd->CloseWindow();
	delete pFormatWnd;


	unsigned int nMatchingPixelFormats;
	hdc = ::GetDC(hwnd);

	float fAttribs[256];
	int iAttribs[256] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
		WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,

		WGL_RED_BITS_ARB,       pf.redBits,
		WGL_GREEN_BITS_ARB,     pf.greenBits,
		WGL_BLUE_BITS_ARB,      pf.blueBits,
		WGL_ALPHA_BITS_ARB,     pf.alphaBits,
		WGL_DEPTH_BITS_ARB,     pf.depthBits,
		WGL_STENCIL_BITS_ARB,   pf.stencilBits,
	};

	int   *iAtt = iAttribs + 18;
	float *fAtt = fAttribs;

	if (WGL_ARB_multisample && fsaaLevel > 0){
		*iAtt++ = WGL_SAMPLE_BUFFERS_ARB;
		*iAtt++ = GL_TRUE;

		*iAtt++ = WGL_SAMPLES_ARB;
		*iAtt++ = fsaaLevel;
	}

	*iAtt++ = 0;
	*fAtt++ = 0;

	if (!WGL_ARB_pixel_format || !wglChoosePixelFormatARB(hdc, iAttribs, fAttribs, 1, &pixelFormat, &nMatchingPixelFormats) || nMatchingPixelFormats == 0){
		pixelFormat = ChoosePixelFormat(hdc, &pfd);
	}

    SetPixelFormat(hdc, pixelFormat, &pfd);

	hglrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hglrc);

	initExtensions(hdc);

	// Set some of my preferred defaults
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glFrontFace(GL_CW);
	static GLfloat r[] = {0.0f, 0.0f, 1.0f, 0.0f};
	static GLfloat q[] = {0.0f, 0.0f, 0.0f, 1.0f};
	glTexGenfv(GL_R, GL_OBJECT_PLANE, r);
	glTexGenfv(GL_Q, GL_OBJECT_PLANE, q);
	glTexGenfv(GL_R, GL_EYE_PLANE, r);
	glTexGenfv(GL_Q, GL_EYE_PLANE, q);


	if (WGL_ARB_multisample && fsaaLevel > 0){
		glEnable(GL_MULTISAMPLE_ARB);
	}


	renderer = new OpenGLRenderer(hdc, hglrc);

	return true;
}

bool OpenGLApp::load(){
	return true;
}

bool OpenGLApp::unload(){
	return true;
}

void OpenGLApp::beginFrame(){
}

bool OpenGLApp::drawFrame(){
	return false;
}

void OpenGLApp::endFrame(){
	SwapBuffers(hdc);
}

bool OpenGLApp::closeAPI(HWND hwnd){
	renderer->clear();
	delete renderer;

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hglrc);
	::ReleaseDC(hwnd, hdc);

	return true;
}

void OpenGLApp::setViewport(int cx, int cy){
	glViewport(0, 0, cx, cy);
}


#include <shlobj.h>

void OpenGLApp::screenShot(){
	char path[MAX_PATH];
	SHGetSpecialFolderPath(NULL, path, CSIDL_DESKTOPDIRECTORY, FALSE);

	FILE *file;
	int pos = strlen(path);
#ifndef NO_PNG
	strcpy(path + pos, "\\ScreenShot00.png");
#else
	strcpy(path + pos, "\\ScreenShot00.tga");
#endif // NO_PNG
	pos += 11;
	
	int i = 0;
	do {
		path[pos]     = '0' + (i / 10);
		path[pos + 1] = '0' + (i % 10);
		
		if (file = fopen(path, "r")){
			fclose(file);
		} else {
			CRect rect;
			m_pMainWnd->GetClientRect(rect);
			int w = rect.right - rect.left;
			int h = rect.bottom - rect.top;

			unsigned char *pixels  = new unsigned char[w * h * 4];
			unsigned char *flipped = new unsigned char[w * h * 4];;

			glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, flipped);
			for (int y = 0; y < h; y++){
				memcpy(pixels + y * w * 4, flipped + (h - y - 1) * w * 4, w * 4);				
			}
			delete flipped;

			Image img;

			img.loadFromMemory(pixels, w, h, FORMAT_RGBA8, true);
			img.saveImage(path, true);

			break;
		}
		i++;
	} while (i < 100);

}

void OpenGLApp::drawText(char *str, float x, float y, float charWidth, float charHeight){
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,1,1,0,-1,100);

    float tx,ty, startx = x;

	glBegin(GL_QUADS);
    while (*str){
        if (*str == '\n'){
            y += charHeight;
            x = startx;
        } else {
            tx = float(((unsigned char) *str) & 15) / 16;
            ty = float(((unsigned char) *str) >> 4) / 16;

			glTexCoord2f(tx,ty);
			glVertex2f(x, y);
			
			glTexCoord2f(tx + 1.0f / 16,ty);
			glVertex2f(x + charWidth, y);
			
			glTexCoord2f(tx + 1.0f / 16,ty + 1.0f / 16);
			glVertex2f(x + charWidth, y + charHeight);
			
			glTexCoord2f(tx,ty + 1.0f / 16);
			glVertex2f(x, y + charHeight);

            x += charWidth;
        }
        str++;
    }
    glEnd();
}
