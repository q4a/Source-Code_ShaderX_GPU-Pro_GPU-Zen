// Shader.h: interface for the Shader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADER_H__87D5BEE0_5329_4688_8F57_A789EBADB4DA__INCLUDED_)
#define AFX_SHADER_H__87D5BEE0_5329_4688_8F57_A789EBADB4DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum ShaderType
{
	SHADER_INITIAL,
	SHADER_OPT
};

class Shader  
{
public:
	virtual D3DVERTEXELEMENT9 * GetDecl();
	virtual HRESULT DeleteDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice, CD3DSettings &d3dSettings );
	virtual HRESULT Render();
	virtual HRESULT FrameMove( float fTime, D3DXMATRIXA16 &matWorld, D3DXMATRIXA16 &matView, D3DXMATRIXA16 &matProj );
	Shader( char *szShaderInitial, char *szShaderOpt );
	virtual ~Shader();
	virtual void SetActiveShader( ShaderType act );

	virtual void SetTessellation( int iTess );

protected:
	ShaderType m_ActiveShader;
    DWORD	m_dwNumVertices;
    DWORD	m_dwNumIndices;
	DWORD	m_dwDimension;
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
    LPDIRECT3DINDEXBUFFER9  m_pIB;
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	CD3DSettings *m_d3dSettings;
	char m_szShaderInitial[_MAX_PATH];
	char m_szShaderOpt[_MAX_PATH];
    LPDIRECT3DVERTEXSHADER9 m_pVertexShaderInitial;
    LPDIRECT3DVERTEXSHADER9 m_pVertexShaderOpt;
    LPDIRECT3DVERTEXDECLARATION9 m_pVertexDeclaration;
};

#endif // !defined(AFX_SHADER_H__87D5BEE0_5329_4688_8F57_A789EBADB4DA__INCLUDED_)
