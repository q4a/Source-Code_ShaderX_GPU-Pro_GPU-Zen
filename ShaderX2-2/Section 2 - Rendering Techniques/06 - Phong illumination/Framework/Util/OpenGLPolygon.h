                  /*--.          .-"-. 
                 /   ._.        / � ` \
                 \_  (__\       \_�v�_/
                 //   \\        //   \\
                ((     ))      ((     ))
 ��������������--""---""--����--""---""--��������������������������������������������
 �                 |||            |||                                               �
 �  Authors note:   |              |                                                �
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

#ifndef _OPENGLPOLYGON_H_
#define _OPENGLPOLYGON_H_

#include "Polygon.h"
#include "../OpenGL/glExtensions.h"

class OpenGLPolygon : public Polygon {
public:
	OpenGLPolygon(const int size) : Polygon(size){
		
	}

	void draw(Renderer *renderer);
	class Polygon *create(const int size) const;

};

class Polygon *openGLPolyCreator(const int size);

#endif // _OPENGLPOLYGON_H_
