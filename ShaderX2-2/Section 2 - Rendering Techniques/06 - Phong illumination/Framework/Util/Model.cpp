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

#include "Model.h"
#include "Tokenizer.h"
#include "OrderedSet.h"

float getValue(const char *src, const unsigned int index, const AttributeFormat attFormat){
	switch (attFormat){
	case ATT_FLOAT:         return *(((float *) src) + index);
	case ATT_UNSIGNED_BYTE: return *(((unsigned char *) src) + index) * (1.0f / 255.0f);
	default:
		return 0;
	}
}

void setValue(const char *dest, const unsigned int index, const AttributeFormat attFormat, float value){
	switch (attFormat){
	case ATT_FLOAT:
		*(((float *) dest) + index) = value;
		break;
	case ATT_UNSIGNED_BYTE:
		*(((unsigned char *) dest) + index) = (unsigned char) (value * 255.0f);
		break;
	}
}

unsigned int getSize(const AttributeFormat attFormat){
	switch (attFormat){
	case ATT_FLOAT:         return 4;
	case ATT_UNSIGNED_BYTE: return 1;
	default:
		return 0;
	}
}


Batch::Batch(){
	vertices = NULL;
	indices  = NULL;

	nVertices = 0;
	nIndices  = 0;
	vertexSize = 0;
	indexSize  = 0;
}

Batch::~Batch(){
	clean();
}

void Batch::clean(){
	if (vertices != NULL) delete vertices;
	if (indices  != NULL) delete indices;
	vertices = NULL;
	indices = NULL;
}

bool Batch::areVerticesEqual(const char *const src0, const char *const src1) const {
	unsigned int i, j, nFormats = formats.getSize();
	for (i = 0; i < nFormats; i++){
		for (j = 0; j < formats[i].size; j++){
			float diff = getValue(src0 + formats[i].offset, j, formats[i].attFormat) - getValue(src1 + formats[i].offset, j, formats[i].attFormat);

			if (fabs(diff) > 0.01f) return false;
		}
	}

	return true;
}

bool Batch::findAttribute(const AttributeType attType, const unsigned int index, unsigned int *where) const {
	for (unsigned int i = 0; i < formats.getSize(); i++){
		if (formats[i].attType == attType && formats[i].index == index){
			if (where != NULL) *where = i;
			return true;
		}
	}
	return false;
}

bool Batch::insertAttribute(const AttributeType attType, const AttributeFormat attFormat, const unsigned int size, const unsigned int index){
	unsigned int i;

	for (i = 0; i < formats.getSize(); i++){
		if (formats[i].attType == attType && formats[i].index == index) return false;
	}

	addFormat(attType, attFormat, size, vertexSize, index);

	unsigned int fSize = size * getSize(attFormat);
	unsigned int newVertexSize = vertexSize + fSize;
	char *newVertices = new char[nVertices * newVertexSize];

	char *dest = newVertices;
	char *src = vertices;
	for (i = 0; i < nVertices; i++){
		memcpy(dest, src, vertexSize);
		memset(dest + vertexSize, 0, fSize);
		dest += newVertexSize;
		src += vertexSize;
	}

	delete vertices;
	vertices = newVertices;
	vertexSize = newVertexSize;

	return true;
}

bool Batch::removeAttribute(const AttributeType attType, const unsigned int index){
	unsigned int i, j;

	for (i = 0; i < formats.getSize(); i++){
		if (formats[i].attType == attType && formats[i].index == index){
			Format format = formats[i];
			formats.remove(i);

			unsigned int fSize = format.size * getSize(format.attFormat);
			for (j = i; j < formats.getSize(); j++) formats[i].offset -= fSize;

			unsigned int newVertexSize = vertexSize - fSize;
			char *newVertices = new char[nVertices * newVertexSize];

			char *dest = newVertices;
			char *src = vertices;
			memcpy(dest, src, format.offset);
			dest += format.offset;
			src += format.offset + fSize;
			for (j = 0; j < nVertices - 1; j++){
				memcpy(dest, src, newVertexSize);
				dest += newVertexSize;
				src += vertexSize;
			}
			memcpy(dest, src, vertexSize - (format.offset + fSize));

			delete vertices;
			vertices = newVertices;
			vertexSize = newVertexSize;

			return true;
		}
	}
	return false;
}

bool Batch::reverseWinding(){
	if (primitiveType != PRIM_TRIANGLES && primitiveType != PRIM_QUADS) return false;

	unsigned int primSize = (primitiveType == PRIM_TRIANGLES)? 3 : 4;
	unsigned int n = primSize / 2;

	if (indices != NULL){
		unsigned int nPrims = nIndices / primSize;
		for (unsigned int i = 0; i < nPrims; i++){
			for (unsigned int j = 0; j < n; j++){
				if (indexSize == 2){
					unsigned short temp = ((unsigned short *) indices)[i * primSize + j];
					((unsigned short *) indices)[i * primSize + j] = ((unsigned short *) indices)[i * primSize + primSize - j - 1];
					((unsigned short *) indices)[i * primSize + primSize - j - 1] = temp;
				} else {
					unsigned int temp = ((unsigned int *) indices)[i * primSize + j];
					((unsigned int *) indices)[i * primSize + j] = ((unsigned int *) indices)[i * primSize + primSize - j - 1];
					((unsigned int *) indices)[i * primSize + primSize - j - 1] = temp;
				}
			}
		}
	} else {
		unsigned int nPrims = nVertices / primSize;
		char *temp = new char[vertexSize];
		for (unsigned int i = 0; i < nPrims; i++){
			for (unsigned int j = 0; j < n; j++){
				char *src0 = vertices + (i * primSize + j) * vertexSize;
				char *src1 = vertices + (i * primSize + primSize - j - 1) * vertexSize;
				memcpy(temp, src0, vertexSize);
				memcpy(src0, src1, vertexSize);
				memcpy(src1, temp, vertexSize);
			}
		}
		delete temp;
	}

	return true;
}

bool Batch::flipNormals(){
	unsigned int normalOffset;
	if (!findAttribute(ATT_NORMAL, 0, &normalOffset)) return false;

	AttributeFormat normalFormat = formats[normalOffset].attFormat;
	normalOffset = formats[normalOffset].offset;

	for (unsigned int i = 0; i < nVertices; i++){
		char *src = vertices + i * vertexSize + normalOffset;
		setValue(src, 0, normalFormat, -getValue(src, 0, normalFormat));		
		setValue(src, 1, normalFormat, -getValue(src, 1, normalFormat));		
		setValue(src, 2, normalFormat, -getValue(src, 2, normalFormat));		
	}

	return true;
}

bool Batch::transform(const Matrix &mat, const AttributeType attType, const unsigned int index){
	AttributeFormat format;
	unsigned int i,j,offset, size;
	if (!findAttribute(attType, index, &offset)) return false;
	size   = formats[offset].size;
	format = formats[offset].attFormat;
	offset = formats[offset].offset;

	for (i = 0; i < nVertices; i++){
		char *src = vertices + i * vertexSize + offset;

		Vertex vec(0,0,0);
		for (j = 0; j < size; j++){
			vec.operator [](j) = getValue(src, j, format);
		}
		vec = mat * vec;
		for (j = 0; j < size; j++){
			setValue(src, j, format, vec.operator [](j));
		}
	}

	return true;
}

void Batch::optimize(unsigned int dupVertSearch){
	unsigned int nVert = nVertices;
	unsigned int nInds;
	char *verts;
	unsigned int *inds;
	unsigned int i;

	if (indices == NULL){
		nInds = nVertices;
		inds = new unsigned int[nInds];
		for (i = 0; i < nInds; i++){
			inds[i] = i;
		}
	} else {
		nInds = nIndices;
		inds = new unsigned int[nInds];
		if (indexSize == 2){
			for (i = 0; i < nInds; i++){
				inds[i] = ((unsigned short *) indices)[i];
			}
		} else memcpy(inds, indices, nInds * indexSize);
	}

	verts = new char[nVert * vertexSize];
	memcpy(verts, vertices, nVert * vertexSize);

	bool *removed = new bool[nVert];
	memset(removed, 0, nVert * sizeof(bool));

	Set <unsigned int> refersTo;

	for (i = 1; i < nVert; i++){
		unsigned int m = (i < dupVertSearch)? i : dupVertSearch;

		unsigned int c = 0;
		for (unsigned int j = 1; j <= m; j++){
			if (removed[i - j]){
				c++;
				continue;
			}

			char *src0 = verts +  i      * vertexSize;
			char *src1 = verts + (i - j) * vertexSize;

			if (areVerticesEqual(src0, src1)){
				removed[i] = true;
				refersTo.add(i - j + c);

				break;
			}
		}
	}

	unsigned int *indRef = new unsigned int[nVert];
	unsigned int n = 0;
	for (i = 0; i < nVert; i++){
		if (removed[i]){
			indRef[i] = refersTo[n] - n;
			n++;
		} else {
			indRef[i] = i - n;
		}
	}



	nVert -= refersTo.getSize();

	unsigned int indSize = (nVert < 65536)? 2 : 4;
	if (nVert * vertexSize + nInds * indSize < nVertices * vertexSize + nIndices * indexSize){
		clean();

		nVertices = nVert;
		nIndices = nInds;

		char *newVertices = new char[nVert * vertexSize];

		unsigned int y = (unsigned int) (-1);
		for (unsigned int x = 0; x < nVert; x++){
			do y++; while (removed[y]);
			
			char *dest = newVertices + x * vertexSize;
			char *src = verts + y * vertexSize;

			memcpy(dest, src, vertexSize);
		}
		delete verts;
		vertices = newVertices;

		if ((indexSize = indSize) == 2){
			unsigned short *newInds = new unsigned short[nInds];
			for (i = 0; i < nInds; i++){
				newInds[i] = indRef[inds[i]];
			}
			
			indices = (char *) newInds;
		} else {
			unsigned int *newInds = new unsigned int[nInds];
			for (i = 0; i < nInds; i++){
				newInds[i] = indRef[inds[i]];
			}

			indices = (char *) newInds;
		}
		delete inds;
	}

	delete indRef;
	delete removed;
}

bool Batch::addNormals(){
	if (indices == NULL) return false;
	if (!insertAttribute(ATT_NORMAL, ATT_FLOAT, 3)) return false;
	if (primitiveType != PRIM_TRIANGLES) return false;

	unsigned int vertexOffset, normalOffset;
	if (!findAttribute(ATT_VERTEX, 0, &vertexOffset) || !findAttribute(ATT_NORMAL, 0, &normalOffset)) return false;

	AttributeFormat vertFormat = formats[vertexOffset].attFormat;
	vertexOffset = formats[vertexOffset].offset;
	normalOffset = formats[normalOffset].offset;

	unsigned int i, nTris = nIndices / 3;
	for (i = 0; i < nTris; i++){
		unsigned int i0, i1, i2;
		if (indexSize == 2){
			i0 = ((unsigned short *) indices)[3 * i];
			i1 = ((unsigned short *) indices)[3 * i + 1];
			i2 = ((unsigned short *) indices)[3 * i + 2];
		} else {
			i0 = ((unsigned int *) indices)[3 * i];
			i1 = ((unsigned int *) indices)[3 * i + 1];
			i2 = ((unsigned int *) indices)[3 * i + 2];
		}

		char *vert, *norm;
		vert = vertices + i0 * vertexSize + vertexOffset;
		Vertex v0(getValue(vert, 0, vertFormat), getValue(vert, 1, vertFormat), getValue(vert, 2, vertFormat));
		vert = vertices + i1 * vertexSize + vertexOffset;
		Vertex v1(getValue(vert, 0, vertFormat), getValue(vert, 1, vertFormat), getValue(vert, 2, vertFormat));
		vert = vertices + i2 * vertexSize + vertexOffset;
		Vertex v2(getValue(vert, 0, vertFormat), getValue(vert, 1, vertFormat), getValue(vert, 2, vertFormat));

		Vector normal = (cross(v1 - v0, v2 - v0));

		norm = vertices + i0 * vertexSize + normalOffset;
		*(Vector *) norm += normal;
		norm = vertices + i1 * vertexSize + normalOffset;
		*(Vector *) norm += normal;
		norm = vertices + i2 * vertexSize + normalOffset;
		*(Vector *) norm += normal;
	}

	for (i = 0; i < nVertices; i++){
		char *norm = vertices + i * vertexSize + normalOffset;
		((Vector *) norm)->normalize();
	}

	return true;
}

bool Batch::saveToObj(const char *fileName){
	bool normals, texCoords;
	unsigned int vertexOffset, normalOffset, texCoordOffset, texCoordSize = 0;
	AttributeFormat vertexFormat, normalFormat = ATT_FLOAT, texCoordFormat = ATT_FLOAT;
	unsigned int primSize, nPrim;

	if (primitiveType == PRIM_TRIANGLES) primSize = 3;
	else if (primitiveType == PRIM_QUADS) primSize = 4;
	else return false;
	nPrim = nIndices / primSize;

	if (!findAttribute(ATT_VERTEX, 0, &vertexOffset)) return false;
	vertexFormat = formats[vertexOffset].attFormat;
	vertexOffset = formats[vertexOffset].offset;

	if (normals = findAttribute(ATT_NORMAL, 0, &normalOffset)){
		normalFormat = formats[normalOffset].attFormat;
		normalOffset = formats[normalOffset].offset;
	}
	if (texCoords = findAttribute(ATT_TEXCOORD, 0, &texCoordOffset)){
		texCoordFormat = formats[texCoordOffset].attFormat;
		texCoordSize   = formats[texCoordOffset].size;
		texCoordOffset = formats[texCoordOffset].offset;
	}

	FILE *file = fopen(fileName, "wb");
	if (file == NULL) return false;

	fwrite("g default\n", 10, 1, file);

	char str[256];
	unsigned int len;

	for (unsigned int i = 0; i < nVertices; i++){
		char *base = vertices + i * vertexSize;
		char *src = base + vertexOffset;
		len = sprintf(str, "v %.4f %.4f %.4f\n", getValue(src, 0, vertexFormat), getValue(src, 1, vertexFormat), getValue(src, 2, vertexFormat));

		fwrite(str, len, 1, file);
	}
	if (texCoords){
		for (unsigned int i = 0; i < nVertices; i++){
			char *base = vertices + i * vertexSize;
			char *src = base + texCoordOffset;

			len = sprintf(str, "vt ");
			for (unsigned int j = 0; j < texCoordSize; j++){
				len += sprintf(str + len, "%.4f", getValue(src, j, texCoordFormat));
			}
			len += sprintf(str + len, "\n");

			fwrite(str, len, 1, file);
		}
	}
	if (normals){
		for (unsigned int i = 0; i < nVertices; i++){
			char *base = vertices + i * vertexSize;
			char *src = base + normalOffset;
			len = sprintf(str, "vn %.4f %.4f %.4f\n", getValue(src, 0, normalFormat), getValue(src, 1, normalFormat), getValue(src, 2, normalFormat));

			fwrite(str, len, 1, file);
		}
	}

	fwrite("s 1\ng object\nusemtl initialShadingGroup\n", 40, 1, file);

	if (indices == NULL){

	} else {
		unsigned int n = 0;
		for (unsigned int i = 0; i < nPrim; i++){
			len = sprintf(str, "f");
			for (unsigned int j = 0; j < primSize; j++){
				unsigned int index = 1 + ((indexSize == 2)? ((unsigned short *) indices)[n] : ((unsigned int *) indices)[n]);

				len += sprintf(str + len, " %u", index);
				if (normals || texCoords) len += sprintf(str + len, "/");
				if (texCoords) len += sprintf(str + len, "%u", index);
				if (normals) len += sprintf(str + len, "/%u", index);

				n++;
			}
			len += sprintf(str + len, "\n");
			fwrite(str, len, 1, file);
		}
	}

	fclose(file);

	return true;
}

void Batch::readFromFile(FILE *file){
	fread(&nVertices,  sizeof(nVertices),  1, file);
	fread(&nIndices,   sizeof(nIndices),   1, file);
	fread(&vertexSize, sizeof(vertexSize), 1, file);
	fread(&indexSize,  sizeof(indexSize),  1, file);

	fread(&primitiveType, sizeof(primitiveType), 1, file);

	unsigned int nFormats;
	fread(&nFormats, sizeof(nFormats), 1, file);
	formats.setSize(nFormats);
	fread(formats.getArray(), nFormats * sizeof(Format), 1, file);

	vertices = new char[nVertices * vertexSize];
	fread(vertices, nVertices * vertexSize, 1, file);

	if (nIndices > 0){
		indices = new char[nIndices  * indexSize];
		fread(indices,  nIndices  * indexSize,  1, file);
	} else indices = NULL;
}

void Batch::writeToFile(FILE *file){
	fwrite(&nVertices,  sizeof(nVertices),  1, file);
	fwrite(&nIndices,   sizeof(nIndices),   1, file);
	fwrite(&vertexSize, sizeof(vertexSize), 1, file);
	fwrite(&indexSize,  sizeof(indexSize),  1, file);

	fwrite(&primitiveType, sizeof(primitiveType), 1, file);

	unsigned int nFormats = formats.getSize();
	fwrite(&nFormats, sizeof(nFormats), 1, file);
	fwrite(formats.getArray(), nFormats * sizeof(Format), 1, file);

	fwrite(vertices, nVertices * vertexSize, 1, file);
	fwrite(indices,  nIndices  * indexSize,  1, file);
}


Model::Model(){

}

Model::~Model(){
	for (unsigned int i = 0; i < batches.getSize(); i++){
		delete batches[i];
	}
}

float readFloat(Tokenizer &tok, char *str){
	tok.next(str);
	if (str[0] == '-'){
		tok.next(str);
		return -(float) atof(str);
	} else {
		return (float) atof(str);
	}
}

#define nextFloat(tok) readFloat(tok, str)

/*void fillBatch(Batch *batch, const Set <unsigned int> &indices, const Set <Vertex> &vertices, const Set <Vector> &normals, const Set <float> &texCoords){
	unsigned int nVertices = indices.getSize() / 3;
	float *vertexArray = new float[8 * nVertices];

	float *dest = vertexArray;
	for (unsigned int i = 0; i < nVertices; i++){
		*dest++ = vertices[indices[3 * i]].x;
		*dest++ = vertices[indices[3 * i]].y;
		*dest++ = vertices[indices[3 * i]].z;
		*dest++ = normals[indices[3 * i + 2]].x;
		*dest++ = normals[indices[3 * i + 2]].y;
		*dest++ = normals[indices[3 * i + 2]].z;
		*dest++ = texCoords[2 * indices[3 * i + 1]];
		*dest++ = texCoords[2 * indices[3 * i + 1] + 1];
	}

	batch->setVertices(vertexArray, nVertices, 32);
	batch->addFormat(ATT_VERTEX,   ATT_FLOAT, 3, 0);
	batch->addFormat(ATT_NORMAL,   ATT_FLOAT, 3, 12);
	batch->addFormat(ATT_TEXCOORD, ATT_FLOAT, 2, 24);
}
*/

struct ObjIndex {
	unsigned int vert, tex, norm;
	unsigned int index;
};

bool operator != (const ObjIndex &o0, const ObjIndex &o1){
	return (o0.vert != o1.vert || o0.tex != o1.tex || o0.norm != o1.norm);
}

bool operator < (const ObjIndex &o0, const ObjIndex &o1){
	return (o0.vert < o1.vert) || (o0.vert == o1.vert && o0.tex < o1.tex) || (o0.vert == o1.vert && o0.tex == o1.tex && o0.norm < o1.norm);
}

bool Model::loadObj(const char *fileName){
	Tokenizer tok, ltok;
	unsigned int n, start;

	Set <Vertex> vertices;
	Set <Vector> normals;
	Set <float> texCoords;
	Set <ObjIndex> objIndices;
	ObjIndex index;
	memset(&index, 0, sizeof(index));

	if (!tok.setFile(fileName)) return false;

	char str[512];
	while (tok.next(str)){
		switch (str[0]){
		case 'f':
			tok.nextLine(str);
			ltok.setString(str);

			start = objIndices.getSize();
			n = 0;
			while (ltok.next(str)){
				index.vert = atoi(str) - 1;
				if (texCoords.getSize() || normals.getSize()) ltok.next();
				if (texCoords.getSize()){
					ltok.next(str);
					index.tex = atoi(str) - 1;
				}
				if (normals.getSize()){
					ltok.next();
					ltok.next(str);
					index.norm = atoi(str) - 1;
				}

				if (n > 2){
					objIndices.add(objIndices[start]);
					objIndices.add(objIndices[objIndices.getSize() - 2]);
				}
				objIndices.add(index);
				n++;
			}

			break;
		case 'v':
			switch(str[1]){
			case '\0':
				vertices.add(Vertex(nextFloat(tok), nextFloat(tok), nextFloat(tok)));
				break;
			case 'n':
				normals.add(Vertex(nextFloat(tok), nextFloat(tok), nextFloat(tok)));
				break;
			case 't':
				texCoords.add(nextFloat(tok));
				texCoords.add(nextFloat(tok));
				break;
			}
			break;
		default:
			tok.nextLine();
			break;
		}
	}

	OrderedSet <ObjIndex> indexSet;
	unsigned int nIndices = objIndices.getSize();
	unsigned int *indexArray = new unsigned int[nIndices];

	for (unsigned int i = 0; i < nIndices; i++){
		index = objIndices[i];
		if (indexSet.goToObject(index)){
			indexArray[i] = indexSet.getCurrent().index;
		} else {
			index.index = indexSet.getSize();
			indexSet.insert(index);
			indexArray[i] = index.index;
		}
	}



	unsigned int nVertices = indexSet.getSize();
	unsigned int vertexSize = 3 * sizeof(float);
	
	Batch *batch = newBatch();
	batch->addFormat(ATT_VERTEX, ATT_FLOAT, 3, 0);
	if (normals.getSize()){
		batch->addFormat(ATT_NORMAL, ATT_FLOAT, 3, vertexSize);
		vertexSize += 3 * sizeof(float);
	}
	if (texCoords.getSize()){
		batch->addFormat(ATT_TEXCOORD, ATT_FLOAT, 2, vertexSize);
		vertexSize += 2 * sizeof(float);
	}

	char *vertexArray = new char[nVertices * vertexSize];

	bool result = indexSet.goToFirst();
	while (result){
		index = indexSet.getCurrent();

		char *dest = vertexArray + index.index * vertexSize;

		((Vector *) dest)[0] = vertices[index.vert];
		if (normals.getSize()){
			((Vector *) dest)[1] = -normals[index.norm];
		}
		if (texCoords.getSize()){
			if (normals.getSize()){
				((float *) dest)[6] = texCoords[2 * index.tex];
				((float *) dest)[7] = texCoords[2 * index.tex + 1];
			} else {
				((float *) dest)[3] = texCoords[2 * index.tex];
				((float *) dest)[4] = texCoords[2 * index.tex + 1];
			}
		}

		result = indexSet.next();
	}

	batch->setVertices(vertexArray, nVertices, vertexSize);
	batch->setIndices(indexArray, nIndices, sizeof(unsigned int));
	batch->setPrimitiveType(PRIM_TRIANGLES);

	batches.add(batch);

	return true;
}

bool Model::loadFromFile(const char *fileName){
	FILE *file = fopen(fileName, "rb");
	if (file == NULL) return false;

	unsigned int version;
	fread(&version, sizeof(version), 1, file);
	unsigned int nBatches;
	fread(&nBatches, sizeof(nBatches), 1, file);

	for (unsigned int i = 0; i < nBatches; i++){
		Batch *batch = newBatch();
		batch->readFromFile(file);
		batches.add(batch);
	}

	fclose(file);
	
	return true;
}

bool Model::saveToFile(const char *fileName){
	FILE *file = fopen(fileName, "wb");
	if (file == NULL) return false;

	unsigned int version = 1;
	fwrite(&version, sizeof(version), 1, file);
	unsigned int nBatches = batches.getSize();
	fwrite(&nBatches, sizeof(nBatches), 1, file);

	for (unsigned int i = 0; i < nBatches; i++){
		batches[i]->writeToFile(file);
	}

	fclose(file);

	return true;
}
