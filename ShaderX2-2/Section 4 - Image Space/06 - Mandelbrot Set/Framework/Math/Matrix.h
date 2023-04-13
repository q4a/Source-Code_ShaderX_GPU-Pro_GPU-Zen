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

#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "Plane.h"
#include "Quaternion.h"

// The Matrix structure defines a 4x4 Matrix

#define POSITIVE_X 0
#define NEGATIVE_X 1
#define POSITIVE_Y 2
#define NEGATIVE_Y 3
#define POSITIVE_Z 4
#define NEGATIVE_Z 5

struct Matrix {
	float m[16];

	Matrix(){}
	Matrix(float initVal){
		for (int i = 0; i < 16; i++) m[i] = initVal;
	}
	Matrix(float im[]){
		for (int i = 0; i < 16; i++) m[i] = im[i];
	}
	Matrix(const Quaternion &q);

	void loadIdentity();
	
	void loadRotateX(const float angle);
	void loadRotateY(const float angle);
	void loadRotateZ(const float angle);
	void loadRotateXY(const float angleX, const float angleY);
	void loadRotateZXY(const float angleX, const float angleY, const float angleZ);
	void loadRotate(const Vertex &v, const float angle);

	void loadProjectionMatrix(float fov, float aspect, float zNear, float zFar);
	void loadShadowMatrix(const Plane &plane, const Vertex &lightPos);
	void loadCubemapMatrix(const unsigned int side);
	void loadCubemapMatrixOGL(const unsigned int side);


	void translate(const Vertex &v);
	void scale(const float sx, const float sy, const float sz);

	void rotateX(const float angle);
	void rotateY(const float angle);
	void rotateZ(const float angle);
	void rotateXY(const float angleX, const float angleY);
	void rotateZXY(const float angleX, const float angleY, const float angleZ);
	void rotate(const Vertex &v, const float angle);

	void transpose();

	operator float *(){
		return m;
	}
	const float &operator () (const int row, const int column) const {
		return m[row + (column << 2)];
	}
	const float &operator [] (const int index) const {
		return m[index];
	}
	void operator += (const Matrix &v);
	void operator -= (const Matrix &v);
	void operator *= (const Matrix &v);

	void operator += (const float term);
	void operator -= (const float term);
	void operator *= (const float scalar);
	void operator /= (const float dividend);
};

Matrix operator + (const Matrix &u, const Matrix &v);
Matrix operator - (const Matrix &u, const Matrix &v);
Matrix operator - (const Matrix &v);
Matrix operator * (const Matrix &u, const Matrix &v);
Vertex operator * (const Matrix &mat, const Vertex &v);
Matrix operator + (const Matrix &v, const float term);
Matrix operator - (const Matrix &v, const float term);
Matrix operator * (const float scalar, const Matrix &v);
Matrix operator * (const Matrix &v, const float scalar);
Matrix operator / (const Matrix &v, const float dividend);
Matrix operator ! (const Matrix &v);

Matrix transpose(const Matrix &u);

#endif
