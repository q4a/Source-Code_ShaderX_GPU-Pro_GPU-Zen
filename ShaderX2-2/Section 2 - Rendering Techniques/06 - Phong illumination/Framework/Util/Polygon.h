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

#ifndef _POLYGON_H_
#define _POLYGON_H_

#include "../Renderer.h"
#include "../Math/Plane.h"

typedef unsigned int PolygonFlags;

#define PF_NONE        0
#define PF_DOUBLESIDED 0x1
#define PF_UNLIT       0x2
#define PF_TRANSLUCENT 0x4
#define PF_NONBLOCKING 0x8
#define PF_PORTAL      0x10

class Polygon {
protected:
	Vertex *vertices;
	unsigned int nVertices;

	Plane plane;
	Vertex origin, s, t;
	float scaleS, scaleT;

	PolygonFlags flags;
	Material *material;

public:
	Polygon(){}
	Polygon(const int size);
	virtual ~Polygon();

	PolygonFlags getFlags() const { return flags; }
	const Plane &getPlane() const { return plane; }

	void setVertex(const unsigned int index, const Vertex &v){ vertices[index] = v; }
	void insertVertex(const unsigned int index, const Vertex &v);

	unsigned int getnVertices() const { return nVertices; }
	const Vertex &getVertex(const int index) const { return vertices[index]; }
	const Vertex &getLastVertex() const { return vertices[nVertices - 1]; }
	const Vertex *getVertices() const { return vertices; }
	
	void setFlags(const PolygonFlags Flags);
	void setFlags(const PolygonFlags Flags, bool value);
	
	void setTexCoordSystem(const Vertex &Origin, const Vertex &S, const Vertex &T);
	const Vertex &getOrigin() const { return origin; }
	const Vertex &getS() const { return s; }
	const Vertex &getT() const { return t; }
	float getScaleS() const { return scaleS; }
	float getScaleT() const { return scaleT; }

	void setMaterial(Material *mat){ material = mat; }
	Material *getMaterial() const { return material; }

	virtual class Polygon *create(const int size) const;
	virtual void initialize(class Polygon *poly);
	void copyVertex(Polygon *&poly, unsigned int destIndex, unsigned int srcIndex);
	void setInterpolatedVertex(Polygon *&poly, unsigned int destIndex, int srcIndex0, int srcIndex1, float k);
	void finalize();

	bool similar(Polygon *poly);

	RELATION polygonRelation(const Polygon &other);
	RELATION split(const Plane &splitPlane, Polygon *&back, Polygon *&front);
	bool linePassesThrough(const Vertex &v0, const Vertex &v1, Vertex *result = NULL);

	virtual void draw(Renderer *renderer){}
};

class Polygon *polyCreator(const int size);

#endif // _POLYGON_H_
