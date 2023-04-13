//-----------------------------------------------------------------------------
// File: Light.h
//
// Desc: 
//-----------------------------------------------------------------------------
#ifndef LIGHT_H
#define LIGHT_H
#include <tchar.h>
#include <d3d9.h>
#include <d3dx9.h>





//-----------------------------------------------------------------------------
// Name: class Scene
// Desc: 
//-----------------------------------------------------------------------------
class Light
{
public:
	DWORD			m_Type;	// same as D3DLIGHTTYPE

	D3DXVECTOR3		m_Pos;
	D3DXVECTOR3		m_Dir;

	D3DXCOLOR		m_Ambient;
	D3DXCOLOR		m_Diffuse;
	D3DXCOLOR		m_Specular;

    Light();
    ~Light();
};

#endif



