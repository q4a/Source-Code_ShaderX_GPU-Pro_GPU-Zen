//-----------------------------------------------------------------------------
// File: ShaderUtils.h
//
// Desc: 
//-----------------------------------------------------------------------------
#ifndef SHADERUTILS_H
#define SHADERUTILS_H
#include <tchar.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "Mesh.h"


//-----------------------------------------------------------------------------
// Name: class ShaderUtils
// Desc: 
//-----------------------------------------------------------------------------
class ShaderUtils
{
public:
	static void CreateDiffuseShaders(
						const MeshDesc& desc, 
						IDirect3DVertexShader9** vShader, 
						IDirect3DPixelShader9** pShader, 
						LPDIRECT3DDEVICE9 pd3dDevice);

	static void CreateSpecularShaders(
						const MeshDesc& desc, 
						IDirect3DVertexShader9** vShader, 
						IDirect3DPixelShader9** pShader, 
						LPDIRECT3DDEVICE9 pd3dDevice);

	static void Append(char*& text, char* add);
	static void AppendNL(char*& text, char* add);

private:
    ShaderUtils();
    ~ShaderUtils();

	static void AppendVStart(char*& text);
	static void AppendPStart(char*& text);

	static void AssembleShaders(
						char* vText, 
						char* pText, 
						IDirect3DVertexShader9** vShader, 
						IDirect3DPixelShader9** pShader, 
						LPDIRECT3DDEVICE9 pd3dDevice);
};

#endif



