/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  ShaderManager.h

Copyright NVIDIA Corporation 2002
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
*AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.


Comments:

ShaderManager - a class for loading various pixel & vertex shaders.  Takes
	care of not loading shaders twice if they are loaded multiple times.



******************************************************************************/


#ifndef __NV_ShaderManager_89_H
#define	__NV_ShaderManager_89_H


#include <NVD3DVers.h>


#pragma warning(disable: 4786)
#include <vector>
#pragma warning(disable: 4786)
#include <string>

using namespace std;


////////////////////////////////////////////////


typedef DWORD		SM_SHADER_INDEX;


typedef enum tagSM_SHADERTYPE
{
	SM_SHADERTYPE_VERTEX = 0,
	SM_SHADERTYPE_PIXEL = 1,
	SM_SHADERTYPE_FORCEDWORD = 0xFFFFFFFF
} SM_SHADERTYPE;



struct ShaderDescription
{

	string		m_ShaderDefinitionFile;	// text file name (.nvv, .nvp file)
	string		m_ShaderObjectFile;		// compiled shader object file (.vso, .pso)
										//  as created by nvasm or other assembler

#ifdef NV_USING_D3D9
	void				*	m_pShader;	// Pointer to shader object
#endif
#ifdef NV_USING_D3D8
	DWORD					m_dwShader;	// handle to shader created by device
#endif

	SM_SHADERTYPE	m_Type;
};



////////////////////////////////////////////////


class ShaderManager
{
public:
	// Function that searches for a file name and returns the full path to the file.
	typedef std::string (*GetFilePathCallback)(const std::string &);

private:
	struct ShaderIndirection
	{
		DWORD				m_dwShaderIndex;	// index to m_Shaders
		SM_SHADER_INDEX *	m_pUserIndex;		// Pointer to user's SM_SHADER_INDEX
	};


	std::vector < ShaderDescription >	m_Shaders;			// m_Shaders[0] is always = 0
	std::vector < SM_SHADER_INDEX >		m_ShaderIndices;	// indirection to m_Shaders
											// Programs hold an index to this array for
											// their shaders.  Each element holds an index
											// into m_Shaders and the address of the SM_SHADER_INDEX
											// that the manager returned to the program, so the 
											// indices can be reordered by the manager.
	std::vector< ShaderIndirection >	m_ShaderIndirections;
	
	NV_D3DDEVICETYPE		* m_pD3DDev;
	GetFilePathCallback		m_pGetFilePathCallbackSM;

	HRESULT CreateShader( LPD3DXBUFFER pbufShader,
							const DWORD * pDeclaration,
							SM_SHADERTYPE ShaderType,
							ShaderDescription * pDesc );


public:

	ShaderManager();
	~ShaderManager();

	HRESULT Free();
	HRESULT Initialize( NV_D3DDEVICETYPE* pDev, GetFilePathCallback = NULL );

	// Set pointer to function which searches for full file path based on
	//  short file name
	void	SetFilePathCallback( GetFilePathCallback pFilePathCallback );

	bool	IsShaderLoaded( const std::string & strFilePath, SM_SHADER_INDEX * out_index );

	HRESULT HandleErrorString( const std::string & strError );

	std::string	GetFilePath( const std::string & file_name );


	// assemble shader using DX runtime
	HRESULT LoadAndAssembleShader( const std::string & strFilePath,
									SM_SHADERTYPE ShaderType,
									SM_SHADER_INDEX * outIndex );

	HRESULT LoadAndAssembleShader( const std::string & strFilePath,
									const DWORD * pDeclaration,
									DWORD Usage,
									SM_SHADERTYPE ShaderType,
									SM_SHADER_INDEX * outIndex );

	// Create shader from text buffer in memory
	// Text is shader assembly code
	HRESULT LoadAndCreateShaderFromMemory( const char * program,
											const char * shader_name,
											SM_SHADERTYPE ShaderType,
											SM_SHADER_INDEX * outIndex );

	HRESULT LoadAndCreateShaderFromMemory( const char * program,
											const char * shader_name,
											const DWORD * pDeclaration,
											DWORD Usage,
											SM_SHADERTYPE ShaderType,
											SM_SHADER_INDEX * outIndex );

	// Create shader from pre-compiled object file.
	// strSourceFile is not required and is just a convenient name to track
	HRESULT	LoadAndCreateShader(	const std::string & strFilePath,
									const std::string & strSourceFile,
									const DWORD * pDeclaration,
									DWORD Usage,
									SM_SHADERTYPE ShaderType,
									SM_SHADER_INDEX * outIndex	);

	HRESULT	LoadAndCreateShader(	const std::string & strFilePath,
									const std::string & strSourceFile,
									SM_SHADERTYPE ShaderType,
									SM_SHADER_INDEX * outIndex	);

	// Same as above but with no strSourceFile arg
	HRESULT	LoadAndCreateShader(	const std::string & strFilePath,
									const DWORD * pDeclaration,
									DWORD Usage,
									SM_SHADERTYPE ShaderType,
									SM_SHADER_INDEX * outIndex	);

	HRESULT	LoadAndCreateShader(	const std::string & strFilePath,
									SM_SHADERTYPE ShaderType,
									SM_SHADER_INDEX * outIndex	);



	HRESULT SetShader( SM_SHADER_INDEX index );		// sets device's vertex or pixel shader

	std::string GetShaderObjName( SM_SHADER_INDEX index );

	void	ListAllShaders();
	HRESULT CheckNoVShaderDecl( SM_SHADERTYPE ShaderType, char * func_name );

};



#endif			// __NV_ShaderManager_89_H
