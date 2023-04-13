                  /*--.          .-"-. 
                 /   ._.        / ´ ` \
                 \_  (__\       \_°v°_/
                 //   \\        //   \\
                ((     ))      ((     ))
 ¤¤¤¤¤¤¤¤¤¤¤¤¤¤--""---""--¤¤¤¤--""---""--¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
 ¤                 |||            |||                                               ¤
 ¤  Authors note:   |              |                                                ¤
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

#include "BSP.h"

BSPNode::BSPNode(){

}

BSPNode::~BSPNode(){
	delete polygon;

	if (back  != NULL) delete back;
	if (front != NULL) delete front;
}

void BSPNode::build(Set <class Polygon *> &polys){
	Set <class Polygon *> backList, frontList;

	int cutterScore = 0x7FFFFFFF, score, diff;
	int i, j, rel, cutterIndex = 0;
	
	
	// Select the cutting surface
	for (i = 0; i < polys.getSize(); i++){
		score = 0;
		diff = 0;
		for (j = 0; j < polys.getSize(); j++){
			if (i != j){
				rel = polys[i]->polygonRelation(*polys[j]);

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

	polygon = polys[cutterIndex];
	polys.remove(cutterIndex);
	
	// Split with the selected surface
	class Polygon *backPoly, *frontPoly;
	for (i = 0; i < polys.getSize(); i++){
		//rel = polygon->split(*polys[i], backPoly, frontPoly);
		rel = polys[i]->split(polygon->getPlane(), backPoly, frontPoly);

		if (rel == BACK)  backList.add(polys[i]);
		if (rel == FRONT || rel == PLANAR) frontList.add(polys[i]);
		if (rel == CUTS){
			backList.add(backPoly);
			frontList.add(frontPoly);
			delete polys[i];
		}
	}

	if (backList.getSize() > 0){
		back = new BSPNode;
		back->build(backList);
	} else back = NULL;

	if (frontList.getSize() > 0){
		front = new BSPNode;
		front->build(frontList);
	} else front = NULL;
}

void BSPNode::draw(Renderer *renderer){
	polygon->draw(renderer);

	if (front != NULL) front->draw(renderer);
	if (back  != NULL) back->draw(renderer);
}


/* ------------------------------------------------------------------------------ */



BSPTree::BSPTree(){
	top = NULL;
}

BSPTree::~BSPTree(){
	if (top != NULL) delete top;
}

void BSPTree::build(Set <class Polygon *> &polys){
	top = new BSPNode;
	top->build(polys);
}

void BSPTree::draw(Renderer *renderer){
	if (top != NULL) top->draw(renderer);
}

