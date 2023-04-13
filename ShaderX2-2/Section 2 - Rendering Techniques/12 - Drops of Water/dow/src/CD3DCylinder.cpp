//-------------------------------------------------------
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <commctrl.h>
#include <math.h>
//-------------------------------------------------------
#include "CD3DCylinder.h"
//-------------------------------------------------------
DWORD CD3DCylinder::FVF = D3DFVF_XYZ |
							  D3DFVF_NORMAL |
							  D3DFVF_TEX3 |
							  D3DFVF_TEXCOORDSIZE2(0) |
							  D3DFVF_TEXCOORDSIZE2(1) |
							  D3DFVF_TEXCOORDSIZE2(2);


CD3DCylinder::CD3DCylinder(LPDIRECT3DDEVICE9 p)
{
	m_pd3dDevice=p;
	genCylinder(0.7,2.0,
		        2,20);
}
//-------------------------------------------------------
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif
//-------------------------------------------------------
void CD3DCylinder::genCylinder(double r,double h,int lodh,int lodr)
{
	int         i,j;
	double      z1,jf;

	m_iLodH=lodh;
	m_iLodR=lodr;

	CD3DCylinder::VERTEX *v=new CD3DCylinder::VERTEX[(lodh+1)*(lodr+1)];
	int                  *indices=new int[lodh*lodr*6];

    m_pd3dDevice->CreateVertexBuffer(((lodh+1)*(lodr+1))*sizeof(CD3DCylinder::VERTEX),
									 0, CD3DCylinder::FVF,
		                             D3DPOOL_DEFAULT, &m_VB, NULL);

	m_pd3dDevice->CreateIndexBuffer((lodh*lodr*6)*sizeof(int),
									 0, D3DFMT_INDEX32,
		                             D3DPOOL_DEFAULT, &m_IB, NULL);

	// cylinder
	// -> verticies
	for(i=0;i<=lodh;i++)
	{
		z1=((double )i)/((double )lodh)*h-h/2;
		for (j=0;j<=lodr;j++)
		{
			jf=M_PI*2.0*(((double )j)/((double )lodr));
			
			v[i*(lodr+1)+j].p=D3DXVECTOR3(r*cos(jf),z1,r*sin(jf));
			v[i*(lodr+1)+j].t=D3DXVECTOR3(-sin(jf),0.0,cos(jf));
			v[i*(lodr+1)+j].n=D3DXVECTOR3(cos(jf),0.0,sin(jf));
			D3DXVECTOR3 cross;
			D3DXVec3Cross(&cross,&(v[j].t),&(v[j].n));
			v[i*(lodr+1)+j].b=cross;
			v[i*(lodr+1)+j].uv=D3DXVECTOR2(-2.0*M_PI*r/h*((double)(j))/((double)lodr),
										                 ((double)(i))/((double)lodh));
		}
	}

	// -> triangles
	for(i=0;i<lodh;i++)
	{
		for (j=0;j<lodr;j++)
		{
			indices[(i*lodr + j)*6+1]= i   *(lodr+1)+ j;
			indices[(i*lodr + j)*6  ]=(i+1)*(lodr+1)+(j+1);
			indices[(i*lodr + j)*6+2]=(i+1)*(lodr+1)+ j;

			indices[(i*lodr + j)*6+4]= i   *(lodr+1)+ j;
			indices[(i*lodr + j)*6+3]= i   *(lodr+1)+(j+1);
			indices[(i*lodr + j)*6+5]=(i+1)*(lodr+1)+(j+1);
		}
	}

	// fill in vertex buffer
    VOID* pVertices;
    m_VB->Lock(0,0,(void**)&pVertices,0);
    memcpy( pVertices,v,((lodh+1)*(lodr+1))*sizeof(CD3DCylinder::VERTEX) );
    m_VB->Unlock();

	// fill in index buffer
    VOID* pIndices;
    m_IB->Lock(0,0,(void**)&pIndices,0);
    memcpy( pIndices,indices,(lodh*lodr*6)*sizeof(int) );
    m_IB->Unlock();

	delete [](v);
	delete [](indices);
}

CD3DCylinder::~CD3DCylinder()
{
	m_VB->Release();
	m_IB->Release();
}

void CD3DCylinder::Render()
{

	m_pd3dDevice->SetFVF( CD3DCylinder::FVF );
	m_pd3dDevice->SetStreamSource( 0, m_VB, 0, sizeof(CD3DCylinder::VERTEX) );
	m_pd3dDevice->SetIndices( m_IB );
	m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,(m_iLodH*m_iLodR)*6,0,m_iLodR*m_iLodH*2);

}
