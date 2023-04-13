#ifndef __D3DTEX__
#define __D3DTEX__

#include <windows.h>
#include <D3DX9.h>

#include "CTexture.h" 

namespace d3dtex
{

  inline LPDIRECT3DTEXTURE9 d3dtexFromTexture(LPDIRECT3DDEVICE9 d3dDevice,CTexture *tex)
  {
	D3DLOCKED_RECT      d3dlr;
	LPDIRECT3DTEXTURE9  dtex;

	if( FAILED( d3dDevice->CreateTexture( tex->getWidth(), tex->getHeight(), 1, 0,
		D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &dtex, NULL ) ) )
	{
		throw CException("Unable to create D3D texture !");
	}
	dtex->LockRect( 0, &d3dlr, 0, 0 );

	CHAR *pDst = (CHAR*)d3dlr.pBits;
	CHAR *pSrc = (CHAR *)tex->getData();

	int t=tex->isAlpha()?4:3;
	for (UINT i=0;i<tex->getWidth();i++)
	{
		for (UINT j=0;j<tex->getHeight();j++)
		{
			pDst[i*4+j*d3dlr.Pitch+2]=pSrc[(i+j*tex->getWidth())*t  ];
			pDst[i*4+j*d3dlr.Pitch+1]=pSrc[(i+j*tex->getWidth())*t+1];
			pDst[i*4+j*d3dlr.Pitch  ]=pSrc[(i+j*tex->getWidth())*t+2];
			if (t == 4)
				pDst[i*4+j*d3dlr.Pitch+3]=pSrc[(i+j*tex->getWidth())*t+3];
			else
				pDst[i*4+j*d3dlr.Pitch+3]=(char)255;
		}
	}
	dtex->UnlockRect(0);
	return (dtex);
  }

  inline LPDIRECT3DTEXTURE9 d3dtexLoadTexture(LPDIRECT3DDEVICE9 d3dDevice,const char *n)
  {
    CTexture *tex=CTexture::loadTexture(n);
	LPDIRECT3DTEXTURE9 dtex=d3dtexFromTexture(d3dDevice,tex);
	delete (tex);
	return (dtex);
  }

}

#endif
