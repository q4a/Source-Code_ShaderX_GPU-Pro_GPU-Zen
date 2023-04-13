#include "stdafx.h"

#include "Shadows.h"
#include "utils/Camera.h"
#include "utils/DXHelpers.h"
#include "Vertices.h"


CShadows::CShadows( const CCamera& camera )
:	mCamera( &camera ),
	//mTexObject(0), mTexGround(0),
	mEfxAmbient(0), mEfxCasterNormal(0), mEfxCasterReverse(0), mEfxReceiver(0),
	mMeshObject(0), mMeshObjectSimple(0), mMeshGround(0),
	mAnimateLight(false), mAnimateObject(false), mLowPolyCaster(true), mReverseExtrusion(true)
{
}


HRESULT CShadows::onCreateDevice( IDirect3DDevice8* device )
{
	CDXObject::onCreateDevice( device );
	
	// load textures
	//dx::loadTexture( mTexObject, getDevice(), "tex/Object.png" );
	//dx::loadTexture( mTexGround, getDevice(), "tex/Ground.png" );
	
	// load effects
	dx::loadEffect( mEfxAmbient, getDevice(), "efx/Ambient.sha" );
	dx::loadEffect( mEfxCasterNormal, getDevice(), "efx/CasterNormal.sha" );
	dx::loadEffect( mEfxCasterReverse, getDevice(), "efx/CasterReverse.sha" );
	dx::loadEffect( mEfxReceiver, getDevice(), "efx/Receiver.sha" );
	
	// load meshes
	dx::loadMesh( mMeshObject, getDevice(), "mesh/objectFull.x", D3DXMESH_MANAGED, NULL );
	dx::loadMesh( mMeshObjectSimple, getDevice(), "mesh/objectSimple.x", D3DXMESH_MANAGED, NULL );
	dx::loadMesh( mMeshGround, getDevice(), "mesh/ground.x", D3DXMESH_MANAGED, NULL );

	// init data
	D3DXMatrixIdentity( &mGroundMatrix );
	mGroundMatrix._11 = mGroundMatrix._22 = mGroundMatrix._33 = 4.0f;
	D3DXMatrixIdentity( &mObjectMatrix );
	mObjectMatrix._42 = 15.0f;
	mLightPos = D3DXVECTOR4( 30, 50, 0, 1 );
	
	return S_OK;
}

HRESULT CShadows::onLostDevice()
{
	CDXObject::onLostDevice();

	assert( mEfxAmbient );
	mEfxAmbient->OnLostDevice();
	assert( mEfxCasterNormal );
	mEfxCasterNormal->OnLostDevice();
	assert( mEfxCasterReverse );
	mEfxCasterReverse->OnLostDevice();
	assert( mEfxReceiver );
	mEfxReceiver->OnLostDevice();

	return S_OK;
}

HRESULT CShadows::onResetDevice()
{
	CDXObject::onResetDevice();

	assert( mEfxAmbient );
	mEfxAmbient->OnResetDevice();
	dx::findValidTechnique( mEfxAmbient );

	assert( mEfxCasterNormal );
	mEfxCasterNormal->OnResetDevice();
	dx::findValidTechnique( mEfxCasterNormal );

	assert( mEfxCasterReverse );
	mEfxCasterReverse->OnResetDevice();
	dx::findValidTechnique( mEfxCasterReverse );
	
	assert( mEfxReceiver );
	mEfxReceiver->OnResetDevice();
	dx::findValidTechnique( mEfxReceiver );
	
	return S_OK;
}

HRESULT CShadows::onDestroyDevice()
{
	CDXObject::onDestroyDevice();
	
	// release textures
	//dx::assertRelease( mTexObject );
	//dx::assertRelease( mTexGround );
	
	// release effects
	dx::assertRelease( mEfxAmbient );
	dx::assertRelease( mEfxCasterNormal );
	dx::assertRelease( mEfxCasterReverse );
	dx::assertRelease( mEfxReceiver );
	
	// release meshes
	dx::assertRelease( mMeshObject );
	dx::assertRelease( mMeshObjectSimple );
	dx::assertRelease( mMeshGround );

	return S_OK;
}


void CShadows::render( float lastFrameTime, float time )
{
	UINT passes, p;

	//
	// animate

	if( mAnimateLight ) {
		D3DXMATRIX m;
		D3DXMatrixRotationY( &m, lastFrameTime );
		D3DXVec4Transform( &mLightPos, &mLightPos, &m );
	}

	if( mAnimateObject ) {
		mObjectMatrix._42 = 0.0f;
		D3DXMATRIX m;
		D3DXMatrixRotationY( &m, -lastFrameTime*0.7f );
		D3DXMatrixMultiply( &mObjectMatrix, &mObjectMatrix, &m );
		D3DXMatrixRotationX( &m, lastFrameTime*0.4f );
		D3DXMatrixMultiply( &mObjectMatrix, &mObjectMatrix, &m );
		mObjectMatrix._42 = 15.0f;
	}

	//
	// calc some stuff for shaders

	// view*proj matrix
	D3DXMATRIX matViewProj;
	D3DXMatrixMultiply( &matViewProj, &mCamera->getViewMatrix(), &mCamera->getProjMatrix() );

	// world*view*proj matrices
	D3DXMATRIX matFinalObj;
	D3DXMatrixMultiply( &matFinalObj, &mObjectMatrix, &matViewProj );
	D3DXMATRIX matFinalGround;
	D3DXMatrixMultiply( &matFinalGround, &mGroundMatrix, &matViewProj );

	// inverse matrices of objects
	D3DXMATRIX matInvObj;
	D3DXMatrixInverse( &matInvObj, NULL, &mObjectMatrix );
	D3DXMATRIX matInvGround;
	D3DXMatrixInverse( &matInvGround, NULL, &mGroundMatrix );

	// light pos in object space
	D3DXVECTOR4 lightPosObj;
	D3DXVec4Transform( &lightPosObj, &mLightPos, &matInvObj );
	D3DXVECTOR4 lightPosGround;
	D3DXVec4Transform( &lightPosGround, &mLightPos, &matInvGround );

	//
	// ambient

	mEfxAmbient->Begin( &passes, 0 );
	for( p = 0; p < passes; ++p ) {
		mEfxAmbient->Pass(p);

		D3DXMATRIX m;
		
		// object
		mEfxAmbient->SetMatrix( "mwvp", &matFinalObj );
		renderMeshVS( *mMeshObject );

		// ground
		mEfxAmbient->SetMatrix( "mwvp", &matFinalGround );
		renderMeshVS( *mMeshGround );
	}
	mEfxAmbient->End();

	//
	// shadow volume

	ID3DXEffect* fx = (mReverseExtrusion) ? mEfxCasterReverse : mEfxCasterNormal;

	fx->SetVector( "vLT0", &lightPosObj );
	fx->SetMatrix( "mwvp", &matFinalObj );

	fx->Begin( &passes, 0 );
	for( p = 0; p < passes; ++p ) {
		fx->Pass(p);
		
		// object
		renderMeshVS( mLowPolyCaster ? *mMeshObjectSimple : *mMeshObject );
	}
	fx->End();

	//
	// shadow receivers

	mEfxReceiver->Begin( &passes, 0 );
	for( p = 0; p < passes; ++p ) {
		mEfxReceiver->Pass(p);
		
		// object
		mEfxReceiver->SetVector( "vLT0", &lightPosObj );
		mEfxReceiver->SetMatrix( "mwvp", &matFinalObj );
		renderMeshVS( *mMeshObject );

		// ground
		mEfxReceiver->SetVector( "vLT0", &lightPosGround );
		mEfxReceiver->SetMatrix( "mwvp", &matFinalGround );
		renderMeshVS( *mMeshGround );
	}
	mEfxReceiver->End();
}

void CShadows::renderMeshVS( ID3DXMesh& mesh )
{
	IDirect3DIndexBuffer8* indexBuffer = NULL;
	IDirect3DVertexBuffer8* vertexBuffer = NULL;

	// get buffers
	mesh.GetIndexBuffer( &indexBuffer );
	mesh.GetVertexBuffer( &vertexBuffer );
	
	// set buffers
	getDevice().SetIndices( indexBuffer, 0 );
	int stride = D3DXGetFVFVertexSize( mesh.GetFVF() );
	getDevice().SetStreamSource( 0, vertexBuffer, stride );

	// render
	getDevice().DrawIndexedPrimitive( 
		D3DPT_TRIANGLELIST,
		0,
		mesh.GetNumVertices(),
		0,
		mesh.GetNumFaces() );

	// unset buffers
	getDevice().SetIndices( NULL, 0 );
	getDevice().SetStreamSource( 0, NULL, 0 );
}

