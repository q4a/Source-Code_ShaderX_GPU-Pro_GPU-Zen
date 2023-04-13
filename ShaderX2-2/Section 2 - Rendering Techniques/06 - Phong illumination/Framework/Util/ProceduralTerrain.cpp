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

#include "ProceduralTerrain.h"
#include "../OpenGL/glExtensions.h"
#include "../Math/MyMath.h"

struct TerrVertex {
	Vertex vertex;
	Vector normal;
	float s, t;
};

struct Block {
	TerrVertex *vertices;
	int x, z;
	GLuint vertexBuffer;
};

bool operator < (const Block &b0, const Block &b1){
	if (b0.z < b1.z) return true;
	if (b0.z > b1.z) return false;
	return (b0.x < b1.x);
}

bool operator != (const Block &b0, const Block &b1){
	return (b0.x != b1.x || b0.z != b1.z);
}

void ProceduralTerrain::setBlock(Block *block){
	block->vertices = new TerrVertex[blockDensity * blockDensity];
	int pDensity = blockDensity + 2;
	float *paddedHeights = new float[pDensity * pDensity];

	TerrVertex *dest = block->vertices;
	float *pDest = paddedHeights;

	float minX = (block->x - 0.5f) * blockSize;
	//float maxX = (block->x + 0.5f) * blockSize;
	float minZ = (block->z - 0.5f) * blockSize;
	//float maxZ = (block->z + 0.5f) * blockSize;
	float w = blockSize / (blockDensity - 1);

	for (int z = -1; z <= blockDensity; z++){
		float fz = minZ + z * w;
		for (int x = -1; x <= blockDensity; x++){
			float fx = minX + x * w;
			float h = terrainFunc(fx, fz);

			*pDest++ = h;

			if (x >= 0 && x < blockDensity && z >= 0 && z < blockDensity){
				dest->vertex = Vertex(fx, h, fz);
				dest->s = texRepeat * float(x) / (blockDensity - 1);
				dest->t = texRepeat * float(z) / (blockDensity - 1);
				dest++;
			}
		}
	}

	// Apply Sobel filter to find mesh normals
	dest = block->vertices;

	w *= w;
	for (int j = 1; j < pDensity-1; j++){
		for (int i = 1; i < pDensity-1; i++){
			dest->normal.x =
				     paddedHeights[(j - 1) * pDensity + (i - 1)]  -
				     paddedHeights[(j - 1) * pDensity + (i + 1)]  +
				2 * (paddedHeights[ j      * pDensity + (i - 1)]  -
				     paddedHeights[ j      * pDensity + (i + 1)]) +
					 paddedHeights[(j + 1) * pDensity + (i - 1)]  -
					 paddedHeights[(j + 1) * pDensity + (i + 1)];

			dest->normal.y = w;

			dest->normal.z =
				     paddedHeights[(j - 1) * pDensity + (i - 1)]  -
				     paddedHeights[(j + 1) * pDensity + (i - 1)]  +
				2 * (paddedHeights[(j - 1) * pDensity +  i     ]  -
				     paddedHeights[(j + 1) * pDensity +  i     ]) +
					 paddedHeights[(j - 1) * pDensity + (i + 1)]  -
					 paddedHeights[(j + 1) * pDensity + (i + 1)];

			dest->normal.fastNormalize();

			dest++;
		}
	}

	delete paddedHeights;

	if (GL_ARB_vertex_buffer_object){
		glGenBuffersARB(1, &block->vertexBuffer);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, block->vertexBuffer);
	    glBufferDataARB(GL_ARRAY_BUFFER_ARB, blockDensity * blockDensity * sizeof(TerrVertex), block->vertices, GL_STATIC_DRAW_ARB);

	} else if (GL_ATI_vertex_array_object){
		block->vertexBuffer = glNewObjectBufferATI(blockDensity * blockDensity * sizeof(TerrVertex), block->vertices, GL_STATIC_ATI);

		
		//delete block.vertices;
	}
}

void ProceduralTerrain::freeBlock(const Block *block){
	delete block->vertices;

	if (GL_ARB_vertex_buffer_object){
		glDeleteBuffersARB(1, &block->vertexBuffer);
	} else if (GL_ATI_vertex_array_object){
		glDeleteObjectBufferATI(block->vertexBuffer);
	} else {
		//delete block.vertices;
	}
}


float ProceduralTerrain::blockDistance(const int x, const int z, const Vertex &pos){
	// Find the square of the distance from the sphere to the box

	float d = 0;

	float minX = (x - 0.5f) * blockSize;
	float maxX = (x + 0.5f) * blockSize;
	float minZ = (z - 0.5f) * blockSize;
	float maxZ = (z + 0.5f) * blockSize;

	if (pos.x < minX){
		d += sqrf(pos.x - minX);
	} else if (pos.x > maxX){
		d += sqrf(pos.x - maxX);
	}

	if (pos.z < minZ){
		d += sqrf(pos.z - minZ);
	} else if (pos.z > maxZ){
		d += sqrf(pos.z - maxZ);
	}

	return sqrtf(d);
}

int levelSize(int size, int level){
	return ((size - 1) >> level) + 1;
}

unsigned int *generateIndexArray(const int size, const int level){
	int lSize = levelSize(size, level);
	unsigned int *dest, *indexArray = new unsigned int[(lSize - 1) * (lSize * 2 + 1)];

	dest = indexArray;
	int base = 0;
	int step = 1 << level;

	for (int j = 1; j < lSize; j++){
		if (j & 1){
			for (int i = 0; i < size; i += step){
				*dest++ = base + i;
				*dest++ = base + i + size * step;
			}
			*dest++ = base + size - 1 + size * step;
		} else {
			for (int i = size - 1; i >= 0; i -= step){
				*dest++ = base + i;
				*dest++ = base + i + size * step;
			}
			*dest++ = base + size * step;
		}

		base += size * step;
	}

	return indexArray;	
}

ProceduralTerrain::ProceduralTerrain(ProceduralTerrainFunc terrFunc, float bSize, int bDensity, int blockTexRepeat, int mBlocks){
	terrainFunc = terrFunc;
	blockSize = bSize;
	blockDensity = getClosestPowerOfTwo(bDensity) + 1;
	maxBlocks = mBlocks;
	texRepeat = blockTexRepeat;

	int level = 0;
	do {
		unsigned int *indexArray = generateIndexArray(blockDensity, level);
		indices.add(indexArray);
		if (GL_ARB_vertex_buffer_object){
			GLuint indexBuffer;
			glGenBuffersARB(1, &indexBuffer);
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBuffer);
			int lSize = levelSize(blockDensity, level);
			glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 4 * (lSize - 1) * (lSize * 2 + 1), indexArray, GL_STATIC_DRAW_ARB);
			indexBuffers.add(indexBuffer);
		}
		level++;
	} while (levelSize(blockDensity, level) > 8);
}

ProceduralTerrain::~ProceduralTerrain(){

	unsigned int i, len = indices.getSize();
	for (i = 0; i < len; i++){
		delete indices[i];
	}

	for (i = 0; i < blocks.getSize(); i++){
		freeBlock(blocks[i]);
	}
}

int ProceduralTerrain::draw(Renderer *renderer, const Vertex &pos, float viewDistance, float lodFactor){
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	int minx = (int) roundf((pos.x - viewDistance) / blockSize);
	int maxx = (int) roundf((pos.x + viewDistance) / blockSize);
	int minz = (int) roundf((pos.z - viewDistance) / blockSize);
	int maxz = (int) roundf((pos.z + viewDistance) / blockSize);

	Block *block = NULL;

	int nDrawnVertices = 0;

	for (int z = minz; z <= maxz; z++){
		for (int x = minx; x <= maxx; x++){
			float d = blockDistance(x, z, pos);
			if (d < viewDistance){
				//block.x = x;
				//block.z = z;

				bool found = false;
				for (unsigned int i = 0; i < blocks.getSize(); i++){
					if (blocks[i]->x == x && blocks[i]->z == z){
						block = blocks[i];
						found = true;
						break;
					}
				}

				//if (blocks.goToObject(block)){
				if (found){
					//block = blocks.getCurrent();
				} else {
					block = new Block;
					block->x = x;
					block->z = z;
					setBlock(block);
					//blocks.insert(block);
					blocks.add(block);
					//blockQueue.enqueue(block);
				}


				if (GL_ARB_vertex_buffer_object){
				    glBindBufferARB(GL_ARRAY_BUFFER_ARB, block->vertexBuffer);
					glVertexPointer  (3, GL_FLOAT, sizeof(TerrVertex), BUFFER_OFFSET(0));
					glNormalPointer  (   GL_FLOAT, sizeof(TerrVertex), BUFFER_OFFSET(sizeof(Vertex)));
					glTexCoordPointer(2, GL_FLOAT, sizeof(TerrVertex), BUFFER_OFFSET(sizeof(Vertex) * 2));
				} else if (GL_ATI_vertex_array_object){
					glArrayObjectATI(GL_VERTEX_ARRAY,        3, GL_FLOAT, sizeof(TerrVertex), block->vertexBuffer, 0);
					glArrayObjectATI(GL_NORMAL_ARRAY,        3, GL_FLOAT, sizeof(TerrVertex), block->vertexBuffer, sizeof(Vertex));
					glArrayObjectATI(GL_TEXTURE_COORD_ARRAY, 2, GL_FLOAT, sizeof(TerrVertex), block->vertexBuffer, sizeof(Vertex) * 2);
				} else {
					char *vertices = (char *) block->vertices;
					glVertexPointer  (3, GL_FLOAT, sizeof(TerrVertex), vertices);
					glNormalPointer  (   GL_FLOAT, sizeof(TerrVertex), vertices + sizeof(Vertex));
					glTexCoordPointer(2, GL_FLOAT, sizeof(TerrVertex), vertices + sizeof(Vertex) * 2);
				}

				unsigned int level = (int) (lodFactor * d);
				if (level >= indices.getSize()) level = indices.getSize() - 1;
				int lSize = levelSize(blockDensity, level);
				int nIndices = (lSize - 1) * (lSize * 2 + 1);

				if (GL_ARB_vertex_buffer_object){
					glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBuffers[level]);
					glDrawElements(GL_TRIANGLE_STRIP, nIndices, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
				} else {
					glDrawElements(GL_TRIANGLE_STRIP, nIndices, GL_UNSIGNED_INT, indices[level]);
				}

				nDrawnVertices += nIndices;

				/*if (x == 0 && z == 0){
					glLineWidth(2);

					TerrVertex *verts = block.vertices;
					glBegin(GL_LINES);
					for (int i = 0; i < blockDensity * blockDensity; i++){
						glColor3f(0,0,0);
						glVertex3fv(verts[i].vertex);
						glColor3f(1,1,1);
						glVertex3fv(verts[i].vertex + 10 * verts[i].normal);
					}
					glEnd();
				}*/

			}
		}
	}

	glDisable(GL_CULL_FACE);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

/*	while (blockQueue.getSize() > maxBlocks){
		block = blockQueue.getFirst();
		blockQueue.dequeue();
		freeBlock(block);
		//blocks.remove(block);
		for (int i = 0; i < blocks.getSize(); i++){
			if (blocks[i].x == block.x && blocks[i].z == block.z){
				blocks.orderedRemove(i);
				break;
			}
		}
	}
	*/

	while (blocks.getSize() > maxBlocks){
		int maxIndex = 0;
		float maxDist = 0;
		for (unsigned int i = 0; i < blocks.getSize(); i++){
			float l = sqrf(pos.x - blockSize * blocks[i]->x) + sqrf(pos.z - blockSize * blocks[i]->z);

			if (l > maxDist){
				maxIndex = i;
				maxDist = l;
			}
		}
		freeBlock(blocks[maxIndex]);
		blocks.remove(maxIndex);
	}

	//if (blockQueue.getSize() != blocks.getSize()) MessageBox(NULL, "mu", "Mu", MB_OK);

	return nDrawnVertices;
}
