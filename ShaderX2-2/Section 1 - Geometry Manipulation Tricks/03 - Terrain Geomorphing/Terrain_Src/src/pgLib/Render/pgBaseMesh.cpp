///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgBaseMesh
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgBaseMesh.cpp,v 1.15 2003/01/06 14:37:01 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgBaseMesh.h"

#include <pgLib/Base/pgIDirectX.h>
#include <pgLib/Base/pgIStringTool.h>
#include <pgLib/Base/pgLog.h>
#include <pgLib/FrameWork/DXUtil.h>
#include <pgLib/Math/pgAABBox.h>


pgBaseMesh::pgBaseMesh() : vBuffer(NULL), iBuffer(NULL)
{
	vertexSettings = 0;
	primType = PRIM_TRIANGLES;
}


void
pgBaseMesh::deleteDeviceObjects()
{
	int i,num=frames.getSize();

	for(i=0; i<num; i++)
	{
		SAFE_RELEASE(frames[i]->iBuffer);
		SAFE_RELEASE(frames[i]->vBuffer);
	}

	SAFE_RELEASE(vBuffer);
	SAFE_RELEASE(iBuffer);
}


bool
pgBaseMesh::restoreDeviceObjects()
{
	if(vertexSettings & SET_TWEENINGSUPPORT)
		return true;

	deleteDeviceObjects();

	int i,num=frames.getSize();

	for(i=0; i<num; i++)
	{
		frames[i]->createBuffers(vertexSettings);
		frames[i]->fillBuffers(vertexSettings);
	}

	return true;
}


void
pgBaseMesh::render()
{
	render(0);
}


void
pgBaseMesh::render(int nFrame)
{
	if(!checkDevice("can not render mesh: no render device set"))
		return;

	if(nFrame<0 || nFrame>=frames.getSize())
	{
		pgLog::error("can not render blended mesh: illegal frame selection");
		return;
	}

	Frame* f = frames[nFrame];

	switch(vertexSettings)
	{
	case (SET_TEXTURE1):
		pd3dDevice->SetVertexShader(D3DFVF_MESHVERTEX_TEX1);
		pd3dDevice->SetStreamSource(0, f->vBuffer, sizeof(MESHVERTEX_TEX1));
		break;

	case (SET_INDEXED|SET_TEXTURE1):
		pd3dDevice->SetVertexShader(D3DFVF_MESHVERTEX_TEX1);
		pd3dDevice->SetStreamSource(0, f->vBuffer, sizeof(MESHVERTEX_TEX1));
		break;

	case (SET_LIGHT|SET_TEXTURE1):
		pd3dDevice->SetVertexShader(D3DFVF_MESHVERTEX_LIG1);
		pd3dDevice->SetStreamSource(0, f->vBuffer, sizeof(MESHVERTEX_LIG1));
		break;

	case (SET_INDEXED|SET_LIGHT|SET_TEXTURE1):
		pd3dDevice->SetVertexShader(D3DFVF_MESHVERTEX_LIG1);
		pd3dDevice->SetStreamSource(0, f->vBuffer, sizeof(MESHVERTEX_LIG1));
		break;

	case (SET_COLOR|SET_TEXTURE1):
		pd3dDevice->SetVertexShader(D3DFVF_MESHVERTEX_COL1);
		pd3dDevice->SetStreamSource(0, f->vBuffer, sizeof(MESHVERTEX_COL1));
		break;

	case (SET_INDEXED|SET_COLOR|SET_TEXTURE1):
		pd3dDevice->SetVertexShader(D3DFVF_MESHVERTEX_COL1);
		pd3dDevice->SetStreamSource(0, f->vBuffer, sizeof(MESHVERTEX_COL1));
		break;

	case (SET_TEXTURE2):
		pd3dDevice->SetVertexShader(D3DFVF_MESHVERTEX_TEX2);
		pd3dDevice->SetStreamSource(0, f->vBuffer, sizeof(MESHVERTEX_TEX2));
		break;

	case (SET_INDEXED|SET_TEXTURE2):
		pd3dDevice->SetVertexShader(D3DFVF_MESHVERTEX_TEX2);
		pd3dDevice->SetStreamSource(0, f->vBuffer, sizeof(MESHVERTEX_TEX2));
		break;

	default:
		// all other combinations not supported yet...
		//
		pgLog::error("pgBaseMesh can not render combination '%s'", getStringFromRenderSettings(vertexSettings));
		return;
	}


	pd3dDevice->SetTransform(D3DTS_VIEW, pgIDirectX::getD3DViewMatrix());

	if(vertexSettings & SET_INDEXED)
	{
		pd3dDevice->SetIndices(f->iBuffer, 0);
		pd3dDevice->DrawIndexedPrimitive((D3DPRIMITIVETYPE)primType, 0, f->numVertices, 0, f->numTriangles);
	}
	else
		pd3dDevice->DrawPrimitive((D3DPRIMITIVETYPE)primType, 0, f->numTriangles);

	pgIDirectX::addNonLevelTris(f->numTriangles);
}


void
pgBaseMesh::renderTweened(int nFrame0, int nFrame1, float nBlendWeight)
{
	float w0 = nBlendWeight, w1 = 1.0f-nBlendWeight;

	if(!checkDevice("can not render blended mesh: no render device set"))
		return;

	if(nFrame0<0 || nFrame1<0 || nFrame0>=frames.getSize() || nFrame1>=frames.getSize())
	{
		pgLog::error("can not render blended mesh: illegal frame selection");
		return;
	}

	if(frames[nFrame0]->numVertices!=frames[nFrame1]->numVertices)
	{
		pgLog::error("can not render blended mesh: frames have differen vertex count");
		return;
	}

	if(!frames[nFrame0]->vertices || !frames[nFrame1]->vertices)
	{
		pgLog::error("can not render blended mesh: frames don't have vertices");
		return;
	}

	//pgLog::info("%d -> %d   f: %.3f", nFrame0, nFrame1, nBlendWeight);

	// check if vertex & index buffers have already been created
	//
	if((vBuffer==NULL || iBuffer==NULL))
	{
		if(!createBuffers(vertexSettings, iBuffer, frames[nFrame0]->numIndices, vBuffer, frames[nFrame0]->numVertices, true))
		{
			pgLog::error("can not render blended mesh: failed to create vertex buffer");
			return;
		}

		if(frames[nFrame0]->numIndices)		// fill index buffer ?
		{
			WORD* dstPtr=NULL;
			assert(frames[nFrame0]->indices);

			iBuffer->Lock(0, 0, (BYTE**)&dstPtr, 0);
			memcpy(dstPtr, frames[nFrame0]->indices, frames[nFrame0]->numIndices*sizeof(WORD));
			iBuffer->Unlock();
		}		
	}


	switch(vertexSettings)
	{
	case (SET_INDEXED|SET_TEXTURE1):
	case (SET_TEXTURE1):
		{
		MESHVERTEX_TEX1	*vPtr;
		int				i, num = frames[nFrame0]->numVertices;
		MESHVERTEX_TEX1	*v1 = (MESHVERTEX_TEX1*)frames[nFrame0]->vertices,
						*v2 = (MESHVERTEX_TEX1*)frames[nFrame1]->vertices;

		vBuffer->Lock(0, 0, (BYTE**)&vPtr, 0);

		for(i=0; i<num; i++,v1++,v2++)
		{
			vPtr[i].p.x = w1 * v1->p.x + w0 * v2->p.x;
			vPtr[i].p.y = w1 * v1->p.y + w0 * v2->p.y;
			vPtr[i].p.z = w1 * v1->p.z + w0 * v2->p.z;
			vPtr[i].t1.x = v1->t1.x;
			vPtr[i].t1.y = v1->t1.y;
		}

		vBuffer->Unlock();

		pd3dDevice->SetVertexShader(D3DFVF_MESHVERTEX_TEX1);
		pd3dDevice->SetStreamSource(0, vBuffer, sizeof(MESHVERTEX_TEX1));
		break;
		}

	case (SET_INDEXED|SET_COLOR|SET_TEXTURE1):
	case (SET_COLOR|SET_TEXTURE1):
		{
		MESHVERTEX_COL1	*vPtr;
		int				i, num = frames[nFrame0]->numVertices;
		MESHVERTEX_COL1	*v1 = (MESHVERTEX_COL1*)frames[nFrame0]->vertices,
						*v2 = (MESHVERTEX_COL1*)frames[nFrame1]->vertices;

		vBuffer->Lock(0, 0, (BYTE**)&vPtr, 0);

		for(i=0; i<num; i++,v1++,v2++)
		{
			vPtr[i].p.x = w1 * v1->p.x + w0 * v2->p.x;
			vPtr[i].p.y = w1 * v1->p.y + w0 * v2->p.y;
			vPtr[i].p.z = w1 * v1->p.z + w0 * v2->p.z;
			vPtr[i].t1.x = v1->t1.x;
			vPtr[i].t1.y = v1->t1.y;
			vPtr[i].color = v1->color;
		}

		vBuffer->Unlock();

		pd3dDevice->SetVertexShader(D3DFVF_MESHVERTEX_COL1);
		pd3dDevice->SetStreamSource(0, vBuffer, sizeof(MESHVERTEX_COL1));
		break;
		}

	case (SET_INDEXED|SET_LIGHT|SET_TEXTURE1):
	case (SET_LIGHT|SET_TEXTURE1):
		{
		MESHVERTEX_LIG1	*vPtr;
		int				i, num = frames[nFrame0]->numVertices;
		MESHVERTEX_LIG1	*v1 = (MESHVERTEX_LIG1*)frames[nFrame0]->vertices,
						*v2 = (MESHVERTEX_LIG1*)frames[nFrame1]->vertices;

		vBuffer->Lock(0, 0, (BYTE**)&vPtr, 0);

		for(i=0; i<num; i++,v1++,v2++)
		{
			vPtr[i].p.x = w1 * v1->p.x + w0 * v2->p.x;
			vPtr[i].p.y = w1 * v1->p.y + w0 * v2->p.y;
			vPtr[i].p.z = w1 * v1->p.z + w0 * v2->p.z;
			vPtr[i].n.x = w1 * v1->n.x + w0 * v2->n.x;
			vPtr[i].n.y = w1 * v1->n.y + w0 * v2->n.y;
			vPtr[i].n.z = w1 * v1->n.z + w0 * v2->n.z;
			vPtr[i].t1.x = v1->t1.x;
			vPtr[i].t1.y = v1->t1.y;
		}

		vBuffer->Unlock();

		pd3dDevice->SetVertexShader(D3DFVF_MESHVERTEX_LIG1);
		pd3dDevice->SetStreamSource(0, vBuffer, sizeof(MESHVERTEX_LIG1));
		break;
		}

	default:
		pgLog::error("unsupported mesh type for tweening");
		return;
	}


	pd3dDevice->SetTransform(D3DTS_VIEW, pgIDirectX::getD3DViewMatrix());

	if(vertexSettings & SET_INDEXED)
	{
		pd3dDevice->SetIndices(frames[nFrame0]->iBuffer, 0);
		pd3dDevice->DrawIndexedPrimitive((D3DPRIMITIVETYPE)primType, 0, frames[nFrame0]->numVertices, 0, frames[nFrame0]->numTriangles);
	}
	else
		pd3dDevice->DrawPrimitive((D3DPRIMITIVETYPE)primType, 0, frames[nFrame0]->numTriangles);

	pgIDirectX::addNonLevelTris(frames[nFrame0]->numTriangles);
}


pgString
pgBaseMesh::getStringFromRenderSettings(int nSet)
{
	pgString str;
	int texSet = nSet & SET_TEXTURE4;

	if(nSet & SET_INDEXED)
		str += " indexed";
	if(nSet & SET_TWEENINGSUPPORT)
		str += " tweeningsupport";
	if(nSet & SET_COLOR)
		str += " color";
	if(nSet & SET_LIGHT)
		str += " light";
	if(texSet == SET_TEXTURE4)
		str += " texture4";
	else if(texSet == SET_TEXTURE3)
		str += " texture3";
	else if(texSet == SET_TEXTURE2)
		str += " texture2";
	else if(texSet == SET_TEXTURE1)
		str += " texture1";

	// cut the leading blank
	str = str.getSubString(1, str.getLength()-1);
	return str;
}


int
pgBaseMesh::getRenderSettingsFromString(const pgString& nString)
{
	pgList<pgString>	list;
	int					i, sets=0;

	pgIStringTool::fillTokenList(nString, ' ', list);

	for(i=0; i<list.getSize(); i++)
	{
		const pgString& str = list[i];
		
		if(str=="indexed")
			sets |= SET_INDEXED;
		else if(str=="tweeningsupport")
			sets |= SET_TWEENINGSUPPORT;
		else if(str=="color")
			sets |= SET_COLOR;
		else if(str=="light")
			sets |= SET_LIGHT;
		else if(str=="texture4")
			sets |= SET_TEXTURE4;
		else if(str=="texture3")
			sets |= SET_TEXTURE3;
		else if(str=="texture2")
			sets |= SET_TEXTURE2;
		else if(str=="texture1")
			sets |= SET_TEXTURE1;
	}

	return sets;
}


pgString
pgBaseMesh::getStringFromPrimType(PRIMTYPE nType)
{
	switch(nType)
	{
	case PRIM_POINTS:
		return "points";
	case PRIM_LINES:
		return "lines";
	case PRIM_LINESTRIP:
		return "linestrip";
	case PRIM_TRIANGLES:
		return "triangles";
	case PRIM_TRIANGLESTRIP:
		return "trianglestrip";
	case PRIM_TRIANGLEFAN:
		return "trianglefan";
	default:
	case PRIM_UNDEFINED:
		return "undefined";
	}
}


pgBaseMesh::PRIMTYPE
pgBaseMesh::getPrimTypeFromString(const pgString& nString)
{
	if(nString=="points")
		return PRIM_POINTS;
	if(nString=="lines")
		return PRIM_LINES;
	if(nString=="linestrip")
		return PRIM_LINESTRIP;
	if(nString=="triangles")
		return PRIM_TRIANGLES;
	if(nString=="trianglestrip")
		return PRIM_TRIANGLESTRIP;
	if(nString=="trianglefan")
		return PRIM_TRIANGLEFAN;

	return PRIM_UNDEFINED;
}


void*
pgBaseMesh::createVerticesForSettings(int nSettings, int nNumVertices)
{
	switch(nSettings)
	{
	case (SET_TEXTURE1):
		return new MESHVERTEX_TEX1[nNumVertices];

	case (SET_LIGHT|SET_TEXTURE1):
		return new MESHVERTEX_LIG1[nNumVertices];

	case (SET_COLOR|SET_TEXTURE1):
		return new MESHVERTEX_COL1[nNumVertices];

	case (SET_TEXTURE2):
		return new MESHVERTEX_TEX2[nNumVertices];

	default:
		return NULL;
	}
}


bool
pgBaseMesh::Frame::createBuffers(int nSettings)
{
	return pgBaseMesh::createBuffers(nSettings, iBuffer, numIndices, vBuffer, numVertices);
}


bool
pgBaseMesh::Frame::fillBuffers(int nSettings)
{
	if(!vBuffer)
		return false;

	int		vertexSize = 0;
	void*	dstPtr = NULL;

	// vertex buffer
	//
	switch(nSettings)
	{
	case (SET_TEXTURE1):
		vertexSize = sizeof(MESHVERTEX_TEX1);
		break;
	case (SET_LIGHT|SET_TEXTURE1):
		vertexSize = sizeof(MESHVERTEX_LIG1);
		break;
	case (SET_COLOR|SET_TEXTURE1):
		vertexSize = sizeof(MESHVERTEX_COL1);
		break;
	case (SET_TEXTURE2):
		vertexSize = sizeof(MESHVERTEX_TEX2);
		break;
	default:
		return false;
	}

	vBuffer->Lock(0, 0, (BYTE**)&dstPtr, 0);
	memcpy(dstPtr, vertices, numVertices*vertexSize);
	vBuffer->Unlock();


	// index buffer
	//
	if(numIndices==0)
		return true;

	if(!iBuffer || !indices)
		return false;

	iBuffer->Lock(0, 0, (BYTE**)&dstPtr, 0);
	memcpy(dstPtr, indices, numIndices*sizeof(WORD));
	iBuffer->Unlock();

	return true;
}


bool
pgBaseMesh::createBuffers(int nSettings, PDIRECT3DINDEXBUFFER8& iBuffer, int nNumIndices, PDIRECT3DVERTEXBUFFER8& vBuffer, int nNumVertices, bool nForceDynamicVertexBuffer)
{
	HRESULT				hr = 0;
	LPDIRECT3DDEVICE8	pd3dDevice = pgIDirectX::getDevice();
	DWORD				flag = (pgIDirectX::getDisableWriteOnlyBuffers() ? 0 : D3DUSAGE_WRITEONLY) |
							   (nForceDynamicVertexBuffer ? D3DUSAGE_DYNAMIC : 0);


	if(pd3dDevice==NULL)
		return false;

	SAFE_RELEASE(iBuffer);
	SAFE_RELEASE(vBuffer);

	if(nNumIndices>0)
	{
		if(FAILED(pd3dDevice->CreateIndexBuffer(nNumIndices * sizeof(WORD), flag, D3DFMT_INDEX16, pgIDirectX::getPoolType(), &iBuffer)))
			return false;
	}

	switch(nSettings)
	{
	case (SET_TEXTURE1):
		hr = pd3dDevice->CreateVertexBuffer(nNumVertices * sizeof(MESHVERTEX_TEX1), flag, D3DFVF_MESHVERTEX_TEX1, pgIDirectX::getPoolType(), &vBuffer);
		break;

	case (SET_LIGHT|SET_TEXTURE1):
		hr = pd3dDevice->CreateVertexBuffer(nNumVertices * sizeof(MESHVERTEX_LIG1), flag, D3DFVF_MESHVERTEX_LIG1, pgIDirectX::getPoolType(), &vBuffer);
		break;

	case (SET_COLOR|SET_TEXTURE1):
		hr = pd3dDevice->CreateVertexBuffer(nNumVertices * sizeof(MESHVERTEX_COL1), flag, D3DFVF_MESHVERTEX_COL1, pgIDirectX::getPoolType(), &vBuffer);
		break;

	case (SET_TEXTURE2):
		hr = pd3dDevice->CreateVertexBuffer(nNumVertices * sizeof(MESHVERTEX_TEX2), flag, D3DFVF_MESHVERTEX_TEX2, pgIDirectX::getPoolType(), &vBuffer);
		break;

	default:
		return NULL;
	}

	if(FAILED(hr))
	{
		SAFE_RELEASE(iBuffer);
		return false;
	}

	return true;
}
