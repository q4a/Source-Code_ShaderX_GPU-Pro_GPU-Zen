///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgBSPTools
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgBSPTools.h,v 1.1.1.1 2002/10/20 09:33:03 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __BSPTOOLS_HEADEFFILE__
#define __BSPTOOLS_HEADEFFILE__

bool
twoSame(int a0, int a1, int a2, int b0, int b1, int b2);

void
triangulatePatch(const bsp_face* nFace, const bsp_vertex* nSVertices, WORD* nIndices, BSPVERTEX* nDVertices,
				 int& nNumIndices, int& nNumVertices, int nSubFact);

void
triangulatePoly(const bsp_face* nFace, const bsp_vertex* nSVertices, const int* nMeshIndices, WORD* nIndices, BSPVERTEX* nDVertices,
				int& nNumIndices, int& nNumVertices);

unsigned char
lightUp(unsigned char c0);



#endif //__BSPTOOLS_HEADEFFILE__
