///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgBSPTools
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgBSPTools.cpp,v 1.1.1.1 2002/10/20 09:33:03 daniel Exp $
///////////////////////////////////////////////////////////////////


#include <D3DX8.h>

#include <pgLib/Render/BSP/pgBSPClasses.h>
#include <pgLib/Render/BSP/pgBSPFile.h>

bool twoSame(int a0, int a1, int a2, int b0, int b1, int b2)
{
	return	(a0==b0 && a1==b1) || (a0==b0 && a1==b2) || (a0==b1 && a1==b0) || (a0==b1 && a1==b2) || (a0==b2 && a1==b1) || (a0==b2 && a1==b0) ||
			(a0==b0 && a2==b1) || (a0==b0 && a2==b2) || (a0==b1 && a2==b0) || (a0==b1 && a2==b2) || (a0==b2 && a2==b1) || (a0==b2 && a2==b0) ||
			(a1==b0 && a2==b1) || (a1==b0 && a2==b2) || (a1==b1 && a2==b0) || (a1==b1 && a2==b2) || (a1==b2 && a2==b0) || (a1==b2 && a1==b1);
}


// searches if an index is already in the list
// if not the new index is added
// the index' position in the list is returned
WORD getIndexPos(int* nField, int& nFieldSize, int nIndex)
{
	WORD i;

	for(i=0; i<nFieldSize; i++)
		if(nField[i]==nIndex)
			break;

	if(i==nFieldSize)
		nField[nFieldSize++] = nIndex;

	return i;
}


void triangulatePoly(const bsp_face* nFace, const bsp_vertex* nSVertices, const int* nMeshIndices,
					 WORD* nIndices, BSPVERTEX* nDVertices, int& nNumIndices, int& nNumVertices)
{
	int		vIdx = nFace->vertex,
			//numV = nFace->n_vertexes,
			mvIdx = nFace->meshvert,
			numF = nFace->n_meshverts/3,
			j;
	int*	tInd = new int[nFace->n_meshverts];

	nNumIndices = nNumVertices = 0;

	for(j=0; j<numF; j++)
	{
		int idx0 = vIdx+nMeshIndices[mvIdx+2],
			idx1 = vIdx+nMeshIndices[mvIdx+1],
			idx2 = vIdx+nMeshIndices[mvIdx];

		WORD	i0 = getIndexPos(tInd, nNumVertices, idx0),
				i1 = getIndexPos(tInd, nNumVertices, idx1),
				i2 = getIndexPos(tInd, nNumVertices, idx2);

		nIndices[nNumIndices+0] = i0;
		nIndices[nNumIndices+1] = i1;
		nIndices[nNumIndices+2] = i2;
		nNumIndices += 3;
		mvIdx += 3;
	}

	for(j=0; j<nNumVertices; j++)
	{
		int idx = tInd[j];
		const bsp_vertex& v = nSVertices[idx];

		FILL_BSPVERTEX(nDVertices[j], v.pos[0], v.pos[2], v.pos[1], 0, v.texc[0][0], v.texc[0][1], v.texc[1][0], v.texc[1][1]);
	}

	delete tInd;
}


static float fact(int v)
{
	int r=1;

	for(int i=2; i<=v; i++)
		r *= i;

	return (float)r;
}

static float bernstein(int i, int n, float t)
{
	float	f = fact(n) / (fact(i)*fact(n-i)),
			f1 = (float)pow(t, i),
			f2 = (float)pow(1.0f-t, n-i);

	return f * f1 * f2;
}


static float getCurviness(const bsp_vertex* nV0, const bsp_vertex* nV1, const bsp_vertex* nV2)
{
	float	dx0 = nV0->pos[0] - nV1->pos[0],
			dy0 = nV0->pos[2] - nV1->pos[2],
			dz0 = nV0->pos[1] - nV1->pos[1],
			dx1 = nV2->pos[0] - nV1->pos[0],
			dy1 = nV2->pos[2] - nV1->pos[2],
			dz1 = nV2->pos[1] - nV1->pos[1];
	float	len0 = (float)sqrt(dx0*dx0 + dy0*dy0 + dz0*dz0),
			len1 = (float)sqrt(dx1*dx1 + dy1*dy1 + dz1*dz1);
	float	f, phi, ret;

	f = (dx0*dx1 + dy0*dy1 + dz0*dz1)/(len0*len1);
	phi = (float)acos(f) * 180.0f / 3.141592653f;

	ret = 1.0f - phi/180.0f;

	if(ret<0.0f)
		ret = 0.0f;

	return ret;
}


void triangulatePatch(const bsp_face* nFace, const bsp_vertex* nSVertices,
					  WORD* nIndices, BSPVERTEX* nDVertices, int& nNumIndices, int& nNumVertices, int nSubFact)
{
	int numSX = nFace->size[0],
		numSY = nFace->size[1];
	//int numPatches = (numSX-1)*(numSY-1)/4;
	int xs,ys, xd,yd, xi,yi, idx, n0;

	nNumIndices = 0;
	nNumVertices = 0;

	const bsp_vertex*	sVerts = nSVertices + nFace->vertex;


	for(ys=0; ys<numSY-1; ys+=2)								// run through all 3x3 patchs...
		for(xs=0; xs<numSX-1; xs+=2)
		{
			const bsp_vertex*	vs = sVerts+(xs+ys*numSX);
			int					numDX = nSubFact,
								numDY = nSubFact;
			//int					numD = (numDX+1)*(numDY+1);
			float				fx=0.0f, fy=0.0f;

			for(yi=0; yi<=2; yi++)
				fx += getCurviness(vs+yi*numSX, vs+yi*numSX+1, vs+yi*numSX+2) / 3.0f;

			for(xi=0; xi<=2; xi++)
				fy += getCurviness(vs+xi, vs+xi+numSX, vs+xi+2*numSX) / 3.0f;

			if(fy<0.001f)
				numDY = 1;
			if(fx<0.001f)
				numDX = 1;

			// calculate the vertices per 3x3 patch
			//
			n0 = nNumVertices;
			for(yd=0; yd<=numDY; yd++)							// run through all destination vertices in a 3x3 patch...
				for(xd=0; xd<=numDX; xd++)
				{
					float	px=0.0f, py=0.0f, pz=0.0f, pu1=0.0f, pv1=0.0f, pu2=0.0f, pv2=0.0f;
					float	xn = (float)(xd)/(float)(numDX),
							yn = (float)(yd)/(float)(numDY);

					for(yi=0; yi<=2; yi++)						// run through all 9 source vertices for every destination vertex...
						for(xi=0; xi<=2; xi++)
						{
							const bsp_vertex*	v = sVerts+(xs+xi+(ys+yi)*numSX);
							float				fx = bernstein(xi, 2, xn),
												fy = bernstein(yi, 2, yn);
							float				f = fx*fy;

							px  += v->pos[0] * f;
							py  += v->pos[1] * f;
							pz  += v->pos[2] * f;
							pu1 += v->texc[0][0] * f;
							pv1 += v->texc[0][1] * f;
							pu2 += v->texc[1][0] * f;
							pv2 += v->texc[1][1] * f;
						}

					idx = xd * (numDY+1) + yd;
					FILL_BSPVERTEX(nDVertices[n0+idx], px, pz, py, 0, pu1,pv1, pu2,pv2);
					nNumVertices++;										// count the vertices created for the current 3x3 patch
				}

			// create triangles from the calculated vertices
			//
			for(yd=0; yd<numDY; yd++)							// run through all quads in a 3x3 patch...
				for(xd=0; xd<numDX; xd++)
				{
					idx = n0 + xd * (numDY+1) + yd;

					nIndices[nNumIndices+0] = (unsigned short)(idx);
					nIndices[nNumIndices+1] = (unsigned short)(idx+numDY+1);
					nIndices[nNumIndices+2] = (unsigned short)(idx+numDY+1+1);

					nIndices[nNumIndices+3] = (unsigned short)(idx);
					nIndices[nNumIndices+4] = (unsigned short)(idx+numDY+1+1);
					nIndices[nNumIndices+5] = (unsigned short)(idx+1);

					nNumIndices += 6;
				}
		}
}


unsigned char lightUp(unsigned char c0)
{
	//return 170;

	int c = (int)(5+c0*1.4f);
	if(c>255)
		c = 255;
	return (unsigned char)(c);
}

