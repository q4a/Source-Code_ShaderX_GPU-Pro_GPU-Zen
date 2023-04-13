#include <assert.h>
#define STRICT
#include <windows.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include "dxutil.h"
#include <stdio.h>
#include "Matrix.h"
#include "Time.h"

#include <fstream>
#include <algorithm>
#include <functional>
#include <string>
#include <sstream>
#include <vector>

LPDIRECT3DDEVICE9 Matrix::d3dDevice = 0;

LPDIRECT3DVERTEXSHADER9 Matrix::vertexShaders[VS_NUM_SHADERS];
LPDIRECT3DPIXELSHADER9  Matrix::pixelShaders[PS_NUM_SHADERS];
IDirect3DVertexBuffer9* Matrix::quadVertexBuffer = 0;
IDirect3DVertexBuffer9* Matrix::quadsVertexBuffer = 0;
LPDIRECT3DVERTEXDECLARATION9 Matrix::vertexDeclaration = 0;
LPDIRECT3DVERTEXDECLARATION9 Matrix::vertexDeclaration2 = 0;
D3DCAPS9 Matrix::caps;

bool Matrix::debug = false;


//number of 4x4 * 4x1 matrix multiplies the pixel shaders will do (each samples the a 4 times and b 1 time).
//note: for small matrices large values are not optimal!
#define numMOpsPerFragment 6
#define numMTOpsPerFragment 4



void Matrix::startUp(LPDIRECT3DDEVICE9 d, bool debug_)
	{
	debug = debug_;
	d3dDevice = d;

	for (int i=0; i<VS_NUM_SHADERS; i++)
		{
		vertexShaders[i] = 0;
		}

		{
	for (int i=0; i<PS_NUM_SHADERS; i++)
		{
		pixelShaders[i] = 0;
		}

		}



	// Declare the vertex position and diffuse color data. (A triangle fan.)
	MathVertex quad[4]=		//at the same time clip space position and texture coordinate.
		{
		//  x        y
		{ -1.0f, -1.0f},	
		{ +1.0f, -1.0f}, 
		{ +1.0f, +1.0f}, 
		{ -1.0f, +1.0f}
		};
/*
trig 2
*-----* <vertex 1
    /
  / trig 1
/
*-----* <vertex 2
^
vertex 0

*/



	/*
		//  x        y
		{ -1.0f, -1.0f}, 
		{ +1.0f, -1.0f}, 
		{ +1.0f, +1.0f}, 
		{ -1.0f, +1.0f}

	*/


    // Create the vertex buffer
    if(FAILED(d3dDevice->CreateVertexBuffer( sizeof(quad), 0, 0, D3DPOOL_MANAGED, &quadVertexBuffer, NULL)))
		throw "can't create vertex buffer";

    // Fill the vertex buffer with triangles
    MathVertex* pVertices;

    if(FAILED(quadVertexBuffer->Lock(0,0,(void**)&pVertices, 0)))
		throw "can't lock vertex buffer";
        
	memcpy(pVertices, quad, sizeof(quad));

    quadVertexBuffer->Unlock();


	D3DVERTEXELEMENT9 decl[] = 
		{
		//stream 0:
			{ 
			0,					//stream number
			0,					//offset from start of stream to start of data
			D3DDECLTYPE_FLOAT2,	//data type
			D3DDECLMETHOD_DEFAULT, //what to do with data
			D3DDECLUSAGE_POSITION, //not sure why this still matters
			0					//more nonsense
			},
		//no more streams:
		D3DDECL_END()
		};


	
	if (FAILED(d3dDevice->CreateVertexDeclaration( decl, &vertexDeclaration )))
		throw "can't create declaration";

	if (FAILED(d3dDevice->SetVertexDeclaration( vertexDeclaration )))
		throw "can't set declaration";

    // Create a vertex buffer with a bunch of these quads for the mult code
	//this is a triangle list because sequential quads need different indices at every vertex.
	// x,y will be stored as: -1 == 0, 1 == 2

#define TINYVERTEX_SIZE 4

	/*
		//  x  y		quadIndex
		{ -1, -1,		0	}, 
		{ +1, -1,		0	}, 
		{ +1, +1,		0	}, 

		{ -1, -1,		0	}, 
		{ +1, +1,		0	}, 
		{ -1, +1,		0	},

		{ -1, -1,		1	}, 
		{ +1, -1,		1	}, 
		{ +1, +1,		1	}, 

		{ -1, -1,		1	}, 
		{ +1, +1,		1	}, 
		{ -1, +1,		1	},

	.... 

		{ -1, -1,		99	}, 
		{ +1, -1,		99	}, 
		{ +1, +1,		99	}, 

		{ -1, -1,		99	}, 
		{ +1, +1,		99	}, 
		{ -1, +1,		99	},

	We create 100 quads, each has 6 4-byte vertices for a total of 2400 bytes.
	With 1 mop this should be enough to multiply 400x400 matrices.
	With 4 mop this should be enough to multiply 1600x1600 matrices.
	With 6 mop this should be enough to multiply 2400x2400 matrices.
	*/
    if(FAILED(d3dDevice->CreateVertexBuffer( 4 * 6 * 100, 0, 0, D3DPOOL_MANAGED, &quadsVertexBuffer, NULL)))
		throw "can't create vertex buffer";

	DWORD * data;
    if(FAILED(quadsVertexBuffer->Lock(0,0,(void**)&data, 0)))
		throw "can't lock vertex buffer";
        
	//memcpy(pVertices, quad, sizeof(quad));
		{
	for (unsigned i = 0; i < 100; i++)
		{
		*data = (0) | (0 << 8) | (i << 24);		
		data++;
		*data = (2) | (0 << 8) | (i << 24);		
		data++;
		*data = (2) | (2 << 8) | (i << 24);		
		data++;

		*data = (0) | (0 << 8) | (i << 24);		
		data++;
		*data = (2) | (2 << 8) | (i << 24);		
		data++;
		*data = (0) | (2 << 8) | (i << 24);		
		data++;
		}
		}

    quadsVertexBuffer->Unlock();



	D3DVERTEXELEMENT9 decl2[] = 
		{
		//stream 0:
			{ 
			0,					//stream number
			0,					//offset from start of stream to start of data
			D3DDECLTYPE_UBYTE4,	//data type
			D3DDECLMETHOD_DEFAULT, //what to do with data
			D3DDECLUSAGE_POSITION, //not sure why this still matters
			0					//more nonsense
			},
		//no more streams:
		D3DDECL_END()
		};
/*
#define TINYVERTEX_SIZE 4*sizeof(float)
    if(FAILED(d3dDevice->CreateVertexBuffer( 4*4 * 6 * 100, 0, 0, D3DPOOL_MANAGED, &quadsVertexBuffer, NULL)))
		throw "can't create vertex buffer";

	float * data;
    if(FAILED(quadsVertexBuffer->Lock(0,0,(void**)&data, 0)))
		throw "can't lock vertex buffer";
        
		{
	for (unsigned i = 0; i < 100; i++)
		{
		*data = 0;	data++; *data = 0;	data++; *data = 0;	data++; *data = i;	data++;
		*data = 2;	data++; *data = 0;	data++; *data = 0;	data++; *data = i;	data++;
		*data = 2;	data++; *data = 2;	data++; *data = 0;	data++; *data = i;	data++;

		*data = 0;	data++; *data = 0;	data++; *data = 0;	data++; *data = i;	data++;
		*data = 2;	data++; *data = 2;	data++; *data = 0;	data++; *data = i;	data++;
		*data = 0;	data++; *data = 2;	data++; *data = 0;	data++; *data = i;	data++;
		}
		}

    quadsVertexBuffer->Unlock();



	D3DVERTEXELEMENT9 decl2[] = 
		{
		//stream 0:
			{ 
			0,					//stream number
			0,					//offset from start of stream to start of data
			D3DDECLTYPE_FLOAT4,	//data type
			D3DDECLMETHOD_DEFAULT, //what to do with data
			D3DDECLUSAGE_POSITION, //not sure why this still matters
			0					//more nonsense
			},
		//no more streams:
		D3DDECL_END()
		};
*/
	
	if (FAILED(d3dDevice->CreateVertexDeclaration( decl2, &vertexDeclaration2 )))
		throw "can't create declaration 2";



	char * shaderCodes[VS_NUM_SHADERS] =
		{
/*VS_UNTEXTURED_QUAD
	float consts[] = { 
		?, ?, 0.5, 1,
		};
*/
	"vs_1_1\n"              // version instruction
	"dcl_position v0\n"     // define position data in register v0
	"mov oPos, v0\n"   // transform vertices by view/projection matrix
	"mov oPos.zw, c0.zw\n"   //
	,

/*VS_SINGLE_TEX_QUAD
	float consts[] = { 
		0.5, -0.5, 0.5, 1,
		0.5+ TexcoordBiasW,  0.5 + TexcoordBiasH, 0  , 0
		};
*/

	"vs_1_1\n"              // version instruction
	"dcl_position v0\n"     // define position data in register v0
	"mov oPos, v0\n"   // transform vertices by view/projection matrix
	"mov oPos.zw, c0.zw\n"   //
	"mov r0, c1\n"			//prefix to next op because we can't use two const regs in 1 instr.
	"mad oT0.xy, v0.xy, c0.xy, r0.xy\n" //clipspace to texture coord mapping.  (invert y coordinate axis and scale [-1..1] to [0..1]
	,
/*
VS_MULT_0
    out: 
	pos
	tex0123 - texcoords for texture a, 1 texel apart along u.
	tex4 - texcoord for texture b

	float consts[] = { 
		0.5, -0.5, 0.5, 1,
		0.5 + TexcoordBiasW,  0.5 + TexcoordBiasH, constantForPassB  , 0
		constantForPassA0, constantForPassA1, constantForPassA2, constantForPassA3
		};
*/

	"vs_1_1\n"
	"dcl_position v0\n"
	"mov oPos, v0\n"			
	"mov oPos.zw, c0.zw\n"
	"mov r0.xy, c1.xy\n"
	"mad r0.xy, v0.xy, c0.xy, r0.xy\n"		
	//so far we had exactly the stuff in the above shader.
	//tex0 = [constantForPassA0, r0.y]
	//tex1 = [constantForPassA1, r0.y]
	//tex2 = [constantForPassA2, r0.y]
	//tex3 = [constantForPassA3, r0.y]
	//tex4 = [r0.x, constantForPassB]

	"mov oT0.x, c2.x\n"
	"mov oT1.x, c2.y\n"
	"mov oT2.x, c2.z\n"
	"mov oT3.x, c2.w\n"

	"mov oT0.y,  r0.y\n"
	"mov oT1.y,  r0.y\n"
	"mov oT2.y,  r0.y\n"
	"mov oT3.y,  r0.y\n"

	"mov oT4.x, r0.x\n"
	"mov oT4.y, c1.z\n"
	,
/*
VS_MULT_1	-- same as VS_MULT_0, but works with single vertex buffer, instead of separate drawPrim calls per quad.
	input:	[a, b, 0, index]	a and b are 0 or 2, subtract 1 to map to -+1, which is then the same as what the 
			above shader gets in v0 to begin with.  index is the pass number.  Because this time we don't get the
			pass constants set between passes, we need to compute them based on this index.
    out: 
	pos
	tex0123 - texcoords for texture a, 1 texel apart along u.
	tex4 - texcoord for texture b

 extern consts:

	c0 =	0.5, -0.5, 0.5, 1,
	c1 =	0.5 + TexcoordBiasW,  0.5 + TexcoordBiasH, 0 + TexcoordBiasBH, ?,
	c2 =	0 + TexcoordBiasAW, tcPixelAW + TexcoordBiasAW, 2 * tcPixelAW + TexcoordBiasAW, 3 * tcPixelAW + TexcoordBiasAW
	c3 =	tcPassIncrementBH, tcPassIncrementAW, 0, ?

 local consts: 
	c4

notes:
	c1.z += index * tcPassIncrementBH; 
	constantForPassB = c1

	c2 += index * tcPassIncrementAW; 
	constantForPassA = c2


*/

	"vs_1_1\n"
	"dcl_position v0\n"
	"def c4, -1, -1, 0, 0"
	"add r3.xy, v0.xy, c4.xy\n"	//map from [0,2] to [-1, 1]
	"mov oPos.xy, r3.xy\n"		//emit pos
	"mov oPos.zw, c0.zw\n"

	"mov r0.xy, c1.xy\n"		//transform viewport axes to tex uv axes
	"mad r0.xy, r3.xy, c0.xy, r0.xy\n"		

								//execute the += index * tcPassInc shown above in notes
	"mul r1, v0.w, c3.zzxz\n"	//can't use mad because it would reference 2 const regs in 1 instr
	"add r1, r1, c1\n"
	"mul r2, v0.w, c3.yyyy\n"
	"add r2, r2, c2\n"
								//emit texcoords
	"mov oT0.x, r2.x\n"
	"mov oT1.x, r2.y\n"
	"mov oT2.x, r2.z\n"
	"mov oT3.x, r2.w\n"

	"mov oT0.y,  r0.y\n"
	"mov oT1.y,  r0.y\n"
	"mov oT2.y,  r0.y\n"
	"mov oT3.y,  r0.y\n"

	"mov oT4.x, r0.x\n"
	"mov oT4.y, r1.z\n"
	,

/*VS_MULT_T

	input:	[a, b, 0, index]	a and b are 0 or 2, subtract 1 to map to -+1, which is then the same as what the 
			above shader gets in v0 to begin with.  index is the pass number.  Because this time we don't get the
			pass constants set between passes, we need to compute them based on this index.

extern consts:
	c0 = 0.5, -q, 0.5, 1,
	c1 = 0.5	+ TexcoordBiasW,  0.5 + TexcoordBiasH, 0 + TexcoordBiasBH, 0,
	c2 = 0   + TexcoordBiasAH, 0, 0, 0, 
	c3 = 0, q + TexcoordBiasAW, 0, 0
	c4 = tcPassIncrementBH, tcPassIncrementAH, 0, 0
local consts: 
	c5

  notes:
  		c1.z += index * tcPassIncrementBH; 

		c2.x += index * tcPassIncrementAH;

  */	
	"vs_1_1\n"
	"dcl_position v0\n"

	"def c5, -1, -1, 0, 0"
	"add r3.xy, v0.xy, c5.xy\n"	//map from [0,2] to [-1, 1]
	"mov oPos.xy, r3.xy\n"		//emit pos
	"mov oPos.zw, c0.zw\n"

	"mov r0.xy, c1.xy\n"		//transform viewport axes to tex uv axes
	"mad r0.xy, r3.xy, c0.xy, r0.xy\n"		

	"mov r1.xy, c3.xy\n"
	"mad r1.xy, r3.xy, c0.xy, r1.xy\n"	
	

								//execute the += index * tcPassInc shown above in notes
	"mul r3, v0.w, c4.zzxz\n"	//can't use mad because it would reference 2 const regs in 1 instr
	"add r3, r3, c1\n"
	"mul r2, v0.w, c4.yyyy\n"
	"add r2, r2, c2\n"


	"mov oT0.x, r1.y\n"
	"mov oT0.y, r2.x\n"
	"mov oT1.x, r0.x\n"
	"mov oT1.y, r3.z\n"

/*
	"vs_1_1\n"
	"dcl_position v0\n"
	"mov oPos, v0\n"			
	"mov oPos.zw, c0.zw\n"

	"mov r0.xy, c1.xy\n"
	"mad r0.xy, v0.xy, c0.xy, r0.xy\n"		

	"mov r1.xy, c3.xy\n"
	"mad r1.xy, v0.xy, c0.xy, r1.xy\n"		

	"mov oT0.x, r1.y\n"
	"mov oT0.y, c2.x\n"
	"mov oT1.x, r0.x\n"
	"mov oT1.y, c1.z\n"

*/
		}
	;

	
	


	LPD3DXBUFFER pCode = 0;                  // Buffer with the assembled shader code
	LPD3DXBUFFER pErrorMsgs = 0;             // Buffer with error messages

	{
	for (int i=0; i<VS_NUM_SHADERS; i++)
		{
		if (FAILED(D3DXAssembleShader(shaderCodes[i], strlen(shaderCodes[i]), 0, 0, 0, &pCode, &pErrorMsgs)))
			{
			const char * err = pErrorMsgs ? (const char *)pErrorMsgs->GetBufferPointer() : 0;
			throw "can't assemble vshader";
			}

		if (FAILED(d3dDevice->CreateVertexShader((DWORD*)pCode->GetBufferPointer(), &(vertexShaders[i]))))
			throw "can't create vshader";
		SAFE_RELEASE(pCode);
		SAFE_RELEASE(pErrorMsgs);
		}
	}


	char * pixelShaderCodes[PS_NUM_SHADERS] = {
//PS_CONSTCOLOR		out = c0
	"ps_2_0\n"              // Pixel shader version. NOTE: versions 1_1 clamp to [0,1] on output, which is why these are recoded as 2_0
	"mov oC0, c0\n"		//texld can't write this directly
	,
//PS_COPY			out = tex
	"ps_2_0\n"
	"dcl_2d s0\n"
	"dcl t0\n"
	"texld r0, t0, s0\n"              // Texture register t0 is loaded from stage 0.
	"mov oC0, r0\n"		//texld can't write this directly

/*
	"ps_1_1\n"              // Pixel shader version.
	"tex t0\n"              // Texture register t0 is loaded from stage 0.
	"mov r0, t0\n"			// copy texture.
*/
	,
//PS_MAXCOPY		//out = max(c0, tex)
	"ps_2_0\n"
	"dcl_2d s0\n"
	"dcl t0\n"
	"texld r0, t0, s0\n"
	"max r0, r0, c0\n"
	"mov oC0, r0\n"
	,
//PS_SCALEDCOPY				// out = tex * c0
	"ps_2_0\n"
	"dcl_2d s0\n"
	"dcl t0\n"
	"texld r0, t0, s0\n"              // Texture register t0 is loaded from stage 0.
	"mul r0, r0, c0\n"
	"mov oC0, r0\n"
	,
//PS_BLEND			//out = tex0 * tex1
	"ps_2_0\n"
//	"def c0, 0, 0, 0, 0\n"
	"dcl_2d s0\n"
	"dcl_2d s1\n"
	"dcl t0\n"
	"texld r0, t0, s0\n"
	"texld r1, t0, s1\n"
	"mul r0, r0, r1\n"
//	"max r0, r0, c0\n"		//c0 is zero.
	"mov oC0, r0\n"
	,
//PS_ADD				//out = tex0 + tex1
	"ps_2_0\n"              
	"dcl_2d s0\n"
	"dcl_2d s1\n"
	"dcl t0\n"
	"texld r0, t0, s0\n"
	"texld r1, t0, s1\n"
	//"mov r0, t1\n"
	"add r0, r0, r1\n"//"add r0, t0, r0\n"		
	"mov oC0, r0\n"
	,
//PS_SCALEDADD		out = tex0 + tex1 * c0
	"ps_2_0\n"   
	"dcl_2d s0\n"
	"dcl_2d s1\n"
	"dcl t0\n"
	"texld r0, t0, s0\n"
	"texld r1, t0, s1\n"
	"mad r0, r1, c0, r0\n"
	"mov oC0, r0\n"
	,
//PS_ADD_BLEND_ADD		out =  tex0 + (tex1 + tex2) * tex3
	"ps_2_0\n"   
	"dcl_2d s0\n"
	"dcl_2d s1\n"
	"dcl_2d s2\n"
	"dcl_2d s3\n"
	"dcl t0\n"
	"texld r0, t0, s0\n"
	"texld r1, t0, s1\n"
	"texld r2, t0, s2\n"
	"texld r3, t0, s3\n"
	"add r1, r1, r2\n"
	"mul r1, r1, r3\n"
	"add r0, r0, r1\n"
	"mov oC0, r0\n"
,
//PS_MULT_0

#if 0
	"ps_2_0\n"
	"dcl_2d s0\n"
	"dcl_2d s1\n"
//	"dcl_2d s2\n"
//	"dcl_2d s3\n"
	"dcl t0\n"
	"dcl t1\n"
	"dcl t2\n"
	"dcl t3\n"
	
	"texld r0, t0, s0\n"//"tex t0\n"
	"texld r1, t1, s1\n"//"tex t1\n"
	"texld r2, t2, s0\n"//"tex t2\n"
	"texld r3, t3, s1\n"//"tex t3\n"
	/*
	"mul r0, r0, r1\n"//"mul r0, t0, t1\n"
	"mad r0, r2, r3, r0\n"//"mad r0, t2, t3, r0\n"
	*/

	//perform two 2x2 submatrix multiplies, and add the results:	
	"mul r4.x, r0.x, r1.x\n"
	"mad r4.x, r0.y, r1.z, r4.x\n"
	
	"mul r4.y, r0.x, r1.y\n"
	"mad r4.y, r0.y, r1.w, r4.y\n"

	"mul r4.z, r0.z, r1.x\n"
	"mad r4.z, r0.w, r1.z, r4.z\n"

	"mul r4.w, r0.z, r1.y\n"
	"mad r4.w, r0.w, r1.w, r4.w\n"

	//second 2x2 matrix multiply, summed with first:
	"mad r4.x, r2.x, r3.x, r4.x\n"
	"mad r4.x, r2.y, r3.z, r4.x\n"
	
	"mad r4.y, r2.x, r3.y, r4.y\n"
	"mad r4.y, r2.y, r3.w, r4.y\n"

	"mad r4.z, r2.z, r3.x, r4.z\n"
	"mad r4.z, r2.w, r3.z, r4.z\n"

	"mad r4.w, r2.z, r3.y, r4.w\n"
	"mad r4.w, r2.w, r3.w, r4.w\n"

	"mov oC0, r4\n"	

//	"mov r0, t0\n"	//DEBUG
//	"mov oC0, r0\n"
#else	//mult53.phl, multiply with formats rrrr x rrrr -> rrrr

//	tex0123 - texcoords for texture a, 1 texel apart along u.
//	tex4 - texcoord for texture b

//c0.xy [tex0123.x 4 pixel increment, 0]
//c1.xy [0, tex4.y 1 pixel increment]
//c2 = c0 * 2
//c3 = c1 * 2
//c4 = c0 * 3
//c5 = c1 * 3
//c6 = c0 * 4
//c7 = c1 * 4
//...
    "ps_2_0\n"
    "dcl t0.xyzw\n"	//TODO: its really stupid that I'm forced to declare all 4 compos, not just xy, but otherwise adds fail below because there is no xy swizzle.
    "dcl t1.xyzw\n"
    "dcl t2.xyzw\n"
    "dcl t3.xyzw\n"
    "dcl t4.xyzw\n"
    "dcl_2d s0\n"
    "dcl_2d s1\n"
    "texld r0, t0, s0\n"
    "texld r1, t1, s0\n"
    "texld r2, t2, s0\n"
    "texld r3, t3, s0\n"
    "texld r4, t4, s1\n"

    "mul r5, r4.xxxx, r0\n"
    "mad r5, r4.yyyy, r1, r5\n"
    "mad r5, r4.zzzz, r2, r5\n"
    "mad r5, r4.wwww, r3, r5\n"
#if numMOpsPerFragment >= 2
///Mop start
	"add r6, t0, c0\n"
	"add r7, t1, c0\n"
	"add r8, t2, c0\n"
	"add r9, t3, c0\n"
	"add r10, t4, c1\n"

    "texld r6, r6, s0\n"
    "texld r7, r7, s0\n"
    "texld r8, r8, s0\n"
    "texld r9, r9, s0\n"
    "texld r10, r10, s1\n"

    "mad r5, r10.xxxx, r6, r5\n"
    "mad r5, r10.yyyy, r7, r5\n"
    "mad r5, r10.zzzz, r8, r5\n"
    "mad r5, r10.wwww, r9, r5\n"
///Mop end
#endif
#if numMOpsPerFragment >= 3
///Mop start
	"add r0, t0, c2\n"
	"add r1, t1, c2\n"
	"add r2, t2, c2\n"
	"add r3, t3, c2\n"
	"add r4, t4, c3\n"

    "texld r0, r0, s0\n"
    "texld r1, r1, s0\n"
    "texld r2, r2, s0\n"
    "texld r3, r3, s0\n"
    "texld r4, r4, s1\n"

    "mad r5, r4.xxxx, r0, r5\n"
    "mad r5, r4.yyyy, r1, r5\n"
    "mad r5, r4.zzzz, r2, r5\n"
    "mad r5, r4.wwww, r3, r5\n"
///Mop end
#endif
#if numMOpsPerFragment >= 4
///Mop start
	"add r6, t0, c4\n"
	"add r7, t1, c4\n"
	"add r8, t2, c4\n"
	"add r9, t3, c4\n"
	"add r10, t4, c5\n"

    "texld r6, r6, s0\n"
    "texld r7, r7, s0\n"
    "texld r8, r8, s0\n"
    "texld r9, r9, s0\n"
    "texld r10, r10, s1\n"

    "mad r5, r10.xxxx, r6, r5\n"
    "mad r5, r10.yyyy, r7, r5\n"
    "mad r5, r10.zzzz, r8, r5\n"
    "mad r5, r10.wwww, r9, r5\n"
///Mop end
#endif
#if numMOpsPerFragment >= 5
///Mop start
	"add r0, t0, c6\n"
	"add r1, t1, c6\n"
	"add r2, t2, c6\n"
	"add r3, t3, c6\n"
	"add r4, t4, c7\n"

    "texld r0, r0, s0\n"
    "texld r1, r1, s0\n"
    "texld r2, r2, s0\n"
    "texld r3, r3, s0\n"
    "texld r4, r4, s1\n"

    "mad r5, r4.xxxx, r0, r5\n"
    "mad r5, r4.yyyy, r1, r5\n"
    "mad r5, r4.zzzz, r2, r5\n"
    "mad r5, r4.wwww, r3, r5\n"
///Mop end
#endif
#if numMOpsPerFragment >= 6
///Mop start
	"add r6, t0, c8\n"
	"add r7, t1, c8\n"
	"add r8, t2, c8\n"
	"add r9, t3, c8\n"
	"add r10, t4, c9\n"

    "texld r6, r6, s0\n"
    "texld r7, r7, s0\n"
    "texld r8, r8, s0\n"
    "texld r9, r9, s0\n"
    "texld r10, r10, s1\n"

    "mad r5, r10.xxxx, r6, r5\n"
    "mad r5, r10.yyyy, r7, r5\n"
    "mad r5, r10.zzzz, r8, r5\n"
    "mad r5, r10.wwww, r9, r5\n"
///Mop end
#endif
//here we have 2 more tex* instructions left, but no decent way to use them, we have 30 so far.
    "mov oC0, r5\n"
#endif
	,
/*
    "ps_2_0\n"
    "dcl t0.xyzw\n"
    "dcl t1.xyzw\n"
	"mov r5, c3\n"
    "mov oC0, r5\n"
*/
//PS_MULT_T
    "ps_2_0\n"
    "dcl t0.xyzw\n"	//TODO: its really stupid that I'm forced to declare all 4 compos, not just xy, but otherwise adds fail below because there is no xy swizzle.
    "dcl t1.xyzw\n"
    "dcl_2d s0\n"
    "dcl_2d s1\n"
    "texld r0, t0, s0\n"	//0th order
	"add   r4, t0, c0\n"
    "texld r1, r4, s0\n"	//1st order (because it reads r?)
	"add   r4, r4, c0\n"
    "texld r2, r4, s0\n"	//1st order
	"add   r4, r4, c0\n"
    "texld r3, r4, s0\n"	//1st order

    "texld r4, t1, s1\n"

    "dp4 r5.x, r4, r0\n"
    "dp4 r5.y, r4, r1\n"
    "dp4 r5.z, r4, r2\n"
    "dp4 r5.w, r4, r3\n"
#if numMTOpsPerFragment >= 2
///Mop start
	"add   r0, t0, c2\n"
    "texld r6, r0, s0\n"	//0th order
	"add   r0, r0, c0\n"
    "texld r7, r0, s0\n"	//1st order (because it reads r?)
	"add   r0, r0, c0\n"
    "texld r8, r0, s0\n"	//1st order
	"add   r0, r0, c0\n"
    "texld r9, r0, s0\n"	//1st order

	"add r1, t1, c1\n"
    "texld r10, r1, s1\n"	//1st order


    "dp4 r6.x, r10, r6\n"
    "dp4 r6.y, r10, r7\n"
    "dp4 r6.z, r10, r8\n"
    "dp4 r6.w, r10, r9\n"
	"add r5, r5, r6\n"
///Mop end
#endif
#if numMTOpsPerFragment >= 3
///Mop start
	"add   r4, t0, c4\n"
    "texld r0, r4, s0\n"	//1st order I think, unless r0 counts as being determined by 1st order texop, which is not the case.
	"add   r4, r4, c0\n"
    "texld r1, r4, s0\n"	//1st order
	"add   r4, r4, c0\n"
    "texld r2, r4, s0\n"	//1st order
	"add   r4, r4, c0\n"
    "texld r3, r4, s0\n"	//1st order

	"add r4, t1, c3\n"
    "texld r4, r4, s1\n"	//1st order


    "dp4 r6.x, r4, r0\n"
    "dp4 r6.y, r4, r1\n"
    "dp4 r6.z, r4, r2\n"
    "dp4 r6.w, r4, r3\n"
	"add r5, r5, r6\n"
///Mop end
#endif
#if numMTOpsPerFragment >= 4
///Mop start
	"add   r0, t0, c6\n"
    "texld r6, r0, s0\n"
	"add   r0, r0, c0\n"
    "texld r7, r0, s0\n"
	"add   r0, r0, c0\n"
    "texld r8, r0, s0\n"
	"add   r0, r0, c0\n"
    "texld r9, r0, s0\n"

	"add r1, t1, c5\n"
    "texld r10, r1, s1\n"


    "dp4 r6.x, r10, r6\n"
    "dp4 r6.y, r10, r7\n"
    "dp4 r6.z, r10, r8\n"
    "dp4 r6.w, r10, r9\n"
	"add r5, r5, r6\n"
///Mop end
#endif
#if numMTOpsPerFragment >= 5
///Mop start
	"add   r7, t0, c8\n"
    "texld r0, r7, s0\n"	//4th order texop error here!
	"add   r7, r7, c0\n"
    "texld r1, r7, s0\n"
	"add   r7, r7, c0\n"
    "texld r2, r7, s0\n"
	"add   r7, r7, c0\n"
    "texld r3, r7, s0\n"

	"add r4, t1, c7\n"
    "texld r4, r4, s1\n"


    "dp4 r6.x, r4, r0\n"
    "dp4 r6.y, r4, r1\n"
    "dp4 r6.z, r4, r2\n"
    "dp4 r6.w, r4, r3\n"
	"add r5, r5, r6\n"
///Mop end
#endif
#if numMTOpsPerFragment >= 6
///Mop start
	"add   r7, t0, c2\n"
    "texld r0, r7, s0\n"
	"add   r7, r7, c0\n"
    "texld r1, r7, s0\n"
	"add   r7, r7, c0\n"
    "texld r2, r7, s0\n"
	"add   r7, r7, c0\n"
    "texld r3, r7, s0\n"

	"add r4, t1, c1\n"
    "texld r4, r4, s1\n"


    "dp4 r6.x, r4, r0\n"
    "dp4 r6.y, r4, r1\n"
    "dp4 r6.z, r4, r2\n"
    "dp4 r6.w, r4, r3\n"
	"add r5, r5, r6\n"
///Mop end
#endif
    "mov oC0, r5\n"




		};


	{
	for (int i=0; i<PS_NUM_SHADERS; i++)
		{
		if (FAILED(D3DXAssembleShader(pixelShaderCodes[i], strlen(pixelShaderCodes[i]), 0, 0, 0, &pCode, &pErrorMsgs)))
			{
			const char * err = pErrorMsgs ? (const char *)pErrorMsgs->GetBufferPointer() : 0;
			throw "can't assemble pshader";
			}

		if (FAILED(d3dDevice->CreatePixelShader((DWORD*)pCode->GetBufferPointer(), &(pixelShaders[i]))))
			throw "can't create pshader";

		SAFE_RELEASE(pCode);
		SAFE_RELEASE(pErrorMsgs);
		}
	}


    d3dDevice->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
	d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	d3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE );
	d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	d3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	d3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	//we will be using additive blending:
	d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	d3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);

	//get number of samplers:

	d3dDevice->GetDeviceCaps(&caps);

	//so out of bounds texcoords end up being sampled as zero.  This is so we can work with fixed size submatrices, even if a matrix isn't an exact multiple of this size. 

	if (caps.TextureAddressCaps & D3DPTADDRESSCAPS_BORDER)
		{
		for (unsigned i=0; i<caps.MaxSimultaneousTextures; i++)
			{
			//Note: ATI Radeon >=9500 doesn't support border mode!
			//mirroronce is not good enough!!
			d3dDevice->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
			d3dDevice->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
			}
		}
	else
		{
		printf("Warning: Your graphics card (like my Radeon 9500) doesn't support D3DTADDRESS_BORDER, which would be nice to have. "
			"Performance timings will still be representative, you will however get incorrect matrix multiply results unless you carefully choose your matrix dimensions (see paper).\n\n");
		}
	}


void Matrix::shutDown()
	{
	for (int i=0; i<VS_NUM_SHADERS; i++)
		{
		SAFE_RELEASE(vertexShaders[i]);
		}
		{
	for (int i=0; i<PS_NUM_SHADERS; i++)
		{
		SAFE_RELEASE(pixelShaders[i]);
		}
		}


	SAFE_RELEASE(quadVertexBuffer);
	SAFE_RELEASE(quadsVertexBuffer);
	SAFE_RELEASE(vertexDeclaration);
	SAFE_RELEASE(vertexDeclaration2);
	}

Matrix::Matrix (char * name_)
	{
	strcpy(name, name_);	//Note: we assume name is less than 100 chars.
	next = 0;
	nRows = 0;
	nCols = 0;
	mathSurface = 0;
	mathTexture = 0;
	}

Matrix::~Matrix()
	{
	//release all our stuff
	d3DfreeData();
	}

unsigned Matrix::getNRows()
	{
	return nRows;
	}

unsigned Matrix::getNCols()
	{
	return nCols;
	}

char * Matrix::getName()
	{
	return name;
	}

void Matrix::resize(unsigned mrows, unsigned ncols)
	{
	if ((unsigned)mrows != nRows || (unsigned)ncols != nCols)
		{
		d3DfreeData();
		nRows = mrows;
		nCols = ncols;
		if (mrows * ncols > 0)
			d3DcreateData();
		}
	}

void Matrix::zeros(unsigned mrows, unsigned ncols)
	{
	//resize matrix if needed:
	resize(mrows, ncols);
	if (mrows * ncols == 0)
		return;

#ifdef ZERO_ON_CPU
	double * data = new double[mrows * ncols];	//column major data.
	memset(data, 0, mrows * ncols * sizeof(double));	//we could write the zeros directly into the texture, but this way we can reuse the d3DloadData() method below.
	//transfer the data:
	d3DloadData(data);
	delete data;
#else
	if (FAILED(d3dDevice->SetRenderTarget(0,mathSurface)))
		throw "can't set rendertarget";
	d3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0, 0, 0 );
	

#endif
	}

void Matrix::identity(unsigned mrows)
	{
	unsigned ncols = mrows;

	resize(mrows, ncols);
	if (mrows * ncols == 0)
		return;


	double * data = new double[mrows * ncols];	//column major data.
	memset(data, 0, mrows * ncols * sizeof(double));	//we could write the zeros directly into the texture, but this way we can reuse the d3DloadData() method below.

	//set diagonal:
	for (unsigned i = 0; i<mrows; i++)
		data[i*mrows + i] = 1.0;

	//transfer the data:
	d3DloadData(data);

	delete data;
	}

void Matrix::randPD(Matrix & d, Matrix & v, unsigned mrows )
//the product of a random matrix and its transpose should be PD.
	{
	struct F
		{
		static inline double rand(double a, double b)
			{
			const double r = (double)::rand()/((double)RAND_MAX+1);
			return r*(b-a) + a;
			}
		};

	if (mrows == 0)
		return;

	unsigned i;
	unsigned ncols = mrows;
	resize(mrows, ncols);
	d.resize(ncols, 1);
	v.resize(ncols, 1);


	unsigned count = mrows * ncols;

	double * rmatrix = new double[count];
	double * pmatrix = new double[count];	//rmatrix * rmatrix'
	double * rvector = new double[ncols];	//column major data.

	for (i=0; i < count; i++)
		{
		rmatrix[i] = F::rand(-1, 1);
		}

	for (i=0; i < ncols; i++)
		{
		rvector[i] = F::rand(-1, 1);
		}
	v.d3DloadData(rvector);

	//pmatrix = rmatrix * rmatrix'
	//we do this multiply in software because we needed this code to produce us good matrices to help write the hardware implementation. 
	//once hw works we could do this in hw too.
	for (unsigned y=0; y < ncols; y++)
		{
		unsigned ycols = ncols * y;
		for (unsigned x=0; x < ncols; x++)
			{
			unsigned xcols = ncols * x; 
			double sum = 0;
			for (i=0; i < ncols; i++)
				{
				sum += rmatrix[i + xcols] * rmatrix[i + ycols];
				}
			pmatrix[x + ycols] = sum;
			}
		}
	for (i=0; i < ncols; i++)
		{
		rvector[i] = -0.85 / pmatrix[i + ncols * i];
		}


	d3DloadData(pmatrix);



	d.d3DloadData(rvector);


	delete rmatrix;
	delete pmatrix;
	delete rvector;


	}

void Matrix::copy(Matrix & other)
	{
	if (this == &other)
		return;	//this is a nop.

	resize(other.getNRows(), other.getNCols());
	if (nRows * nCols == 0)
		return;


	if (FAILED(d3dDevice->SetRenderTarget(0,mathSurface)))
		throw "can't set rendertarget";

    // Begin the scene
    if(FAILED( d3dDevice->BeginScene()))
		throw "can't begin scene";

	//2) set an appropriate vertex program.  
	if(FAILED( d3dDevice->SetVertexShader( vertexShaders[VS_SINGLE_TEX_QUAD] )))
		throw "can't set vshader";
	//3) set a pixel program
	//we want:  out = tex[0] + tex[1].
	if(FAILED( d3dDevice->SetPixelShader( pixelShaders[PS_COPY] )))
		throw "can't set pshader";

	//4) set the textures
	if (FAILED(d3dDevice->SetTexture(0,other.mathTexture)))
		throw "can't set texture 0";
	if (FAILED(d3dDevice->SetTexture(1,0)))
		throw "can't clear texture 1";

	//5) render some geometry.  We will render two quads which are [0,1]x[0,1] in texture space, and aligned with the rendertarget.
    if(FAILED( d3dDevice->SetStreamSource( 0, quadVertexBuffer, 0, sizeof(MathVertex) )))
		throw "can't set stream";

	//gotta bias the texcoords by 1/2 pixel so the texel and pixel centers line up.
	float TexcoordBiasW = (1.0f/cols2TextureWidth(nCols))  * 0.5f;
	float TexcoordBiasH = (1.0f/rows2TextureHeight(nRows)) * 0.5f;


	float consts[4 * 2] = { 
		0.5, -0.5, 0.5, 1,
		0.5+ TexcoordBiasW,  0.5 + TexcoordBiasH, 0  , 0
		};

	if(FAILED( d3dDevice->SetVertexShaderConstantF(0, consts, 2)))
		throw "can't set vs consts";

    if(FAILED( d3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 )))
		throw "can't draw prims";

    // End the scene.
    d3dDevice->EndScene();
	}

void Matrix::max(Matrix & other, float c)			//this = max(other, c)
	{
	if (this == &other)
		throw "can't do max inplace!";

	resize(other.getNRows(), other.getNCols());
	if (nRows * nCols == 0)
		return;


	if (FAILED(d3dDevice->SetRenderTarget(0,mathSurface)))
		throw "can't set rendertarget";

    // Begin the scene
    if(FAILED( d3dDevice->BeginScene()))
		throw "can't begin scene";

	//2) set an appropriate vertex program.  
	if(FAILED( d3dDevice->SetVertexShader( vertexShaders[VS_SINGLE_TEX_QUAD] )))
		throw "can't set vshader";
	//3) set a pixel program
	//we want:  out = tex[0] + tex[1].
	if(FAILED( d3dDevice->SetPixelShader( pixelShaders[PS_MAXCOPY] )))
		throw "can't set pshader";

	//4) set the textures
	if (FAILED(d3dDevice->SetTexture(0,other.mathTexture)))
		throw "can't set texture 0";
	if (FAILED(d3dDevice->SetTexture(1,0)))
		throw "can't clear texture 1";

	//5) render some geometry.  We will render two quads which are [0,1]x[0,1] in texture space, and aligned with the rendertarget.
    if(FAILED( d3dDevice->SetStreamSource( 0, quadVertexBuffer, 0, sizeof(MathVertex) )))
		throw "can't set stream";

	//gotta bias the texcoords by 1/2 pixel so the texel and pixel centers line up.
	float TexcoordBiasW = (1.0f/cols2TextureWidth(nCols))  * 0.5f;
	float TexcoordBiasH = (1.0f/rows2TextureHeight(nRows)) * 0.5f;


	float consts[4 * 2] = { 
		0.5, -0.5, 0.5, 1,
		0.5+ TexcoordBiasW,  0.5 + TexcoordBiasH, 0  , 0
		};

	if(FAILED( d3dDevice->SetVertexShaderConstantF(0, consts, 2)))
		throw "can't set vs consts";

	float pconsts[] = { 
		c, c, c,c	//constant color
		};

	if(FAILED( d3dDevice->SetPixelShaderConstantF(0, pconsts, 1)))
		throw "can't set ps consts";


    if(FAILED( d3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 )))
		throw "can't draw prims";

    // End the scene.
    d3dDevice->EndScene();
	}

void Matrix::add(Matrix & a, Matrix & b)//this = A + B		Note: A and B may be the same object.
	{
	if (this == &a)		
		{
		add(b);
		return;
		}
	else if (this == &b)
		{
		add(a);
		return;
		}

	//here we just do a regular additive blending of the two textures, no need to multipass

	//check if dimensions agree:

	if (a.nRows != b.nRows || a.nCols != b.nCols)
		throw "matrix dimensions don't agree";

	resize(a.nRows, a.nCols);
	if (a.nRows * a.nCols == 0)
		return;


	//1) set this as render target
	if (FAILED(d3dDevice->SetRenderTarget(0,mathSurface)))
		throw "can't set rendertarget";

    // Begin the scene
    if(FAILED( d3dDevice->BeginScene()))
		throw "can't begin scene";

	//2) set an appropriate vertex program.  
	if(FAILED( d3dDevice->SetVertexShader( vertexShaders[VS_SINGLE_TEX_QUAD] )))
		throw "can't set vshader";
	//3) set a pixel program
	//we want:  out = tex[0] + tex[1].
	if(FAILED( d3dDevice->SetPixelShader( pixelShaders[PS_ADD] )))
		throw "can't set pshader";

	//4) set the textures
	if (FAILED(d3dDevice->SetTexture(0,a.mathTexture)))
		throw "can't set texture 0";
	if (FAILED(d3dDevice->SetTexture(1,b.mathTexture)))
		throw "can't set texture 1";

	//5) render some geometry.  We will render one quad of [0,1]x[0,1] in texture space, and aligned with the rendertarget.
    if(FAILED( d3dDevice->SetStreamSource( 0, quadVertexBuffer, 0, sizeof(MathVertex) )))
		throw "can't set stream";

	//gotta bias the texcoords by 1/2 pixel so the texel and pixel centers line up.
	float TexcoordBiasW = (1.0f/cols2TextureWidth(nCols))  * 0.5f;
	float TexcoordBiasH = (1.0f/rows2TextureHeight(nRows)) * 0.5f;


	float consts[4 * 2] = { 
		0.5, -0.5, 0.5, 1,
		0.5+ TexcoordBiasW,  0.5 + TexcoordBiasH, 0  , 0
		};

	if(FAILED( d3dDevice->SetVertexShaderConstantF(0, consts, 2)))
		throw "can't set vs consts";


    if(FAILED( d3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 )))
		throw "can't draw prims";


    // End the scene.
    d3dDevice->EndScene();
	}

void Matrix::multiply(float c)						//this *= c
	{
	if (nRows == 0 || nCols == 0)
		return;

	//multiplicative blend a 2.0f colored quad.

	//1) set this as render target
	if (FAILED(d3dDevice->SetRenderTarget(0,mathSurface)))
		throw "can't set rendertarget";

    // Begin the scene
    if(FAILED( d3dDevice->BeginScene()))
		throw "can't begin scene";

	//2) set an appropriate vertex program.  
	if(FAILED( d3dDevice->SetVertexShader( vertexShaders[VS_UNTEXTURED_QUAD] )))
		throw "can't set vshader";
	//3) set a pixel program
	//we want:  out = tex[0] + tex[1].
	if(FAILED( d3dDevice->SetPixelShader( pixelShaders[PS_CONSTCOLOR] )))
		throw "can't set pshader";

	//5) render some geometry.  We will render one quad of [0,1]x[0,1] in texture space, and aligned with the rendertarget.
    if(FAILED( d3dDevice->SetStreamSource( 0, quadVertexBuffer, 0, sizeof(MathVertex) )))
		throw "can't set stream";

	//gotta bias the texcoords by 1/2 pixel so the texel and pixel centers line up.
	float TexcoordBiasW = (1.0f/cols2TextureWidth(nCols))  * 0.5f;
	float TexcoordBiasH = (1.0f/rows2TextureHeight(nRows)) * 0.5f;


	float consts[] = { 
		0, 0, 0.5, 1,
		};

	if(FAILED( d3dDevice->SetVertexShaderConstantF(0, consts, 1)))
		throw "can't set vs consts";

	float pconsts[] = { 
		c, c, c,c	//constant color
		};

	if(FAILED( d3dDevice->SetPixelShaderConstantF(0, pconsts, 1)))
		throw "can't set ps consts";


    d3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );		//enable additive additive blending

	//FinalColor = TexelColor × SourceBlendFactor + PixelColor × DestBlendFactor
	//we can do either of:
	//FinalColor = TexelColor × D3DBLEND_DESTCOLOR + PixelColor × D3DBLEND_ZERO
	//FinalColor = TexelColor × D3DBLEND_ZERO + PixelColor × D3DBLEND_SRCCOLOR
	d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
	d3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ZERO);


    if(FAILED( d3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 )))
		throw "can't draw prims";

    // End the scene.
    d3dDevice->EndScene();
	d3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
	//reset to default of additive blending:
	d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	d3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
	}

void Matrix::add(Matrix & b)				//this += b
	{
	if (this == &b)
		{
		multiply(2.0f);
		return;
		}

	//here we just do a regular additive blending of the two textures, no need to multitexture

	//check if dimensions agree:

	if (nRows != b.nRows || nCols != b.nCols)
		throw "matrix dimensions don't agree";

	//1) set this as render target
	if (FAILED(d3dDevice->SetRenderTarget(0,mathSurface)))
		throw "can't set rendertarget";

    // Begin the scene
    if(FAILED( d3dDevice->BeginScene()))
		throw "can't begin scene";

	//2) set an appropriate vertex program.  
	if(FAILED( d3dDevice->SetVertexShader( vertexShaders[VS_SINGLE_TEX_QUAD] )))
		throw "can't set vshader";
	//3) set a pixel program
	//we want:  out = tex[0]
	if(FAILED( d3dDevice->SetPixelShader( pixelShaders[PS_COPY] )))
		throw "can't set pshader";

	//4) set the textures
	if (FAILED(d3dDevice->SetTexture(0,b.mathTexture)))
		throw "can't set texture 0";
	if (FAILED(d3dDevice->SetTexture(1,0)))
		throw "can't clear texture 1";

	//5) render some geometry.  We will render one quad of [0,1]x[0,1] in texture space, and aligned with the rendertarget.
    if(FAILED( d3dDevice->SetStreamSource( 0, quadVertexBuffer, 0, sizeof(MathVertex) )))
		throw "can't set stream";

	//gotta bias the texcoords by 1/2 pixel so the texel and pixel centers line up.
	float TexcoordBiasW = (1.0f/cols2TextureWidth(nCols))  * 0.5f;
	float TexcoordBiasH = (1.0f/rows2TextureHeight(nRows)) * 0.5f;


	float consts[4 * 2] = { 
		0.5, -0.5, 0.5, 1,
		0.5+ TexcoordBiasW,  0.5 + TexcoordBiasH, 0  , 0
		};

	if(FAILED( d3dDevice->SetVertexShaderConstantF(0, consts, 2)))
		throw "can't set vs consts";

    d3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );		//enable additive additive blending

    if(FAILED( d3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 )))
		throw "can't draw prims";


    // End the scene.
    d3dDevice->EndScene();
	d3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
	}

void Matrix::mad(Matrix & b, float c)				//this += b * c
	{
	if (this == &b)
		throw "can't operate inplace -- not supported by D3D.";	//works on the refrast though!


	//here we just do a regular additive blending of the two textures, no need to multitexture

	//check if dimensions agree:

	if (nRows != b.nRows || nCols != b.nCols)
		throw "matrix dimensions don't agree";

	//1) set this as render target
	if (FAILED(d3dDevice->SetRenderTarget(0,mathSurface)))
		throw "can't set rendertarget";

    // Begin the scene
    if(FAILED( d3dDevice->BeginScene()))
		throw "can't begin scene";

	//2) set an appropriate vertex program.  
	if(FAILED( d3dDevice->SetVertexShader( vertexShaders[VS_SINGLE_TEX_QUAD] )))
		throw "can't set vshader";
	//3) set a pixel program
	//we want:  out = tex[0] * c
	if(FAILED( d3dDevice->SetPixelShader( pixelShaders[PS_SCALEDCOPY] )))
		throw "can't set pshader";

	//4) set the textures
	if (FAILED(d3dDevice->SetTexture(0,b.mathTexture)))
		throw "can't set texture 0";
	if (FAILED(d3dDevice->SetTexture(1,0)))
		throw "can't clear texture 1";

	//5) render some geometry.  We will render one quad of [0,1]x[0,1] in texture space, and aligned with the rendertarget.
    if(FAILED( d3dDevice->SetStreamSource( 0, quadVertexBuffer, 0, sizeof(MathVertex) )))
		throw "can't set stream";

	//gotta bias the texcoords by 1/2 pixel so the texel and pixel centers line up.
	float TexcoordBiasW = (1.0f/cols2TextureWidth(nCols))  * 0.5f;
	float TexcoordBiasH = (1.0f/rows2TextureHeight(nRows)) * 0.5f;


	float consts[4 * 2] = { 
		0.5, -0.5, 0.5, 1,
		0.5+ TexcoordBiasW,  0.5 + TexcoordBiasH, 0  , 0
		};

	if(FAILED( d3dDevice->SetVertexShaderConstantF(0, consts, 2)))
		throw "can't set vs consts";

	float pconsts[] = { 
		c, c, c,c	//constant color
		};

	if(FAILED( d3dDevice->SetPixelShaderConstantF(0, pconsts, 1)))
		throw "can't set ps consts";

    d3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );		//enable additive additive blending

    if(FAILED( d3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 )))
		throw "can't draw prims";

    // End the scene.
    d3dDevice->EndScene();
	d3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );	
	}

void Matrix::mad(Matrix & a, Matrix & b, float c)	//this = a + b * c
	{
	if (this == &a)
		{
		mad(b, c);
		return;
		}

	if (this == &b)	//can't do this in-place.  Issue a warning that op got split up?
		{
		multiply(c);
		add(a);
		return;
		}


	//check if dimensions agree:

	if (a.nRows != b.nRows || a.nCols != b.nCols)
		throw "matrix dimensions don't agree";

	resize(a.nRows, a.nCols);
	if (a.nRows * a.nCols == 0)
		return;


	//1) set this as render target
	if (FAILED(d3dDevice->SetRenderTarget(0,mathSurface)))
		throw "can't set rendertarget";

    // Begin the scene
    if(FAILED( d3dDevice->BeginScene()))
		throw "can't begin scene";

	//2) set an appropriate vertex program.  
	if(FAILED( d3dDevice->SetVertexShader( vertexShaders[VS_SINGLE_TEX_QUAD] )))
		throw "can't set vshader";
	//3) set a pixel program
	//we want:  out = tex[0] + tex[1].
	if(FAILED( d3dDevice->SetPixelShader( pixelShaders[PS_SCALEDADD] )))
		throw "can't set pshader";

	//4) set the textures
	if (FAILED(d3dDevice->SetTexture(0,a.mathTexture)))
		throw "can't set texture 0";
	if (FAILED(d3dDevice->SetTexture(1,b.mathTexture)))
		throw "can't set texture 1";

	//5) render some geometry.  We will render one quad of [0,1]x[0,1] in texture space, and aligned with the rendertarget.
    if(FAILED( d3dDevice->SetStreamSource( 0, quadVertexBuffer, 0, sizeof(MathVertex) )))
		throw "can't set stream";

	//gotta bias the texcoords by 1/2 pixel so the texel and pixel centers line up.
	float TexcoordBiasW = (1.0f/cols2TextureWidth(nCols))  * 0.5f;
	float TexcoordBiasH = (1.0f/rows2TextureHeight(nRows)) * 0.5f;


	float consts[4 * 2] = { 
		0.5, -0.5, 0.5, 1,
		0.5+ TexcoordBiasW,  0.5 + TexcoordBiasH, 0  , 0
		};

	if(FAILED( d3dDevice->SetVertexShaderConstantF(0, consts, 2)))
		throw "can't set vs consts";

	float pconsts[] = { 
		c, c, c,c	//constant color
		};

	if(FAILED( d3dDevice->SetPixelShaderConstantF(0, pconsts, 1)))
		throw "can't set ps consts";


    if(FAILED( d3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 )))
		throw "can't draw prims";


    // End the scene.
    d3dDevice->EndScene();
	}

void Matrix::mad(Matrix & a, Matrix & b)			//this = this + a .* b
	{
	if (this == &b || this == &a)
		throw "can't operate inplace -- not supported by D3D.";	//works on the refrast though!



	//here we just do a regular additive blending of the two textures, no need to multitexture

	//check if dimensions agree:

	if (nRows != b.nRows || nCols != b.nCols
		|| nRows != a.nRows || nCols != a.nCols)
		throw "matrix dimensions don't agree";

	if (a.nRows * a.nCols == 0)
		return;


	//1) set this as render target
	if (FAILED(d3dDevice->SetRenderTarget(0,mathSurface)))
		throw "can't set rendertarget";

    // Begin the scene
    if(FAILED( d3dDevice->BeginScene()))
		throw "can't begin scene";

	//2) set an appropriate vertex program.  
	if(FAILED( d3dDevice->SetVertexShader( vertexShaders[VS_SINGLE_TEX_QUAD] )))
		throw "can't set vshader";
	//3) set a pixel program
	//we want:  out = clampPositive(tex[0] * tex[1])
	if(FAILED( d3dDevice->SetPixelShader( pixelShaders[PS_BLEND] )))
		throw "can't set pshader";

	//4) set the textures
	if (FAILED(d3dDevice->SetTexture(0,a.mathTexture)))
		throw "can't set texture 0";
	if (FAILED(d3dDevice->SetTexture(1,b.mathTexture)))
		throw "can't set texture 1";

	//5) render some geometry.  We will render one quad of [0,1]x[0,1] in texture space, and aligned with the rendertarget.
    if(FAILED( d3dDevice->SetStreamSource( 0, quadVertexBuffer, 0, sizeof(MathVertex) )))
		throw "can't set stream";

	//gotta bias the texcoords by 1/2 pixel so the texel and pixel centers line up.
	float TexcoordBiasW = (1.0f/cols2TextureWidth(nCols))  * 0.5f;
	float TexcoordBiasH = (1.0f/rows2TextureHeight(nRows)) * 0.5f;


	float consts[4 * 2] = { 
		0.5, -0.5, 0.5, 1,
		0.5+ TexcoordBiasW,  0.5 + TexcoordBiasH, 0  , 0
		};

	if(FAILED( d3dDevice->SetVertexShaderConstantF(0, consts, 2)))
		throw "can't set vs consts";

    d3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );		//enable additive additive blending

    if(FAILED( d3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 )))
		throw "can't draw prims";

    // End the scene.
    d3dDevice->EndScene();
	d3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );	
	}

void Matrix::madad(Matrix & a, Matrix & b, Matrix & c, Matrix & d) //this = a + (b + c) .* d
	{
	if (this == &b || this == &a || this == &c || this == &d)
		throw "can't operate inplace -- not supported by D3D.";

	resize(a.nRows, a.nCols);
	if (a.nRows * a.nCols == 0)
		return;


	//here we just do a regular additive blending of the two textures, no need to multitexture

	//check if dimensions agree:

	if (nRows != b.nRows || nCols != b.nCols
		|| nRows != a.nRows || nCols != a.nCols)
		throw "matrix dimensions don't agree";

	//1) set this as render target
	if (FAILED(d3dDevice->SetRenderTarget(0,mathSurface)))
		throw "can't set rendertarget";

    // Begin the scene
    if(FAILED( d3dDevice->BeginScene()))
		throw "can't begin scene";

	//2) set an appropriate vertex program.  
	if(FAILED( d3dDevice->SetVertexShader( vertexShaders[VS_SINGLE_TEX_QUAD] )))
		throw "can't set vshader";
	//3) set a pixel program
	//we want:  out = tex[0] + (tex[1] + tex[2]) * tex[3]
	if(FAILED( d3dDevice->SetPixelShader( pixelShaders[PS_ADD_BLEND_ADD] )))
		throw "can't set pshader";

	//4) set the textures
	if (FAILED(d3dDevice->SetTexture(0,a.mathTexture)))
		throw "can't set texture 0";
	if (FAILED(d3dDevice->SetTexture(1,b.mathTexture)))
		throw "can't set texture 1";
	if (FAILED(d3dDevice->SetTexture(2,c.mathTexture)))
		throw "can't set texture 1";
	if (FAILED(d3dDevice->SetTexture(3,d.mathTexture)))
		throw "can't set texture 1";

	//5) render some geometry.  We will render one quad of [0,1]x[0,1] in texture space, and aligned with the rendertarget.
    if(FAILED( d3dDevice->SetStreamSource( 0, quadVertexBuffer, 0, sizeof(MathVertex) )))
		throw "can't set stream";

	//gotta bias the texcoords by 1/2 pixel so the texel and pixel centers line up.
	float TexcoordBiasW = (1.0f/cols2TextureWidth(nCols))  * 0.5f;
	float TexcoordBiasH = (1.0f/rows2TextureHeight(nRows)) * 0.5f;


	float consts[4 * 2] = { 
		0.5, -0.5, 0.5, 1,
		0.5+ TexcoordBiasW,  0.5 + TexcoordBiasH, 0  , 0
		};

	if(FAILED( d3dDevice->SetVertexShaderConstantF(0, consts, 2)))
		throw "can't set vs consts";

    if(FAILED( d3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 )))
		throw "can't draw prims";

    // End the scene.
    d3dDevice->EndScene();
	}


void Matrix::multiply(Matrix & a, Matrix & b)
	{
	if (this == &a || this == &b)
		throw "can't operate inplace -- not supported by D3D.";	//works in some cases on the refrast though!


	//check if dimensions agree:

	if (a.nCols != b.nRows)
		throw "matrix dimensions don't agree";

	resize(a.nRows, b.nCols);
	if (nRows * nCols == 0)
		return;

	const unsigned numQuads = roundUpDivide(rows2TextureHeight(b.nRows),numMOpsPerFragment);		//number of quads with additive blending.

	//gotta bias the texcoords by 1/2 pixel so the texel and pixel centers line up.
	const float TexcoordBiasW = (1.0f/cols2TextureWidth(nCols))  * 0.5f;
	const float TexcoordBiasH = (1.0f/rows2TextureHeight(nRows)) * 0.5f;

	const float TexcoordBiasAW = (1.0f/cols2TextureWidth(a.nCols))  * 0.5f;
	const float TexcoordBiasBH = (1.0f/rows2TextureHeight(b.nRows)) * 0.5f;

	const float tcPixelBH = 2 * TexcoordBiasBH;
	const float tcMOpIncrementBH = tcPixelBH;
	const float tcPassIncrementBH = numMOpsPerFragment * tcPixelBH;

	const float tcPixelAW = 2 * TexcoordBiasAW;
	const float tcMOpIncrementAW = 4 * tcPixelAW;
	const float tcPassIncrementAW = numMOpsPerFragment * tcMOpIncrementAW;



	//1) set this as render target
	if (FAILED(d3dDevice->SetRenderTarget(0,mathSurface)))
		throw "can't set rendertarget";

/*	single quad render
    // Begin the scene
    if(FAILED( d3dDevice->BeginScene()))
		throw "can't begin scene";

	//2) set an appropriate vertex program.  
	if(FAILED( d3dDevice->SetVertexShader( vertexShaders[VS_MULT_0] )))
		throw "can't set vshader";

	//3) set a pixel program
	//we want:  
	if(FAILED( d3dDevice->SetPixelShader( pixelShaders[PS_MULT_0] )))
		throw "can't set pshader";

	//4) set the textures
	if (FAILED(d3dDevice->SetTexture(0,a.mathTexture)))
		throw "can't set texture 0";
	if (FAILED(d3dDevice->SetTexture(1,b.mathTexture)))
		throw "can't set texture 1";

	//5) render some geometry.  We will render numQuads number of quads aligned with the rendertarget.
    if(FAILED( d3dDevice->SetStreamSource( 0, quadVertexBuffer, 0, sizeof(MathVertex) )))
		throw "can't set stream";


	float vconsts[] = { 
		0.5, -0.5, 0.5, 1,
		0.5 + TexcoordBiasW,  0.5 + TexcoordBiasH, 0 + TexcoordBiasBH, 0,
		0 + TexcoordBiasAW, tcPixelAW + TexcoordBiasAW, 2 * tcPixelAW + TexcoordBiasAW, 3 * tcPixelAW + TexcoordBiasAW
		};

	//c0.x tex0123.x 4 pixel increment
	//c0.y tex4.y 1 pixel increment
	float pconsts[] = { 
		1 * tcMOpIncrementAW, 0, 0,0,	//2 mops
		0, 1 * tcMOpIncrementBH, 0,0,	
		2 * tcMOpIncrementAW, 0, 0,0,	//3 mops
		0, 2 * tcMOpIncrementBH, 0,0,
		3 * tcMOpIncrementAW, 0, 0,0,	//4 mops
		0, 3 * tcMOpIncrementBH, 0,0,
		4 * tcMOpIncrementAW, 0, 0,0,	//5 mops
		0, 4 * tcMOpIncrementBH, 0,0,
		5 * tcMOpIncrementAW, 0, 0,0,	//6 mops
		0, 5 * tcMOpIncrementBH, 0,0,
		};


	if(FAILED( d3dDevice->SetVertexShaderConstantF(0, vconsts, 3)))
		throw "can't set vs consts";
	if(FAILED( d3dDevice->SetPixelShaderConstantF(0, pconsts, 2 * numMOpsPerFragment)))
		throw "can't set ps consts";

	if(FAILED( d3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 )))
		throw "can't draw prims";

	//subsequent passes?
	if (numQuads > 1)
	    d3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );		//enable additive additive blending

	for (unsigned i=1; i<numQuads; i++)
		{
		vconsts[6] += tcPassIncrementBH; 

		vconsts[8] += tcPassIncrementAW; 
		vconsts[9] += tcPassIncrementAW; 
		vconsts[10] += tcPassIncrementAW; 
		vconsts[11] += tcPassIncrementAW; 

		if(FAILED( d3dDevice->SetVertexShaderConstantF(0, vconsts, 3)))
			throw "can't set vs consts";
		if(FAILED( d3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 )))
			throw "can't draw prims";
		}


    // End the scene.
    d3dDevice->EndScene();
*/
    // Begin the scene
    if(FAILED( d3dDevice->BeginScene()))
		throw "can't begin scene";

	if (FAILED(d3dDevice->SetVertexDeclaration( vertexDeclaration2 )))
		throw "can't set declaration";

	//2) set an appropriate vertex program.  
	if(FAILED( d3dDevice->SetVertexShader( vertexShaders[VS_MULT_1] )))
		throw "can't set vshader";

	//3) set a pixel program
	//we want:  
	if(FAILED( d3dDevice->SetPixelShader( pixelShaders[PS_MULT_0] )))
		throw "can't set pshader";

	//4) set the textures
	if (FAILED(d3dDevice->SetTexture(0,a.mathTexture)))
		throw "can't set texture 0";
	if (FAILED(d3dDevice->SetTexture(1,b.mathTexture)))
		throw "can't set texture 1";

	//5) render some geometry.  We will render numQuads number of quads aligned with the rendertarget.
    if(FAILED( d3dDevice->SetStreamSource( 0, quadsVertexBuffer, 0,TINYVERTEX_SIZE )))
		throw "can't set stream";


	float vconsts[] = { 
		0.5, -0.5, 0.5, 1,
		0.5 + TexcoordBiasW,  0.5 + TexcoordBiasH, 0 + TexcoordBiasBH, 0,
		0 + TexcoordBiasAW, tcPixelAW + TexcoordBiasAW, 2 * tcPixelAW + TexcoordBiasAW, 3 * tcPixelAW + TexcoordBiasAW,
		tcPassIncrementBH, tcPassIncrementAW, 0, 0
		};

	//c0.x tex0123.x 4 pixel increment
	//c0.y tex4.y 1 pixel increment
	float pconsts[] = { 
		1 * tcMOpIncrementAW, 0, 0,0,	//2 mops
		0, 1 * tcMOpIncrementBH, 0,0,	
		2 * tcMOpIncrementAW, 0, 0,0,	//3 mops
		0, 2 * tcMOpIncrementBH, 0,0,
		3 * tcMOpIncrementAW, 0, 0,0,	//4 mops
		0, 3 * tcMOpIncrementBH, 0,0,
		4 * tcMOpIncrementAW, 0, 0,0,	//5 mops
		0, 4 * tcMOpIncrementBH, 0,0,
		5 * tcMOpIncrementAW, 0, 0,0,	//6 mops
		0, 5 * tcMOpIncrementBH, 0,0,
		};


	if(FAILED( d3dDevice->SetVertexShaderConstantF(0, vconsts, 4)))
		throw "can't set vs consts";
	if(FAILED( d3dDevice->SetPixelShaderConstantF(0, pconsts, 2 * numMOpsPerFragment)))
		throw "can't set ps consts";

	if(FAILED( d3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 )))
		throw "can't draw prims";

	//subsequent passes?
	if (numQuads > 1)
		{
	    d3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );		//enable additive additive blending
		if(FAILED( d3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 6, 2 *  (numQuads - 1))))
			throw "can't draw prims";
		d3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
		}


    // End the scene.
    d3dDevice->EndScene();

	//put back to default:
	if (FAILED(d3dDevice->SetVertexDeclaration( vertexDeclaration2 )))
		throw "can't set declaration";

	}

/*
#define TIME_IT
#ifdef TIME_IT
	unsigned textureWidth =  cols2TextureWidth(nCols);
	unsigned textureHeight = rows2TextureHeight(nRows);


	//because its not possible to lock and read rendertargets directly, we have to create a temp surface,
	//copy the data to it from D3D, and then lock that.

	IDirect3DSurface9 * tempSurface;
	IDirect3DTexture9 * tempTexture;


	if (FAILED(d3dDevice->CreateTexture(textureWidth, textureHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_A32B32G32R32F, D3DPOOL_SYSTEMMEM, &tempTexture, NULL)))
		throw "can't create temp texture";

	assert(tempTexture != 0);
	//retrieve a surface interface that we will need.
	if (FAILED(tempTexture->GetSurfaceLevel(0, &tempSurface)))
		throw "can't get surface level";

	Time timer;
	timer.GetElapsedSeconds();
#endif

#ifdef TIME_IT
	//now update the real vidmem surface from the temp sysmem surface:
	if (FAILED(d3dDevice->GetRenderTargetData(mathSurface, tempSurface)))
		throw "can't get rendertarget data";

	D3DLOCKED_RECT inf;
	if (FAILED(tempSurface->LockRect(&inf,0,D3DLOCK_READONLY)))
		throw "can't lock surface for reading";

	second t = timer.GetElapsedSeconds();
	unsigned n = a.nRows;
	unsigned flops = n*n*(n*2 - 1);	//number of flops needed for a nxn matrix multiply.
	char buffer[300];
	sprintf(buffer, "matrix multiply: %g (%g MFlops)\n", t, flops/t/1.0e6f );
	OutputDebugString(buffer);


	//may want to actually touch the mem here?
	tempSurface->UnlockRect();

	//we don't need the temp surface anymore:
	tempSurface->Release();
	tempTexture->Release();

#endif



	}
*/

void Matrix::multiplyAT(Matrix & a, Matrix & b)	//similar to multiply, but a transposed a is used.
	{
	if (this == &a || this == &b)
		throw "can't operate inplace -- not supported by D3D.";	//works in some cases on the refrast though!



	//check if dimensions agree:

	if (a.nRows != b.nRows)
		throw "matrix dimensions don't agree";

	resize(a.nCols, b.nCols);
	if (nRows * nCols == 0)
		return;

	const unsigned numQuads = roundUpDivide(rows2TextureHeight(b.nRows),numMTOpsPerFragment);		//number of quads with additive blending.

	//gotta bias the texcoords by 1/2 pixel so the texel and pixel centers line up.
	const float TexcoordBiasW = (1.0f/cols2TextureWidth(nCols))  * 0.5f;
	const float TexcoordBiasH = (1.0f/rows2TextureHeight(nRows)) * 0.5f;

	const float TexcoordBiasAW = (1.0f/cols2TextureWidth(a.nCols))  * 0.5f;
	const float TexcoordBiasAH = (1.0f/rows2TextureHeight(a.nRows))  * 0.5f;
	const float TexcoordBiasBH = (1.0f/rows2TextureHeight(b.nRows)) * 0.5f;

	const float tcPixelBH = 2 * TexcoordBiasBH;
	const float tcMOpIncrementBH = tcPixelBH;
	const float tcPassIncrementBH = numMTOpsPerFragment * tcPixelBH;

	const float tcPixelAW = 2 * TexcoordBiasAW;
	const float tcPixelAH = 2 * TexcoordBiasAH;
	const float tcMOpIncrementAH = tcPixelAH;
	const float tcPassIncrementAH = numMTOpsPerFragment * tcMOpIncrementAH;
	const unsigned awidth = cols2TextureWidth(a.nCols);

//we need to map the triangle's vertex range [1,-1] not to texcoord [0,1] but to [0, quotient], 
//where quotient is <= 1 to account for the 1..3 unused elements.
//i.e. virtually rounding up the texture size to the nearest %4.
	unsigned modu = awidth % 4;
	if (modu != 0) modu = 4 - modu;
	//modu is now the number of 'wasted' elements, 0..3.
	const float quotient = (awidth + modu)/(float)awidth;	
	const float q = quotient * 0.5f;	//this follows from: q+b = 0 ;  -q + b = quotient  

	//1) set this as render target
	if (FAILED(d3dDevice->SetRenderTarget(0,mathSurface)))
		throw "can't set rendertarget";

    // Begin the scene
    if(FAILED( d3dDevice->BeginScene()))
		throw "can't begin scene";


#ifdef MANY_PASSES
	//2) set an appropriate vertex program.  
	if(FAILED( d3dDevice->SetVertexShader( vertexShaders[VS_MULT_T] )))
		throw "can't set vshader";

	//3) set a pixel program
	//we want:  
	if(FAILED( d3dDevice->SetPixelShader( pixelShaders[PS_MULT_T] )))
		throw "can't set pshader";

	//4) set the textures
	if (FAILED(d3dDevice->SetTexture(0,a.mathTexture)))
		throw "can't set texture 0";
	if (FAILED(d3dDevice->SetTexture(1,b.mathTexture)))
		throw "can't set texture 1";

	//5) render some geometry.  We will render numQuads number of quads aligned with the rendertarget.
    if(FAILED( d3dDevice->SetStreamSource( 0, quadVertexBuffer, 0, sizeof(MathVertex) )))
		throw "can't set stream";


	float vconsts[] = { 
		0.5, -q, 0.5, 1,
		0.5	+ TexcoordBiasW,  0.5 + TexcoordBiasH, 0 + TexcoordBiasBH, 0,
		0   + TexcoordBiasAH, 0, 0, 0, //tcPixelAH + TexcoordBiasAH, 2 * tcPixelAH + TexcoordBiasAH, 3 * tcPixelAH + TexcoordBiasAH,
		0, q + TexcoordBiasAW, 0, 0
		};

	//c0.x tex0123.x 1 pixel increment
	//c0.y tex4.y 1 pixel increment
	float pconsts[] = { 
		tcPixelAW, 0, 0,0,
		0, 1 * tcMOpIncrementBH, 0,0,
		0, 1 * tcPixelAH, 0,0,
		0, 2 * tcMOpIncrementBH, 0,0,
		0, 2 * tcPixelAH, 0,0,
		0, 3 * tcMOpIncrementBH, 0,0,
		0, 3 * tcPixelAH, 0,0,
		0, 4 * tcMOpIncrementBH, 0,0,
		0, 4 * tcPixelAH, 0,0,
		0, 5 * tcMOpIncrementBH, 0,0,
		0, 5 * tcPixelAH, 0,0,
		};


	if(FAILED( d3dDevice->SetVertexShaderConstantF(0, vconsts, 4)))
		throw "can't set vs consts";
	if(FAILED( d3dDevice->SetPixelShaderConstantF(0, pconsts, 1 + 2 * (numMTOpsPerFragment - 1) )))
		throw "can't set ps consts";

	if(FAILED( d3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 )))
		throw "can't draw prims";
	//subsequent passes?
	if (numQuads > 1)
	    d3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );		//enable additive additive blending

	for (unsigned i=1; i<numQuads; i++)
		{
		vconsts[6] += tcPassIncrementBH; 

		vconsts[8] += tcPassIncrementAH;

		if(FAILED( d3dDevice->SetVertexShaderConstantF(0, vconsts, 3)))
			throw "can't set vs consts";
		if(FAILED( d3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 )))
			throw "can't draw prims";
		}
    // End the scene.
    d3dDevice->EndScene();

	if (numQuads > 1)	//disable alpha blending again.
		d3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );

#else
	if (FAILED(d3dDevice->SetVertexDeclaration( vertexDeclaration2 )))
		throw "can't set declaration";

	//2) set an appropriate vertex program.  
	if(FAILED( d3dDevice->SetVertexShader( vertexShaders[VS_MULT_T] )))
		throw "can't set vshader";

	//3) set a pixel program
	//we want:  
	if(FAILED( d3dDevice->SetPixelShader( pixelShaders[PS_MULT_T] )))
		throw "can't set pshader";

	//4) set the textures
	if (FAILED(d3dDevice->SetTexture(0,a.mathTexture)))
		throw "can't set texture 0";
	if (FAILED(d3dDevice->SetTexture(1,b.mathTexture)))
		throw "can't set texture 1";

	//5) render some geometry.  We will render numQuads number of quads aligned with the rendertarget.
    if(FAILED( d3dDevice->SetStreamSource( 0, quadsVertexBuffer, 0, TINYVERTEX_SIZE )))
		throw "can't set stream";


	float vconsts[] = { 
		0.5, -q, 0.5, 1,
		0.5	+ TexcoordBiasW,  0.5 + TexcoordBiasH, 0 + TexcoordBiasBH, 0,
		0   + TexcoordBiasAH, 0, 0, 0, 
		0, q + TexcoordBiasAW, 0, 0,
		tcPassIncrementBH, tcPassIncrementAH, 0, 0
		};

	//c0.x tex0123.x 1 pixel increment
	//c0.y tex4.y 1 pixel increment
	float pconsts[] = { 
		tcPixelAW, 0, 0,0,
		0, 1 * tcMOpIncrementBH, 0,0,
		0, 1 * tcPixelAH, 0,0,
		0, 2 * tcMOpIncrementBH, 0,0,
		0, 2 * tcPixelAH, 0,0,
		0, 3 * tcMOpIncrementBH, 0,0,
		0, 3 * tcPixelAH, 0,0,
		0, 4 * tcMOpIncrementBH, 0,0,
		0, 4 * tcPixelAH, 0,0,
		0, 5 * tcMOpIncrementBH, 0,0,
		0, 5 * tcPixelAH, 0,0,
		};


	if(FAILED( d3dDevice->SetVertexShaderConstantF(0, vconsts, 5)))
		throw "can't set vs consts";
	if(FAILED( d3dDevice->SetPixelShaderConstantF(0, pconsts, 1 + 2 * (numMTOpsPerFragment - 1) )))
		throw "can't set ps consts";

	if(FAILED( d3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 )))
		throw "can't draw prims";
	//subsequent passes?
	if (numQuads > 1)
		{
	    d3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );		//enable additive additive blending
		if(FAILED( d3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 6, 2 * (numQuads - 1) )))
			throw "can't draw prims";
		d3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
		}
    // End the scene.
    d3dDevice->EndScene();

	if (FAILED(d3dDevice->SetVertexDeclaration( vertexDeclaration )))
		throw "can't set declaration";


#endif
	}

float Matrix::dot(Matrix & vec)
	{
	Matrix dest("temp");

	//column vectors needed
	if (nCols != 1 || vec.nCols != 1	|| nRows < 1	|| vec.nRows != nRows)
		throw "dot product args are not column vectors or dimensions don't match!";


	dest.multiplyAT(*this, vec);
	

	//dest should be 1x1:
	assert(dest.nCols == 1 && dest.nRows == 1);
	double data;
	dest.d3DsaveData(&data);
	return (float)data;
	}

float Matrix::normSquared()
	{
	return dot(*this);	//TODO: write an optimized implementation.
	}


void Matrix::loadFromFile(char * matFileName)
	{
	//FILE * fp = fopen(matFileName, "rt");//ASCII data, each matrix row on a separate line

	//double * data = (double *)preal;	
	//1) count number of rows

	std::ifstream in(matFileName);
	if (!in.is_open())
		throw "can't open file!";
	size_t nlines = std::count(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>(), '\n');

	in.seekg(0);	//rewind

	//2) count number of columns:
	
	std::string s;
	std::getline(in,s,'\n');
	std::istringstream is(s);
	double mess;
	size_t ncols = 0;
	do
		{
		is >> mess;
		if (!is.fail())
			ncols++;
		}
	while (is.good());

	//2) allocate space (+1 in case there is no last \n)
	std::vector<double> data;
	data.reserve((nlines+1) * ncols);

	in.seekg(0);	//rewind

	do
		{
		in >> mess;
		if (in.good())
			data.push_back(mess);
		}
	while (in.good());


	if (data.size() % ncols != 0)
		throw "bad data file!";
	
	unsigned mrows = data.size() / ncols;

	



	resize(mrows, ncols);
	if (nRows * nCols == 0)
		return;


	if (debug)
		debugDumpMatrix(&data[0], "Read:  ", matFileName);

	//transfer the data:
	d3DloadData(&data[0]);
	}

void Matrix::saveToFile(char * matFileName)
	{
	if (nCols < 1 || nRows < 1)
		throw "can't save zero dim matrix!";

	double * data = new double[nCols * nRows];
	d3DsaveData(data);


	if (debug)
		{
		debugDumpMatrix(data, "would have written:  ", matFileName);
		}
	else
		{
		double * datap = data;
		std::ofstream out(matFileName);
		for (unsigned y = 0; y < nRows; y++)
			{
			for (unsigned x = 0; x < nCols; x++)
				{
				out << "  " << *datap;
				datap++;
				}
			out << std::endl;
			}
		}
	delete data;
	}

void Matrix::debugDumpMatrix(double * data, const char * message1, const char * message2)
	{
	char buffer[100];
	//dump it to the debugger instead:
	OutputDebugString(message1);
	OutputDebugString(message2);
	OutputDebugString(":\n[\n");
	for (unsigned r = 0; r < nRows; r++)
		{
		for (unsigned c = 0; c < nCols; c++)
			{
			sprintf(buffer, "%g\t\t", data[nCols * r + c]);
			OutputDebugString(buffer);
			Sleep(1);
			}
		OutputDebugString(";\n");
		}
	OutputDebugString("]\n");
	}

//operations:
void Matrix::d3DfreeData()
	{
	SAFE_RELEASE( mathSurface );	
	SAFE_RELEASE( mathTexture );	
	}

void Matrix::d3DcreateData()
	{
	assert(mathSurface == 0);
	assert(mathTexture == 0);
	unsigned textureWidth =  cols2TextureWidth(nCols);
	unsigned textureHeight = rows2TextureHeight(nRows);

	if (FAILED(d3dDevice->CreateTexture(textureWidth, textureHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, &mathTexture, NULL)))
		throw "can't create texture";
	assert(mathTexture != 0);
	//retrieve a surface interface that we will need.
	if (FAILED(mathTexture->GetSurfaceLevel(0, &mathSurface)))
		throw "can't get surface level";
	assert(mathSurface != 0);
	}

void Matrix::d3DloadDataRGRG(double * data)
	{
	unsigned textureWidth =  (nCols + 1)/2;	//number of times divisible by two, rounding up!
	unsigned textureHeight = (nRows + 1)/2;
	unsigned widthQuotient = nCols / 2;
	unsigned widthRemainder = nCols % 2;
	unsigned heightQuotient = nRows / 2;	//number of fully filled blocks
	unsigned heightRemainder = nRows % 2;

	assert(widthQuotient*2 + widthRemainder == nCols);
	assert(textureWidth >= widthQuotient);
	assert(textureWidth*2 >= nCols);


	//because its not possible to lock and write rendertargets directly, we have to create a temp surface,
	//lock that, and then make D3D do the copy.

	IDirect3DSurface9 * tempSurface;
	IDirect3DTexture9 * tempTexture;


	if (FAILED(d3dDevice->CreateTexture(textureWidth, textureHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_A32B32G32R32F, D3DPOOL_SYSTEMMEM, &tempTexture, NULL)))
		throw "can't create temp texture";

	assert(tempTexture != 0);
	//retrieve a surface interface that we will need.
	if (FAILED(tempTexture->GetSurfaceLevel(0, &tempSurface)))
		throw "can't get surface level";



	D3DLOCKED_RECT inf;

	if (FAILED(tempSurface->LockRect(&inf,0,D3DLOCK_DISCARD)))
		throw "can't lock surface for writing";

	//now we write the data
	//we let each pixel be a 4x4 submatrix of the matrix.  
	float * surfData = (float *)inf.pBits;
	double * data2 = data + nCols;	//always a row below data.

	for (unsigned y = 0; y < heightQuotient; y ++)
		{
		char * scanStart = (char *)surfData;
		for (unsigned x = 0; x < widthQuotient; x ++)
			{
			*surfData = (float)*data;//red
			data++;
			surfData++;
			*surfData = (float)*data;//green
			data++;
			surfData++;
			*surfData = (float)*data2;//blue
			data2++;
			surfData++;
			*surfData = (float)*data2;//alpha
			data2++;
			surfData++;
			}
		//the last pixel may be incomplete, as we rounded the size up to the nearest /2:
		if (widthRemainder)
			{
			*surfData = (float)*data;//red
			data++;
			surfData++;

			*surfData = 0.0f;		//no more data
			surfData++;

			*surfData = (float)*data2;//blue
			data2++;
			surfData++;

			*surfData = 0.0f;		//no more data2
			surfData++;
			}

		data += nCols;	//skip the data2 row we just processed.
		data2 += nCols;	//skip the data row we're about to process.

		scanStart += inf.Pitch;
		surfData = (float *)scanStart;
		}
	if (heightRemainder)	//last scan, same as above but with zeros instead of data2.
		{
		for (unsigned x = 0; x < widthQuotient; x ++)
			{
			*surfData = (float)*data;//red
			data++;
			surfData++;
			*surfData = (float)*data;//green
			data++;
			surfData++;
			*surfData = 0.0f;
			surfData++;
			*surfData = 0.0f;
			surfData++;
			}
		//the last pixel may be incomplete, as we rounded the size up to the nearest /2:
		if (widthRemainder)
			{
			*surfData = (float)*data;//red
			data++;
			surfData++;

			*surfData = 0.0f;		//no more data
			surfData++;

			*surfData = 0.0f;
			surfData++;

			*surfData = 0.0f;
			surfData++;
			}
		}

	tempSurface->UnlockRect();


	//now update the real vidmem surface from the temp sysmem surface:
	if (FAILED(d3dDevice->UpdateSurface(tempSurface, 0, mathSurface, 0)))	
		throw "can't update surface";

	//not sure if this is needed:
	if (FAILED(d3dDevice->UpdateTexture(tempTexture, mathTexture)))
		throw "can't update texture";

	//we don't need the temp surface anymore:
	tempSurface->Release();
	tempTexture->Release();
	}

void Matrix::flush()
	{
	unsigned textureWidth =  cols2TextureWidth(nCols);
	unsigned textureHeight = rows2TextureHeight(nRows);
	unsigned heightQuotient = nRows / 4;
	unsigned heightRemainder = nRows % 4;


	//because its not possible to lock and read rendertargets directly, we have to create a temp surface,
	//copy the data to it from D3D, and then lock that.

	IDirect3DSurface9 * tempSurface;
	IDirect3DTexture9 * tempTexture;


	if (FAILED(d3dDevice->CreateTexture(textureWidth, textureHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_A32B32G32R32F, D3DPOOL_SYSTEMMEM, &tempTexture, NULL)))
		throw "can't create temp texture";

	assert(tempTexture != 0);
	//retrieve a surface interface that we will need.
	if (FAILED(tempTexture->GetSurfaceLevel(0, &tempSurface)))
		throw "can't get surface level";

	//now update the real vidmem surface from the temp sysmem surface:
	if (FAILED(d3dDevice->GetRenderTargetData(mathSurface, tempSurface)))
		throw "can't get rendertarget data";

	//we don't need the temp surface anymore:
	tempSurface->Release();
	tempTexture->Release();

	}

void Matrix::d3DsaveDataRGRG(double * data)
	{
	unsigned textureWidth =  (nCols + 1)/2;	//number of times divisible by two, rounding up!
	unsigned textureHeight = (nRows + 1)/2;
	unsigned widthQuotient = nCols / 2;
	unsigned widthRemainder = nCols % 2;
	unsigned heightQuotient = nRows / 2;	//number of fully filled blocks
	unsigned heightRemainder = nRows % 2;


	//because its not possible to lock and read rendertargets directly, we have to create a temp surface,
	//copy the data to it from D3D, and then lock that.

	IDirect3DSurface9 * tempSurface;
	IDirect3DTexture9 * tempTexture;


	if (FAILED(d3dDevice->CreateTexture(textureWidth, textureHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_A32B32G32R32F, D3DPOOL_SYSTEMMEM, &tempTexture, NULL)))
		throw "can't create temp texture";

	assert(tempTexture != 0);
	//retrieve a surface interface that we will need.
	if (FAILED(tempTexture->GetSurfaceLevel(0, &tempSurface)))
		throw "can't get surface level";

	//now update the real vidmem surface from the temp sysmem surface:
	if (FAILED(d3dDevice->GetRenderTargetData(mathSurface, tempSurface)))
		throw "can't get rendertarget data";

	D3DLOCKED_RECT inf;
	if (FAILED(tempSurface->LockRect(&inf,0,D3DLOCK_READONLY)))
		throw "can't lock surface for reading";


	float * surfData = (float *)inf.pBits;
	double * data2 = data + nCols;	//always a row below data.

	for (unsigned y = 0; y < heightQuotient; y ++)
		{
		char * scanStart = (char *)surfData;
		for (unsigned x = 0; x < widthQuotient; x ++)
			{
			*data = *surfData;//red
			data++;
			surfData++;
			*data = *surfData;//green
			data++;
			surfData++;
			*data2 = *surfData;//blue
			data2++;
			surfData++;
			*data2 = *surfData;//alpha
			data2++;
			surfData++;
			}
		//the last pixel may be incomplete, as we rounded the size up to the nearest /4:
		if (widthRemainder)
			{
			*data = *surfData;//red
			data++;
			surfData++;

			surfData++;	//this is zero!	We could assert it!

			*data2 = *surfData;//red
			data2++;
			surfData++;

			//surfData++;	//this is zero!	We could assert it!
			}
		data += nCols;	//skip the data2 row we just processed.
		data2 += nCols;	//skip the data row we're about to process.

		scanStart += inf.Pitch;
		surfData = (float *)scanStart;
		}
	if (heightRemainder)
		{
		for (unsigned x = 0; x < widthQuotient; x ++)
			{
			*data = *surfData;//red
			data++;
			surfData++;
			*data = *surfData;//green
			data++;
			surfData++;

			surfData++;
			surfData++;
			}
		//the last pixel may be incomplete, as we rounded the size up to the nearest /4:
		if (widthRemainder)
			{
			*data = *surfData;//red
			}
		}


	tempSurface->UnlockRect();

	//we don't need the temp surface anymore:
	tempSurface->Release();
	tempTexture->Release();
	}


/**
	Loads data into texture from data pointer, which is rowmajor, nRows x nCols.
	The texture has already been sized to the correct proportions.

    Data is written in the rrrr format, which means that 1 pixel is a 4x1 submatrix of the texture.
*/
void Matrix::d3DloadData(double * data)
	{
	unsigned textureWidth = cols2TextureWidth(nCols);	//number of times divisible by four, rounding up!
	unsigned textureHeight = rows2TextureHeight(nRows);
	unsigned heightQuotient = nRows / 4;
	unsigned heightRemainder = nRows % 4;
	unsigned i = 0;

	assert(heightQuotient*4 + heightRemainder == nRows);
	assert(textureHeight >= heightQuotient);
	assert(textureHeight*4 >= nRows);


	//because its not possible to lock and write rendertargets directly, we have to create a temp surface,
	//lock that, and then make D3D do the copy.

	IDirect3DSurface9 * tempSurface;
	IDirect3DTexture9 * tempTexture;


	if (FAILED(d3dDevice->CreateTexture(textureWidth, textureHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_A32B32G32R32F, D3DPOOL_SYSTEMMEM, &tempTexture, NULL)))
		throw "can't create temp texture";

	assert(tempTexture != 0);
	//retrieve a surface interface that we will need.
	if (FAILED(tempTexture->GetSurfaceLevel(0, &tempSurface)))
		throw "can't get surface level";



	D3DLOCKED_RECT inf;

	if (FAILED(tempSurface->LockRect(&inf,0,D3DLOCK_DISCARD)))
		throw "can't lock surface for writing";

	//now we write the data

	//destination texture:
	float * surfData = (float *)inf.pBits;

	//source in memory, we will write 4 consecutive rows of the source at once:
	double * dataR = data;
	double * dataG = dataR + nCols;
	double * dataB = dataG + nCols;
	double * dataA = dataB + nCols;

	unsigned x;


	for (unsigned y = 0; y < heightQuotient; y ++)
		{
		char * scanStart = (char *)surfData;
		for (x = 0; x < nCols; x ++)
			{
			//write 1 RGBA pixel == 4 floats.
			*surfData = (float)*dataR;//red
			surfData++;	dataR++;
			*surfData = (float)*dataG;//green
			surfData++;	dataG++;
			*surfData = (float)*dataB;//blue
			surfData++;	dataB++;
			*surfData = (float)*dataA;//alpha
			surfData++;	dataA++;
			}
		scanStart += inf.Pitch;
		surfData = (float *)scanStart;

		//each row ptr has to move down 3 more rows:
		dataR += 3*nCols;
		dataG += 3*nCols;
		dataB += 3*nCols;
		dataA += 3*nCols;
		}

	//the last row's pixels may be incomplete, as we rounded the size up to the nearest /4:
	switch (heightRemainder)
		{
		case 3:
		for (x = 0; x < nCols; x ++)
			{
			//write 1 RGBA pixel == 4 floats.
			*surfData = (float)*dataR;//red
			surfData++;	dataR++;
			*surfData = (float)*dataG;//green
			surfData++;	dataG++;
			*surfData = (float)*dataB;//blue
			surfData++;	dataB++;
			*surfData = 0;//alpha
			surfData++;
			}
			break;
		case 2:
		for (x = 0; x < nCols; x ++)
			{
			//write 1 RGBA pixel == 4 floats.
			*surfData = (float)*dataR;//red
			surfData++;	dataR++;
			*surfData = (float)*dataG;//green
			surfData++;	dataG++;
			*surfData = 0;//blue
			surfData++;
			*surfData = 0;//alpha
			surfData++;
			}
			break;
		case 1:
		for (x = 0; x < nCols; x ++)
			{
			//write 1 RGBA pixel == 4 floats.
			*surfData = (float)*dataR;//red
			surfData++;	dataR++;
			*surfData = 0;//green
			surfData++;
			*surfData = 0;//blue
			surfData++;
			*surfData = 0;//alpha
			surfData++;
			}
			break;
		}


	tempSurface->UnlockRect();


	//now update the real vidmem surface from the temp sysmem surface:
	if (FAILED(d3dDevice->UpdateSurface(tempSurface, 0, mathSurface, 0)))	
		throw "can't update surface";

	//not sure if this is needed:
	if (FAILED(d3dDevice->UpdateTexture(tempTexture, mathTexture)))
		throw "can't update texture";

	//we don't need the temp surface anymore:
	tempSurface->Release();
	tempTexture->Release();
	}



void Matrix::d3DsaveData(double * data)
	{
	unsigned textureWidth =  cols2TextureWidth(nCols);
	unsigned textureHeight = rows2TextureHeight(nRows);
	unsigned heightQuotient = nRows / 4;
	unsigned heightRemainder = nRows % 4;



	//because its not possible to lock and read rendertargets directly, we have to create a temp surface,
	//copy the data to it from D3D, and then lock that.

	IDirect3DSurface9 * tempSurface;
	IDirect3DTexture9 * tempTexture;


	if (FAILED(d3dDevice->CreateTexture(textureWidth, textureHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_A32B32G32R32F, D3DPOOL_SYSTEMMEM, &tempTexture, NULL)))
		throw "can't create temp texture";

	assert(tempTexture != 0);
	//retrieve a surface interface that we will need.
	if (FAILED(tempTexture->GetSurfaceLevel(0, &tempSurface)))
		throw "can't get surface level";



	//now update the real vidmem surface from the temp sysmem surface:
	if (FAILED(d3dDevice->GetRenderTargetData(mathSurface, tempSurface)))
		throw "can't get rendertarget data";


	D3DLOCKED_RECT inf;
	if (FAILED(tempSurface->LockRect(&inf,0,D3DLOCK_READONLY)))
		throw "can't lock surface for reading";


	//destination texture:
	float * surfData = (float *)inf.pBits;

	//source in memory, we will write 4 consecutive rows at once:
	double * dataR = data;
	double * dataG = dataR + nCols;
	double * dataB = dataG + nCols;
	double * dataA = dataB + nCols;

	unsigned x;


	for (unsigned y = 0; y < heightQuotient; y ++)
		{
		char * scanStart = (char *)surfData;
		for (x = 0; x < nCols; x ++)
			{
			//1 RGBA pixel == 4 floats.
			*dataR = *surfData;//red
			surfData++;	dataR++;
			*dataG = *surfData;//green
			surfData++;	dataG++;
			*dataB = *surfData;//blue
			surfData++;	dataB++;
			*dataA = *surfData;//alpha
			surfData++;	dataA++;
			}
		scanStart += inf.Pitch;
		surfData = (float *)scanStart;

		//each row ptr has to move down 3 more rows:
		dataR += 3*nCols;
		dataG += 3*nCols;
		dataB += 3*nCols;
		dataA += 3*nCols;
		}

	//the last row's pixels may be incomplete, as we rounded the size up to the nearest /4:
	switch (heightRemainder)
		{
		case 3:
		for (x = 0; x < nCols; x ++)
			{
			*dataR = *surfData;//red
			surfData++;	dataR++;
			*dataG = *surfData;//green
			surfData++;	dataG++;
			*dataB = *surfData;//blue
			surfData+=2;	dataB++;
			}
			break;
		case 2:
		for (x = 0; x < nCols; x ++)
			{
			*dataR = *surfData;//red
			surfData++;	dataR++;
			*dataG = *surfData;//green
			surfData+=3;	dataG++;
			}
			break;
		case 1:
		for (x = 0; x < nCols; x ++)
			{
			*dataR = *surfData;//red
			surfData+=4;	dataR++;
			}
			break;
		}

	tempSurface->UnlockRect();

	//we don't need the temp surface anymore:
	tempSurface->Release();
	tempTexture->Release();
	}