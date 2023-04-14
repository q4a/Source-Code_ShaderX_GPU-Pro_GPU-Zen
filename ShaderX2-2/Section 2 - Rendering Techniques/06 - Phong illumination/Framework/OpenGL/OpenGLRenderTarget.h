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

#ifndef _OPENGLRENDERERTARGET_H_
#define _OPENGLRENDERERTARGET_H_


#include "../RenderTarget.h"
#include "glExtensions.h"

class OpenGLRenderTarget : public RenderTarget {
protected:
#if defined(WIN32)
	HPBUFFERARB hPBuffer;
	HDC hPdc;
	HGLRC hPrc;
#elif defined(LINUX)

#endif
	
public:
	bool bind();
	bool unbind();
	
	bool create(const int w, const int h, const bool mipmapped, const bool cubemap, RenderPixelFormat rPixelFormat);
	bool destroy();

	bool setCurrent();
	bool unsetCurrent();

	bool isLost();
	bool setRenderFace(const int face);
};

#endif // _OPENGLRENDERERTARGET_H_
