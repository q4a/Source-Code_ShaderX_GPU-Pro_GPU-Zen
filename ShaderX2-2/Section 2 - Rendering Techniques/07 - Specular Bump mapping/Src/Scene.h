//-----------------------------------------------------------------------------
// File: Scene.h
//
// Desc: 
//-----------------------------------------------------------------------------
#ifndef SCENE_H
#define SCENE_H
#include <tchar.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>


class Light;


//-----------------------------------------------------------------------------
// Name: class Scene
// Desc: 
//-----------------------------------------------------------------------------
class Scene
{
public:
	D3DXMATRIX				m_matWorld;
	D3DXMATRIX				m_matView;
	D3DXMATRIX				m_matProj;
	D3DXVECTOR3				m_camPos;

	D3DXCOLOR				m_Ambient;

	FLOAT					m_SpecularPower;

	std::vector<Light*>		m_Lights;

    Scene();
    virtual ~Scene();
};

#endif



