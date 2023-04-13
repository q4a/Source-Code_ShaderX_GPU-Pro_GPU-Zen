// --------------------------------------------------------------
// --------------------------------------------------------------
//
// CD3DQuad.h
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
//      Quad with per vertex TBN matrix (for bump mapping)
//
// --------------------------------------------------------------
// --------------------------------------------------------------
#ifndef __CD3DQUAD__
#define __CD3DQUAD__
// --------------------------------------------------------------
class CD3DQuad
{
struct VERTEX
{
    FLOAT x, y, z;
    FLOAT nx, ny, nz;
	FLOAT u, v;
    FLOAT tx, ty, tz;
    FLOAT bx, by, bz;

};

static DWORD FVF;

protected:

	LPDIRECT3DDEVICE9       m_pd3dDevice;
	LPDIRECT3DVERTEXBUFFER9 m_QuadVB;
	

public:

  CD3DQuad(LPDIRECT3DDEVICE9 p);
  ~CD3DQuad();

  void Render();
};
// --------------------------------------------------------------
#endif
// --------------------------------------------------------------
