///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       Wavefront OBJ Tool Methods
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgMeshUtil_OBJ.cxx,v 1.5 2002/12/18 22:00:37 daniel Exp $
///////////////////////////////////////////////////////////////////


#include <pgLib/Base/pgList.h>
#include <pgLib/Base/pgLog.h>
#include <pgLib/Base/pgIResourceManager.h>
#include <pgLib/Base/pgISettings.h>
#include <pgLib/Base/pgIStringTool.h>

#include <pgLib/Math/pgVec2.h>
#include <pgLib/Math/pgVec3n.h>

#include <pgLib/Render/pgMaterial.h>
#include <pgLib/Render/pgMesh.h>
#include <pgLib/Render/pgSegment.h>


bool
pgMeshUtil::fillFrameOBJ(pgMaterial* nMaterial, pgMesh* mesh, pgSegment* segment, pgBaseMesh* baseMesh, pgBaseMesh::Frame* frame,
						 const pgMeshUtil::OBJInfo& nOI)
{
	int							i, i0, numV, face, v;
	pgVec3						vertex0;
	pgVec2						texCoord0;
	pgVec3n						normal0;
	pgBaseMesh::MESHVERTEX_LIG1	*vsLig1 = NULL;
	pgBaseMesh::MESHVERTEX_COL1	*vsCol1 = NULL;
	pgBaseMesh::MESHVERTEX_TEX2	*vsTex2 = NULL;
	bool						bTex2 = nOI.tIndices2.getSize()>0;


	if(!frame || !baseMesh || !segment)			// did we already parse a segment ?
		return false;

	if(nOI.vIndices.getSize()<3)
	{
		pgLog::error("error parsing OBJ file '%s': no vertices in group %d", nOI.nName, mesh->getNumSegments()-1);
		return false;
	}

	bool lighting = nOI.nIndices[0]!=-1 && nOI.nLighting;

	frame->numIndices = 0;
	frame->numTriangles = 0;
	for(i=0; i<nOI.lengths.getSize(); i++)
		frame->numTriangles += nOI.lengths[i]-2;
	frame->numVertices = 3 * frame->numTriangles;

	if(bTex2)
	{
		if(FAILED(baseMesh->pd3dDevice->CreateVertexBuffer(frame->numVertices * sizeof(pgBaseMesh::MESHVERTEX_TEX2),
														   pgIDirectX::getDisableWriteOnlyBuffers() ? 0 : D3DUSAGE_WRITEONLY,
														   D3DFVF_MESHVERTEX_TEX2, pgIDirectX::getPoolType(), &frame->vBuffer)))
		{
			pgLog::error("creating vertex buffer for TEXTURE2-basemesh failed");
			return false;
		}
		frame->vBuffer->Lock(0, 0, (BYTE**)&vsTex2, 0);
		baseMesh->vertexSettings |= pgBaseMesh::SET_TEXTURE2;
	}
	else
	{
		if(lighting)
		{
			if(FAILED(baseMesh->pd3dDevice->CreateVertexBuffer(frame->numVertices * sizeof(pgBaseMesh::MESHVERTEX_LIG1),
															   pgIDirectX::getDisableWriteOnlyBuffers() ? 0 : D3DUSAGE_WRITEONLY,
															   D3DFVF_MESHVERTEX_LIG1, pgIDirectX::getPoolType(), &frame->vBuffer)))
			{
				pgLog::error("creating vertex buffer for LIGHT1-basemesh failed");
				return false;
			}
			frame->vBuffer->Lock(0, 0, (BYTE**)&vsLig1, 0);
			baseMesh->vertexSettings |= pgBaseMesh::SET_LIGHT | pgBaseMesh::SET_TEXTURE1;
		}
		else
		{
			if(FAILED(baseMesh->pd3dDevice->CreateVertexBuffer(frame->numVertices * sizeof(pgBaseMesh::MESHVERTEX_COL1),
															   pgIDirectX::getDisableWriteOnlyBuffers() ? 0 : D3DUSAGE_WRITEONLY,
															   D3DFVF_MESHVERTEX_COL1, pgIDirectX::getPoolType(), &frame->vBuffer)))
			{
				pgLog::error("creating vertex buffer for COLOR1-basemesh failed");
				return false;
			}
			frame->vBuffer->Lock(0, 0, (BYTE**)&vsCol1, 0);
			baseMesh->vertexSettings |= pgBaseMesh::SET_COLOR | pgBaseMesh::SET_TEXTURE1;
		}
	}


	// this method creates all the faces from the indexed data
	//
	i0 = 0;
	numV = 0;
	for(face=0; face<nOI.lengths.getSize(); face++)
	{
		int fLen = nOI.lengths[face];

		int	vi0 = nOI.vIndices[i0+0];
		int	ti0a = nOI.tIndices[i0+0];
		int	ti0b = bTex2 ? nOI.tIndices2[i0+0] : 0;
		int	ni0 = nOI.nIndices[i0+0];

		const pgVec3& v0 = (vi0>=1 && vi0<=nOI.vertices.getSize()) ? nOI.vertices[vi0-1] : vertex0;
		const pgVec2& t0a = (ti0a>=1 && ti0a<=nOI.texCoords.getSize()) ? nOI.texCoords[ti0a-1] : texCoord0;
		const pgVec2& t0b = (ti0b>=1 && ti0b<=nOI.texCoords2.getSize()) ? nOI.texCoords2[ti0b-1] : texCoord0;
		const pgVec3n& n0 = (ni0>=1 && ni0<=nOI.normals.getSize()) ? nOI.normals[ni0-1] : normal0;


		// create triangles from arbitrary (convex) polygons)
		//
		for(v=1; v<fLen-1; v++)
		{
			assert(numV<=frame->numVertices-3);

			int vi1 = nOI.vIndices[i0+v];
			int ti1a = nOI.tIndices[i0+v];
			int ti1b = bTex2 ? nOI.tIndices2[i0+v] : 0;
			int ni1 = nOI.nIndices[i0+v];

			int vi2 = nOI.vIndices[i0+v+1];
			int ti2a = nOI.tIndices[i0+v+1];
			int ti2b = bTex2 ? nOI.tIndices2[i0+v+1] : 0;
			int ni2 = nOI.nIndices[i0+v+1];

			const pgVec3& v1 = (vi1>=1 && vi1<=nOI.vertices.getSize()) ? nOI.vertices[vi1-1] : vertex0;
			const pgVec2& t1a = (ti1a>=1 && ti1a<=nOI.texCoords.getSize()) ? nOI.texCoords[ti1a-1] : texCoord0;
			const pgVec2& t1b = (ti1b>=1 && ti1b<=nOI.texCoords.getSize()) ? nOI.texCoords2[ti1b-1] : texCoord0;
			const pgVec3n& n1 = (ni1>=1 && ni1<=nOI.normals.getSize()) ? nOI.normals[ni1-1] : normal0;

			const pgVec3& v2 = (vi2>=1 && vi2<=nOI.vertices.getSize()) ? nOI.vertices[vi2-1] : vertex0;
			const pgVec2& t2a = (ti2a>=1 && ti2a<=nOI.texCoords.getSize()) ? nOI.texCoords[ti2a-1] : texCoord0;
			const pgVec2& t2b = (ti2b>=1 && ti2b<=nOI.texCoords2.getSize()) ? nOI.texCoords2[ti2b-1] : texCoord0;
			const pgVec3n& n2 = (ni2>=1 && ni2<=nOI.normals.getSize()) ? nOI.normals[ni2-1] : normal0;

			if(bTex2)
			{
				// currently: ignore lighting and color if there are two tex coords per vertex
				//
				FILL_MESHVERTEX_TEX2B(vsTex2[numV+0], v0, t0a, t0b)
				FILL_MESHVERTEX_TEX2B(vsTex2[numV+1], v1, t1a, t1b)
				FILL_MESHVERTEX_TEX2B(vsTex2[numV+2], v2, t2a, t2b)
			}
			else
			{
				if(lighting)
				{
					FILL_MESHVERTEX_LIG1B(vsLig1[numV+0], v0, n0, t0a)
					FILL_MESHVERTEX_LIG1B(vsLig1[numV+1], v1, n1, t1a)
					FILL_MESHVERTEX_LIG1B(vsLig1[numV+2], v2, n2, t2a)
				}
				else
				{
					FILL_MESHVERTEX_COL1B(vsCol1[numV+0], v0, 0xffffffff, t0a)
					FILL_MESHVERTEX_COL1B(vsCol1[numV+1], v1, 0xffffffff, t1a)
					FILL_MESHVERTEX_COL1B(vsCol1[numV+2], v2, 0xffffffff, t2a)
				}
			}

			numV += 3;
		}

		i0 += fLen;
	}

	frame->vBuffer->Unlock();

	pgMaterial* mat;

	if(nMaterial)
		mat = nMaterial;
	else
	{
		mat = new pgMaterial();
		mat->addStage(pgTextureStage(pgIResourceManager::getTexture("test2", pgIResourceManager::SOURCE_STD)));
		mat->setCulling(pgMaterial::CULL_CW);
	}

	segment->setRenderSettings(pgSegment::SET_ZTEST|pgSegment::SET_ZWRITE|pgSegment::SET_FILL);

	mat->setBlending(pgMaterial::BLEND_NONE, pgMaterial::BLEND_NONE);

	if(mat->getStage(0).getTexture())
		segment->enableRenderSettings(pgSegment::SET_TEXTURED);
	if(lighting)
		segment->enableRenderSettings(pgSegment::SET_LIGHT);
	if(mat->getNumStages()>1 || nOI.nIndices.getSize()==0)
		segment->disableRenderSettings(pgSegment::SET_LIGHT);
	for(i=0; i<mat->getNumStages(); i++)
	{
		const pgTexture* tex = mat->getStage(i).getTexture();
		pgImage::FORMAT form = tex ? tex->getFormat() : pgImage::UNKNOWN;
		if(pgImage::hasAlpha(form))
		{
			mat->setBlending(pgMaterial::BLEND_SRCALPHA, pgMaterial::BLEND_INVSRCALPHA);
			segment->disableRenderSettings(pgSegment::SET_LIGHT);
		}
	}
	

	segment->setMaterial(mat);

	return true;
}


bool
pgMeshUtil::initObjects(pgMesh* &mesh, pgSegment* &segment, pgBaseMesh* &baseMesh, pgBaseMesh::Frame* &frame)
{
	frame = new pgBaseMesh::Frame();
	baseMesh = new pgBaseMesh();
	baseMesh->getFrames().addTail(frame);
	baseMesh->vertexSettings = 0;
	if(!setDevice(baseMesh, "DirectX Device does not exist yet. cannot create mesh from OBJ file"))
	{
		delete baseMesh;
		return false;
	}
	segment = new pgSegment();
	segment->setBaseMesh(baseMesh);
	mesh->addSegment(segment);

	return true;
}


bool
pgMeshUtil::loadMTL(const char* nName, pgPtrList<pgMaterial>& nMaterials)
{
	const char*			fullPath = pgISettings::tmpFullOBJPath(nName);
	pgMaterial*			curMat = NULL;
	int					lineNr = 0;
	pgInTextFile		inFile;

	if(!inFile.open(fullPath))
	{
		pgLog::error("can not open MTL file '%s' for reading", nName);
		return false;
	}


	while(!inFile.eof())
	{
		const char* line = inFile.readLine();
		lineNr++;

		if(pgIStringTool::startsWith(line, "#") || pgIStringTool::isEmpty(line))
			continue;																	// ignore this line

		if(pgIStringTool::startsWith(line, "newmtl"))
		{
			curMat = new pgMaterial();
			curMat->setCulling(pgMaterial::CULL_CW);
			curMat->setName(pgIStringTool::skipSpaces(pgIStringTool::skipNonSpaces(line)));
			nMaterials.addTail(curMat);
		}
		else
		if(pgIStringTool::startsWith(line, "Ka"))
		{
			const char* str = pgIStringTool::skipSpaces(line);
			float r,g,b;
			sscanf(str, "Ka %f %f %f", &r, &g, &b);
			if(curMat)
				curMat->setAmbient(pgVec4(r,g,b,1.0f));
		}
		else
		if(pgIStringTool::startsWith(line, "Kd"))
		{
			const char* str = pgIStringTool::skipSpaces(line);
			float r,g,b;
			sscanf(str, "Kd %f %f %f", &r, &g, &b);
			if(curMat)
				curMat->setDiffuse(pgVec4(r,g,b,1.0f));
		}
		else
		if(pgIStringTool::startsWith(line, "Ks"))
		{
			const char* str = pgIStringTool::skipSpaces(line);
			float r,g,b;
			sscanf(str, "Ks %f %f %f", &r, &g, &b);
			if(curMat)
				curMat->setSpecular(pgVec4(r,g,b,1.0f));
		}
		else
		if(pgIStringTool::startsWith(line, "map_Kd"))
		{
			const char* str = pgIStringTool::skipSpaces(line);
			char name[512];

			sscanf(str, "map_Kd %s", name);
			if(curMat)
				curMat->addStage(pgTextureStage(pgIResourceManager::getTexture(name, pgIResourceManager::SOURCE_OBJ)));
		}
		else
		if(pgIStringTool::startsWith(line, "map_Ks"))
		{
			const char* str = pgIStringTool::skipSpaces(line);
			char name[512];

			sscanf(str, "map_Ks %s", name);
			if(curMat)
				curMat->addStage(pgTextureStage(pgIResourceManager::getTexture(name, pgIResourceManager::SOURCE_OBJ)));
		}
	}

	return true;
}


pgMesh*
pgMeshUtil::loadOBJ(const char* nName, bool nLighting)
{
	pgMesh*					mesh = NULL;
	pgSegment*				segment = NULL;
	pgBaseMesh*				baseMesh = NULL;
	pgBaseMesh::Frame*		frame = NULL;
	pgPtrList<pgMaterial>	materials;
	OBJInfo					oi;
	pgMaterial*				curMat = NULL;
	pgInTextFile			inFile;
	const char*				fullPath = pgISettings::tmpFullOBJPath(nName, ".obj");
	int						lineNr=0;
	int						i,j;
	pgVec4					ambient(1.0f,1.0f,1.0f,1.0f), diffuse(1.0f,1.0f,1.0f,1.0f), specular, emissive;

	oi.nLighting = nLighting;
	oi.nName = nName;

	if(!inFile.open(fullPath))
	{
		pgLog::error("can not open OBJ file '%s' for reading", nName);
		return NULL;
	}

	mesh = new pgMesh();

	while(!inFile.eof())
	{
		const char* line = inFile.readLine();
		lineNr++;

		if(pgIStringTool::startsWith(line, "#") || pgIStringTool::isEmpty(line))
			continue;																	// ignore this line

		if(pgIStringTool::startsWith(line, "mtllib"))
		{
			const char* name = pgIStringTool::skipSpaces(pgIStringTool::skipNonSpaces(pgIStringTool::skipSpaces(line)));
			if(!loadMTL(name, materials))
				pgLog::error("load material file '%s' failed", name);

			for(i=0; i<materials.getSize(); i++)
				if(materials[i]->getNumStages()<1)
					materials[i]->addStage(pgTextureStage());
		}
		else
		if(pgIStringTool::startsWith(line, "usemtl"))
		{
			if(segment && oi.lengths.getSize())
			{
				//segment->setMaterial(curMat);

				fillFrameOBJ(curMat, mesh, segment, baseMesh, frame, oi);

				oi.lengths.removeAll();
				oi.nIndices.removeAll();
				oi.tIndices.removeAll();
				oi.tIndices2.removeAll();
				oi.vIndices.removeAll();

				initObjects(mesh, segment, baseMesh, frame);
			}

			const char* name = pgIStringTool::skipSpaces(pgIStringTool::skipNonSpaces(pgIStringTool::skipSpaces(line)));

			for(i=0; i<materials.getSize(); i++)
				if(materials[i]->getName()==name)
				{
					curMat = materials[i];
					break;
				}
		}
		else
		if(pgIStringTool::startsWith(line, "g "))
		{
			fillFrameOBJ(curMat, mesh, segment, baseMesh, frame, oi);

			oi.lengths.removeAll();
			oi.nIndices.removeAll();
			oi.tIndices.removeAll();
			oi.tIndices2.removeAll();
			oi.vIndices.removeAll();

			initObjects(mesh, segment, baseMesh, frame);
		}
		else
		if(pgIStringTool::startsWith(line, "v "))
		{
			float x,y,z;

			sscanf(line, "v %f %f %f", &x,&y,&z);
			oi.vertices.addTail(pgVec3(-x,y,z));
		}
		else
		if(pgIStringTool::startsWith(line, "vt "))
		{
			float u,v;

			sscanf(line, "vt %f %f", &u,&v);
			oi.texCoords.addTail(pgVec2(u,v));
		}
		else
		if(pgIStringTool::startsWith(line, "vt2 "))
		{
			float u,v;

			sscanf(line, "vt2 %f %f", &u,&v);
			oi.texCoords2.addTail(pgVec2(u,v));
		}
		else
		if(pgIStringTool::startsWith(line, "vn "))
		{
			float x,y,z;

			sscanf(line, "vn %f %f %f", &x,&y,&z);
			oi.normals.addTail(pgVec3n(-x,y,z));
		}
		else
		if(pgIStringTool::startsWith(line, "f "))
		{
			pgList<pgString> strList, strList2;
			pgIStringTool::fillTokenList(line, ' ', strList);

			if(strList.getSize()<4)
			{
				pgLog::error("invalid command in line %d in OBJ file '%s'", lineNr, nName);
				continue;
			}

			for(i=1; i<strList.getSize(); i++)
			{
				strList2.removeAll();
				pgIStringTool::fillTokenList(strList[i], '/', strList2, false);

				if(strList2.getSize()>0)
				{
					if(strList2.getSize()==4)				// our own extended obj format for 2 texture coordinates per vertex
					{
						oi.vIndices.addTail(atoi(strList2[0]));
						oi.tIndices.addTail(atoi(strList2[1]));
						oi.tIndices2.addTail(atoi(strList2[2]));
						oi.nIndices.addTail(atoi(strList2[3]));
					}
					else
					{
						oi.vIndices.addTail(atoi(strList2[0]));
						oi.tIndices.addTail(strList2.getSize()>1 ? atoi(strList2[1]) : -1);
						oi.nIndices.addTail(strList2.getSize()>2 ? atoi(strList2[2]) : -1);
					}
				}
			}

			oi.lengths.addTail(strList.getSize()-1);
		}
	}

	if(!mesh)
		initObjects(mesh, segment, baseMesh, frame);

	fillFrameOBJ(curMat, mesh, segment, baseMesh, frame, oi);


	// sort the segments by the materials in the MTL file...
	//
	pgPtrList<pgSegment>&	mSegs = mesh->getSegments();
	pgPtrList<pgSegment>	segs = mSegs;							// copy the list...
	pgSegment*				minSeg;
	int						minMat;

	mSegs.removeAll();

	while(segs.getSize())
	{
		minMat = materials.getSize()+1;
		minSeg = NULL;
		for(j=0; j<segs.getSize(); j++)
		{
			for(i=0; i<materials.getSize(); i++)
				if(segs[j]->material==materials[i])
					break;
			if(i<minMat)
			{
				minMat = i;
				minSeg = segs[j];
			}
		}

		mSegs.addTail(minSeg);
		segs.removeItem(minSeg);
	}

	return mesh;
}
