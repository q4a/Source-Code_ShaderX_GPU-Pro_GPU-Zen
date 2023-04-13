/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  SceneFactory.cpp

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





******************************************************************************/


#include <shared/NV_Common.h>
#include <shared/NV_Error.h>


#include <NV_D3DCommon/SceneFactory.h>
#include <NV_D3DCommon/SimpleObject.h>
#include <NV_D3DCommon/SimpleVBObject.h>
#include <NV_D3DCommon/MatrixNode.h>




SceneFactory::SceneFactory()
{
	TRACE0( "SceneFactory::SceneFactory()\n" );
	SetAllNull();
}


SceneFactory::~SceneFactory()
{
	TRACE0( "SceneFactory::~SceneFactory()\n" );
	Free();
}

void SceneFactory::SetAllNull()
{
	TRACE0( "SceneFactory::SetAllNull()\n" );

	m_pD3DDev = NULL;
	m_pGetFilePathCallback = NULL;
}

//////////////////////////////////////////////

HRESULT SceneFactory::Free()
{
	TRACE0( "SceneFactory::Free()\n" );

	HRESULT hr = S_OK;

	FreeMasterLists();		// also frees instance lists
		
	SAFE_RELEASE( m_pD3DDev );

	SetAllNull();

	return( hr );
}


HRESULT	SceneFactory::FreeInstanceLists()
{
	HRESULT hr = S_OK;

	SAFE_VECTOR_DELETE( m_vInstanceLists );

	return( hr );
}


HRESULT SceneFactory::FreeMasterLists()
{
	TRACE0( "SceneFactory::FreeMasterLists()\n" );

	HRESULT hr = S_OK;

	hr = FreeInstanceLists();

	MASTERLIST_DELETE( m_vML_MatrixVs );
	MASTERLIST_DELETE( m_vML_MatrixNodes );
	MASTERLIST_DELETE( m_vML_SimpleVBObjects );
	MASTERLIST_DELETE( m_vML_SimpleObjects );
	MASTERLIST_DELETE( m_vML_Vector4s );

	int i;
	for( i=0; i < m_vML_ppTextures.size(); i++ )
	{
		NV_IDIRECT3DTEXTURE ** ppTex;
		ppTex = m_vML_ppTextures.at(i);

		if( ppTex != NULL )
		{
			SAFE_RELEASE( *ppTex );
			delete ppTex;
			m_vML_ppTextures.at(i) = NULL;
		}
	}
	m_vML_ppTextures.clear();

	return( hr );
}


HRESULT SceneFactory::Initialize( NV_D3DDEVICETYPE * pDev,
									GetFilePathCallback file_path_callback )
{
	TRACE0( "SceneFactory::Initialize( pDev = 0x%8.8x )\n", pDev );

	HRESULT hr = S_OK;

	hr = Free();

	FAIL_IF_NULL( pDev );
	m_pD3DDev = pDev;
	m_pD3DDev->AddRef();

	m_pGetFilePathCallback = file_path_callback;

	m_bTrackNames = true;

	return( hr );
}


void	SceneFactory::TrackNames( bool track_on )
{
	// false to disable storing of names
	m_bTrackNames = track_on;
}

std::string	SceneFactory::GetFilePath( const char * filename )
{	
	// Uses m_pGetFilePathCallback to try to find the file
	if( m_pGetFilePathCallback != NULL )
	{
		return( (*m_pGetFilePathCallback)(filename) );
	}
	return( filename );
}


D3DXVECTOR4	** SceneFactory::NewVector4( const D3DXVECTOR4 & vec4 )
{
	// Allocates D3DXVECTOR4 that you don't have to worry about
	//  freeing elsewhere.  Also allocates a pointer to the
	//  vector.
	D3DXVECTOR4 ** ppv = new D3DXVECTOR4 *;
	RET_NULL_IF_NULL( ppv );
	*ppv = new D3DXVECTOR4;
	RET_NULL_IF_NULL( *ppv );
	**ppv = vec4;
	m_vML_Vector4s.push_back( ppv );

//	FMsg("NewVector4 ptr = %8.8x   h = %8.8x\n", *ppv, ppv );
	return( ppv );
}


MatrixNode ** SceneFactory::NewMatrixNode( MatrixV ** ppLocalMatrix,
											MatrixNode ** ppParent )
{
	// Allocate a pointer to a MatrixNode and a MatrixNode
	MatrixNode ** ppo = new MatrixNode *;
	RET_NULL_IF_NULL( ppo );
	*ppo = new MatrixNode( ppLocalMatrix, ppParent );
	RET_NULL_IF_NULL( *ppo );
	m_vML_MatrixNodes.push_back( ppo );
	return( ppo );
}


MatrixNode ** SceneFactory::NewMatrixNode( MatrixV ** ppLocalMatrix,
											const char * pName,
											MatrixNode ** ppParent )
{
	MatrixNode ** ppo = new MatrixNode *;
	RET_NULL_IF_NULL( ppo );

	if( m_bTrackNames == true )
	{
		*ppo = new MatrixNodeNamed( ppLocalMatrix, pName, ppParent );
	}
	else
	{
		*ppo = new MatrixNode( ppLocalMatrix, ppParent );
	}

	RET_NULL_IF_NULL( *ppo );
	m_vML_MatrixNodes.push_back( ppo );
	return( ppo );
}




MatrixV ** SceneFactory::NewMatrix( const D3DXMATRIX & matrix )
{
	// Allocate a pointer to a versioned matrix and a MatrixV
	MatrixV ** ppo = new MatrixV *;
	RET_NULL_IF_NULL( ppo );
	*ppo = new MatrixV( matrix );
	RET_NULL_IF_NULL( *ppo );
	m_vML_MatrixVs.push_back( ppo );
	return( ppo );
}

MatrixV ** SceneFactory::NewMatrix()
{
	// initializes to identity matrix
	D3DXMATRIX mat;
	D3DXMatrixIdentity( & mat );

	return( NewMatrix( mat ) );
}



SimpleVBObject ** SceneFactory::NewSimpleVBObject( SimpleObject * pObj,
													bool bDynamic )
{
	// Allocate a pointer to a SimpleVBObject and a SimpleVBObject
	SimpleVBObject ** ppo = new SimpleVBObject *;
	RET_NULL_IF_NULL( ppo );
	*ppo = new SimpleVBObject;
	RET_NULL_IF_NULL( *ppo );
	m_vML_SimpleVBObjects.push_back( ppo );

	if( pObj != NULL )
	{
		RET_NULL_IF_NULL( m_pD3DDev );
		if( bDynamic )
			(*ppo)->CreateFromSimpleObject( pObj, m_pD3DDev, SimpleVBObject::DYNAMIC );
		else
			(*ppo)->CreateFromSimpleObject( pObj, m_pD3DDev, SimpleVBObject::STATIC );
	}

	return( ppo );
}


SimpleObject ** SceneFactory::NewSimpleObject()
{
	// Allocate a pointer to a SimpleObject and a SimpleObject
	SimpleObject ** ppo = new SimpleObject *;
	RET_NULL_IF_NULL( ppo );
	*ppo = new SimpleObject;
	RET_NULL_IF_NULL( *ppo );
	m_vML_SimpleObjects.push_back( ppo );
	return( ppo );
}



NV_IDIRECT3DTEXTURE ** SceneFactory::NewTexture( char * filename, bool verbose )
{
	// If m_pGetFilePathCallback != NULL then use it to try to 
	// find the file.
	HRESULT hr = S_OK;
	if( m_pD3DDev == NULL )
	{
		FMsg("SceneFactory::NewTexture() m_pD3DDev == NULL!\n");
		assert( false );
		return( NULL );
	}

	std::string filepath;
	filepath = GetFilePath( filename );

	NV_IDIRECT3DTEXTURE ** ppTex = new (NV_IDIRECT3DTEXTURE*);
	RET_NULL_IF_NULL( ppTex );

	hr = D3DXCreateTextureFromFile( m_pD3DDev,
									filepath.c_str(),
									ppTex );
	if( FAILED(hr) || (*ppTex == NULL ) )
	{
		FMsg("SceneFactory::NewTexture() couldn't load texture [%s]\n", filepath.c_str() );
		assert( false );
		return( NULL );
	}
	else if( verbose == true )
	{
		FMsg("SceneFactory loaded : %s\n", filepath.c_str() );
	}

	m_vML_ppTextures.push_back( ppTex );

	return( ppTex );
}


HRESULT	SceneFactory::FreeTexture( NV_IDIRECT3DTEXTURE ** ppTex )
{
	// No need to search the vector of texture handles (m_vML_ppTextures)
	// because the pointer will be set to NULL.  You could search it if
	// you want to eliminate the handle, or do a pass to eliminate all
	// handles pointing to a NULL pointer, but this function doesn't 
	// do that.

	HRESULT hr = S_OK;
	if( ppTex == NULL )
		return( hr );
	SAFE_RELEASE( *ppTex );
	return( hr );
}
