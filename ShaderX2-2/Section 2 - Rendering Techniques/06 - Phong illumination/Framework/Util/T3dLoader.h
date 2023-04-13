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


#ifndef _T3DLOADER_H_
#define _T3DLOADER_H_

#include "Tokenizer.h"
#include "Stack.h"
#include <string.h>

#include "Polygon.h"

typedef class Polygon *(*PolygonCreator)(const int size);
typedef bool (*PolygonFilter)(const int flags);

#define PF_Invisible        1
#define PF_Masked           2
#define PF_Translucent      4
#define PF_NotSolid         8
#define PF_Environment      16
#define PF_ForceViewZone    16
#define PF_Semisolid        32
#define PF_Modulated        64
#define PF_FakeBackdrop     128
#define PF_TwoSided         256
#define PF_AutoUPan         512
#define PF_AutoVPan         1024
#define PF_NoSmooth         2048
#define PF_BigWavy          4096
#define PF_SpecialPoly      4096
#define PF_SmallWavy        8192
#define PF_Flat             16384
#define PF_LowShadowDetail  32768
#define PF_NoMerge          65536
#define PF_CloudWavy        131072
#define PF_DirtyShadows     262144
#define PF_BrightCorners    524288
#define PF_SpecialLit       1048576
#define PF_Gouraud          2097152
#define PF_NoBoundRejection 2097152
#define PF_Unlit            4194304
#define PF_HighShadowDetail 8388608
#define PF_Portal           67108864
#define PF_Mirrored         134217728

#define PF_NoOcclude (PF_Masked | PF_Translucent | PF_Invisible | PF_Modulated)
#define PF_NoShadows (PF_Unlit | PF_Invisible | PF_Environment | PF_FakeBackdrop)

enum STATE {
	NONE,
	MAP,
	BRUSH,
	POLYGON,
	POLYGONLIST,
	ACTOR
};

/* ########################################################################################################### */

struct MaterialName {
    char *name;
	Material *material;
    unsigned int sizeU, sizeV;
    PolygonFlags flags;
};

class MaterialTable {
private:
    Set <MaterialName *> materialNames;
public:
    MaterialTable();
    ~MaterialTable();

	void clear();
    void addMaterialName(char *name, unsigned int sizeU, unsigned int sizeV, Material *mat, PolygonFlags flags);
    bool getMaterialFromName(char *name, unsigned int &sizeU, unsigned int &sizeV, Material *&mat, PolygonFlags &flags);        
};

/* ########################################################################################################### */

class T3dLoader {
private:
	Tokenizer tok;

	const Vertex fix(const Vertex &v){
		return Vertex(v.x, v.z, -v.y);
	}

	Vertex readVertex();
public:
	T3dLoader();
	~T3dLoader();

	bool loadFromFile(const char *fileName, Set <class Polygon *> &polygons, MaterialTable &materialTable, PolygonCreator polyCreator, PolygonFilter polygonFilter, const Vertex &displacement);
};

void fixTJunctions(Set <class Polygon *> &polys);
void tesselate(Set <class Polygon *> &polys, const float maxArea, const float maxLength);

#endif // _T3DLOADER_H_
