// --------------------------------------------------------------
// --------------------------------------------------------------
//
// CD3DCylinder.h
//
//    Drops of Water source code
//
//                      ShaderX2 - www.shaderx2.com
//
//
//    Author:   Sylvain Lefebvre - 04/2003
//    ------
//
//    -= Check for updates at http://www.aracknea.net =-
//
//    File contents:
//    -------------
//
//      Cylinder with per vertex TBN matrix (for bump mapping)
//
// --------------------------------------------------------------
// --------------------------------------------------------------
#ifndef __CD3DCYLINDER__
#define __CD3DCYLINDER__
// --------------------------------------------------------------
#include <d3dx9.h>
// --------------------------------------------------------------
class CD3DCylinder
{
struct VERTEX
{
    D3DXVECTOR3 p;
    D3DXVECTOR3 n;
	D3DXVECTOR2 uv;
    D3DXVECTOR3 t;
    D3DXVECTOR3 b;
};

static DWORD FVF;

protected:

	LPDIRECT3DDEVICE9       m_pd3dDevice;
	LPDIRECT3DVERTEXBUFFER9 m_VB;
	LPDIRECT3DINDEXBUFFER9  m_IB;

	int m_iLodR;
	int m_iLodH;
	
	void CD3DCylinder::genCylinder(double r,double h,int lodh,int lodr);

public:

  CD3DCylinder(LPDIRECT3DDEVICE9 p);
  ~CD3DCylinder();

  void Render();
};
// --------------------------------------------------------------
#endif
// --------------------------------------------------------------
