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

#include "CollisionSet.h"

#define SAFETY_OFFSET 0.01f

struct CollPoly {
	Vertex *vertices;
	unsigned int nVertices;
	Plane plane;
	CollPoly *front, *back;

	CollPoly(){
		front = back = NULL;
	}

	~CollPoly(){
		delete vertices;
		if (front != NULL) delete front;
		if (back  != NULL) delete back;
	}


	RELATION polyRelation(const CollPoly *other) const;
	RELATION split(const Plane &splitPlane, CollPoly *&back, CollPoly *&front) const;

	// Assumes that the two points are on different sides of the plane
	bool intersect(const Vertex &v0, const Vertex &v1, Vertex *intersection = NULL) const;
	bool isAbove(const Vertex &pos) const;
	bool touchesEdge(const Vertex &pos, const float radius, Vertex *touchPoint = NULL) const;

	bool pushSphere(const Vertex &oldPos, Vertex &currPos, const float radius) const;

};


RELATION CollPoly::polyRelation(const CollPoly *other) const {
	RELATION rel;
	unsigned int i, front, back;

	front = back = 0;

	for (i = 0; i < other->nVertices; i++){
		rel = plane.getVertexRelation(other->vertices[i]);
		if (rel == FRONT) front++;
		if (rel == BACK)  back++;
	}
	if (front > 0){
		if (back > 0) return CUTS; else return FRONT;
	} else {
		if (back > 0) return BACK; else return PLANAR;
	}
}

RELATION CollPoly::split(const Plane &splitPlane, CollPoly *&back, CollPoly *&front) const {
	RELATION rel, prev, rel0, rel1, relprev0;
	int split0, split1, offset;
	unsigned int i, j, nBack, nFront;
	float k;

	// To make g++ happy ... not really needed
	rel0 = rel1 = relprev0 = FRONT;

	nFront = nBack = 0;
	split0 = -1, split1 = -1;

	prev = splitPlane.getVertexRelation(vertices[nVertices - 1]);

	for (i = 0; i < nVertices; i++){
		rel = splitPlane.getVertexRelation(vertices[i]);

		if (rel == BACK)  nBack++;
		if (rel == FRONT) nFront++;

		if (rel != prev){
			if (prev != PLANAR){
				if (split0 < 0){
					split0 = i;
					rel0 = rel;
					relprev0 = prev;
				} else {
					split1 = i;
					rel1 = rel;
				}
			}
		}

		prev = rel;
	}

	if (nBack == 0){
		back = front = NULL;
		if (nFront == 0){
			return PLANAR;
		} else {
			return FRONT;
		}
	}

	if (nFront == 0){
		back = front = NULL;
		return BACK;
	}
	
	if (relprev0 == FRONT){
		int temp = nFront;
		nFront = nBack;
		nBack = temp;
	}
/*
	back  = create(nBack  + 2);
	initialize(back);
	front = create(nFront + 2);
	initialize(front);
	*/
	front = new CollPoly();
	front->nVertices = nFront + 2;
	front->vertices = new Vertex[nFront + 2];

	back = new CollPoly();
	back->nVertices = nBack + 2;
	back->vertices = new Vertex[nBack + 2];

	offset = (rel0 == PLANAR)? split0 + 1 : split0;
	for (i = 0; i < nFront; i++){
		front->vertices[i] = vertices[offset + i];
	}

	offset = (rel1 == PLANAR)? split1 + 1 : split1;
	for (i = 0; i < nBack; i++){
		back->vertices[i] = vertices[(offset + i) % nVertices];
	}

	j = (split0 == 0)? nVertices - 1 : split0 - 1;

	k = splitPlane.getPlaneHitInterpolationConstant(vertices[j], vertices[split0]);
	front->vertices[nFront + 1] = back->vertices[nBack] = vertices[j] * (1 - k) + vertices[split0] * k;
	//setInterpolatedVertex(front, nFront + 1, j, split0, k);
	//setInterpolatedVertex(back,  nBack,      j, split0, k);

	k = splitPlane.getPlaneHitInterpolationConstant(vertices[split1-1], vertices[split1]);
	front->vertices[nFront] = back->vertices[nBack + 1] = vertices[split1 - 1] * (1 - k) + vertices[split1] * k;
	//setInterpolatedVertex(front, nFront,    split1-1, split1, k);
	//setInterpolatedVertex(back,  nBack + 1, split1-1, split1, k);
	
	back->plane  = plane;
	front->plane = plane;
	//back->finalize();
	//front->finalize();

	if (relprev0 == FRONT){
		CollPoly *temp = back;
		back = front;
		front = temp;
	}
	

	return CUTS;
}


bool CollPoly::intersect(const Vertex &v0, const Vertex &v1, Vertex *intersection) const {
	// Find interpolation constant
	float k = plane.distance(v0) / (plane.normal * (v0 - v1));

	Vertex p = v0 + k * (v1 - v0);

	unsigned int i, last = nVertices - 1;
	for (i = 0; i < nVertices; i++){
		Vector edgeNormal = cross(plane.normal, vertices[i] - vertices[last]);

		if (edgeNormal * p - (edgeNormal * vertices[i]) < 0) return false;

		last = i;
	}

	if (intersection != NULL) *intersection = p;
	return true;
}



bool CollPoly::isAbove(const Vertex &pos) const {
	unsigned int i, last = nVertices - 1;
	for (i = 0; i < nVertices; i++){
		Vector edgeNormal = cross(plane.normal, vertices[i] - vertices[last]);

		if (edgeNormal * pos - (edgeNormal * vertices[i]) < 0) return false;

		last = i;
	}

	return true;
}

bool CollPoly::touchesEdge(const Vertex &pos, const float radius, Vertex *touchPoint) const {
	unsigned int i, last = nVertices - 1;
	for (i = 0; i < nVertices; i++){
		// d = abs( ((x2 - x1) cross (x1 - x0)) / (x2 - x1) );
		Vector diff = vertices[i] - vertices[last];
		
		float t = diff * (pos - vertices[last]);
		if (t > 0){
			float f = lengthSqr(diff);		
			if (t < f){
				Vertex v = vertices[last] + (t / f) * diff;
				if (lengthSqr(v - pos) < radius * radius){
					if (touchPoint != NULL) *touchPoint = v;
					return true;
				}
			}
		}

		last = i;
	}
	

	return false;
}

bool CollPoly::pushSphere(const Vertex &oldPos, Vertex &currPos, const float radius) const {
	float oldDist = plane.distance(oldPos);
	float currDist = plane.distance(currPos);

	bool pushed = false;
	if (oldDist > 0){

		if (currDist < 0){
			Vertex hitPoint;
			if (intersect(oldPos, currPos, &hitPoint)){
				currPos = hitPoint + (radius + SAFETY_OFFSET) * plane.normal;
				pushed = true;
			}
			
		} else if (currDist < radius){
			if (isAbove(currPos)){
				currPos += (radius - currDist + SAFETY_OFFSET) * plane.normal;
				pushed = true;
			} else {
				Vertex touchPoint;
				if (touchesEdge(currPos, radius, &touchPoint)){
					Vertex diff = currPos - touchPoint;
					diff.normalize();

					currPos = touchPoint + (radius + SAFETY_OFFSET) * diff;
					pushed = true;
				} else {
					for (unsigned int i = 0; i < nVertices; i++){
						Vector diff = currPos - vertices[i];
						float d = lengthSqr(diff);
						if (d < radius * radius){
							diff *= 1.0f / sqrtf(d);
							currPos = vertices[i] + (radius + SAFETY_OFFSET) * diff;
							pushed = true;
						}
					}
				}
			}
		}
	}

	if (front != NULL && currDist > -radius) pushed |= front->pushSphere(oldPos, currPos, radius);
	if (back  != NULL && currDist <  radius) pushed |= back->pushSphere(oldPos,  currPos, radius);

	return pushed;
}


/* ----------------------------------------------------------------- */


CollisionSet::CollisionSet(){
	top = NULL;
}

CollisionSet::~CollisionSet(){
	clear();
}

void CollisionSet::clear(){
	if (top != NULL){
		delete top;
		top = NULL;
	}

	int i, len = polys.getSize();

	for (i = 0; i < len; i++){
		delete polys[i];
	}
	polys.reset();
}


void CollisionSet::addStaticPolygon(const Vertex *vertices, const int nVertices){
	CollPoly *poly = new CollPoly();

	poly->nVertices = nVertices;
	poly->vertices  = new Vertex[nVertices];
	memcpy(poly->vertices, vertices, nVertices * sizeof(Vertex));

	poly->plane = Plane(vertices[0], vertices[1], vertices[2]);

	polys.add(poly);
}

void build(CollPoly *&currNode, Set <CollPoly *> &polys){
	RELATION rel;
	unsigned int i, j, k, n, len = polys.getSize();
	int cutterIndex = 0, cutterScore = 0x7FFFFFFF, score, diff;

	// Select the cutting surface, instead of trying to find something "optimal",
	// which would take loads of time, we try to find something "pretty good".

	if (len < 16) n = len; else n = 16 + (len >> 5);

	for (k = 0; k < n; k++){
		score = 0;
		diff  = 0;

		if (len < 16){
			i = k;
		} else {
			i = (rand() % len);
		}

		for (j = 0; j < len; j++){
			if (i != j){
				rel = polys[i]->polyRelation(polys[j]);

				if (rel == CUTS){
					score += 3;
				} else {
					if (rel == FRONT) diff++; else diff--;
				}
			}
		}
		score += abs(diff);
		
		if (score < cutterScore){
			cutterIndex = i;
			cutterScore = score;
		}
	}

	currNode = polys[cutterIndex];
	polys.remove(cutterIndex);
	len--;
	
	// Split with the selected surface
	Set <CollPoly *> backList;
	Set <CollPoly *> frontList;
	CollPoly *backPoly, *frontPoly;

	for (i = 0; i < len; i++){
		rel = polys[i]->split(currNode->plane, backPoly, frontPoly);

		if (rel == CUTS){
			backList.add(backPoly);
			frontList.add(frontPoly);
			delete polys[i];
		} else {
			if (rel == BACK){
				backList.add(polys[i]);
			} else {
				frontList.add(polys[i]);
			}
		}
	}

	if (backList.getSize() > 0){
		build(currNode->back, backList);
	} else currNode->back = NULL;

	if (frontList.getSize() > 0){
		build(currNode->front, frontList);
	} else currNode->front = NULL;
}

void CollisionSet::finalizeStatics(){
	build(top, polys);
	polys.reset();
}

void CollisionSet::pushSphere(const Vertex &oldPos, Vertex &currPos, const float radius) const {
	int i = 0;
	do {
		i++;
	} while (top->pushSphere(oldPos, currPos, radius) && i < 4);
}

float findDepth(const CollPoly *currPoly, const Vertex &v0, const Vertex &v1){
	float v0dist = currPoly->plane.distance(v0);
	float v1dist = currPoly->plane.distance(v1);
	float depth = 0;

	if (v0dist * v1dist < 0){
		// Different sides of plane
		Vertex hitPoint;
		if (currPoly->intersect(v0, v1, &hitPoint)){
			depth = length(hitPoint - v0);
			if (v0dist > 0) depth = -depth;
		}

		if (currPoly->front != NULL) depth += findDepth(currPoly->front, v0, v1);
		if (currPoly->back  != NULL) depth += findDepth(currPoly->back,  v0, v1);
	} else {
		// Same side of plane, no need to check other side of plane
		if (v0dist > 0){
			if (currPoly->front != NULL) depth += findDepth(currPoly->front, v0, v1);
		} else {
			if (currPoly->back  != NULL) depth += findDepth(currPoly->back,  v0, v1);
		}
	}

	return depth;
}


float CollisionSet::depth(const Vertex &v0, const Vertex &v1) const {
	float d = findDepth(top, v0, v1);
	if (d < 0) d += length(v1 - v0);

	return d;
}

