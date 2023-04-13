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
#include "../CPU.h"

#include <unistd.h>
#include <stdio.h>

bool toggleFullscreen;

void OpenGLApp::setKey(UINT key, bool value){
	keys[key & 0xFFFF] = value;

	if (value){
		if (key == showFpsKey){
			showFPS = !showFPS;
		} else if (key == screenshotKey){
			screenShot();
		}
	}
}

void OpenGLApp::controls(){
    float sqrLen;

	Vertex dir(0,0,0);

	Vertex dx(modelView[0], modelView[4], modelView[8]);
	Vertex dy(modelView[1], modelView[5], modelView[9]);
	Vertex dz(modelView[2], modelView[6], modelView[10]);

	if (keys[leftKey ]) dir -= dx;
	if (keys[rightKey]) dir += dx;

	if (keys[backwardsKey]) dir -= dz;
	if (keys[forwardsKey ]) dir += dz;

	if (keys[downKey]) dir -= dy;
	if (keys[upKey  ]) dir += dy;

	if (keys[resetKey]) resetCamera();

	if ((sqrLen = lengthSqr(dir)) != 0){
		dir *= rsqrtf(sqrLen);
	}

	processMovement(position + time * speed * dir);
}

void OpenGLApp::processMovement(const Vertex &newPosition){
	position = newPosition;
}

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
		//return RenderApp::processCommand(cmd, response);
		/*bool RenderApp::processCommand(const char *cmd, String &response){
	char str[256];

	Tokenizer tok;
	tok.setString(cmd);
	tok.next(str);

	if (stricmp(str, "pos") == 0){
		response.sprintf("(%f, %f, %f)", position.x, position.y, position.z);
	} else if (stricmp(str, "setpos") == 0){
		tok.next(str);
		position.x = (float) atof(str);
		tok.next(str);
		if (*str == ',') tok.next(str);
		position.y = (float) atof(str);
		tok.next(str);
		if (*str == ',') tok.next(str);
		position.z = (float) atof(str);
	} else if (stricmp(str, "angles") == 0){
		response.sprintf("(%f, %f, %f)", Wx, Wy, Wz);
	} else if (stricmp(str, "setangles") == 0){
		tok.next(str);
		Wx = (float) atof(str);
		tok.next(str);
		if (*str == ',') tok.next(str);
		Wy = (float) atof(str);
		tok.next(str);
		if (*str == ',') tok.next(str);
		Wz = (float) atof(str);
	} else {
		return false;
	}

	return true;
}
*/
	}

	return true;
}


bool OpenGLApp::initAPI(Display *disp, int scr){
	display = disp;
	screen = scr;
    Atom wmDelete;

	PixelFormat pf;
	memset(&pf, 0, sizeof(pf));

	selectPixelFormat(pf);

	int iAttribs[256] = {
		GLX_RGBA,
		GLX_DOUBLEBUFFER,
		GLX_RED_SIZE,     pf.redBits,
		GLX_GREEN_SIZE,   pf.greenBits,
		GLX_BLUE_SIZE,    pf.blueBits,
		GLX_ALPHA_SIZE,   pf.alphaBits,
		GLX_DEPTH_SIZE,   pf.depthBits,
		GLX_STENCIL_SIZE, pf.stencilBits
	};

	int *iAtt = iAttribs + 14;

	/*if (GLX_ARB_multisample && fsaaLevel > 0){
		*iAtt++ = 100000;
		*iAtt++ = GL_FALSE;

		*iAtt++ = 100001;
		*iAtt++ = 0;//fsaaLevel;
	}*/

	*iAtt++ = None;

	if (toggleFullscreen) fullscreen = !fullscreen;
	captureMouse = fullscreen;


    XVisualInfo *vi = glXChooseVisual(display, screen, iAttribs);
	if (vi == NULL){
		printf("Error: Couldn't set visual\n");
		return false;
	}

	modes = new DisplayModeHandler(display, screen);
	int x,y,w,h;
	if (fullscreen){
		modes->setDisplayMode(width, height, 32, 85);
		x = 0;
		y = 0;
		w = width;
		h = height;
	} else {
		x = left;
		y = top;
		w = right - left;
		h = bottom - top;
	}
	middleX = w / 2;
	middleY = h / 2;

    ctx = glXCreateContext(display, vi, 0, GL_TRUE);


	XSetWindowAttributes attr;
	attr.colormap = XCreateColormap(display, RootWindow(display, screen), vi->visual, AllocNone);

	attr.border_pixel = 0;
	attr.override_redirect = fullscreen;
    attr.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | PointerMotionMask | StructureNotifyMask;

	window = XCreateWindow(display, RootWindow(display, vi->screen),
			x, y, w, h, 0, vi->depth, InputOutput, vi->visual,
			CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect, &attr);

	if (!fullscreen){
        wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", True);
        XSetWMProtocols(display, window, &wmDelete, 1);
		char *title = "OpenGL";
        XSetStandardProperties(display, window, title, title, None, NULL, 0, NULL);
	}
    XMapRaised(display, window);

	Pixmap blank;
	XColor dummy;
	char data = 0;

	// Create a blank cursor for cursor hiding
	blank = XCreateBitmapFromData(display, window, &data, 1, 1);
	blankCursor = XCreatePixmapCursor(display, blank, blank, &dummy, &dummy, 0, 0);
	XFreePixmap(display, blank);


	XGrabKeyboard(display, window, True, GrabModeAsync, GrabModeAsync, CurrentTime);
	if (captureMouse) XGrabPointer(display, window, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, window, blankCursor, CurrentTime);


    glXMakeCurrent(display, window, ctx);

	//printf((char *) glXGetClientString(display, GLX_EXTENSIONS));


	initExtensions(display, screen);

	// Set some of my preferred defaults
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glFrontFace(GL_CW);
	static GLfloat r[] = {0.0f, 0.0f, 1.0f, 0.0f};
	static GLfloat q[] = {0.0f, 0.0f, 0.0f, 1.0f};
	glTexGenfv(GL_R, GL_OBJECT_PLANE, r);
	glTexGenfv(GL_Q, GL_OBJECT_PLANE, q);
	glTexGenfv(GL_R, GL_EYE_PLANE, r);
	glTexGenfv(GL_Q, GL_EYE_PLANE, q);


	/*if (WGL_ARB_multisample && fsaaLevel > 0){
		glEnable(GL_MULTISAMPLE_ARB);
	}*/



	renderer = new OpenGLRenderer(ctx, display);

	renderer->setAnisotropic(anisotropic);

	if (captureMouse) XWarpPointer(display, None, window, 0,0,0,0, middleX, middleY);

	return true;
}

bool OpenGLApp::load(){
	return true;
}

bool OpenGLApp::unload(){
	return true;
}

bool OpenGLApp::drawFrame(){
	return false;
}

bool OpenGLApp::closeAPI(){
	renderer->clear();
	delete renderer;

    glXMakeCurrent(display, None, NULL);
    glXDestroyContext(display, ctx);

	if (fullscreen)	modes->resetDisplayMode();
	delete modes;

	return true;
}

void OpenGLApp::setViewport(int cx, int cy){
	glViewport(0, 0, cx, cy);
}


void OpenGLApp::screenShot(){
	FILE *file;

	int i = 0;
	do {
		String path = screenshotPath;
#ifndef NO_PNG
		path.sprintf("/ScreenShot%d%d.png", i / 10, i %10);
#else
		path.sprintf("/ScreenShot%d%d.tga", i / 10, i %10);
#endif // NO_PNG

		if ((file = fopen(path, "r")) != NULL){
			fclose(file);
		} else {
			int w, h;
			if (fullscreen){
				w = width;
				h = height;
			} else {
				w = right - left;
				h = bottom - top;
			}

			unsigned char *pixels  = new unsigned char[w * h * 4];
			unsigned char *flipped = new unsigned char[w * h * 4];

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

bool OpenGLApp::InitInstance(const String &commandLine){
	m_lpCmdLine = commandLine;

	cpuHz = getHz();
	memset(keys, 0, sizeof(keys));

	leftKey       = XK_Left;
	rightKey      = XK_Right;
	upKey         = XK_Control_R;
	downKey       = XK_Shift_R;
	forwardsKey   = XK_Up;
	backwardsKey  = XK_Down;
	resetKey      = XK_Return;
	screenshotKey = XK_F9;
	showFpsKey    = XK_space;
	consoleKey    = XK_Tab;
	demoModeKey   = XK_f;

	textTexture = TEXTURE_NONE;
	speed = 1024;
	currTime = 0;

	resetCamera();

	showFPS = true;
	showDebugInfo = false;


	invertMouse = true;
	mouseSpeed = 5;
	width  = 1024;
	height = 768;
	left = 0;
	top  = 0;
	right  = width;
	bottom = height;
	fsaaLevel = 0;
	fullscreen = false;
	anisotropic = false;

	home = getenv("HOME");

	screenshotPath = home;
	screenshotPath += "/Desktop";

	config.loadFromFile(home + "/.humusdemos.conf");

	config.getBool("InvertMouse", invertMouse);
	config.getBool("FullScreen",  fullscreen);
	config.getBool("Anisotropic", anisotropic);

	config.getInt("Width",  width);
	config.getInt("Height", height);
	config.getInt("WindowedLeft",   left);
	config.getInt("WindowedTop",    top);
	config.getInt("WindowedRight",  right);
	config.getInt("WindowedBottom", bottom);
	config.getInt("MouseSpeed", mouseSpeed);

	return true;
}

bool OpenGLApp::ExitInstance(){
	config.setBool("InvertMouse", invertMouse);
	config.setBool("FullScreen",  fullscreen);
	config.setBool("Anisotropic", anisotropic);

	config.setInt("Width",  width);
	config.setInt("Height", height);
	config.setInt("WindowedLeft",   left);
	config.setInt("WindowedTop",    top);
	config.setInt("WindowedRight",  right);
	config.setInt("WindowedBottom", bottom);
	config.setInt("MouseSpeed", mouseSpeed);

	config.writeToFile(home + "/.humusdemos.conf");

	return true;
}

bool OpenGLApp::OnIdle(long lCount){
	prev = curr;
	curr = getCycleNumber();
	time = (float) (double(curr - prev) / cpuHz);
	currTime += time;

	controls();

	bool result = drawFrame();

	if (showFPS && (textTexture != TEXTURE_NONE)){
		renderer->setTextures(textTexture);
		renderer->setTexEnvs(TEXENV_REPLACE);
		renderer->setBlending(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
		renderer->apply();

		static float fps[15];
		static int currPos = 0;

		fps[currPos] = 1.0f / time;
		currPos++;
		if (currPos > 14) currPos = 0;

		// Apply a median filter to get rid of temporal peeks
		float min = 0, cmin;
		for (int i = 0; i < 8; i++){
			cmin = 1e30f;
			for (int j = 0; j < 15; j++){
				if (fps[j] > min && fps[j] < cmin){
					cmin = fps[j];
				}
			}
			min = cmin;
		}

		String str;
		str.sprintf("%d", (int) (min + 0.5f));

		drawText((char *) (const char *) str, 0.02f, 0.02f, 0.035f, 0.05f);
	}

	if (showDebugInfo && (textTexture != TEXTURE_NONE)){
		renderer->setTextures(textTexture);
		renderer->setTexEnvs(TEXENV_REPLACE);
		renderer->setBlending(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
		renderer->apply();

		if (!debugString.isEmpty()){
			drawText((char *) (const char *) ("Debug:\n" + debugString), 0.02f, 0.3f, 0.025f, 0.04f);
		}

		String error = getErrorString();

		if (!error.isEmpty()){
			drawText((char *) (const char *) ("Errors:\n" + error), 0.02f, 0.6f, 0.025f, 0.04f);
		}
	}

	glXSwapBuffers(display, window);

	return result;
}


int OpenGLApp::Run(){
	XEvent event;
	bool done = false;

	long idleCount = 0;
	unsigned int key;

	curr = getCycleNumber();

    do {
		while (XPending(display) > 0){
			idleCount = 0;

			XNextEvent(display, &event);
			switch (event.type){
			case Expose:
				if (event.xexpose.count != 0) break;
				break;
			case ConfigureNotify:
				setViewport(event.xconfigure.width, event.xconfigure.height);
				if (!fullscreen){
					left   = event.xconfigure.x;
					right  = event.xconfigure.width + left;
					top    = event.xconfigure.y;
					bottom = event.xconfigure.height + top;
				}
				middleX = event.xconfigure.width  / 2;
				middleY = event.xconfigure.height / 2;
				break;
			case ButtonPress:
				if (!captureMouse){
					XGrabPointer(display, window, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, window, blankCursor, CurrentTime);
					XWarpPointer(display, None, window, 0,0,0,0, middleX, middleY);
					captureMouse = true;
				}
				break;
			case KeyPress:
				key = XLookupKeysym(&event.xkey, 0);
				setKey(key, true);
				if (key == XK_Escape){
					if (fullscreen || !captureMouse){
						done = true;
					} else {
						XUngrabPointer(display, CurrentTime);
						captureMouse = false;
					}
				} else if (key == XK_Return && keys[XK_Alt_L]){
					toggleFullscreen = true;
					done = true;
				}
				break;
			case KeyRelease:
				key = XLookupKeysym(&event.xkey, 0);
				setKey(key, false);
				break;
			case MotionNotify:
				if (captureMouse){
					static bool changeMouse;
					float mouseSensibility = 0.0005f * mouseSpeed;

					rotateView(mouseSensibility * (middleY - event.xmotion.y) * (invertMouse? 1 : -1),
							mouseSensibility * (middleX - event.xmotion.x));

					if (changeMouse = !changeMouse) XWarpPointer(display, None, window, 0,0,0,0, middleX, middleY);
				}
				break;
			case ClientMessage:
				if (*XGetAtomName(display, event.xclient.message_type) == *"WM_PROTOCOLS"){
					done = true;
				}
				break;
			default:
				break;
			}
		}

		if (!done){
			if (!OnIdle(idleCount++)) done = true;
		}

    } while (!done);

	return 0;
}

extern OpenGLApp *mainApp;

int main(int argc, char **argv){
	String cwd(*argv);

	unsigned int index;
	if (cwd.find('/', &index)){
		cwd.setLength(index);
		chdir(cwd);
	}
/*	FILE *file = fopen("/home/humus/tjoho.txt", "wb");
	fwrite((const char *) cwd, cwd.getLength(), 1, file);
	fclose(file);
	*/


	String cmdLine;
	for (int i = 1; i < argc; i++){
		if (i != 1) cmdLine += " ";
		cmdLine += argv[i];
	}

	Display *display;
	int screen;

	initCPU();

	mainApp->InitInstance(cmdLine);
	mainApp->init();

	do {
		display = XOpenDisplay(0);
		screen = DefaultScreen(display);

		if (mainApp->initAPI(display, screen)){

			if (mainApp->load()){
				toggleFullscreen = false;
				mainApp->Run();

				mainApp->unload();
			}

			mainApp->closeAPI();
		}

		XCloseDisplay(display);
	} while (toggleFullscreen);

	mainApp->exit();
	mainApp->ExitInstance();

	return 0;
}
