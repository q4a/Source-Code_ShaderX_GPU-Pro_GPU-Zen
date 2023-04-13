///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgComplexTree
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgComplexTree.cpp,v 1.3 2002/12/22 10:50:55 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgComplexTree.h"

#include <pgLib/Base/pgIDirectX.h>
#include <pgLib/Base/pgInTextFile.h>
#include <pgLib/Base/pgIResourceManager.h>
#include <pgLib/Base/pgISettings.h>
#include <pgLib/Base/pgIStringTool.h>
#include <pgLib/Base/pgList.h>
#include <pgLib/Base/pgLog.h>
#include <pgLib/FrameWork/dxutil.h>
#include <pgLib/Math/pgIMathTool.h>


#define D3DFVF_VERTEX ( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

#define FILL_VERTEX( v, ptx, pty, ptz, col)  \
{   v.pos.x = (ptx);  v.pos.y = (pty);  v.pos.z = (ptz);  \
    v.color = (col);  \
}


pgComplexTree::CBranch::CBranch(const pgList<pgVec3>& nKnotList, int nNumSides,
								float nRadius0, float nRadius1, const pgVec3n& nBaseDir) : texRepeat(1.0f, 1.0f)
{
	knotList = nKnotList;
	numSides = nNumSides;
	radius0 = nRadius0;
	radius1 = nRadius1;
	baseDir = nBaseDir;
}


pgComplexTree::pgComplexTree() : iBuffer(NULL), vBuffer(NULL)
{
	indices = NULL;
	vertices = NULL;
	branchTex = leafTex = NULL;
	numBranchIndices = numLeafIndices = numVertices = 0;
}


void
pgComplexTree::addBranch(const pgList<pgVec3>& nPosList, int nNumSides,
						 float nRadius0, float nRadius1, const pgVec3n& nBaseDir)
{
	branches.addTail(new CBranch(nPosList, nNumSides, nRadius0, nRadius1, nBaseDir));
}


void 
pgComplexTree::addCylinder(const CBranch& nBranch, pgList<WORD>& nIList, pgList<VERTEX>& nVList)
{
	const pgList<pgVec3>&	knotList = nBranch.knotList;
	const pgVec3&			baseDir = nBranch.baseDir;
	float					radius0 = nBranch.radius0,
							radius1 = nBranch.radius1;
	int						numSides = nBranch.numSides;
	const pgVec2&			texRepeat = nBranch.texRepeat;
	bool					addfirstTwice = false;

	int p,s, numP = knotList.getSize(), idx;
	int baseIdx = nVList.getSize();

	for(p=0; p<numP; p++)
	{
		const pgVec3	&p0 = knotList[p];
		pgVec3n			dir;

		if(p<numP-1)
			dir = knotList[p+1]-p0;
		else
			dir = p0 - knotList[p-1];

		if(p==0)
			dir = baseDir;

		pgVec3n			someVec = dir.getDifferentVector();
		pgVec3n			nu, nv;

		nu.cross(dir, someVec);
		nv.cross(nu, dir);

		float	radius = radius0 + (radius1-radius0) * (float)(p) / numP;

		for(s=0; s<=numSides; s++)
		{
			float	phi = (s * pgFTWOPI) / numSides;
			float	du = (float)cos(phi) * radius,
					dv = (float)sin(phi) * radius;
			VERTEX	v;

			v.pos = p0 + nu * du + nv * dv;
			v.color = 0xff503700;
			v.tex.x = texRepeat.x * (float)s / (float)numSides;
			v.tex.y = texRepeat.y * (float)p / (float)(numP-1);

			nVList.addTail(v);
		}
	}

	// if this is not the first cylinder,
	// we need to add a degen triangle
	if(nIList.getSize())
	{
		nIList.addTailAgain();
		addfirstTwice = true;
	}

	for(p=0; p<numP-1; p++)
	{
		for(s=0; s<=numSides; s++)
		{
			idx = baseIdx + s + p*(numSides+1);
			assert(idx<nVList.getSize());
			nIList.addTail((WORD)idx);
			if((p>0 && s==0) || addfirstTwice)
				nIList.addTailAgain();

			idx = baseIdx + s + (p+1)*(numSides+1);
			assert(idx<nVList.getSize());
			nIList.addTail((WORD)idx);
			if(p<numP-2 && s==numSides)
				nIList.addTailAgain();

			addfirstTwice = false;
		}
	}
}


void
pgComplexTree::addLeaf(const pgVec3& nPos, const pgVec3n& nNormal, float nAngle, float nSizeU, float nSizeV,
					   float repeatX, float repeatY, pgList<WORD>& nIList, pgList<VERTEX>& nVList)
{
	bool		addfirstTwice = false;
	int			baseIdx = nVList.getSize();
	VERTEX		v;
	pgVec3n		someVec = nNormal.getDifferentVector();
	pgVec3n		nu, nv;
	float		du=nSizeU/2, dv=nSizeV/2;
	D3DXMATRIX	mat;
	D3DXVECTOR3	dxvec3;
	D3DXVECTOR4 dxvec4;

	nu.cross(nNormal, someVec);
	nv.cross(nu, nNormal);

	// rotate the two unit vector around nNormal by angle nAngle
	//
	pgIDirectX::setD3DVecFromVec3(dxvec3, nNormal);
	D3DXMatrixRotationAxis(&mat, &dxvec3, nAngle*pgFPI/180.0f);

	pgIDirectX::setD3DVecFromVec3(dxvec3, nu);
	D3DXVec3Transform(&dxvec4, &dxvec3, &mat);
	pgIDirectX::setVec3FromVecD3D(nu, dxvec4);


	pgIDirectX::setD3DVecFromVec3(dxvec3, nv);
	D3DXVec3Transform(&dxvec4, &dxvec3, &mat);
	pgIDirectX::setVec3FromVecD3D(nv, dxvec4);


	v.color = 0xff00aa00;

	v.pos = nPos - nu * du - nv * dv;
	v.tex = pgVec2(0.0f, repeatY);
	nVList.addTail(v);

	v.pos = nPos - nu * du + nv * dv;
	v.tex = pgVec2(0.0f, 0.0f);
	nVList.addTail(v);

	v.pos = nPos + nu * du - nv * dv;
	v.tex = pgVec2(repeatX, repeatY);
	nVList.addTail(v);

	v.pos = nPos + nu * du + nv * dv;
	v.tex = pgVec2(repeatX, 0.0f);
	nVList.addTail(v);


	// if this is not the first cylinder,
	// we need to add a degen triangle
	if(nIList.getSize())
	{
		nIList.addTailAgain();
		addfirstTwice = true;
	}

	nIList.addTail((WORD)(baseIdx));
	if(addfirstTwice)
	{
		nIList.addTailAgain();
		addfirstTwice = false;
	}

	nIList.addTail((WORD)(baseIdx+1));
	nIList.addTail((WORD)(baseIdx+2));
	nIList.addTail((WORD)(baseIdx+3));
}


void
pgComplexTree::addLeafs(int nNumLeafs, const pgVec3& nPos, const pgVec3& nSize, float nFuzzy,
						pgList<WORD>& nIList, pgList<VERTEX>& nVList)
{
	float	r, rMin = 0.5f-nFuzzy, rMax = 0.5f+nFuzzy;
	pgVec3	d;

	for(int i=0; i<nNumLeafs; i++)
	{
		pgVec3n n(pgIMathTool::random(-1.0f, 1.0f),
			      pgIMathTool::random(-1.0f, 1.0f),
				  pgIMathTool::random(-1.0f, 1.0f));
		float	phi = pgIMathTool::random(0.0f, 360.0f);

		do
		{
			d = pgVec3(pgIMathTool::random(-0.5f, 0.5f),
					   pgIMathTool::random(-0.5f, 0.5f),
					   pgIMathTool::random(-0.5f, 0.5f));
			r = d.length();
		} while(r<rMin || r>rMax);

		d.x *= nSize.x;
		d.y *= nSize.y;
		d.z *= nSize.z;

		addLeaf(nPos+d, n, phi, 100.0f, 100.0f, 1.0f, 1.0f,
				nIList, nVList);
	}
}


void
pgComplexTree::create()
{
	if(!checkDevice("cannot create complex tree: no render device available"))
		return;

	pgList<WORD>			iList;
	pgList<VERTEX>			vList;

	for(int i=0; i<branches.getSize(); i++)
		addCylinder(*branches[i], iList, vList);
	numBranchIndices = iList.getSize();

	addLeafs(20, pgVec3( 200.0f, 520.0f,  80.0f), pgVec3(150.0f, 150.0f, 150.0f), 0.05f, iList, vList);
	addLeafs(20, pgVec3(-200.0f, 450.0f,  30.0f), pgVec3(150.0f, 150.0f, 150.0f), 0.05f, iList, vList);
	addLeafs(20, pgVec3(  60.0f, 350.0f,-180.0f), pgVec3(150.0f, 150.0f, 150.0f), 0.05f, iList, vList);
	addLeafs(20, pgVec3( -60.0f, 470.0f, 220.0f), pgVec3(150.0f, 150.0f, 150.0f), 0.05f, iList, vList);
	addLeafs(20, pgVec3( -60.0f, 470.0f,-150.0f), pgVec3(150.0f, 150.0f, 150.0f), 0.05f, iList, vList);

	numLeafIndices = iList.getSize() - numBranchIndices;

	numIndices = numBranchIndices + numLeafIndices;

	if(indices)
		delete indices;
	indices = new WORD[numIndices];
	memcpy(indices, &iList.getHead(), numIndices * sizeof(WORD));

	if(vertices)
		delete vertices;
	numVertices = vList.getSize();
	vertices = new VERTEX[numVertices];
	memcpy(vertices, &vList.getHead(), numVertices * sizeof(VERTEX));

	createBuffers();
}


void
pgComplexTree::freeBuffers()
{
	SAFE_RELEASE(iBuffer);
	SAFE_RELEASE(vBuffer);
}


bool
pgComplexTree::createBuffers()
{
	SAFE_RELEASE(iBuffer);
	SAFE_RELEASE(vBuffer);

	if(numIndices<=0 || numVertices<=0)
		return true;


	if(FAILED(pd3dDevice->CreateIndexBuffer(numIndices* sizeof(WORD), D3DUSAGE_WRITEONLY,
											D3DFMT_INDEX16, pgIDirectX::getPoolType(), &iBuffer)))
	{
		pgLog::error("creating index buffer for tree failed");
		return false;
	}

	if(FAILED(pd3dDevice->CreateVertexBuffer(numVertices * sizeof(VERTEX), D3DUSAGE_WRITEONLY,
											 D3DFVF_VERTEX, pgIDirectX::getPoolType(), &vBuffer)))
	{
		pgLog::error("creating vertex buffer for tree failed");
		SAFE_RELEASE(iBuffer);
		return false;
	}


	WORD*	pIndices = NULL;
	VERTEX*	pVertices = NULL;

	iBuffer->Lock(0, 0, (BYTE**)&pIndices, D3DLOCK_NOSYSLOCK);
	vBuffer->Lock(0, 0, (BYTE**)&pVertices, D3DLOCK_NOSYSLOCK);

	memcpy(pIndices, indices, numIndices * sizeof(WORD));
	memcpy(pVertices, vertices, numVertices * sizeof(VERTEX));

	iBuffer->Unlock();
	vBuffer->Unlock();

	return true;
}


void
pgComplexTree::render()
{
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   branchTex ? D3DTOP_SELECTARG1 : D3DTOP_SELECTARG2);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, pgIDirectX::getCullFlag(D3DCULL_CCW));
	pd3dDevice->SetRenderState(D3DRS_FILLMODE, pgIDirectX::getFillFlag());

	if(iBuffer && vBuffer)
	{
		pd3dDevice->SetVertexShader(D3DFVF_VERTEX);
		pd3dDevice->SetStreamSource(0, vBuffer, sizeof(VERTEX));
		pd3dDevice->SetIndices(iBuffer, 0);

		if(branchTex && numBranchIndices>2)
		{
			pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

			pd3dDevice->SetTexture(0, branchTex->getD3DTexture());
			pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, numVertices, 0, numBranchIndices-2);
			pgIDirectX::addTerrainTris(numBranchIndices-2);
		}

		if(leafTex && numLeafIndices>2)
		{
			pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
			pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			pd3dDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000000);
			pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); 
			pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

			pd3dDevice->SetTexture(0, leafTex->getD3DTexture());
			pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, numVertices, numBranchIndices, numLeafIndices-2);
			pgIDirectX::addTerrainTris(numLeafIndices-2);
		}
	}
}


bool
pgComplexTree::loadFromFile(const pgString& nFile)
{
	pgInTextFile	file;
	pgString		fullPath = pgISettings::getAppPath();
	CBranch*		branch = NULL;

	fullPath += pgString("/tree/") + nFile;

	if(!file.open(fullPath))
	{
		pgLog::error("can not load tree: '%s'", nFile);
		return false;
	}


	while(!file.eof())
	{
		const char *line, * line0 = file.readLine();

		if(pgIStringTool::startsWith(line0, "#") || pgIStringTool::isEmpty(line0))
			continue;																	// ignore this line

		line = pgIStringTool::skipNonSpaces(line0);
		line = pgIStringTool::skipSpaces(line);


		if(pgIStringTool::startsWith(line0, "BRANCHTEX"))
			branchTex = pgIResourceManager::getTexture(line, pgIResourceManager::SOURCE_STD);


		if(pgIStringTool::startsWith(line0, "LEAFTEX"))
			leafTex = pgIResourceManager::getTexture(line, pgIResourceManager::SOURCE_STD);


		if(pgIStringTool::startsWith(line0, "BRANCH"))
		{
			if(branch)
				branches.addTail(branch);
			branch = new CBranch();
		}

		if(pgIStringTool::startsWith(line0, "SIDES"))
		{
			if(!branch)
			{
				pgLog::error("SIDES before BRANCH in tree file '%s'", nFile);
				return false;
			}

			branch->numSides = atoi(line);
		}

		if(pgIStringTool::startsWith(line0, "RADIUS"))
		{
			if(!branch)
			{
				pgLog::error("RADIUS before BRANCH in tree file '%s'", nFile);
				return false;
			}

			pgVec2 vec2;

			pgIStringTool::readVec2(line, vec2);
			branch->radius0 = vec2[0];
			branch->radius1 = vec2[1];
		}

		if(pgIStringTool::startsWith(line0, "BASEDIR"))
		{
			if(!branch)
			{
				pgLog::error("BASEDIR before BRANCH in tree file '%s'", nFile);
				return false;
			}

			pgIStringTool::readVec3(line, branch->baseDir);
		}

		if(pgIStringTool::startsWith(line0, "TEXREPEAT"))
		{
			if(!branch)
			{
				pgLog::error("TEXREPEAT before BRANCH in tree file '%s'", nFile);
				return false;
			}

			pgIStringTool::readVec2(line, branch->texRepeat);
		}

		if(pgIStringTool::startsWith(line0, "KNOT"))
		{
			if(!branch)
			{
				pgLog::error("KNOT before BRANCH in tree file '%s'", nFile);
				return false;
			}

			pgVec3 vec3;
			pgIStringTool::readVec3(line, vec3);
			branch->knotList.addTail(vec3);
		}

	}

	if(branch)
		branches.addTail(branch);

	file.close();
	return true;
}

