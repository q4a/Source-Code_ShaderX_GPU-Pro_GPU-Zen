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

#include "T3dLoader.h"
#include "../Platform.h"

MaterialTable::MaterialTable(){

}

MaterialTable::~MaterialTable(){
	clear();
}

void MaterialTable::clear(){
	MaterialName *matName;
	int i, len = materialNames.getSize();
	
	for (i = 0; i < len; i++){
		matName = materialNames[i];
		delete matName->name;
		delete matName->material;
		delete matName;
	}

	materialNames.clear();
}

void MaterialTable::addMaterialName(char *name, unsigned int sizeU, unsigned int sizeV, Material *mat, PolygonFlags flags){
	MaterialName *matName = new MaterialName;
	
	matName->name        = new char[strlen(name) + 1];
	strcpy(matName->name, name);
	matName->sizeU       = sizeU;
	matName->sizeV       = sizeV;
	matName->material    = mat;
	matName->flags       = flags;
	materialNames.add(matName);
}

bool MaterialTable::getMaterialFromName(char *name, unsigned int &sizeU, unsigned int &sizeV, Material *&mat, PolygonFlags &flags){
	MaterialName *matName;
	int i, len = materialNames.getSize();
	
	for (i = 0; i < len; i++){
		matName = materialNames[i];
		
		if (stricmp(name, matName->name) == 0){
			sizeU       = matName->sizeU;
			sizeV       = matName->sizeV;
			mat			= matName->material;
			flags       = matName->flags;
			return true;
		}
	}
	
	matName     = materialNames[0];
	sizeU       = matName->sizeU;
	sizeV       = matName->sizeV;
	mat			= matName->material;
	flags       = matName->flags;
	return false;
}
        
/* ------------------------------------------------------------------- */

T3dLoader::T3dLoader(){

}

T3dLoader::~T3dLoader(){

}

Vertex T3dLoader::readVertex(){
	char str[256];
	float x,y,z;
	
	tok.next(str);  // sign
	x = (*str == '+')? 1.0f : -1.0f;
	tok.next(str);  // number
	x *= intAdjustf((float) atof(str));
	
	tok.next(NULL); // ,
	tok.next(str);  // sign
	y = (*str == '+')? 1.0f : -1.0f;
	tok.next(str);  // number
	y *= intAdjustf((float) atof(str));
	
	tok.next(NULL); // ,
	tok.next(str);  // sign
	z = (*str == '+')? 1.0f : -1.0f;
	tok.next(str);  // number
	z *= intAdjustf((float) atof(str));
	
	return Vertex(x,y,z);
}

bool textureNameAlphabetical(const char ch){
	return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_' || ch == '.' || ch == '-');
}

bool T3dLoader::loadFromFile(const char *fileName, Set <class Polygon *> &polygons, MaterialTable &materialTable, PolygonCreator polyCreator, PolygonFilter polygonFilter, const Vertex &displacement){
	char str[256];
	Stack <STATE> states;
	STATE state = NONE;
	int i,len;

	// Polygon variables
	class Polygon *poly;
	//class Polygon *poly;
	Set <Vertex> vertices;
	int flags = 0;
	PolygonFlags pflags;
	Vertex normal;
	Vertex origo, textureU, textureV;
    float panU = 0, panV = 0;
	Material *material;
    unsigned int sizeU, sizeV;

	// Actor variables
	bool usePolygons = true;
	Vertex location(0,0,0);
	int vertexComponent;



	char debug[256];


	tok.setFile(fileName);

	while (tok.next(str)){
		if (stricmp(str, "Begin") == 0){
			tok.next(str);
			states.push(state);

			if (stricmp(str, "Map") == 0){
				state = MAP;
			} else if (stricmp(str, "Polygon") == 0){
				state = POLYGON;
				if (usePolygons){
					vertices.clear();
					flags = 0;
					pflags = PF_NONE;
                    panU = panV = 0.0f;
				}
			} else if (stricmp(str, "Actor") == 0){
				state = ACTOR;
				location = Vertex(0,0,0);
				usePolygons = true;
			} else {
				state = NONE;
			}
		} else if (stricmp(str, "End") == 0){
			tok.next(NULL);

			switch(state){
			case POLYGON:
				if (usePolygons){
					if (polygonFilter(flags)){
					//if ((flags & PF_Invisible) == 0){
						len = vertices.getSize();
						
						//poly = new class Polygon(len);
						poly = polyCreator(len);


						for (i = 0; i < len; i++){
							//poly->setVertex(i, vertices[i]);
                            poly->setVertex(len - i - 1, vertices[i] + displacement);
						}
						if (flags & PF_TwoSided){
							poly->setFlags(PF_DOUBLESIDED, true);
						}
						if (flags & PF_Unlit){
							poly->setFlags(PF_UNLIT, true);
						}
						if (flags & PF_Translucent){
							poly->setFlags(PF_TRANSLUCENT, true);
						}
						if (flags & PF_Portal){
							poly->setFlags(PF_PORTAL, true);
						}

                        /*if (flags & PF_SpecialPoly){
                            poly->setFlags(PF_FOGVOLUME | PF_NONBLOCKING, true);
                        }*/

                        

                        poly->setFlags(pflags, true);

                        //origo -= (panU * (textureU / length(textureU)) + panV * (textureV / length(textureV)));
                        //origo += ((-panU - 2) * textureU * 2 + panV * textureV * 2);

                        origo -= (panU * textureU) / lengthSqr(textureU);
                        origo -= (panV * textureV) / lengthSqr(textureV);

                        textureU = fix(textureU);
                        textureV = fix(textureV);

						poly->setTexCoordSystem(fix(origo) + displacement, textureU / (float) sizeU, textureV / (float) sizeV);
                        poly->setMaterial(material);
						poly->finalize();
						//poly->setNormal(normal);
						polygons.add(poly);

					}
				}
				break;
			default:
				break;
			}

			state = states.pop();
		} else {


			switch(state){
			case POLYGON:
				if (usePolygons){
					if (stricmp(str, "Flags") == 0){
						tok.next(NULL); // =
						tok.next(str);  // flags
						if (str[0] == '-'){
							tok.next(str);  // flags
							flags = -atoi(str);
						} else flags = atoi(str);
												
					} else if (polygonFilter(flags)){//if ((flags & PF_Invisible) == 0){
						if (stricmp(str, "Vertex") == 0){
							Vertex v = readVertex();
							vertices.add(fix(v + location));
						} else if (stricmp(str, "Normal") == 0){
							normal = fix(readVertex());
						} else if (stricmp(str, "Texture") == 0){
    						tok.next(NULL); // =
	    					tok.next(str, textureNameAlphabetical);  // texture name
                            materialTable.getMaterialFromName(str, sizeU, sizeV, material, pflags);

							strcpy(debug, str);

						} else if (stricmp(str, "TextureU") == 0){
							textureU = readVertex();
						} else if (stricmp(str, "TextureV") == 0){
							textureV = readVertex();
						} else if (stricmp(str, "Origin") == 0){
							origo = readVertex();
						} else if (stricmp(str, "Pan") == 0){
    						tok.next(NULL); // U
    						tok.next(NULL); // =
	    					tok.next(str);  // U pan or -
                            if (str[0] == '-'){
    	    					tok.next(str);  // U pan
		    				    panU = -(float) atof(str);
                            } else panU = (float) atof(str);

    						tok.next(NULL); // V
    						tok.next(NULL); // =
	    					tok.next(str);  // V pan or -
                            if (str[0] == '-'){
    	    					tok.next(str);  // V pan
		    				    panV = -(float) atof(str);
                            } else panV = (float) atof(str);
						}
					}
				}
				break;
			case ACTOR:
				if (stricmp(str, "Group") == 0){
					usePolygons = false;
				} else if (stricmp(str, "Location") == 0){
					float value;
					tok.next(NULL); // =
					tok.next(NULL); // (
					do {
						tok.next(str);
						vertexComponent = (*str - 'X');
						tok.next(NULL); // =
						tok.next(str);  // sign/value
						if (*str == '-'){
							tok.next(str);  // number
							value = -(float) atof(str);
						} else {
							value = (float) atof(str);
						}
						location[vertexComponent] = value;

						tok.next(str);
					} while (*str == ',');
				}
				break;
			default:
				break;
			}
		}
	}

	return true;
}

void fixTJunctions(Set <class Polygon *> &polys){
	unsigned int i, j, k, l, len = polys.getSize();
	Vertex prev, curr, dis;

	for (i = 0; i < len; i++){
		for (j = 0; j < len; j++){
			if (i != j){
				prev = polys[i]->getLastVertex();
				for (k = 0; k < polys[i]->getnVertices(); k++){
					curr = polys[i]->getVertex(k);
					dis = curr - prev;

					for (l = 0; l < polys[j]->getnVertices(); l++){
						
						float t = dis * (polys[j]->getVertex(l) - prev) / lengthSqr(dis);
						
						if (t > 0.1f && t < 0.9f){
							if (lengthSqr(prev + t * dis - polys[j]->getVertex(l)) < 0.05f){
								polys[i]->insertVertex(k, polys[j]->getVertex(l));
								k++;
 								//polys[i]->insertVertex(4, Vertex(0,0,0));
								return;
							}
						}
					}

					prev = curr;
				}
			}	
		}
	}

}

void tesselate(Set <class Polygon *> &polys, const float maxArea, const float maxLength){
	unsigned int i, j, len;

	i = 0;
	do {
		float area = 0, max = 0;
		unsigned maxIndex = 0;

		class Polygon *poly = polys[i];

		len = poly->getnVertices();
		Vertex prev = poly->getVertex(len - 1);
		for (j = 0; j < len; j++){
			Vertex curr = poly->getVertex(j);

			if (j > 1){
				area += length(cross(curr - poly->getVertex(0), prev - poly->getVertex(0)));
			}

			float d = lengthSqr(prev - curr);
			if (d > max){
				max = d;
				maxIndex = j;
			}
			
			prev = curr;			
		}

		if (area > maxArea || max > sqrf(maxLength)){
			Vertex v0 = poly->getVertex(maxIndex == 0? len - 1 : maxIndex - 1);
			Vertex v1 = poly->getVertex(maxIndex);
			Vertex mid0 = 0.5f * (v0 + v1);

			Vertex curr = poly->getVertex(maxIndex);
			float a, halfArea = 0;
			unsigned int otherIndex = maxIndex;
			do {
				otherIndex++;
				prev = curr;			
				curr = poly->getVertex(otherIndex % len);

				a = length(cross(curr - mid0, prev - mid0));
				halfArea += a;

			} while (2 * halfArea < area);

			float t = (halfArea - area * 0.5f) / a;
			Vertex mid1 = curr + t * (prev - curr);

			class Polygon *poly0, *poly1;

			unsigned int n0, n1;
			n0 = (otherIndex < maxIndex)? otherIndex - maxIndex + len : otherIndex - maxIndex;
			n1 = len - n0;

			poly0 = poly->create(n0 + 2);
			poly->initialize(poly0);
			poly1 = poly->create(n1 + 2);
			poly->initialize(poly1);
			
			poly0->setVertex(0, mid1);
			poly0->setVertex(1, mid0);
			for (j = 0; j < n0; j++){
				poly0->setVertex(j + 2, poly->getVertex((maxIndex + j) % len));
			}
			poly1->setVertex(0, mid0);
			poly1->setVertex(1, mid1);
			for (j = 0; j < n1; j++){
				poly1->setVertex(j + 2, poly->getVertex((otherIndex + j) % len));
			}

			poly0->finalize();
			poly1->finalize();
			
			delete poly;
			polys.remove(i);

			polys.add(poly0);
			polys.add(poly1);

		} else {
			i++;
		}

	} while (i < polys.getSize());
}
