#if !defined(COMPRESSED_MESH_H_021201_DC)
#define COMPRESSED_MESH_H_021201_DC
//-----------------------------------------------------------------------------
// File: CompressedMesh.h
//
// Desc: D3DXMesh doesn't support arbitary stream declarations needed for 
//		 vertex compression
//-----------------------------------------------------------------------------

class CompressedMesh
{
protected:
	LPDIRECT3DDEVICE9				m_pD3D;		//pointer to d3d device
	LPDIRECT3DVERTEXBUFFER9			m_pVB;		//pointer to vertex buffer interface
	LPDIRECT3DINDEXBUFFER9			m_pIB;		//pointer to index buffer interface
	D3DVERTEXELEMENT9				m_Decl[MAX_FVF_DECL_SIZE];	// stream decleration
	LPDIRECT3DVERTEXDECLARATION9	m_pDecl;		//pointer to index buffer interface
 	unsigned int				m_vertexSize; // size of a single vertex
	unsigned int				m_numVertex;  // number of vertices
	unsigned int				m_numIndices;  // number of indices
	
	HRESULT			CopyIndexBuffer( ID3DXBaseMesh* in );
	HRESULT			DetermineScaleAndOffset(	ID3DXBaseMesh* in, 
												D3DXMATRIX& transform, 
												D3DXVECTOR3& scale, 
												D3DXVECTOR3& offset );
	HRESULT			DetermineRotationMatrix( ID3DXBaseMesh* in, D3DXMATRIX& matrix);
	unsigned int	SizeOfD3DVertexType( const unsigned int type );

	unsigned int				QuantiseNormal( const float nx, const float ny, const float nz);
	
	D3DXVECTOR3					m_soOffset;		// scale offset method Offset
	D3DXVECTOR3					m_soScale;		// scale offset method Scale
	unsigned int				ScaleAndOffsetPosition8bit( const D3DXVECTOR3& in );
	void						ScaleAndOffsetPosition16bit( const D3DXVECTOR3& in,
													    short& x, short& y, short&z, short& w);

	D3DXMATRIX					m_ctMatrix;		// compression transform matrix
	unsigned int				CompressionTransformPosition8bit( const D3DXVECTOR3& in );
	void						CompressionTransformPosition16bit( const D3DXVECTOR3& in,
																    short& x, short& y, short&z, short& w);
	void						SlidingCompressionTransformPosition16bit(	const D3DXVECTOR3& pos,
																			const D3DXVECTOR3& norm,
																			unsigned int* outStream);

	enum SW_AXIS
	{
		SWA_X,
		SWA_Y,
		SWA_Z
	};

	void CreateSwapMatrix( D3DXMATRIX& in, SW_AXIS xAxis, SW_AXIS yAxis, SW_AXIS zAxis );

	unsigned int CompressionTransformPositionDEC3N( const D3DXVECTOR3& in );
	unsigned int CompressionTransformPosition101012( const D3DXVECTOR3& in );


public:
	CompressedMesh(  LPDIRECT3DDEVICE9 pD3D );		// default ctor
	~CompressedMesh();								// dtor

	// takes a D3XMesh and quantise the normals into this compressed mesh
	HRESULT QuantiseNormals( ID3DXBaseMesh* in );

	// takes a D3XMesh and scales and offset (8bit) into this compressed mesh
	HRESULT ScaleAndOffsetPosition8bit( ID3DXBaseMesh* in );
	HRESULT ScaleAndOffsetPosition16bit( ID3DXBaseMesh* in );

	HRESULT CompressionTransformPosition8bit( ID3DXBaseMesh* in );
	HRESULT CompressionTransformPosition16bit( ID3DXBaseMesh* in );
	HRESULT SlidingCompressionTransformPosition16bit( ID3DXBaseMesh* in );

	// new ones for Dx9
	HRESULT CompressionTransformPositionDEC3N( ID3DXBaseMesh* in );
	HRESULT CompressionTransformPosition101012( ID3DXBaseMesh* in );

	HRESULT Draw(); // equivelent to draw subset for D3XMesh

	LPDIRECT3DVERTEXBUFFER9	GetVertexBuffer(){ return m_pVB; };
	LPDIRECT3DINDEXBUFFER9	GetIndexBuffer() { return m_pIB; };
	D3DVERTEXELEMENT9*		GetStreamDeclaration() { return m_Decl; };
	unsigned int			GetVertexSize() const { return m_vertexSize; };

	D3DXVECTOR3&			GetScaleOffsetScale() { return m_soScale; };
	D3DXVECTOR3&			GetScaleOffsetOffset() { return m_soOffset; };
	D3DXMATRIX&				GetCompressionTransfromMatrix() { return m_ctMatrix; };
};

#endif