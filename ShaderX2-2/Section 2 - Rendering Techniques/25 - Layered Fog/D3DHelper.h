#pragma once

class CD3DHelper
{
public:

	class CVertex
	{
	public:
		enum
		{
			E_FVF = D3DFVF_XYZ + D3DFVF_NORMAL + D3DFVF_TEX1 + D3DFVF_TEXCOORDSIZE2(0),
		};

		D3DXVECTOR3	Pos;
		D3DXVECTOR3 Normal;
		D3DXVECTOR2 TexCoord;
	};
	typedef std::vector<CVertex> vVertex;


	HRESULT BuildSquarePath(LPDIRECT3DDEVICE9 _pD3DDevice, UINT _NbSection, LPDIRECT3DVERTEXBUFFER9 * _ppVertexBuffer, UINT & _NbPrim);

	CD3DHelper(void);
	~CD3DHelper(void);

private:


	void ConcatenateStrip(vVertex & _Buffer, const vVertex & _Strip);
	void CreateAndFillVertexBuffer(LPDIRECT3DDEVICE9 _pD3DDevice, const vVertex & _Buffer, LPDIRECT3DVERTEXBUFFER9 * _ppVertexBuffer);
};
