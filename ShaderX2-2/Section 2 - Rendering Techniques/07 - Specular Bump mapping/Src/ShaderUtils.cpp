//-----------------------------------------------------------------------------
// File: ShaderUtils.cpp
//
// Desc: 
//-----------------------------------------------------------------------------
#include <tchar.h>
#include <stdio.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxfile.h>
#include "ShaderUtils.h"
#include "DXUtil.h"




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
ShaderUtils::ShaderUtils()
{
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
ShaderUtils::~ShaderUtils()
{
}


// constant layout:
// VERTEX:
// c0-c3 = local to screen
// c4 = local camera pos
// c5 = local light pos/dir
// c6.xy = Diffuse uv scale
// c6.zw = Bump uv scale
// PIXEL
// c0 = ambient light colour
// c1 = diffuse light colour
// c2 = specular light colour


void ShaderUtils::CreateDiffuseShaders(
						const MeshDesc& desc, 
						IDirect3DVertexShader9** vShader, 
						IDirect3DPixelShader9** pShader, 
						LPDIRECT3DDEVICE9 pd3dDevice)
{
	// VERTEX
	char* vText = NULL;
	AppendVStart(vText);
	AppendNL(vText, "m4x4 oPos, v0, c0");
	AppendNL(vText, "mul oT0.xy, v1, c6.xy");
	AppendNL(vText, "mul oT1.xy, v1, c6.zw");

	AppendNL(vText, "sub r0, c5, v0");
	AppendNL(vText, "dp3 r0.w, r0, r0");
	AppendNL(vText, "rsq r0.w, r0.w");
	AppendNL(vText, "mul r0, r0, r0.w");
	// Normalised light dir in r0

	AppendNL(vText, "dp3 oT2.x, v3.xyz, r0.xyz");
	AppendNL(vText, "dp3 oT2.y, v4.xyz, r0.xyz");
	AppendNL(vText, "dp3 oT2.z, v2.xyz, r0.xyz");


	//PIXEL
	char* pText = NULL;
	AppendPStart(pText);
	AppendNL(pText, "tex t0");
	AppendNL(pText, "tex t1");
	AppendNL(pText, "tex t2");
	AppendNL(pText, "dp3_sat r0, t1_bx2, t2_bx2");
	AppendNL(pText, "mad r1, r0, c1, c0");
	AppendNL(pText, "mul r0.rgb, r1, t0");
	AppendNL(pText, "+mov r0.a, t0.a");			// copy specular level map to alpha

	AssembleShaders(vText, pText, vShader, pShader, pd3dDevice);
}

void ShaderUtils::CreateSpecularShaders(
						const MeshDesc& desc, 
						IDirect3DVertexShader9** vShader, 
						IDirect3DPixelShader9** pShader, 
						LPDIRECT3DDEVICE9 pd3dDevice)
{
	// VERTEX
	char* vText = NULL;
	AppendVStart(vText);
	AppendNL(vText, "m4x4 oPos, v0, c0");
	AppendNL(vText, "mul oT0.xy, v1, c6.zw");

	AppendNL(vText, "sub r0, c5, v0");
	AppendNL(vText, "dp3 r0.w, r0, r0");
	AppendNL(vText, "rsq r0.w, r0.w");
	AppendNL(vText, "mul r0, r0, r0.w");
	// Normalised light dir in r0

	AppendNL(vText, "sub r1, c4, v0");
	AppendNL(vText, "dp3 r1.w, r1, r1");
	AppendNL(vText, "rsq r1.w, r1.w");
	AppendNL(vText, "mul r1, r1, r1.w");
	// Normalised view dir in r1

	AppendNL(vText, "add r2, r0, r1");
	AppendNL(vText, "dp3 r2.w, r2, r2");
	AppendNL(vText, "rsq r2.w, r2.w");
	AppendNL(vText, "mul r2, r2, r2.w");
	// Normalised half vector in r2

	if (desc.m_SpecularType <= ST_LINEAR_APPROX)
	{
		// Transform half vector into lightspace and output in oT1
		AppendNL(vText, "dp3 oT1.x, v3.xyz, r2.xyz");
		AppendNL(vText, "dp3 oT1.y, v4.xyz, r2.xyz");
		AppendNL(vText, "dp3 oT1.z, v2.xyz, r2.xyz");
	}
	else if (desc.m_SpecularType >=  ST_FIXED_HIGH)
	{
		// Work out lightspace
		// LightY = half vector. (r2)
		// LightZ = Tangent x LightY (r6)
		AppendNL(vText, "mul r6, v3.zxyw, r2.yzxw");
		AppendNL(vText, "mad r6, v3.yzxw, r2.zxyw, -r6");
		// Normalise
		AppendNL(vText, "dp3 r6.w, r6, r6");
		AppendNL(vText, "rsq r6.w, r6.w");
		AppendNL(vText, "mul r6, r6, r6.w");
		// LightX = LightY x LightZ (r7)
		AppendNL(vText, "mul r7, r2.zxyw, r6.yzxw");
		AppendNL(vText, "mad r7, r2.yzxw, r6.zxyw, -r7");
		// Normalise
		AppendNL(vText, "dp3 r7.w, r7, r7");
		AppendNL(vText, "rsq r7.w, r7.w");
		AppendNL(vText, "mul r7, r7, r7.w");

		// Work out Tangent in lightspace
		AppendNL(vText, "dp3 oT1.x, v3.xyz, r7.xyz");
		AppendNL(vText, "dp3 oT2.x, v3.xyz, r2.xyz");
		AppendNL(vText, "dp3 oT3.x, v3.xyz, r6.xyz");
		// Work out Bi-normal in lightspace
		AppendNL(vText, "dp3 oT1.y, v4.xyz, r7.xyz");
		AppendNL(vText, "dp3 oT2.y, v4.xyz, r2.xyz");
		AppendNL(vText, "dp3 oT3.y, v4.xyz, r6.xyz");
		// Work out Normal in lightspace
		AppendNL(vText, "dp3 oT1.z, v2.xyz, r7.xyz");
		AppendNL(vText, "dp3 oT2.z, v2.xyz, r2.xyz");
		AppendNL(vText, "dp3 oT3.z, v2.xyz, r6.xyz");
	}


	//PIXEL
	char* pText = NULL;
	AppendPStart(pText);
	if (desc.m_SpecularType == ST_LINEAR_APPROX)
	{
		AppendNL(pText, "def c3, 0.75, 0.75, 0.75, 1.0");
	}
	if (desc.m_SpecularType >= ST_FIXED_HIGH)
	{
		AppendNL(pText, "def c4, -0.33333, 0.0, 0.33333, 0.66666");
		AppendNL(pText, "def c5, 0.0, 0.33333, 0.66666, 1.0");
		AppendNL(pText, "def c6, 0.75, 0.75, 0.75, 0.75");
	}
	AppendNL(pText, "tex t0");
	if (desc.m_SpecularType <= ST_LINEAR_APPROX)
	{
		AppendNL(pText, "tex t1");
		AppendNL(pText, "dp3_sat r0, t0_bx2, t1_bx2");
		if (desc.m_SpecularType == ST_POW2)
		{
			AppendNL(pText, "mul r0, r0, r0");
		}
		else if (desc.m_SpecularType == ST_POW4)
		{
			AppendNL(pText, "mul r0, r0, r0");
			AppendNL(pText, "mul r0, r0, r0");
		}
		else if (desc.m_SpecularType == ST_POW8)
		{
			AppendNL(pText, "mul r0, r0, r0");
			AppendNL(pText, "mul r0, r0, r0");
			AppendNL(pText, "mul r0, r0, r0");
		}
		else if (desc.m_SpecularType == ST_POW16)
		{
			AppendNL(pText, "mul r0, r0, r0");
			AppendNL(pText, "mul r0, r0, r0");
			AppendNL(pText, "mul r0, r0, r0");
			AppendNL(pText, "mul r0, r0, r0");
		}
		else if (desc.m_SpecularType == ST_POW32)
		{
			AppendNL(pText, "mul r0, r0, r0");
			AppendNL(pText, "mul r0, r0, r0");
			AppendNL(pText, "mul r0, r0, r0");
			AppendNL(pText, "mul r0, r0, r0");
			AppendNL(pText, "mul r0, r0, r0");
		}
		else if (desc.m_SpecularType == ST_LINEAR_APPROX)
		{
			AppendNL(pText, "sub_sat r0, r0, c3");
			AppendNL(pText, "mul_x4 r0, r0, c3.a");
			AppendNL(pText, "mul r0, r0, r0");
			AppendNL(pText, "mul r0, r0, r0");
		}
	}
	else if (desc.m_SpecularType ==  ST_FIXED_HIGH)
	{
		AppendNL(pText, "texm3x3pad t1, t0_bx2");
		AppendNL(pText, "texm3x3pad t2, t0_bx2");
		AppendNL(pText, "texm3x3tex t3, t0_bx2");

		AppendNL(pText, "sub_sat r0, c3.a, c4");
		AppendNL(pText, "mul_x4_sat r0, r0, c6");
		AppendNL(pText, "sub_sat r1, c3.a, c5");
		AppendNL(pText, "mul_x4_sat r1, r1, c6");
		AppendNL(pText, "sub_sat r1, r0, r1");
		AppendNL(pText, "dp3 r0, t3, r1");
		AppendNL(pText, "mad r0, t3.a, r1.a, r0");
	}
	else if (desc.m_SpecularType ==  ST_PER_PIXEL)
	{
		AppendNL(pText, "texm3x3pad t1, t0_bx2");
		AppendNL(pText, "texm3x3pad t2, t0_bx2");
		AppendNL(pText, "texm3x3tex t3, t0_bx2");

		AppendNL(pText, "sub_sat r0, t0.a, c4");
		AppendNL(pText, "mul_x4_sat r0, r0, c6");
		AppendNL(pText, "sub_sat r1, t0.a, c5");
		AppendNL(pText, "mul_x4_sat r1, r1, c6");
		AppendNL(pText, "sub_sat r1, r0, r1");
		AppendNL(pText, "dp3 r0, t3, r1");
		AppendNL(pText, "mad r0, t3.a, r1.a, r0");
	}
	AppendNL(pText, "mul r0, r0, c2");

	AssembleShaders(vText, pText, vShader, pShader, pd3dDevice);
}

void ShaderUtils::Append(char*& text, char* add)
{
	char* old = text;
	if (old)
	{
		text = new char[strlen(old) + strlen(add) + 1];
		strcpy(text, old);
		strcat(text, add);
		delete [] old;
	}
	else
	{
		text = new char[strlen(add) + 1];
		strcpy(text, add);
	}
}

void ShaderUtils::AppendNL(char*& text, char* add)
{
	char* old = text;
	if (old)
	{
		text = new char[strlen(old) + strlen(add) + 2];
		strcpy(text, old);
		strcat(text, add);
		strcat(text, "\n");
		delete [] old;
	}
	else
	{
		text = new char[strlen(add) + 2];
		strcpy(text, add);
		strcat(text, "\n");
	}
}

void ShaderUtils::AssembleShaders(
						char* vText, 
						char* pText, 
						IDirect3DVertexShader9** vShader, 
						IDirect3DPixelShader9** pShader, 
						LPDIRECT3DDEVICE9 pd3dDevice)
{
	HRESULT hr;

	LPD3DXBUFFER pCode;
	LPD3DXBUFFER pError;

	// Vertex shader
	if ( FAILED( hr = D3DXAssembleShader(vText, strlen(vText), NULL, NULL, 0, &pCode, &pError)))
	{
		const char* errorText = (const char*)(pError->GetBufferPointer());
		return;
	}
	if ( FAILED( hr = pd3dDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(), vShader )))
	{
		return;
	}
	pCode->Release();

	// Pixel shader
	if ( FAILED( hr = D3DXAssembleShader(pText, strlen(pText), NULL, NULL, 0, &pCode, &pError)))
	{
		const char* errorText = (const char*)(pError->GetBufferPointer());
		return;
	}
	if ( FAILED( hr = pd3dDevice->CreatePixelShader( (DWORD*)pCode->GetBufferPointer(), pShader )))
	{
		return;
	}
	pCode->Release();
}

void ShaderUtils::AppendVStart(char*& text)
{
	AppendNL(text, "vs_1_1");
	AppendNL(text, "dcl_position0 v0");
	AppendNL(text, "dcl_texcoord0 v1");
	AppendNL(text, "dcl_normal v2");
	AppendNL(text, "dcl_tangent v3");
	AppendNL(text, "dcl_binormal v4");
}

void ShaderUtils::AppendPStart(char*& text)
{
	AppendNL(text, "ps_1_1");
}
