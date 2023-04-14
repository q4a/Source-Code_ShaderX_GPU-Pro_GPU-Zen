                  /*--.          .-"-. 
                 /   ._.        / � ` \
                 \_  (__\       \_�v�_/
                 //   \\        //   \\
                ((     ))      ((     ))
 ��������������--""---""--����--""---""--��������������������������������������������
 �                 |||            |||                                               �
 �  Author's note:  |              |                                                �
 �                                                                                  �
 �  This file is part of work done by Emil Persson, aka Humus.                      �
 �  You're free to use the code in any way you like, modified, unmodified           �
 �  or cut'n'pasted into your own work. But for the good of the whole               �
 �  programming community I will ask you to as long as possible and to the          �
 �  extent you find suitable to follow these points:                                �
 �   * If you use this file and it's contents unmodified, please leave this note.   �
 �   * If you use a major part of the file in your app, please credit the author(s) �
 �     in one way or the other as you find suitable, for instance a small notice    �
 �     in the readme file may do, or perhaps a link to the authors site.            �
 �   * Share your work and ideas too as much as you can.                            �
 �    _______                                                                       �
 �  /` _____ `\;,    Humus                                                          �
 � (__(^===^)__)';,  emiper-8@student.luth.se                         ___           �
 �   /  :::  \   ,;  ICQ #47010716                                  /^   ^\         �
 �  |   :::   | ,;'                                                ( �   � )        �
 ���'._______.'`����������������������������������������������--�oOo--(_)--oOo�--��*/

#ifndef _OPENGLAPP_H_
#define _OPENGLAPP_H_

#include "RenderApp.h"
#include "../OpenGL/OpenGLRenderer.h"

class OpenGLApp : public RenderApp {
protected:
	HDC hdc;
	HGLRC hglrc;
	int pixelFormat;
public:
	String getErrorString();
	bool processCommand(const char *cmd, String &response);

	bool initAPI(HWND hwnd, int fsaaLevel, bool fullscreen, unsigned int refreshRate);
	virtual bool load();
	virtual bool unload();
	virtual void beginFrame();
	virtual bool drawFrame();
	virtual void endFrame();
	bool closeAPI(HWND hwnd);

	void setViewport(int cx, int cy);
	void screenShot();
	void drawText(char *str, float x, float y, float charWidth, float charHeight);
};




#endif // _OPENGLAPP_H_
