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

#include "OpenGLModel.h"

OpenGLBatch::OpenGLBatch() : Batch(){
	vertexBuffer = 0;
	indexBuffer  = 0;
}

OpenGLBatch::~OpenGLBatch(){
	if (GL_ARB_vertex_buffer_object){
		if (vertexBuffer != 0) glDeleteBuffersARB(1, &vertexBuffer);
		if (indexBuffer  != 0) glDeleteBuffersARB(1, &indexBuffer);
	} else if (GL_ATI_vertex_array_object){
		if (vertexBuffer != 0) glDeleteObjectBufferATI(vertexBuffer);
	}
}

OpenGLModel::OpenGLModel() : Model(){

}

OpenGLModel::~OpenGLModel(){

}

bool OpenGLBatch::uploadToVertexBuffer(){
	if (GL_ARB_vertex_buffer_object){
		glGenBuffersARB(1, &vertexBuffer);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertexBuffer);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, nVertices * vertexSize, vertices, GL_STATIC_DRAW_ARB);

		if (nIndices > 0){
			glGenBuffersARB(1, &indexBuffer);
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBuffer);
			glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, nIndices * indexSize, indices, GL_STATIC_DRAW_ARB);
		}
	} else if (GL_ATI_vertex_array_object){
		vertexBuffer = glNewObjectBufferATI(nVertices * vertexSize, vertices, GL_STATIC_ATI);
	}

	return true;
}

GLenum getGLType(const AttributeFormat attFormat){
	switch (attFormat){
	case ATT_FLOAT:         return GL_FLOAT;
	case ATT_UNSIGNED_BYTE: return GL_UNSIGNED_BYTE;
	default:
		return 0;
	}
}

GLenum getGLPrimitive(const PrimitiveType primType){
	switch (primType){
	case PRIM_TRIANGLES: return GL_TRIANGLES;
	case PRIM_QUADS:     return GL_QUADS;
	default:
		return 0;
	}
}

GLenum arrayType[] = { GL_VERTEX_ARRAY, GL_NORMAL_ARRAY, GL_TEXTURE_COORD_ARRAY, GL_COLOR_ARRAY };

void OpenGLBatch::draw(){
	unsigned int i, nFormats = formats.getSize();

	if (GL_ARB_vertex_buffer_object && vertexBuffer != 0){
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertexBuffer);
	}

	for (i = 0; i < nFormats; i++){
		Format format = formats[i];
		GLenum type = getGLType(format.attFormat);
		GLenum array = arrayType[format.attType];

		if (GL_ATI_vertex_array_object && !GL_ARB_vertex_buffer_object && vertexBuffer != 0){
    		glArrayObjectATI(array, format.size, type, vertexSize, vertexBuffer, format.offset);
		} else {
			char *basePointer = (vertexBuffer != 0)? NULL : vertices;

			switch (format.attType){
			case ATT_VERTEX:
				glVertexPointer(format.size, type, vertexSize, basePointer + format.offset);
				break;
			case ATT_NORMAL:
				glNormalPointer(type, vertexSize, basePointer + format.offset);
				break;
			case ATT_TEXCOORD:
				glClientActiveTextureARB(GL_TEXTURE0_ARB + format.index);
				glTexCoordPointer(format.size, type, vertexSize, basePointer + format.offset);
				break;
			case ATT_COLOR:
				glColorPointer(format.size, type, vertexSize, basePointer + format.offset);
				break;
			}
		}
		glEnableClientState(array);
	}

	if (nIndices == 0){
		glDrawArrays(getGLPrimitive(primitiveType), 0, nVertices);
	} else {
		if (indexBuffer != 0) glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBuffer);

		glDrawElements(getGLPrimitive(primitiveType), nIndices, indexSize == 2? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (indexBuffer != 0)? NULL : indices);

		if (indexBuffer != 0) glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
	}

	for (i = 0; i < nFormats; i++){
		if (formats[i].attType == ATT_TEXCOORD){
			glClientActiveTextureARB(GL_TEXTURE0_ARB + formats[i].index);
		}
		glDisableClientState(arrayType[formats[i].attType]);
	}

	if (GL_ARB_vertex_buffer_object && vertexBuffer != 0){
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	}
}


void OpenGLModel::draw(){
	for (unsigned int i = 0; i < batches.getSize(); i++){
		((OpenGLBatch *) batches[i])->draw();
	}
}

