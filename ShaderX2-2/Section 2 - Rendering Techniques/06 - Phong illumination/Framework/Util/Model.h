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

#ifndef _MODEL_H_
#define _MODEL_H_

#include "Set.h"
#include <stdio.h>
#include "../Math/Matrix.h"

enum PrimitiveType {
	PRIM_TRIANGLES = 0,
	PRIM_QUADS     = 1,
};

enum AttributeType {
	ATT_VERTEX   = 0,
	ATT_NORMAL   = 1,
	ATT_TEXCOORD = 2,
	ATT_COLOR    = 3,
};

enum AttributeFormat {
	ATT_FLOAT         = 0,
	ATT_UNSIGNED_BYTE = 1,
};

float getValue(const char *src, const unsigned int index, const AttributeFormat attFormat);
unsigned int getSize(const AttributeFormat attFormat);

struct Format {
	AttributeType attType;
	AttributeFormat attFormat;
	unsigned int size;
	unsigned int offset;
	unsigned int index;
};

class Batch {
protected:
	char *vertices;
	char *indices;

	unsigned int nVertices;
	unsigned int nIndices;
	unsigned int vertexSize;
	unsigned int indexSize;

	Set <Format> formats;
	PrimitiveType primitiveType;

	bool areVerticesEqual(const char *const src0, const char *const src1) const;
public:
	Batch();
	virtual ~Batch();

	void clean();

	unsigned int getIndex(const unsigned int index) const {
		return (indexSize == 2)? ((unsigned short *) indices)[index] : ((unsigned int *) indices)[index];
	}

	void setIndex(const unsigned int index, const unsigned int value){
		if (indexSize == 2){
			((unsigned short *) indices)[index] = value;
		} else {
			((unsigned int *) indices)[index] = value;
		}
	}

	char *getVertices() const { return vertices; }
	char *getIndices()  const { return indices;  }
	const unsigned int getVertexCount() const { return nVertices;  }
	const unsigned int getIndexCount()  const { return nIndices;   }
	const unsigned int getVertexSize()  const { return vertexSize; }
	const unsigned int getIndexSize()   const { return indexSize;  }

	const unsigned int getFormatCount() const { return formats.getSize(); }
	const Format &getFormat(unsigned int index) const { return formats[index]; }

	const PrimitiveType getPrimitiveType() const { return primitiveType; }
	void setPrimitiveType(const PrimitiveType primType){ primitiveType = primType; }

	void setVertices(void *vertexArray, const unsigned int vertexCount, const unsigned int size){
		vertices = (char *) vertexArray;
		nVertices = vertexCount;
		vertexSize = size;
	}
	void setIndices(void *indexArray, const unsigned int indexCount, const unsigned int size){
		indices = (char *) indexArray;
		nIndices = indexCount;
		indexSize = size;
	}

	void addFormat(const AttributeType attType, const AttributeFormat attFormat, const unsigned int size, const unsigned int offset, const unsigned int index = 0){
		Format format;

		format.attType = attType;
		format.attFormat = attFormat;
		format.size = size;
		format.offset = offset;
		format.index = index;

		formats.add(format);
	}

	bool findAttribute(const AttributeType attType, const unsigned int index = 0, unsigned int *where = NULL) const;
	bool insertAttribute(const AttributeType attType, const AttributeFormat attFormat, const unsigned int size, const unsigned int index = 0);
	bool removeAttribute(const AttributeType attType, const unsigned int index = 0);

	bool reverseWinding();
	bool flipNormals();
	bool transform(const Matrix &mat, const AttributeType attType, const unsigned int index = 0);

	void optimize(unsigned int dupVertSearch = 16);
	bool addNormals();

	bool saveToObj(const char *fileName);
	void readFromFile(FILE *file);
	void writeToFile(FILE *file);
};



class Model {
protected:
	Set <Batch *> batches;

	virtual Batch *newBatch(){ return new Batch(); };
public:
	Model();
	virtual ~Model();

	unsigned int getBatchCount() const { return batches.getSize(); }
	Batch *getBatch(unsigned int index) const { return batches[index]; }
	void addBatch(Batch *const batch){ batches.add(batch); }

	bool loadObj(const char *fileName);
	bool loadFromFile(const char *fileName);
	bool saveToFile(const char *fileName);
};



#endif // _MODEL_H_
