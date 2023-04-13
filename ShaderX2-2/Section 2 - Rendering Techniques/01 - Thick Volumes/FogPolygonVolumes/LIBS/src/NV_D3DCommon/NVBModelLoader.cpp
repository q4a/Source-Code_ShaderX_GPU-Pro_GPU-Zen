/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  NVBModelLoader.cpp

Copyright NVIDIA Corporation 2002
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
*AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.



Comments:
See the header file for comments.



******************************************************************************/


#include <string>
#include <shared/NV_Common.h>
#include <shared/NV_Error.h>

#include <NVD3DVers.h>
#include <NV_D3DCommon/NVBModelLoader.h>
#include <NV_D3DCommon/SimpleObject.h>
#include <NV_D3DCommon/SimpleVBObject.h>


using namespace std;


/*
//@@@@ scene INCORPORATE different vertex declarations
	// TEMPORARY //////////////////////////
	// nvbscene mesh always same vertex format

	D3DVERTEXELEMENT9 declaration[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 }, 
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },  
		{ 0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },  
		{ 0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
		{ 0, 48, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 }, 
		{ 0, 60, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 }, 
		D3DDECL_END()
	};

    m_pD3DDev->CreateVertexDeclaration( declaration, &m_pDeclaration );
	*/



NVBModelLoader::NVBModelLoader()
{
	m_pScene = NULL;
	m_pGetFilePathCallback	= NULL;
}


NVBModelLoader::~NVBModelLoader()
{
	FreeNVBFile();
}

/////////////////////////////////////


HRESULT NVBModelLoader::LoadNVBFile( const char * filename, GetFilePathCallback getFilePath )
{
	HRESULT hr = S_OK;

	FreeNVBFile();
	m_pScene = nv_factory::get_factory()->new_scene();

	ASSERT_MSG_RET( m_pScene != NULL, "LoadNVBFile couldn't create scene!\n", E_FAIL );

	std::string fullname;

	if( getFilePath )
	{
		m_pGetFilePathCallback = getFilePath;
		fullname = (*getFilePath)(filename);
	}
	else
	{
		// All other file names should be complete paths or you'll 
		//  have trouble loading them.
		m_pGetFilePathCallback = NULL;
		fullname = filename;
	}


	bool res;
	res = NVBLoad( fullname.c_str(), m_pScene, NVB_LHS );

	if( ! res )
	{
		FMsg("Couldn't load NVB scene [%s]\n", fullname.c_str() );
		return( E_FAIL );
	}

	Analyze();

	return( hr );
}


HRESULT NVBModelLoader::FreeNVBFile()
{
	HRESULT hr = S_OK;

	m_vpMeshes.clear();

	if( m_pScene != NULL )
	{
		m_pScene->release();
		m_pScene = NULL;
	}
    nv_factory::shutdown();

	return( hr );
}


void NVBModelLoader::Analyze()
{
	RET_IF_NULL( m_pScene );

	m_vpMeshes.clear();

	for (unsigned int i = 0; i < m_pScene->num_nodes; ++i)
	{
		const nv_node* node = m_pScene->nodes[i];
		if (node->get_type() == nv_node::GEOMETRY)
		{
			const nv_model* model = reinterpret_cast<const nv_model*>(node);
			for (unsigned int j = 0; j < model->num_meshes; ++j)
			{
				if( model->meshes[j].vertices == 0 )
					break;
				m_vpMeshes.push_back( &(model->meshes[j] ) );
			}
		}
/*
//@@@
		else if (node->get_type() == nv_node::CAMERA)
			++m_NumCameras;
		else if (node->get_type() == nv_node::LIGHT)
			++m_NumLights;
*/
	} 
}

///////////////////////////////////////////////////////////



int	NVBModelLoader::GetNumMeshes()
{
	return( m_vpMeshes.size() );
}


HRESULT NVBModelLoader::LoadMeshToSimpleObject( int mesh_num, SimpleObject & object )
{
	HRESULT hr = S_OK;

	if( mesh_num >= m_vpMeshes.size() )
	{
		FDebug("LoadMeshToSimpleObject no mesh %d\n", mesh_num );
		assert( false );
		return( E_FAIL );
	}

	nv_mesh * pMesh = m_vpMeshes.at( mesh_num );

	if( pMesh == NULL )
	{
		FDebug("Mesh %d is NULL!\n", mesh_num );
		return( E_FAIL );
	}

	////////////////////////////////////////////

	UINT nvert, nind;
	nvert	= pMesh->num_vertices;
	nind	= pMesh->num_faces * 3;
	
	object.Allocate( nvert, nind );

	UINT i;
	for( i=0; i < nvert; i++ )
	{
		object.m_pVertices[i].pos = D3DXVECTOR3(	pMesh->vertices[i].x,
													pMesh->vertices[i].y,
													pMesh->vertices[i].z );
		if( pMesh->normals != NULL )
		{
			object.m_pVertices[i].nrm = D3DXVECTOR3(	pMesh->normals[i].x,
														pMesh->normals[i].y,
														pMesh->normals[i].z );
		}

		if( pMesh->colors != NULL )
		{
			object.m_pVertices[i].diffuse = D3DCOLOR_ARGB(	(BYTE) (pMesh->colors[i].w * 255.0f),
															(BYTE) (pMesh->colors[i].x * 255.0f),
															(BYTE) (pMesh->colors[i].y * 255.0f),
															(BYTE) (pMesh->colors[i].z * 255.0f) );			
		}

		bool bHasTextureCoord = ( pMesh->num_texcoord_sets > 0) && 
								( pMesh->texcoord_sets[0].texcoords != 0 );
		if( bHasTextureCoord )
		{
			object.m_pVertices[i].t0.x = pMesh->texcoord_sets[0].texcoords[ 2*i   ];
			object.m_pVertices[i].t0.y = pMesh->texcoord_sets[0].texcoords[ 2*i+1 ];
		}
	}

	for( i=0; i < nind; i++ )
	{
		object.m_pIndices[i] = pMesh->faces_idx[i];
	}

	object.m_bIsValid = true;

	return( hr );
}


HRESULT NVBModelLoader::LoadMeshToSimpleVBObject( int mesh_num, SimpleVBObject & vb_object )
{
	HRESULT hr = S_OK;

	FMsg("Not yet implemented!\n");
	assert( false );

	return( hr );
}

