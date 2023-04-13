///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgXUtil
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgXUtil.cpp,v 1.10 2003/01/05 19:55:54 daniel Exp $
///////////////////////////////////////////////////////////////////

#include "pgXUtil.h"

#include <pgLib/Base/pgOutTextFile.h>
#include <pgLib/Base/pgIResourceManager.h>
#include <pgLib/Base/pgIStringTool.h>
#include <pgLib/Base/pgIXML.h>
#include <pgLib/Math/pgVec2.h>
#include <pgLib/Render/pgMesh.h>
#include <pgLib/Render/pgMaterial.h>

#include <D3DX8.h>

#pragma warning (disable: 4310)


// parsing methods prototypes
//
static bool parseMesh(const pgString& nFileName, IXMLDOMNode* nNode, pgMesh* nMesh);
static bool parseSegment(const pgString& nFileName, IXMLDOMNode* nNode, pgSegment* nSeg);
static bool parseMaterial(const pgString& nFileName, IXMLDOMNode* nNode, pgMaterial* nMat);
static bool parseTextureStage(const pgString&, IXMLDOMNode* nNode, pgTextureStage& nStage);
static bool parseTransform(const pgString& nFileName, IXMLDOMNode* nNode, pgTransform* nTransform);
static bool parseBaseMesh(const pgString& nFileName, IXMLDOMNode* nNode, pgBaseMesh* nBase);
static bool parseBaseMeshFrame(const pgString& nFileName, IXMLDOMNode* nNode, pgBaseMesh::Frame* nFrame, int nSet);
static bool setFrameVertex(void* nVertices, int nIdx, int nSet, const pgVec3& nPos, const pgVec3& nNormal, DWORD nColor, const pgVec2& nTex1, const pgVec2& nTex2);


bool
pgXUtil::saveMesh(const pgString& nFileName, const pgMesh& nMesh)
{
	pgOutTextFile file;

	if(!file.open(nFileName))
		return false;

	const pgPtrList<pgSegment>	segs = nMesh.segments;
	int						i;

	file.writeString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	
	file.writeFormated("<pgMesh>\n");

	saveTransform(file, nMesh.transform);

	for(i=0; i<segs.getSize(); i++)
		saveSegment(file, *segs[i]);

	file.writeString("</pgMesh>\n");
	file.close();
	return true;
}


void
pgXUtil::saveSegment(pgOutTextFile& nFile, const pgSegment& nSegment)
{
	nFile.writeFormated("\t<pgSegment settings=\"%s\">\n", pgSegment::getStringFromSettings(nSegment.settings));

	saveTransform(nFile, nSegment.transform);
	saveMaterial(nFile, *nSegment.material);
	saveBaseMesh(nFile, *nSegment.baseMesh);

	nFile.writeString("\t</pgSegment>\n");
}


void
pgXUtil::saveTransform(pgOutTextFile& nFile, const pgTransform* nTransform)
{
	if(!nTransform)			// transform is optional...
		return;

	const pgVec3& p = nTransform->getPosition();
	const pgQuat& r = nTransform->getRotation();
	const pgVec3& s = nTransform->getScale();

	nFile.writeFormated("\t\t<pgTransform pos=\"%.3f %.3f %.3f\" rot=\"%.3f %.3f %.3f %.3f\" scale=\"%.3f %.3f %.3f\" />\n",
						p.x,p.y,p.z, r.x,r.y,r.z,r.w, s.x,s.y,s.z);
}


void
pgXUtil::saveMaterial(pgOutTextFile& nFile, const pgMaterial& nMaterial)
{
	const D3DCOLORVALUE	&d = nMaterial.ddMaterial.Diffuse,
						&a = nMaterial.ddMaterial.Ambient,
						&s = nMaterial.ddMaterial.Specular,
						&e = nMaterial.ddMaterial.Emissive;
	float				p  = nMaterial.ddMaterial.Power;

	nFile.writeFormated("\t\t<pgMaterial name=\"%s\" blendSrc=\"%s\" blendDst=\"%s\" culling=\"%s\">\n",
						nMaterial.name,
						pgMaterial::getBlendFromString(nMaterial.blendSrc),
						pgMaterial::getBlendFromString(nMaterial.blendDst),
						pgMaterial::getCullingString(nMaterial.culling));

	nFile.writeFormated("\t\t\t<Diffuse value=\"%.3f %.3f %.3f %.3f\" />\n", d.r, d.g, d.b, d.a);
	nFile.writeFormated("\t\t\t<Ambient value=\"%.3f %.3f %.3f %.3f\" />\n", a.r, a.g, a.b, a.a);
	nFile.writeFormated("\t\t\t<Specular value=\"%.3f %.3f %.3f %.3f\" />\n", s.r, s.g, s.b, s.a);
	nFile.writeFormated("\t\t\t<Emissive value=\"%.3f %.3f %.3f %.3f\" />\n", e.r, e.g, e.b, e.a);
	nFile.writeFormated("\t\t\t<Power value=\"%.3f\" />\n", p);

	for(int i=0; i<nMaterial.getNumStages(); i++)
		saveTextureStage(nFile, nMaterial.getStage(i));

	nFile.writeString("\t\t</pgMaterial>\n");
}


void
pgXUtil::saveTextureStage(pgOutTextFile& nFile, const pgTextureStage& nStage)
{
	nFile.writeFormated("\t\t\t<pgTextureStage image=\"%s\" coordIdx=\"%d\">\n",
						nStage.texture->name,
						nStage.coordIdx);

	nFile.writeFormated("\t\t\t\t<Filter min=\"%s\" mag=\"%s\" mip=\"%s\" />\n",
						pgTextureStage::getStringFromFilter(nStage.minFilter),
						pgTextureStage::getStringFromFilter(nStage.magFilter),
						pgTextureStage::getStringFromFilter(nStage.mipFilter));

	nFile.writeFormated("\t\t\t\t<Color op=\"%s\" arg1=\"%s\" arg2=\"%s\" />\n",
						pgTextureStage::getStringFromTextureOp(nStage.colorOp),
						pgTextureStage::getStringFromTextureArg(nStage.colorArg1),
						pgTextureStage::getStringFromTextureArg(nStage.colorArg2));

	nFile.writeFormated("\t\t\t\t<Alpha op=\"%s\" arg1=\"%s\" arg2=\"%s\" />\n",
						pgTextureStage::getStringFromTextureOp(nStage.alphaOp),
						pgTextureStage::getStringFromTextureArg(nStage.alphaArg1),
						pgTextureStage::getStringFromTextureArg(nStage.alphaArg2));

	nFile.writeString("\t\t\t</pgTextureStage>\n");

}


void
pgXUtil::saveBaseMesh(pgOutTextFile& nFile, const pgBaseMesh& nBaseMesh)
{
	const pgPtrList<pgBaseMesh::Frame>&	frames = nBaseMesh.frames;
	int									v,i,f, set = nBaseMesh.vertexSettings;

	nFile.writeFormated("\t\t<pgBaseMesh settings=\"%s\" primType=\"%s\">\n",
						pgBaseMesh::getStringFromRenderSettings(set), pgBaseMesh::getStringFromPrimType(nBaseMesh.getPrimType()));

	for(f=0; f<frames.getSize(); f++)
	{
		const pgBaseMesh::Frame& fr = *frames[f];
		

		nFile.writeFormated("\t\t\t<Frame numVertices=\"%d\" numIndices=\"%d\" numTriangles=\"%d\">\n",
							fr.numVertices, fr.numIndices, fr.numTriangles);

		// check various vertex types
		// supported by pgBaseMesh
		switch(set & ~(pgBaseMesh::SET_TWEENINGSUPPORT|pgBaseMesh::SET_INDEXED))
		{
		case pgBaseMesh::SET_LIGHT|pgBaseMesh::SET_TEXTURE1:
			{
			pgBaseMesh::MESHVERTEX_LIG1* ve=NULL;

			if(fr.vertices)
				ve = (pgBaseMesh::MESHVERTEX_LIG1*)fr.vertices;
			else
				fr.vBuffer->Lock(0, 0, (BYTE**)&ve, 0);

			for(v=0; v<fr.numVertices; v++)
				nFile.writeFormated("\t\t\t\t<Vertex pos=\"%.3f %.3f %.3f\" normal=\"%.3f %.3f %.3f\" texc=\"%.3f %.3f\" />\n",
									ve[v].p.x, ve[v].p.y, ve[v].p.z, ve[v].n.x, ve[v].n.y, ve[v].n.z, ve[v].t1.x, ve[v].t1.y);

			if(!fr.vertices)
				fr.vBuffer->Unlock();
			}
			break;

		case pgBaseMesh::SET_COLOR|pgBaseMesh::SET_TEXTURE1:
			{
			pgBaseMesh::MESHVERTEX_COL1* ve=NULL;

			if(fr.vertices)
				ve = (pgBaseMesh::MESHVERTEX_COL1*)fr.vertices;
			else
				fr.vBuffer->Lock(0, 0, (BYTE**)&ve, 0);

			for(v=0; v<fr.numVertices; v++)
				nFile.writeFormated("\t\t\t\t<Vertex pos=\"%.3f %.3f %.3f\" color=\"%0x\" texc=\"%.3f %.3f\" />\n",
									ve[v].p.x, ve[v].p.y, ve[v].p.z, ve[v].color, ve[v].t1.x, ve[v].t1.y);

			if(!fr.vertices)
				fr.vBuffer->Unlock();
			}
			break;

		case pgBaseMesh::SET_TEXTURE2:
			{
			pgBaseMesh::MESHVERTEX_TEX2* ve=NULL;

			if(fr.vertices)
				ve = (pgBaseMesh::MESHVERTEX_TEX2*)fr.vertices;
			else
				fr.vBuffer->Lock(0, 0, (BYTE**)&ve, 0);

			for(v=0; v<fr.numVertices; v++)
				nFile.writeFormated("\t\t\t\t<Vertex pos=\"%.3f %.3f %.3f\" texc=\"%.3f %.3f\" texc2=\"%.3f %.3f\" />\n",
									ve[v].p.x, ve[v].p.y, ve[v].p.z, ve[v].t1.x, ve[v].t1.y, ve[v].t2.x, ve[v].t2.y);

			if(!fr.vertices)
				fr.vBuffer->Unlock();
			}
			break;
		}


		// export indices
		//
		if(fr.numIndices>0)
		{
			WORD* in;

			nFile.writeString("\t\t\t\t<Indices>");

			if(fr.indices)
				in = fr.indices;
			else
				fr.iBuffer->Lock(0, 0, (BYTE**)&in, 0);

			for(i=0; i<fr.numIndices; i++)
			{
				if((i%16)==0)
					nFile.writeString("\n\t\t\t\t\t");

				nFile.writeFormated("%d ", in[i]);
			}

			if(!fr.indices)
				fr.iBuffer->Unlock();

			nFile.writeString("\n\t\t\t\t</Indices>\n");
		}

		nFile.writeString("\t\t\t</Frame>\n");
	}

	nFile.writeString("\t\t</pgBaseMesh>\n");
}


pgMesh*
pgXUtil::loadMesh(const pgString& nFileName)
{
	HRESULT				hr;
	IXMLDOMDocument2	*pXMLDoc = NULL;
	IXMLDOMNode			*pNode = NULL;
	VARIANT_BOOL		isSuccessful = VARIANT_FALSE;
	CComVariant			vXMLFile(nFileName.get());
	pgMesh				*mesh = NULL;
	CComBSTR			bstr("pgMesh");


	// initializes the COM library
	//
	hr = CoInitialize(NULL); 
	if(FAILED(hr))
	{
		pgLog::error("failed to execute CoInitialize(). loading of PGX file '%s' aborted", nFileName);
		return NULL;
	}

	try
	{
		// create an instance of the DOM document which will read the xml file
		//
		if(FAILED(CoCreateInstance(CLSID_DOMDocument40, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument2, (void**)&pXMLDoc)))
		{
			pgLog::error("failed to create DOM document. loading of PGX file '%s' aborted", nFileName);
			return NULL;
		}

		// load the xml file into the DOM document
		hr = pXMLDoc->load(vXMLFile, &isSuccessful);
		if((isSuccessful!=VARIANT_TRUE) || (FAILED(hr)))
		{
			pgLog::error("failed to load PXG file '%s'", nFileName);
			pXMLDoc->Release();
			return NULL;
		}
		
		// get an instance to the root node of the XML tree
		//
		if(FAILED(pXMLDoc->selectSingleNode(bstr, &pNode)))
		{
			pgLog::error("failed to query interface for PGX document '%s'", nFileName);
			pXMLDoc->Release();
			return NULL;
		}

		// start parsing the PGX file
		//
		mesh = new pgMesh();
		if(!parseMesh(nFileName, pNode, mesh))
		{
			pXMLDoc->Release();
			pNode->Release();
			CoUninitialize();
			return NULL;
		}

	}
	catch(_com_error e)
	{
		pgLog::error("COM error whike loading PFX file '%s' (%s)", nFileName, e.ErrorMessage());
		if(pXMLDoc)
			pXMLDoc->Release();
		if(pNode)
			pNode->Release();
		CoUninitialize();
		return NULL;
	}


	if(pXMLDoc)
		pXMLDoc->Release();
	if(pNode)
		pNode->Release();
	CoUninitialize();
	pgLog::info("PGX file '%s' successfully loaded", nFileName);
	return mesh;
}



bool
parseMesh(const pgString& nFileName, IXMLDOMNode* nNode, pgMesh* nMesh)
{
	pgString			str;
	IXMLDOMNodeList*	nodeList = NULL;
	IXMLDOMNode*		nd;

	if(nNode==NULL || !pgIXML::checkNodeName(nNode, "pgMesh"))
		return false;

	if(FAILED(nNode->get_childNodes(&nodeList)))
	{
		pgLog::error("loading PGX file '%s' failed. no segments in mesh found", nFileName);
		return false;
	}

	assert(nodeList!=NULL);
	if(FAILED(nodeList->nextNode(&nd)))
		return false;

	while(nd)
	{
		pgIXML::getNodeName(nd, str);

		if(str=="pgTransform")
		{
			pgTransform* trans = new pgTransform();

			if(parseTransform(nFileName, nd, trans))
				nMesh->setTransform(trans);
			else
			{
				delete trans;
				return false;
			}
		}
		else
		if(str=="pgSegment")
		{
			pgSegment* seg = new pgSegment();

			if(parseSegment(nFileName, nd, seg))
				nMesh->addSegment(seg);
			else
			{
				delete seg;
				return false;
			}
		}

		if(FAILED(nodeList->nextNode(&nd)))
			return false;
	}

	return true;
}


bool
parseSegment(const pgString& nFileName, IXMLDOMNode* nNode, pgSegment* nSeg)
{
	pgString str;
	IXMLDOMNodeList*		nodeList = NULL;
	IXMLDOMNode*			nd;

	if(nNode==NULL || !pgIXML::checkNodeName(nNode, "pgSegment"))
		return false;

	// read the segment settings (ztest, zwrite, etc...)
	//
	str = pgIXML::getSubnodeAsString(nNode, "@settings", "ztest zwrite fill textured");
	if(str.getLength())
		nSeg->setRenderSettings(pgSegment::getSettingsFromString(str));


	if(FAILED(nNode->get_childNodes(&nodeList)))
	{
		pgLog::error("loading PGX file '%s' failed. no basemesh or material in segment found", nFileName);
		return false;
	}

	assert(nodeList!=NULL);
	if(FAILED(nodeList->nextNode(&nd)))
		return false;

	while(nd)
	{
		pgIXML::getNodeName(nd, str);

		if(str=="pgTransform")
		{
			pgTransform* trans = new pgTransform();

			if(parseTransform(nFileName, nd, trans))
				nSeg->setTransform(trans);
			else
			{
				delete trans;
				return false;
			}
		}
		else if(str=="pgMaterial")
		{
			pgMaterial* mat = new pgMaterial();

			if(parseMaterial(nFileName, nd, mat))
				nSeg->setMaterial(mat);
			else
			{
				delete mat;
				return false;
			}
		}
		else
		if(str=="pgBaseMesh")
		{
			pgBaseMesh* base = new pgBaseMesh();

			if(parseBaseMesh(nFileName, nd, base))
				nSeg->setBaseMesh(base);
			else
			{
				delete base;
				return false;
			}
		}

		if(FAILED(nodeList->nextNode(&nd)))
			return false;
	}

	return true;
}


bool
parseMaterial(const pgString& nFileName, IXMLDOMNode* nNode, pgMaterial* nMat)
{
	pgString str, str1,str2;
	IXMLDOMNodeList*	nodeList = NULL;
	IXMLDOMNode*		nd;
	pgVec4				vec4;

	if(nNode==NULL || !pgIXML::checkNodeName(nNode, "pgMaterial"))
		return false;


	// read material attributes
	//
	str = pgIXML::getSubnodeAsString(nNode, "@name", "");
	nMat->setName(str);

	str1 = pgIXML::getSubnodeAsString(nNode, "@blendSrc", "");
	str2 = pgIXML::getSubnodeAsString(nNode, "@blendDst", "");
	if(str1.getLength() && str2.getLength())
		nMat->setBlending(pgMaterial::getBlendFromString(str1), pgMaterial::getBlendFromString(str2));

	str = pgIXML::getSubnodeAsString(nNode, "@culling", "");
	if(str.getLength())
		nMat->setCulling(pgMaterial::getCullingFromString(str));


	// read children nodes
	//
	if(FAILED(nNode->get_childNodes(&nodeList)))
		return true;

	assert(nodeList!=NULL);
	if(FAILED(nodeList->nextNode(&nd)))
		return false;

	while(nd)
	{
		pgIXML::getNodeName(nd, str);

		if(str=="Diffuse")
		{
			str = pgIXML::getSubnodeAsString(nd, "@value", "");
			if(str.getLength() && pgIStringTool::readVec4(str, vec4))
				nMat->setDiffuse(vec4);
		}
		else
		if(str=="Ambient")
		{
			str = pgIXML::getSubnodeAsString(nd, "@value", "");
			if(str.getLength() && pgIStringTool::readVec4(str, vec4))
				nMat->setAmbient(vec4);
		}
		else
		if(str=="Specular")
		{
			str = pgIXML::getSubnodeAsString(nd, "@value", "");
			if(str.getLength() && pgIStringTool::readVec4(str, vec4))
				nMat->setSpecular(vec4);
		}
		else
		if(str=="Emissive")
		{
			str = pgIXML::getSubnodeAsString(nd, "@value", "");
			if(str.getLength() && pgIStringTool::readVec4(str, vec4))
				nMat->setEmissive(vec4);
		}
		else
		if(str=="Power")
		{
			str = pgIXML::getSubnodeAsString(nd, "@value", "");
			if(str.getLength())
				nMat->setPower((float)atof(str));
		}
		else
		if(str=="pgTextureStage")
		{
			pgTextureStage stage;
			if(parseTextureStage(nFileName, nd, stage))
				nMat->addStage(stage);
		}


		if(FAILED(nodeList->nextNode(&nd)))
			return false;
	}

	return true;
}


bool
parseTextureStage(const pgString&, IXMLDOMNode* nNode, pgTextureStage& nStage)
{
	pgString	str;
	IXMLDOMNodeList*	nodeList = NULL;
	IXMLDOMNode*		nd;

	if(nNode==NULL || !pgIXML::checkNodeName(nNode, "pgTextureStage"))
		return false;


	str = pgIXML::getSubnodeAsString(nNode, "@image", "");
	if(str.getLength())
		nStage.setTexture(pgIResourceManager::getTexture(str));

	str = pgIXML::getSubnodeAsString(nNode, "@coordIdx", "");
	if(str.getLength())
		nStage.setCoordIndex(atoi(str));


	// read children nodes
	//
	if(FAILED(nNode->get_childNodes(&nodeList)))
		return true;

	assert(nodeList!=NULL);
	if(FAILED(nodeList->nextNode(&nd)))
		return false;

	while(nd)
	{
		pgIXML::getNodeName(nd, str);

		if(str=="Filter")
		{
			str = pgIXML::getSubnodeAsString(nd, "@min", "");
			if(str.getLength())
				nStage.setMinFilter(pgTextureStage::getFilterFromString(str));

			str = pgIXML::getSubnodeAsString(nd, "@mag", "");
			if(str.getLength())
				nStage.setMagFilter(pgTextureStage::getFilterFromString(str));

			str = pgIXML::getSubnodeAsString(nd, "@mip", "");
			if(str.getLength())
				nStage.setMipFilter(pgTextureStage::getFilterFromString(str));
		}
		else
		if(str=="Color")
		{
			str = pgIXML::getSubnodeAsString(nd, "@op", "");
			if(str.getLength())
				nStage.setColorOp(pgTextureStage::getTextureOpFromString(str));

			str = pgIXML::getSubnodeAsString(nd, "@arg1", "");
			if(str.getLength())
				nStage.setColorArg1(pgTextureStage::getTextureArgFromString(str));

			str = pgIXML::getSubnodeAsString(nd, "@arg2", "");
			if(str.getLength())
				nStage.setColorArg2(pgTextureStage::getTextureArgFromString(str));
		}
		else
		if(str=="Alpha")
		{
			str = pgIXML::getSubnodeAsString(nd, "@op", "");
			if(str.getLength())
				nStage.setAlphaOp(pgTextureStage::getTextureOpFromString(str));

			str = pgIXML::getSubnodeAsString(nd, "@arg1", "");
			if(str.getLength())
				nStage.setAlphaArg1(pgTextureStage::getTextureArgFromString(str));

			str = pgIXML::getSubnodeAsString(nd, "@arg2", "");
			if(str.getLength())
				nStage.setAlphaArg2(pgTextureStage::getTextureArgFromString(str));
		}

		if(FAILED(nodeList->nextNode(&nd)))
			return false;
	}

	return true;
}


bool
parseTransform(const pgString&, IXMLDOMNode* nNode, pgTransform* nTransform)
{
	pgString	str;
	pgVec3		vec3;
	pgVec4		vec4;
	pgQuat		quat;


	if(nNode==NULL || !pgIXML::checkNodeName(nNode, "pgTransform"))
		return false;

	str = pgIXML::getSubnodeAsString(nNode, "@pos", "");
	if(str.getLength() && pgIStringTool::readVec3(str, vec3))
		nTransform->setPosition(vec3);

	str = pgIXML::getSubnodeAsString(nNode, "@rot", "");
	if(str.getLength() && pgIStringTool::readVec4(str, vec4))
	{
		quat.x = vec4.x;	quat.y = vec4.y;	quat.z = vec4.z;	quat.w = vec4.w;
		nTransform->setRotation(quat);
	}

	str = pgIXML::getSubnodeAsString(nNode, "@scale", "");
	if(str.getLength() && pgIStringTool::readVec3(str, vec3))
		nTransform->setScale(vec3);

	return true;
}


bool
parseBaseMesh(const pgString& nFileName, IXMLDOMNode* nNode, pgBaseMesh* nBase)
{
	pgString str;
	IXMLDOMNodeList*	nodeList = NULL;
	IXMLDOMNode*		nd;
	int					set = 0;

	if(nNode==NULL || !pgIXML::checkNodeName(nNode, "pgBaseMesh"))
		return false;

	// read the basemesh settings (ztest, zwrite, etc...)
	//
	str = pgIXML::getSubnodeAsString(nNode, "@settings", "");
	if(str.getLength())
	{
		set = pgBaseMesh::getRenderSettingsFromString(str);
		nBase->setRenderSettings(set);
	}
	else
	{
		pgLog::error("loading PGX file '%s' failed. missing render settings in baseMesh", nFileName);
		return false;
	}

	str = pgIXML::getSubnodeAsString(nNode, "@primType", "");
	if(str.getLength())
	{
		pgBaseMesh::PRIMTYPE type = pgBaseMesh::getPrimTypeFromString(str);
		nBase->setPrimType(type);
	}
	else
	{
		pgLog::error("loading PGX file '%s' failed. missing primType in baseMesh", nFileName);
		return false;
	}

	// read children nodes
	//
	if(FAILED(nNode->get_childNodes(&nodeList)))
	{
		pgLog::error("loading PGX file '%s' failed. no frames found in basemesh", nFileName);
		return false;
	}

	assert(nodeList!=NULL);
	if(FAILED(nodeList->nextNode(&nd)))
		return false;

	while(nd)
	{
		pgBaseMesh::Frame* frame = new pgBaseMesh::Frame();

		if(parseBaseMeshFrame(nFileName, nd, frame, set))
			nBase->addFrame(frame);
		else
		{
			delete frame;
			return false;
		}

		if(FAILED(nodeList->nextNode(&nd)))
			return false;
	}

	return true;
}


bool
parseBaseMeshFrame(const pgString& nFileName, IXMLDOMNode* nNode, pgBaseMesh::Frame* nFrame, int nSet)
{
	pgString			str;
	IXMLDOMNodeList*	nodeList = NULL;
	IXMLDOMNode*		nd;
	pgVec3				pos, normal;
	pgVec2				tex1, tex2;
	DWORD				color=0;
	int					numV=0;
	pgList<pgString>	strList;

	if(nNode==NULL || !pgIXML::checkNodeName(nNode, "Frame"))
		return false;

	// read the frame settings (numVertices, numIndices, etc...)
	//
	str = pgIXML::getSubnodeAsString(nNode, "@numVertices", "");
	if(str.getLength())
		nFrame->numVertices = atoi(str);

	str = pgIXML::getSubnodeAsString(nNode, "@numIndices", "0");
	if(str.getLength())
		nFrame->numIndices = atoi(str);

	str = pgIXML::getSubnodeAsString(nNode, "@numTriangles", "");
	if(str.getLength())
		nFrame->numTriangles = atoi(str);

	nFrame->vertices = pgBaseMesh::createVerticesForSettings(nSet, nFrame->numVertices);
	if(!nFrame->vertices)
	{
		str = pgBaseMesh::getStringFromRenderSettings(nSet);
		pgLog::error("loading PGX file '%s' failed. '%s' is an unsupported vertex format", nFileName, str);
		return false;
	}

	if(nFrame->numIndices>pgBaseMesh::MAX_VERTICES)
	{
		pgLog::error("loading PGX file '%s' failed. basemesh frame exceeds maximum allowed number of vertices (%d instead of %d)", nFileName, nFrame->numIndices, pgBaseMesh::MAX_VERTICES);
		return false;
	}

	if(nFrame->numIndices>0)
		nFrame->indices = new WORD[nFrame->numIndices];


	// read children nodes
	//
	if(FAILED(nNode->get_childNodes(&nodeList)))
	{
		pgLog::error("loading PGX file '%s' failed. no vertices in frame found", nFileName);
		return false;
	}

	assert(nodeList!=NULL);
	if(FAILED(nodeList->nextNode(&nd)))
		return false;

	while(nd)
	{
		pgIXML::getNodeName(nd, str);

		if(str=="Vertex")
		{
			str = pgIXML::getSubnodeAsString(nd, "@pos", "");
			if(str.getLength())
				pgIStringTool::readVec3(str, pos);

			str = pgIXML::getSubnodeAsString(nd, "@normal", "");
			if(str.getLength())
				pgIStringTool::readVec3(str, normal);

			str = pgIXML::getSubnodeAsString(nd, "@color", "");
			if(str.getLength())
				 color = atoi(str);

			str = pgIXML::getSubnodeAsString(nd, "@texc", "");
			if(str.getLength())
				pgIStringTool::readVec2(str, tex1);

			str = pgIXML::getSubnodeAsString(nd, "@texc2", "");
			if(str.getLength())
				pgIStringTool::readVec2(str, tex2);

			if(numV<nFrame->numVertices)
				setFrameVertex(nFrame->vertices, numV++, nSet, pos, normal, color, tex1, tex2);
			else
			{
				pgLog::error("loading PGX file '%s' failed. too many vertices in frame", nFileName);
				return false;
			}
		}
		else
		if(str=="Indices")
		{
			pgIXML::getNodeText(nd, str);
			char* strIndices = const_cast<char*>(str.get());		// not nice, but we know what we are doing...

			pgIStringTool::replaceCharacter(strIndices, '\t', ' ');
			pgIStringTool::replaceCharacter(strIndices, '\n', ' ');

			unsigned int idx=0, pos=0, len=str.getLength();
			const char sep = ' ';

			while(pos<len)
			{
				const char* token = pgIStringTool::getNextToken(strIndices, pos, sep);

				if(idx<(unsigned)nFrame->numIndices)
					nFrame->indices[idx++] = (WORD)atoi(token);
				else
				{
					pgLog::error("reading PGX file '%s' failed: number of indíces exceeds numIndices value (%d)", nFileName, nFrame->numIndices);
					return false;
				}

				while(strIndices[pos]==sep)
					pos++;
			}

			if(idx!=(unsigned)nFrame->numIndices)
			{
				pgLog::error("reading PGX file '%s' failed: number of indíces exceeds numIndices value (%d instead of %d)", nFileName, idx, nFrame->numIndices);
				return false;
			}
		}

		if(FAILED(nodeList->nextNode(&nd)))
			return false;
	}

	if(numV<nFrame->numVertices)
	{
		pgLog::error("loading PGX file '%s' failed. not enough vertices in frame", nFileName);
		return false;
	}

	if(!(nSet & pgBaseMesh::SET_TWEENINGSUPPORT))
	{
		nFrame->createBuffers(nSet);
		nFrame->fillBuffers(nSet);
	}

	return true;
}


bool
setFrameVertex(void* nVertices, int nIdx, int nSet, const pgVec3& nPos, const pgVec3& nNormal, DWORD nColor, const pgVec2& nTex1, const pgVec2& nTex2)
{
	switch(nSet & ~(pgBaseMesh::SET_TWEENINGSUPPORT|pgBaseMesh::SET_INDEXED))
	{
	case pgBaseMesh::SET_TEXTURE1:
		{
		pgBaseMesh::MESHVERTEX_TEX1& v = *(((pgBaseMesh::MESHVERTEX_TEX1*)nVertices) + nIdx);

		v.p = nPos;
		v.t1 = nTex1;
		return true;
		}
		break;

	case pgBaseMesh::SET_LIGHT|pgBaseMesh::SET_TEXTURE1:
		{
		pgBaseMesh::MESHVERTEX_LIG1& v = *(((pgBaseMesh::MESHVERTEX_LIG1*)nVertices) + nIdx);

		v.p = nPos;
		v.n = nNormal;
		v.t1 = nTex1;
		return true;
		}
		break;

	case pgBaseMesh::SET_COLOR|pgBaseMesh::SET_TEXTURE1:
		{
		pgBaseMesh::MESHVERTEX_COL1& v = *(((pgBaseMesh::MESHVERTEX_COL1*)nVertices) + nIdx);

		v.p = nPos;
		v.color = nColor;
		v.t1 = nTex1;
		return true;
		}
		break;

	case pgBaseMesh::SET_TEXTURE2:
		{
		pgBaseMesh::MESHVERTEX_TEX2& v = *(((pgBaseMesh::MESHVERTEX_TEX2*)nVertices) + nIdx);

		v.p = nPos;
		v.t1 = nTex1;
		v.t2 = nTex2;
		return true;
		}
		break;
	}

	return false;
}
