//-----------------------------------------------------------------------------
// File: Mesh.h
//
// Desc: 
//-----------------------------------------------------------------------------
#ifndef MESH_H
#define MESH_H
#include <tchar.h>
#include <d3d9.h>
#include <d3dx9.h>


class Scene;
class Light;

enum SpecularType
{
	ST_POW1 = 0,
	ST_POW2,
	ST_POW4,
	ST_POW8,
	ST_POW16,
	ST_POW32,
	ST_LINEAR_APPROX,
	ST_FIXED_HIGH,
	ST_PER_PIXEL
};

struct MeshDesc
{
	TCHAR		m_Mesh[512];
	TCHAR		m_Diffuse[512];
	TCHAR		m_Bump[512];
	TCHAR		m_SpecLevel[512];
	TCHAR		m_SpecPower[512];

	FLOAT		m_Scale;
	FLOAT		m_DiffuseUVScale;
	FLOAT		m_BumpUVScale;
	FLOAT		m_BumpScale;

	DWORD		m_SpecularType;

	bool		m_EnableSpecLevel;

	MeshDesc();
};

//-----------------------------------------------------------------------------
// Name: class Mesh
// Desc: Class for loading and rendering file-based meshes
//-----------------------------------------------------------------------------
class Mesh
{
public:
    LPD3DXMESH						m_pSysMemMesh;    // SysMem mesh, lives through resize
    LPD3DXMESH						m_pLocalMesh;     // Local mesh, rebuilt on resize

    D3DXVECTOR3						m_ObjectCenter;   // Center of bounding sphere of object
    FLOAT							m_ObjectRadius;   // Radius of bounding sphere of object

    DWORD							m_dwNumMaterials; // Materials for the mesh

	IDirect3DVertexShader9*			m_DiffuseVS;
	IDirect3DPixelShader9*			m_DiffusePS;
	IDirect3DVertexShader9*			m_SpecularVS;
	IDirect3DPixelShader9*			m_SpecularPS;

	IDirect3DVertexDeclaration9*	m_VDecl;

    LPDIRECT3DTEXTURE9				m_pDiffuseTexture;		// The diffuse texture
    LPDIRECT3DTEXTURE9				m_pNormalTexture;		// The normal texture

	MeshDesc						m_MeshDesc;

public:
    // Rendering
    HRESULT Render( Scene* scene, LPDIRECT3DDEVICE9 pd3dDevice );

	DWORD GetSpecularType();
	void SetSpecularType(DWORD specularType, LPDIRECT3DDEVICE9 pd3dDevice);

	void SetMesh(TCHAR* name, LPDIRECT3DDEVICE9 pd3dDevice);
	void SetDiffuse(TCHAR* name, LPDIRECT3DDEVICE9 pd3dDevice);
	void SetBump(TCHAR* name, LPDIRECT3DDEVICE9 pd3dDevice);
	void SetSpecLevel(TCHAR* name, LPDIRECT3DDEVICE9 pd3dDevice);
	void SetSpecPower(TCHAR* name, LPDIRECT3DDEVICE9 pd3dDevice);

    // Mesh access
    LPD3DXMESH GetSysMemMesh() { return m_pSysMemMesh; }
    LPD3DXMESH GetLocalMesh()  { return m_pLocalMesh; }
	D3DXVECTOR3 GetObjectCenter() { return m_ObjectCenter; }
	FLOAT GetObjectRadius() { return m_ObjectRadius; }

    // Initializing
    HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT InvalidateDeviceObjects();

    // Creation/destruction
    HRESULT Create( LPDIRECT3DDEVICE9 pd3dDevice, MeshDesc& desc );
    HRESULT Destroy();

    Mesh();
    virtual ~Mesh();

private:
	HRESULT LoadMesh(LPDIRECT3DDEVICE9 pd3dDevice);

	HRESULT SetFormat( LPDIRECT3DDEVICE9 pd3dDevice );

	HRESULT CreateFileBasedNormalMap(LPDIRECT3DDEVICE9 pd3dDevice);

	void ExpandFilename(TCHAR* filename);

	bool FilenameSet(TCHAR* filename);

	void SetLightConstants(
				Light* light, 
				Scene* scene, 
				const D3DXMATRIX& worldToLocal, 
				const D3DXVECTOR3& cameraLocal,
				LPDIRECT3DDEVICE9 pd3dDevice);
};

#endif



