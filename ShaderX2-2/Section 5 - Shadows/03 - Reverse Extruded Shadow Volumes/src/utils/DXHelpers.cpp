#include "../stdafx.h"

#include "DXHelpers.h"


namespace dx {

IDirect3DTexture8* loadTexture( IDirect3DDevice8* device, const char *fileName )
{
	IDirect3DTexture8* tex = NULL;
	D3DXCreateTextureFromFileEx(
		device, fileName, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
		0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL,
		&tex
	);
	assert( tex );
	return tex;
}

ID3DXEffect* loadEffect( IDirect3DDevice8* device, const char *fileName )
{
	ID3DXEffect* efx = NULL;
	D3DXCreateEffectFromFile( device, fileName, &efx, NULL );
	assert( efx );
	return efx;
}

ID3DXMesh* loadMesh( IDirect3DDevice8* device, const char *fileName, DWORD options, D3DMATERIAL8 *outMaterial )
{
	ID3DXBuffer* adjacency = NULL;
	ID3DXBuffer* materials = NULL;
	DWORD numMaterials = 0;
	ID3DXMesh* mesh = NULL;
	D3DXLoadMeshFromX( (char*)fileName, options, device,
		&adjacency, &materials, &numMaterials, &mesh
	);
	assert( mesh );
	if( adjacency )
		adjacency->Release();
	if( materials ) {
		if( numMaterials > 0 && outMaterial ) {
			*outMaterial = ((D3DXMATERIAL*)materials->GetBufferPointer())->MatD3D;
			// HACK
			outMaterial->Ambient.r = 1.0f;
			outMaterial->Ambient.g = 1.0f;
			outMaterial->Ambient.b = 1.0f;
			outMaterial->Ambient.a = 1.0f;
		}
		materials->Release();
	}
	return mesh;
}

void findValidTechnique( ID3DXEffect* efx )
{
	assert( efx );

	D3DXTECHNIQUE_DESC tech;
	HRESULT hr = efx->FindNextValidTechnique( NULL, &tech );
	assert( SUCCEEDED( hr ) );
	efx->SetTechnique( tech.Index );
}

}; // namespace
