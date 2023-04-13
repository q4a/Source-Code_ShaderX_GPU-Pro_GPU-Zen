
#ifndef __DX_HELPERS_H
#define __DX_HELPERS_H


namespace dx {

IDirect3DTexture8*	loadTexture( IDirect3DDevice8* device, const char *fileName );
ID3DXEffect*		loadEffect( IDirect3DDevice8* device, const char *fileName );
ID3DXMesh*			loadMesh( IDirect3DDevice8* device, const char *fileName, DWORD options, D3DMATERIAL8 *outMaterial = NULL );

void findValidTechnique( ID3DXEffect* efx );

inline void loadTexture( IDirect3DTexture8*& tex, IDirect3DDevice8& device, const char *fileName )
{
	assert( !tex );
	tex = loadTexture( &device, fileName );
	assert( tex );
}

inline void loadEffect( ID3DXEffect*& efx, IDirect3DDevice8& device, const char *fileName )
{
	assert( !efx );
	efx = loadEffect( &device, fileName );
	assert( efx );
}

inline void loadMesh( ID3DXMesh*& mesh, IDirect3DDevice8& device, const char *fileName, DWORD options, D3DMATERIAL8 *outMaterial = NULL )
{
	assert( !mesh );
	mesh = loadMesh( &device, fileName, options, outMaterial );
	assert( mesh );
}

template< typename T >
void assertRelease( T& o )
{
	assert( &o );
	DWORD cnt = o->Release();
	o = NULL;
}

};

#endif
