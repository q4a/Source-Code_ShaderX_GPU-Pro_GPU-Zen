//-----------------------------------------------------------------------------
// File: Mesh.cpp
//
// Desc: Support code for loading DirectX .X files.
//-----------------------------------------------------------------------------
//#define STRICT
#include <tchar.h>
#include <stdio.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxfile.h>
#include <rmxfguid.h>
#include "Mesh.h"
#include "DXUtil.h"
#include "D3DUtil.h"
#include "ShaderUtils.h"
#include "TextureUtils.h"
#include "Scene.h"
#include "Light.h"


MeshDesc::MeshDesc()
	: m_Scale(1.0f), m_DiffuseUVScale(1.0f), m_BumpUVScale(1.0f), m_BumpScale(1.0f),
	m_SpecularType(ST_LINEAR_APPROX), m_EnableSpecLevel(false)
{
	_tcscpy( m_Mesh, _T("") );
	_tcscpy( m_Diffuse, _T("") );
	_tcscpy( m_Bump, _T("") );
	_tcscpy( m_SpecLevel, _T("") );
	_tcscpy( m_SpecPower, _T("") );
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
Mesh::Mesh()
{
    m_pSysMemMesh        = NULL;
    m_pLocalMesh         = NULL;
	m_DiffuseVS			 = NULL;
	m_DiffusePS			 = NULL;
	m_SpecularVS		 = NULL;
	m_SpecularPS		 = NULL;
	m_pDiffuseTexture	 = NULL;
	m_pNormalTexture	 = NULL;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
Mesh::~Mesh()
{
    Destroy();
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT Mesh::Create( LPDIRECT3DDEVICE9 pd3dDevice, MeshDesc& desc )
{
	m_MeshDesc = desc;

	// Expand all the filenames to be absolute.
	ExpandFilename(m_MeshDesc.m_Mesh);
	ExpandFilename(m_MeshDesc.m_Diffuse);
	ExpandFilename(m_MeshDesc.m_Bump);
	ExpandFilename(m_MeshDesc.m_SpecLevel);
	ExpandFilename(m_MeshDesc.m_SpecPower);

    HRESULT      hr;

	LoadMesh(pd3dDevice);

	// Create shaders
	ShaderUtils::CreateDiffuseShaders(m_MeshDesc, &m_DiffuseVS, &m_DiffusePS, pd3dDevice);
	ShaderUtils::CreateSpecularShaders(m_MeshDesc, &m_SpecularVS, &m_SpecularPS, pd3dDevice);

    // Load diffuse texture map.
	if (FilenameSet(m_MeshDesc.m_SpecLevel))
	{
		// Have a specular level map, so need to combine it into the diffuse alpha channel
		m_pDiffuseTexture = TextureUtils::CreateTextureAlpha(&(m_MeshDesc.m_Diffuse[0]), &(m_MeshDesc.m_SpecLevel[0]), pd3dDevice);
	}
	else
	{
		if( FAILED( hr = D3DUtil_CreateTexture( pd3dDevice, &(m_MeshDesc.m_Diffuse[0]), 
										   &m_pDiffuseTexture, D3DFMT_A8R8G8B8 ) ) )
		{
			return hr;
		}
	}

	if (FilenameSet(m_MeshDesc.m_SpecPower))
	{
		// Have a specular power map, so need to combine it into the normal map alpha channel
		m_pNormalTexture = TextureUtils::CreateNormalMapAlpha(m_MeshDesc.m_BumpScale, &(m_MeshDesc.m_Bump[0]), &(m_MeshDesc.m_SpecPower[0]), pd3dDevice);
	}
	else
	{
		CreateFileBasedNormalMap(pd3dDevice);
	}

    return S_OK;
}


HRESULT Mesh::LoadMesh(LPDIRECT3DDEVICE9 pd3dDevice)
{
    TCHAR        strPath[MAX_PATH];
    LPD3DXBUFFER pAdjacencyBuffer = NULL;
    LPD3DXBUFFER pMtrlBuffer = NULL;
    HRESULT      hr;

    // Find the path for the file, and convert it to ANSI (for the D3DX API)
    DXUtil_FindMediaFileCb( strPath, sizeof(strPath), m_MeshDesc.m_Mesh );

    // Load the mesh
    if( FAILED( hr = D3DXLoadMeshFromX( strPath, D3DXMESH_SYSTEMMEM, pd3dDevice, 
                                        &pAdjacencyBuffer, &pMtrlBuffer, NULL,
                                        &m_dwNumMaterials, &m_pSysMemMesh ) ) )
    {
        return hr;
    }

    // Lock the vertex buffer, to generate a simple bounding sphere
    LPDIRECT3DVERTEXBUFFER9 pVertexBuffer = NULL;
    hr = m_pSysMemMesh->GetVertexBuffer( &pVertexBuffer );
    if( FAILED(hr) )
	{
		return hr;
	}

    void* pVertices;
    hr = pVertexBuffer->Lock( 0, 0, &pVertices, D3DLOCK_READONLY );//D3DLOCK_NOSYSLOCK );
    if( FAILED(hr) )
	{
		return hr;
	}

    hr = D3DXComputeBoundingSphere( (D3DXVECTOR3*)pVertices, m_pSysMemMesh->GetNumVertices(),
                                    D3DXGetFVFVertexSize(m_pSysMemMesh->GetFVF()),
                                    &m_ObjectCenter, &m_ObjectRadius );
    pVertexBuffer->Unlock();
    pVertexBuffer->Release();

	/*
    // perform simple cleansing operations on mesh
	LPD3DXMESH   pTempMesh;
    if( FAILED( hr = D3DXCleanMesh( m_pSysMemMesh, (DWORD*)pAdjacencyBuffer->GetBufferPointer(), &pTempMesh, 
                                           (DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL ) ) )
    {
		return hr;
    }
    SAFE_RELEASE(m_pSysMemMesh);
    m_pSysMemMesh = pTempMesh;
	//*/

	/*
    //  Perform a weld to try and remove excess vertices.  A small epsilon like 1e-6 works well too.
	D3DXWELDEPSILONS Epsilons;
    memset(&Epsilons, 0, sizeof(D3DXWELDEPSILONS));
    if( FAILED( hr = D3DXWeldVertices( m_pSysMemMesh, 0, &Epsilons, 
                                                (DWORD*)pAdjacencyBuffer->GetBufferPointer(), 
                                                (DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL, NULL ) ) )
    {
		return hr;
    }
	//*/

	/*
    // Optimize the mesh for performance
    if( FAILED( hr = m_pSysMemMesh->OptimizeInplace(
                        D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
                        (DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL, NULL, NULL ) ) )
    {
        SAFE_RELEASE( pAdjacencyBuffer );
        SAFE_RELEASE( pMtrlBuffer );
        return hr;
    }
	//*/

    hr = S_OK;

    SAFE_RELEASE( pAdjacencyBuffer );
    SAFE_RELEASE( pMtrlBuffer );

	// Now clone it to the correct format.
	SetFormat( pd3dDevice );

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT Mesh::SetFormat( LPDIRECT3DDEVICE9 pd3dDevice )
{
	D3DVERTEXELEMENT9 dcl[MAX_FVF_DECL_SIZE];
	DWORD dclOff = 0;
	WORD offset = 0;

	// Position
	dcl[dclOff].Stream = 0;
	dcl[dclOff].Offset = offset;
	dcl[dclOff].Type = D3DDECLTYPE_FLOAT3;
	dcl[dclOff].Method = D3DDECLMETHOD_DEFAULT;
	dcl[dclOff].Usage = D3DDECLUSAGE_POSITION;
	dcl[dclOff].UsageIndex = 0;
	dclOff++;
	offset += 12;
	// UV
	dcl[dclOff].Stream = 0;
	dcl[dclOff].Offset = offset;
	dcl[dclOff].Type = D3DDECLTYPE_FLOAT2;
	dcl[dclOff].Method = D3DDECLMETHOD_DEFAULT;
	dcl[dclOff].Usage = D3DDECLUSAGE_TEXCOORD;
	dcl[dclOff].UsageIndex = 0;
	dclOff++;
	offset += 8;
	// Normal
	dcl[dclOff].Stream = 0;
	dcl[dclOff].Offset = offset;
	dcl[dclOff].Type = D3DDECLTYPE_FLOAT3;
	dcl[dclOff].Method = D3DDECLMETHOD_DEFAULT;
	dcl[dclOff].Usage = D3DDECLUSAGE_NORMAL;
	dcl[dclOff].UsageIndex = 0;
	dclOff++;
	offset += 12;
	// Tangent
	dcl[dclOff].Stream = 0;
	dcl[dclOff].Offset = offset;
	dcl[dclOff].Type = D3DDECLTYPE_FLOAT3;
	dcl[dclOff].Method = D3DDECLMETHOD_DEFAULT;
	dcl[dclOff].Usage = D3DDECLUSAGE_TANGENT;
	dcl[dclOff].UsageIndex = 0;
	dclOff++;
	offset += 12;
	// Bi-Normal
	dcl[dclOff].Stream = 0;
	dcl[dclOff].Offset = offset;
	dcl[dclOff].Type = D3DDECLTYPE_FLOAT3;
	dcl[dclOff].Method = D3DDECLMETHOD_DEFAULT;
	dcl[dclOff].Usage = D3DDECLUSAGE_BINORMAL;
	dcl[dclOff].UsageIndex = 0;
	dclOff++;
	offset += 12;
	// End
	D3DVERTEXELEMENT9 endElement = D3DDECL_END();
	dcl[dclOff] = endElement;
	dclOff++;

    LPD3DXMESH pTempSysMemMesh = NULL;
    LPD3DXMESH pTempLocalMesh  = NULL;

    if( m_pSysMemMesh )
    {
        if( FAILED( m_pSysMemMesh->CloneMesh( D3DXMESH_SYSTEMMEM, dcl,
                                                 pd3dDevice, &pTempSysMemMesh ) ) )
            return E_FAIL;
    }
    if( m_pLocalMesh )
    {
        if( FAILED( m_pLocalMesh->CloneMesh( 0L, dcl, pd3dDevice,
                                                &pTempLocalMesh ) ) )
        {
            SAFE_RELEASE( pTempSysMemMesh );
            return E_FAIL;
        }
    }

    SAFE_RELEASE( m_pSysMemMesh );
    SAFE_RELEASE( m_pLocalMesh );

    if( pTempSysMemMesh ) m_pSysMemMesh = pTempSysMemMesh;
    if( pTempLocalMesh )  m_pLocalMesh  = pTempLocalMesh;

    // Compute normals in case the meshes have them
    if( m_pSysMemMesh )
	{
        //D3DXComputeNormals( m_pSysMemMesh, NULL );
		D3DXComputeTangent( m_pSysMemMesh, 0, 0, 0, 1, NULL);
	}
    if( m_pLocalMesh )
	{
        //D3DXComputeNormals( m_pLocalMesh, NULL );
		D3DXComputeTangent( m_pLocalMesh, 0, 0, 0, 1, NULL);
	}

	// Also create declarator object
	pd3dDevice->CreateVertexDeclaration(dcl, &m_VDecl);

    return S_OK;
}

HRESULT Mesh::CreateFileBasedNormalMap(LPDIRECT3DDEVICE9 pd3dDevice)
{
    HRESULT hr = S_OK;
    LPDIRECT3DTEXTURE9 pFileBasedNormalMapSource = NULL;

    // Load the texture from a file
    if( FAILED( hr = D3DUtil_CreateTexture( pd3dDevice, m_MeshDesc.m_Bump, 
                                            &pFileBasedNormalMapSource, 
                                            D3DFMT_A8R8G8B8 ) ) )
    {
        return hr;
    }

    D3DSURFACE_DESC sDesc;
    pFileBasedNormalMapSource->GetLevelDesc( 0, &sDesc );    

    if( FAILED( hr = D3DXCreateTexture( pd3dDevice, sDesc.Width, sDesc.Height, 
        pFileBasedNormalMapSource->GetLevelCount(), 0, 
        D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pNormalTexture ) ) )
    {
        return hr;
    }

    if( FAILED( hr = D3DXComputeNormalMap( m_pNormalTexture, pFileBasedNormalMapSource,
        NULL, 0, D3DX_CHANNEL_RED, m_MeshDesc.m_BumpScale ) ) )
    {
        return hr;
    }

    SAFE_RELEASE( pFileBasedNormalMapSource );
    return hr;
}

void Mesh::ExpandFilename(TCHAR* filename)
{
	if (FilenameSet(filename))
	{
		TCHAR strPath[MAX_PATH];
		DXUtil_FindMediaFileCb( strPath, sizeof(strPath), filename );
		_tcscpy( filename, strPath );
	}
}

bool Mesh::FilenameSet(TCHAR* filename)
{
	return (_tcslen(filename) > 0);
}

//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT Mesh::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
    if( NULL == m_pSysMemMesh )
        return E_FAIL;

	D3DVERTEXELEMENT9 dcl[MAX_FVF_DECL_SIZE];

	m_pSysMemMesh->GetDeclaration(dcl);

    // Make a local memory version of the mesh. Note: because we are passing in
    // no flags, the default behavior is to clone into local memory.
    if( FAILED( m_pSysMemMesh->CloneMesh( 0L, dcl,
                                          pd3dDevice, &m_pLocalMesh ) ) )
        return E_FAIL;

    return S_OK;

}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT Mesh::InvalidateDeviceObjects()
{
    SAFE_RELEASE( m_pLocalMesh );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT Mesh::Destroy()
{
    InvalidateDeviceObjects();

    SAFE_RELEASE( m_pSysMemMesh );
	SAFE_RELEASE( m_DiffuseVS );
	SAFE_RELEASE( m_DiffusePS );
	SAFE_RELEASE( m_SpecularVS );
	SAFE_RELEASE( m_SpecularPS );
	SAFE_RELEASE( m_VDecl );
	SAFE_RELEASE( m_pDiffuseTexture );
	SAFE_RELEASE( m_pNormalTexture );

    return S_OK;
}


DWORD Mesh::GetSpecularType()
{
	return m_MeshDesc.m_SpecularType;
}
void Mesh::SetSpecularType(DWORD specularType, LPDIRECT3DDEVICE9 pd3dDevice)
{
	if (specularType != m_MeshDesc.m_SpecularType)
	{
		// Changed
		m_MeshDesc.m_SpecularType = specularType;

		// Re-create shaders.
		SAFE_RELEASE(m_SpecularVS);
		SAFE_RELEASE(m_SpecularPS);

		ShaderUtils::CreateSpecularShaders(m_MeshDesc, &m_SpecularVS, &m_SpecularPS, pd3dDevice);
	}
}

void Mesh::SetMesh(TCHAR* name, LPDIRECT3DDEVICE9 pd3dDevice)
{
    SAFE_RELEASE( m_pSysMemMesh );
    SAFE_RELEASE( m_pLocalMesh );
	SAFE_RELEASE( m_VDecl );

	_tcscpy( m_MeshDesc.m_Mesh, name );
	ExpandFilename(m_MeshDesc.m_Mesh);

	LoadMesh(pd3dDevice);
	InvalidateDeviceObjects();
	RestoreDeviceObjects(pd3dDevice);
}
void Mesh::SetDiffuse(TCHAR* name, LPDIRECT3DDEVICE9 pd3dDevice)
{
	SAFE_RELEASE( m_pDiffuseTexture );

	_tcscpy( m_MeshDesc.m_Diffuse, name );
	ExpandFilename(m_MeshDesc.m_Diffuse);

    // Load diffuse texture map.
	if (FilenameSet(m_MeshDesc.m_SpecLevel))
	{
		// Have a specular level map, so need to combine it into the diffuse alpha channel
		m_pDiffuseTexture = TextureUtils::CreateTextureAlpha(&(m_MeshDesc.m_Diffuse[0]), &(m_MeshDesc.m_SpecLevel[0]), pd3dDevice);
	}
	else
	{
		if( FAILED( D3DUtil_CreateTexture( pd3dDevice, &(m_MeshDesc.m_Diffuse[0]), 
										   &m_pDiffuseTexture, D3DFMT_A8R8G8B8 ) ) )
		{
			return;
		}
	}
}
void Mesh::SetBump(TCHAR* name, LPDIRECT3DDEVICE9 pd3dDevice)
{
	SAFE_RELEASE( m_pNormalTexture );

	_tcscpy( m_MeshDesc.m_Bump, name );
	ExpandFilename(m_MeshDesc.m_Bump);

	if (FilenameSet(m_MeshDesc.m_SpecPower))
	{
		// Have a specular power map, so need to combine it into the normal map alpha channel
		m_pNormalTexture = TextureUtils::CreateNormalMapAlpha(m_MeshDesc.m_BumpScale, &(m_MeshDesc.m_Bump[0]), &(m_MeshDesc.m_SpecPower[0]), pd3dDevice);
	}
	else
	{
		CreateFileBasedNormalMap(pd3dDevice);
	}
}
void Mesh::SetSpecLevel(TCHAR* name, LPDIRECT3DDEVICE9 pd3dDevice)
{
	SAFE_RELEASE( m_pDiffuseTexture );

	_tcscpy( m_MeshDesc.m_SpecLevel, name );
	ExpandFilename(m_MeshDesc.m_SpecLevel);

    // Load diffuse texture map.
	if (FilenameSet(m_MeshDesc.m_SpecLevel))
	{
		// Have a specular level map, so need to combine it into the diffuse alpha channel
		m_pDiffuseTexture = TextureUtils::CreateTextureAlpha(&(m_MeshDesc.m_Diffuse[0]), &(m_MeshDesc.m_SpecLevel[0]), pd3dDevice);
	}
	else
	{
		if( FAILED( D3DUtil_CreateTexture( pd3dDevice, &(m_MeshDesc.m_Diffuse[0]), 
										   &m_pDiffuseTexture, D3DFMT_A8R8G8B8 ) ) )
		{
			return;
		}
	}
}
void Mesh::SetSpecPower(TCHAR* name, LPDIRECT3DDEVICE9 pd3dDevice)
{
	SAFE_RELEASE( m_pNormalTexture );

	_tcscpy( m_MeshDesc.m_SpecPower, name );
	ExpandFilename(m_MeshDesc.m_SpecPower);

	if (FilenameSet(m_MeshDesc.m_SpecPower))
	{
		// Have a specular power map, so need to combine it into the normal map alpha channel
		m_pNormalTexture = TextureUtils::CreateNormalMapAlpha(m_MeshDesc.m_BumpScale, &(m_MeshDesc.m_Bump[0]), &(m_MeshDesc.m_SpecPower[0]), pd3dDevice);
	}
	else
	{
		CreateFileBasedNormalMap(pd3dDevice);
	}
}

//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT Mesh::Render( Scene* scene, LPDIRECT3DDEVICE9 pd3dDevice )
{
    if( NULL == m_pLocalMesh )
        return E_FAIL;

	// Get all the matrices, combine them, and set the vertex shader constant matrix.
	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;
	matWorld = scene->m_matWorld;
	matView = scene->m_matView;
	matProj = scene->m_matProj;

	D3DXMATRIX matScale;
	D3DXMatrixScaling( &matScale, m_MeshDesc.m_Scale, m_MeshDesc.m_Scale, m_MeshDesc.m_Scale );
	D3DXMatrixMultiply( &(matWorld), &(matWorld), &matScale );


	D3DXMATRIX matCamera;
	D3DXMATRIX mat;
	D3DXMATRIX matTranspose;
	D3DXMatrixMultiply(&matCamera, &matWorld, &matView);
	D3DXMatrixMultiply(&mat, &matCamera, &matProj);
	D3DXMatrixTranspose(&matTranspose, &mat);

	pd3dDevice->SetVertexShaderConstantF(  0, (float*)&matTranspose, 4 );

	D3DXVECTOR4 uvScale(m_MeshDesc.m_DiffuseUVScale, m_MeshDesc.m_DiffuseUVScale, m_MeshDesc.m_BumpUVScale, m_MeshDesc.m_BumpUVScale);
	pd3dDevice->SetVertexShaderConstantF(  6, (float*)&uvScale, 1 );

	D3DXMATRIX worldToLocal;
	D3DXMatrixInverse(&worldToLocal, NULL, &matWorld);

	D3DXVECTOR3 cameraLocal;
	cameraLocal = scene->m_camPos;
	D3DXVec3TransformCoord(&cameraLocal, &cameraLocal, &worldToLocal);

	LPDIRECT3DVERTEXBUFFER9 vBuffer;
	m_pLocalMesh->GetVertexBuffer(&vBuffer);
	LPDIRECT3DINDEXBUFFER9 iBuffer;
	m_pLocalMesh->GetIndexBuffer(&iBuffer);
    pd3dDevice->SetVertexDeclaration( m_VDecl );
    pd3dDevice->SetStreamSource( 0, vBuffer, 0, 56 );
    pd3dDevice->SetIndices( iBuffer );

	// Disable alpha writes
	pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE );

	bool specLevelWritten = false;

	// Render once for each light
	DWORD l;
	for (l=0;l<scene->m_Lights.size();l++)
	{
		Light* light = scene->m_Lights[l];
		SetLightConstants(light, scene, worldToLocal, cameraLocal, pd3dDevice);
		if (light->m_Type == D3DLIGHT_POINT)
		{
			// Diffuse
			pd3dDevice->SetTexture( 0, m_pDiffuseTexture );
			pd3dDevice->SetTexture( 1, m_pNormalTexture );
			pd3dDevice->SetTexture( 2, TextureUtils::GetNormalisationCubemap(pd3dDevice) );
			if (l==0)
			{
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
			}
			else
			{
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			}
			if (l==0 && m_MeshDesc.m_EnableSpecLevel && FilenameSet(m_MeshDesc.m_SpecLevel))
			{
				// Enable alpha writes, so the specular level is written
				pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE );
			}
			pd3dDevice->SetVertexShader( m_DiffuseVS );
			pd3dDevice->SetPixelShader( m_DiffusePS );
			pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0,
											  0, m_pLocalMesh->GetNumVertices(),
											  0, m_pLocalMesh->GetNumFaces() );
			if (l==0 && m_MeshDesc.m_EnableSpecLevel && FilenameSet(m_MeshDesc.m_SpecLevel))
			{
				// Disable alpha writes
				pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE );
				specLevelWritten = true;
			}

			// Specular
			if (m_MeshDesc.m_SpecularType <= ST_LINEAR_APPROX)
			{
				pd3dDevice->SetTexture( 0, m_pNormalTexture );
				pd3dDevice->SetTexture( 1, TextureUtils::GetNormalisationCubemap(pd3dDevice) );
			}
			else if (m_MeshDesc.m_SpecularType == ST_FIXED_HIGH)
			{
				pd3dDevice->SetTexture( 0, m_pNormalTexture );
				pd3dDevice->SetTexture( 3, TextureUtils::GetSpecularCubemap(pd3dDevice) );
				D3DXVECTOR4 specInterp(scene->m_SpecularPower, scene->m_SpecularPower, scene->m_SpecularPower, scene->m_SpecularPower);
				pd3dDevice->SetPixelShaderConstantF(  3, (float*)(specInterp), 1 );
			}
			else if (m_MeshDesc.m_SpecularType == ST_PER_PIXEL)
			{
				pd3dDevice->SetTexture( 0, m_pNormalTexture );
				pd3dDevice->SetTexture( 3, TextureUtils::GetSpecularCubemap(pd3dDevice) );
			}
			if (specLevelWritten)
			{
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTALPHA );
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			}
			else
			{
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			}
			pd3dDevice->SetVertexShader( m_SpecularVS );
			pd3dDevice->SetPixelShader( m_SpecularPS );
			pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0,
											  0, m_pLocalMesh->GetNumVertices(),
											  0, m_pLocalMesh->GetNumFaces() );
		}
	}
	vBuffer->Release();
    iBuffer->Release();

    return S_OK;
}

void Mesh::SetLightConstants(
				Light* light, 
				Scene* scene,
				const D3DXMATRIX& worldToLocal, 
				const D3DXVECTOR3& cameraLocal,
				LPDIRECT3DDEVICE9 pd3dDevice)
{
	if (light->m_Type == D3DLIGHT_POINT)
	{
		D3DXVECTOR3 localPos;
		D3DXVec3TransformCoord(&localPos, &(light->m_Pos), &worldToLocal);

		pd3dDevice->SetVertexShaderConstantF(  4, (float*)&cameraLocal, 1 );
		pd3dDevice->SetVertexShaderConstantF(  5, (float*)&localPos, 1 );

		pd3dDevice->SetPixelShaderConstantF(  0, (float*)(scene->m_Ambient), 1 );
		pd3dDevice->SetPixelShaderConstantF(  1, (float*)(light->m_Diffuse), 1 );
		pd3dDevice->SetPixelShaderConstantF(  2, (float*)(light->m_Specular), 1 );
	}
}