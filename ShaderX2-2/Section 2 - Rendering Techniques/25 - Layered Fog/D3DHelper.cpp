#include "StdAfx.h"
#include "d3dhelper.h"

CD3DHelper::CD3DHelper(void)
{
}

CD3DHelper::~CD3DHelper(void)
{
}


/*
 *	
 */
HRESULT CD3DHelper::BuildSquarePath(LPDIRECT3DDEVICE9 _pD3DDevice, UINT _NbSection, LPDIRECT3DVERTEXBUFFER9 * _ppVertexBuffer, UINT & _NbPrim)
{
	HRESULT hr = S_OK;

	const float CellSize = 1.0f / float(_NbSection);

	D3DXVECTOR3 Pos, Normal;	
	D3DXVECTOR3 StartPos, StartNormal;
	D3DXVECTOR2 TexCoord, StartTexCoord;

	StartPos.x = -0.5f;
	StartPos.y = 0.0f;
	StartPos.z = -0.5f;
	
	StartTexCoord.x = 0.0f;
	StartTexCoord.y = 0.0f;

	StartNormal.x = 0.0f;
	StartNormal.y = 1.0f;
	StartNormal.z = 0.0f;

	vVertex Strip, Buffer;
	CVertex Vertex;

#define COPY_TO_VERTEX	\
	Vertex.Pos = Pos; \
	Vertex.Normal = Normal; \
	Vertex.TexCoord = TexCoord; \
	Strip.push_back(Vertex);


	for (UINT Height=0; Height<_NbSection; ++Height)
	{
		Pos = StartPos;
		Normal = StartNormal;
		TexCoord = StartTexCoord;

		StartPos.z += CellSize;
		StartTexCoord.y += CellSize;

		Strip.clear();

		for (UINT Width=0; Width<_NbSection + 1; ++Width)
		{
			COPY_TO_VERTEX

			Pos.z += CellSize;
			TexCoord.y += CellSize;

			COPY_TO_VERTEX

			Pos.x += CellSize;
			Pos.z -= CellSize;
			TexCoord.x += CellSize;
			TexCoord.y -= CellSize;
		}

		ConcatenateStrip(Buffer, Strip);
	}

	_NbPrim = Buffer.size() - 2;

	CreateAndFillVertexBuffer(_pD3DDevice, Buffer, _ppVertexBuffer);


	return hr;
}

/*
 *	
 */
void CD3DHelper::ConcatenateStrip(vVertex & _Buffer, const vVertex & _Strip)
{
	if (_Buffer.empty())
	{
		_Buffer = _Strip;
	}
	else
	{
		// insert a degenerated triangle
		_Buffer.push_back(_Buffer.back());
		_Buffer.push_back(_Strip[0]);
		_Buffer.push_back(_Strip[0]);
		_Buffer.push_back(_Strip[1]);

		for (UINT i=0; i<_Strip.size(); ++i)
		{
			_Buffer.push_back(_Strip[i]);
		}
	}
}

/*
 *	
 */
void CD3DHelper::CreateAndFillVertexBuffer(LPDIRECT3DDEVICE9 _pD3DDevice, const vVertex & _Buffer, LPDIRECT3DVERTEXBUFFER9 * _ppVertexBuffer)
{
	UINT Length = _Buffer.size() * sizeof(CVertex);
	DWORD Usage = D3DUSAGE_WRITEONLY;
	DWORD FVF = CVertex::E_FVF;
	D3DPOOL Pool = D3DPOOL_MANAGED;

	HRESULT hr = _pD3DDevice->CreateVertexBuffer(Length, Usage, FVF, Pool, _ppVertexBuffer, NULL);
	assert(SUCCEEDED(hr));

	UINT Offset = 0;
	UINT SizeToLock = Length;
	CVertex * pData = NULL;
	DWORD Flag = 0;

	hr = (*_ppVertexBuffer)->Lock(Offset, SizeToLock, (void**)&pData, Flag);
	assert(SUCCEEDED(hr));

	memcpy(pData, &_Buffer[0], Length);

	(*_ppVertexBuffer)->Unlock();
}
