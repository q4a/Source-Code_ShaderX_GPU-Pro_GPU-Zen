#ifndef __MATRIX_H__
#define __MATRIX_H__
/**
Matrix class 
*/

#include <D3D9.h>

#ifdef max
#undef max
#endif

class Matrix
	{
	public:
	Matrix * next;								//for storing a linked list of these guys.

	static void startUp(IDirect3DDevice9 *, bool debug);
	static void shutDown();

	Matrix (char * name);						//initialized to 0x0 matrix.	Name is no longer than 100 chars.
	~Matrix();

	//internals:
	unsigned getNRows();
	unsigned getNCols();
	char * getName();

	void flush();								//make sure operation completed

	//assignment:
	void zeros(unsigned nrows, unsigned ncolumns);
	void identity(unsigned nrows);
	void randPD(Matrix & d, Matrix & v, unsigned size );	//generates a random PD matrix, extracts its LCP-prepared diagonal into d, and puts a random vector into v.
	void copy(Matrix & other);
	void loadFromFile(char * matFileName);
	void saveToFile(char * matFileName);

	//operations:
	float dot(Matrix & vec);					//same as this' * vec.  
	float normSquared();						//same as this' * this, but faster
	void multiply(Matrix & a, Matrix & b);		//this = A * B
	void multiplyAT(Matrix & a, Matrix & b);	//this = A' * B
	void multiply(float c);						//this *= c
	void add(Matrix & a, Matrix & b);			//this = A + B
	void add(Matrix & b);						//this += b
	void mad(Matrix & b, float c);				//this += b * c
	void mad(Matrix & a, Matrix & b, float c);	//this = a + b * c
	void mad(Matrix & a, Matrix & b);			//this = this + a .* b
	void madad(Matrix & a, Matrix & b, Matrix & c, Matrix & d); //this = a + (b + c) .* d
	void max(Matrix & a, float ref);			//this = max(a, ref)

	//experimental LCP ops:
	//void mcmad(Matrix & a, Matrix & b, Matrix & c, Matrix & d, Matrix &e);

	private:
	void resize(unsigned mrows, unsigned ncols);
	void debugDumpMatrix(double * data, const char * message1, const char * message2);


	void d3DfreeData();
	void d3DcreateData();
	void d3DloadDataRGRG(double * data);//not used
	void d3DsaveDataRGRG(double * data);//not used
	void d3DloadData(double * data);
	void d3DsaveData(double * data);
	//properties:
	unsigned nRows, nCols;						//the matrix' size

	//D3D implementation:
	IDirect3DSurface9 * mathSurface;			//the render target texture surface that stores our data.
	IDirect3DTexture9 * mathTexture;			//the corresponding texture interface

	//some strings:
	char name[100];								//

	//static D3D stuff:
	struct MathVertex
		{
		float x, y;
		};

    static IDirect3DDevice9 * d3dDevice;		        // The D3D rendering device
	enum VertexShaders { VS_UNTEXTURED_QUAD, VS_SINGLE_TEX_QUAD, VS_MULT_0, VS_MULT_1, VS_MULT_T, VS_NUM_SHADERS}; 
	enum PixelShaders  { PS_CONSTCOLOR, PS_COPY, PS_MAXCOPY, PS_SCALEDCOPY, PS_BLEND, PS_ADD, PS_SCALEDADD, PS_ADD_BLEND_ADD, PS_MULT_0, PS_MULT_T, PS_NUM_SHADERS};
	static IDirect3DVertexShader9 * vertexShaders[VS_NUM_SHADERS];
	static IDirect3DPixelShader9 * pixelShaders[PS_NUM_SHADERS];
	static IDirect3DVertexBuffer9* quadVertexBuffer;
	static IDirect3DVertexBuffer9* quadsVertexBuffer;
	static IDirect3DVertexDeclaration9 * vertexDeclaration;
	static IDirect3DVertexDeclaration9 * vertexDeclaration2;
	static bool debug;
	static 	D3DCAPS9 caps;

	//for rrrr format:
	static inline unsigned roundUpDivide     (unsigned a, unsigned b) { return (a + b-1) / b; }
	static inline unsigned rows2TextureHeight(unsigned rows) { return roundUpDivide(rows,4); }
	static inline unsigned cols2TextureWidth (unsigned cols) { return cols; }
	};


#endif //__MATRIX_H__