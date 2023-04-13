// Shader.h: interface for the Shader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MANDELBROTSHADER_H__87D5BEE0_5329_4688_8F57_A789EBADB4DA__INCLUDED_)
#define AFX_MANDELBROTSHADER_H__87D5BEE0_5329_4688_8F57_A789EBADB4DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class MandelbrotShader : public Shader
{
public:
	virtual HRESULT FrameMove( float fTime, D3DXMATRIXA16 &matWorld, D3DXMATRIXA16 &matView, D3DXMATRIXA16 &matProj );
	MandelbrotShader( char *szShaderInitial, char *szShaderOpt );
	virtual ~MandelbrotShader();

	void SetMinx( float fMinx );
	void SetMinz( float fMinz );
	void SetMaxx( float fMaxx );
	void SetMaxz( float fMaxz );

protected:
	float m_fMinX, m_fMinZ, m_fMaxX, m_fMaxZ;

	static float m_Colors[128][4];
};

#endif // !defined(AFX_MANDELBROTSHADER_H__87D5BEE0_5329_4688_8F57_A789EBADB4DA__INCLUDED_)
