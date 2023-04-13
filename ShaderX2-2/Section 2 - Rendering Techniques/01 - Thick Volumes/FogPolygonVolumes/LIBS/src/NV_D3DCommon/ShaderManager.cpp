/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  ShaderManager.cpp

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


******************************************************************************/


#include "NVD3DVers.h"
#include <NV_D3DCommon/ShaderManager.h>

#include <shared/NV_Error.h>
#include <shared/NV_Common.h>


#ifdef NV_D3DCOMMON_NOASSERT
	#undef ASSERT_AND_RET_IF_FAILED
	#define ASSERT_AND_RET_IF_FAILED(h)		return( h );

#endif



////////////////////////////////////////////////////////


ShaderManager::ShaderManager()
{
	m_Shaders.clear();
	m_ShaderIndices.clear();
	m_ShaderIndirections.clear();

	m_pD3DDev = NULL;
}

ShaderManager::~ShaderManager()
{
	Free();
}


////////////////////////////////////////////////////////


HRESULT ShaderManager::Free()
{
	int i;

	if( m_pD3DDev != NULL )
	{
		m_pD3DDev->SetPixelShader( 0 );

#ifdef NV_USING_D3D8
		m_pD3DDev->SetVertexShader( D3DFVF_XYZ | D3DFVF_NORMAL );
#endif
#ifdef NV_USING_D3D9
		m_pD3DDev->SetVertexShader( NULL );
		m_pD3DDev->SetFVF( D3DFVF_XYZ );
#endif

		// Free vertex & pixel shaders
		// Start from 1.  
		// Shader 0 is always null shader

		for( i=1; i < m_Shaders.size() ; i++ )
		{
#ifdef NV_USING_D3D9
			IDirect3DVertexShader9	* pvsh;
			IDirect3DPixelShader9	* ppsh;

			switch( m_Shaders[i].m_Type )
			{
			case	SM_SHADERTYPE_VERTEX :
				pvsh = (IDirect3DVertexShader9*) m_Shaders[i].m_pShader;
				SAFE_RELEASE( pvsh );
				m_Shaders[i].m_pShader = NULL;
				break;

			case	SM_SHADERTYPE_PIXEL :
				ppsh = (IDirect3DPixelShader9*) m_Shaders[i].m_pShader;
				SAFE_RELEASE( ppsh );
				m_Shaders[i].m_pShader = NULL;
				break;
			}
#endif
#ifdef NV_USING_D3D8
			switch( m_Shaders[i].m_Type )
			{
			case	SM_SHADERTYPE_VERTEX:
				m_pD3DDev->DeleteVertexShader( m_Shaders[i].m_dwShader );
				m_Shaders[i].m_dwShader = 0;

				break;

			case	SM_SHADERTYPE_PIXEL:
				m_pD3DDev->DeletePixelShader( m_Shaders[i].m_dwShader );
				m_Shaders[i].m_dwShader = 0;

				break;
			}
#endif
		}

		SAFE_RELEASE( m_pD3DDev );		// we AddRef()'d in Initialize
	}

	m_Shaders.clear();
	m_ShaderIndices.clear();
	m_ShaderIndirections.clear();

	return( S_OK );
}


HRESULT ShaderManager::Initialize( NV_D3DDEVICETYPE* pDev, GetFilePathCallback file_path_callback )
{
	assert( pDev != NULL );

//	FMsg("///////////////////////////////////////////\n");
//	FMsg("Shader Manager init.  Freeing these shaders:\n");
//	ListAllShaders();
//	FMsg("///////////////////////////////////////////\n");

	Free();

	m_pD3DDev = pDev;
	m_pD3DDev->AddRef();


	m_pGetFilePathCallbackSM = file_path_callback;


	// Add one "NULL" shader with handle value of 0
	//  so that SetShader(0) sets a null shader
	//  to disable the vertex or pixel shader.
	//@@@ doesn't work for pixel shaders!!

	ShaderDescription Desc;
	Desc.m_ShaderDefinitionFile = "NULL Shader";
	Desc.m_ShaderObjectFile		= "NULL Shader";
	Desc.m_Type					= SM_SHADERTYPE_VERTEX;

#ifdef NV_USING_D3D9
	Desc.m_pShader		= NULL;
#endif
#ifdef NV_USING_D3D8
	Desc.m_dwShader		= NULL;
#endif

	m_Shaders.push_back( Desc );


	return( S_OK );
}


void ShaderManager::SetFilePathCallback( GetFilePathCallback pFilePathCallback )
{
	m_pGetFilePathCallbackSM = pFilePathCallback;
}


std::string ShaderManager::GetShaderObjName( SM_SHADER_INDEX index )
{
	std::string name;

	if( index < m_Shaders.size() && index >= 0 )
	{
		return( m_Shaders.at(index).m_ShaderObjectFile );
	}
	else
	{
		FMsg("Index out of bounds!\n");
		return( "Index out of bounds!" );
	}

	return( "" );
}


void	ShaderManager::ListAllShaders()
{
	int i;

	FMsg("\nList of all ShaderManager shaders: %d\n", m_Shaders.size() );
	for( i = 0; i < m_Shaders.size(); i++ )
	{
		FMsg("Shader %3.3d   %s\n", i, GetShaderObjName(i).c_str() );
	}
}



std::string	ShaderManager::GetFilePath( const std::string & file_name )
{
	std::string filepath;
	if( m_pGetFilePathCallbackSM != NULL )
	{
		filepath = (*m_pGetFilePathCallbackSM)( file_name );
	}
	else
	{
		filepath = file_name;
	}
	return( filepath );
}



HRESULT ShaderManager::CreateShader( LPD3DXBUFFER pbufShader,
										const DWORD * pDeclaration,
										SM_SHADERTYPE ShaderType,
										ShaderDescription * pDesc )
{
	HRESULT hr = S_OK;

	FAIL_IF_NULL( pbufShader );
	FAIL_IF_NULL( pDesc );
	FAIL_IF_NULL( m_pD3DDev );


#ifdef NV_USING_D3D9
	if( ShaderType == SM_SHADERTYPE_VERTEX )
	{
		hr = m_pD3DDev->CreateVertexShader( (DWORD*) pbufShader->GetBufferPointer(),
											 (NV_VERTEXSHADERTYPE*) & pDesc->m_pShader );
	    SAFE_RELEASE( pbufShader );
		ASSERT_AND_RET_IF_FAILED(hr);		
	}
	else
	{
		hr = m_pD3DDev->CreatePixelShader( (DWORD*) pbufShader->GetBufferPointer(),
											(NV_PIXELSHADERTYPE*) & pDesc->m_pShader );
	    SAFE_RELEASE( pbufShader );		
		ASSERT_AND_RET_IF_FAILED(hr);				
	}
#endif

#ifdef NV_USING_D3D8
	if( ShaderType == SM_SHADERTYPE_VERTEX )
	{
		FAIL_IF_NULL( pDeclaration );
		hr = m_pD3DDev->CreateVertexShader( pDeclaration,
											(DWORD*) pbufShader->GetBufferPointer(),
											& pDesc->m_dwShader,
											NULL );
	    SAFE_RELEASE( pbufShader );
		ASSERT_AND_RET_IF_FAILED(hr);		
	}
	else
	{
		hr = m_pD3DDev->CreatePixelShader(	(DWORD*) pbufShader->GetBufferPointer(),
											& pDesc->m_dwShader	);
	    SAFE_RELEASE( pbufShader );		
		ASSERT_AND_RET_IF_FAILED(hr);				
	}
#endif

	return( hr );
}



// Creates a vertex shader from a vertex shader text buffer in memory
// The text buffer is an uncompiled vertex shader program

HRESULT ShaderManager::LoadAndCreateShaderFromMemory( const char * program,
													  const char * program_name,
													  SM_SHADERTYPE ShaderType,
													  SM_SHADER_INDEX * outIndex )
{
	if( FAILED( CheckNoVShaderDecl( ShaderType, "LoadAndCreateShaderFromMemory" ) ))
	{
		assert( false );
		*outIndex = 0;
		return( E_FAIL );
	}

	return( LoadAndCreateShaderFromMemory( program,
											program_name,
											NULL,
											NULL,
											ShaderType,
											outIndex ) );
}



HRESULT ShaderManager::LoadAndCreateShaderFromMemory( const char * program,		// ASCII assembly code
														const char * shader_name,
														const DWORD * pDeclaration,
														DWORD Usage,
														SM_SHADERTYPE ShaderType,
														SM_SHADER_INDEX * outIndex )
{
	HRESULT hr = S_OK;
	FAIL_IF_NULL( program );
	FAIL_IF_NULL( outIndex );


	//@@@@@ check if shader exists!



	ShaderDescription	Desc;

	Desc.m_ShaderDefinitionFile = "Created from text buffer";
	Desc.m_Type					= ShaderType;
	Desc.m_ShaderObjectFile		= shader_name;


	////////////

    LPD3DXBUFFER pbufShader, pbufErrors;


//	typedef struct D3DXMACRO {
//		LPCSTR Name;
//		LPCSTR Definition;	};


#ifdef NV_USING_D3D9
	hr = D3DXAssembleShader( program, strlen( program ),
							 NULL,		// D3DXMACRO preprocessor definitions
							 NULL,		// include directives
							 NULL,		// compile option flags
							 & pbufShader,
							 & pbufErrors );
#endif
#ifdef NV_USING_D3D8
	LPD3DXBUFFER pbufConstants;

	hr = D3DXAssembleShader( program, strlen( program ),
								NULL,				// flags
								& pbufConstants,	// constants for vshader
								& pbufShader,
								& pbufErrors );

#endif

	if( FAILED( hr ))
	{
		FMsg("ShaderManager::D3DXAssembleShader Errors:\n");
		if( pbufErrors != NULL )
		{
			HandleErrorString( (char*) pbufErrors->GetBufferPointer() );
		}
		else
		{
			FMsg("Error buffer is NULL!\n");
		}
	    SAFE_RELEASE( pbufShader );
		ASSERT_AND_RET_IF_FAILED( hr );
	}



	hr = CreateShader( pbufShader, pDeclaration, ShaderType, & Desc );


	// Add shader description to the array, set its index and 
	//  return the index
	//@@@@ make function for this!!
	
	m_Shaders.push_back( Desc );
	* outIndex = m_Shaders.size() - 1;

	m_ShaderIndices.push_back( *outIndex );

	return( hr );
}


////////////////////////////////////////////////////////////////
//  Loads a .vso or .pso shader object file from strFilePath.

HRESULT ShaderManager::LoadAndCreateShader(	const std::string & strFilePath,
											SM_SHADERTYPE ShaderType,
											SM_SHADER_INDEX * outIndex )
{
	// Check if D3D lets us get away with no shader decl
	if( FAILED( CheckNoVShaderDecl( ShaderType, "LoadAndCreateShader" ) ))
	{
		assert( false );
		*outIndex = 0;
		return( E_FAIL );
	}

	return( LoadAndCreateShader(	strFilePath,
									NULL,
									NULL,
									ShaderType,
									outIndex	) );
}


HRESULT ShaderManager::LoadAndCreateShader(	const std::string & strFilePath,
											const DWORD * pDeclaration,
											DWORD Usage,
											SM_SHADERTYPE ShaderType,
											SM_SHADER_INDEX * outIndex	)
{
	HRESULT hr;

	std::string filepath = "";
	hr = LoadAndCreateShader( strFilePath, filepath, pDeclaration, Usage, ShaderType, outIndex );

	return( hr );
}



////////////////////////////////////////////////////////////////
//  Loads a compiled shader binary file (.vso or .pso) from strFilePath.
//  Also tracks the original ASCII text file name from strSourceFile, but
//   this is not required.  This could be the .nvv or .nvp file path.

HRESULT ShaderManager::LoadAndCreateShader( const std::string & strFilePath,
										    const std::string & strSourceFile,
											SM_SHADERTYPE ShaderType,
											SM_SHADER_INDEX * outIndex )
{
	if( FAILED( CheckNoVShaderDecl( ShaderType, "LoadAndCreateShader" ) ))
	{
		assert( false );
		*outIndex = 0;
		return( E_FAIL );
	}

	return( LoadAndCreateShader(	strFilePath,
									strSourceFile,
									NULL,
									NULL,
									ShaderType,
									outIndex	) );
}


HRESULT	ShaderManager::LoadAndCreateShader(	const std::string & file_name,
											const std::string & strSourceFile,
											const DWORD * pDeclaration,
											DWORD Usage,
											SM_SHADERTYPE ShaderType,
											SM_SHADER_INDEX * outIndex )
{
	// strSourceFile is optional ASCII assembly code file and not used
	//  for shader creation.

	FAIL_IF_NULL( m_pD3DDev );

	std::string strFilePath;
	strFilePath = GetFilePath( file_name );


	// Check to see if shader has already been loaded & created
	SM_SHADER_INDEX index;
	if( IsShaderLoaded( strFilePath, & index ))
	{
		FDebug("Shader [%s] already loaded and has index %u\n", strFilePath.c_str(), index );	
		*outIndex = index;
		return( S_OK );
	}


	try	{
		HANDLE hFile;
		HRESULT hr;

		ShaderDescription	Desc;

		Desc.m_ShaderDefinitionFile = strSourceFile.c_str();
		Desc.m_ShaderObjectFile		= strFilePath.c_str();
		Desc.m_Type					= ShaderType;


		hFile = CreateFile( strFilePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if(hFile == INVALID_HANDLE_VALUE) 
		{
			FDebug("Could not find file %s\n", strFilePath.c_str() );
			return E_FAIL;
		}
		
		DWORD dwFileSize = GetFileSize( hFile, NULL );
		
		const DWORD* pShader = (DWORD*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwFileSize);
		if (!pShader)
		{
			FMsg( "Failed to allocate memory to load shader %s\n", strFilePath.c_str() );
			return E_FAIL;
		}
		
		ReadFile(hFile, (void*)pShader, dwFileSize, &dwFileSize, NULL);
		
		CloseHandle(hFile);
		
#ifdef NV_USING_D3D9
		if (ShaderType == SM_SHADERTYPE_VERTEX )
		{
			hr = m_pD3DDev->CreateVertexShader( pShader, (NV_VERTEXSHADERTYPE*) & Desc.m_pShader );
		} 
		else if (ShaderType == SM_SHADERTYPE_PIXEL )
		{
			hr = m_pD3DDev->CreatePixelShader( pShader, (NV_PIXELSHADERTYPE*) & Desc.m_pShader );
		}
#endif
#ifdef NV_USING_D3D8
		if (ShaderType == SM_SHADERTYPE_VERTEX)
		{
			FAIL_IF_NULL( pDeclaration );
			hr = m_pD3DDev->CreateVertexShader( pDeclaration, pShader, & Desc.m_dwShader, Usage );
		}
		else if (ShaderType == SM_SHADERTYPE_PIXEL)
		{	
			hr = m_pD3DDev->CreatePixelShader( pShader, & Desc.m_dwShader );
		}
#endif

		
		HeapFree(GetProcessHeap(), 0, (void*)pShader);
		
		if (FAILED(hr))	
		{
			FDebug("Failed to create shader %s\n", strFilePath.c_str() );
			return E_FAIL;
		}

		// Add shader description to the array, set its index and 
		//  return the index
		
		Desc.m_Type					= ShaderType;
		Desc.m_ShaderDefinitionFile = strSourceFile;
		Desc.m_ShaderObjectFile		= strFilePath;

		m_Shaders.push_back( Desc );

		* outIndex = m_Shaders.size() - 1;

		m_ShaderIndices.push_back( *outIndex );


	}
	catch(...)
	{
		FDebug("Error opening file %s\n", strFilePath.c_str() );
		return E_FAIL;
	}

	
	FMsg("LOADED Shader %d name [%s]\n", *outIndex, m_Shaders.at(*outIndex).m_ShaderObjectFile.c_str() );

	return S_OK;
}


///////////////////////////////////////////////////////
// Assemble shader using DX runtime

HRESULT ShaderManager::LoadAndAssembleShader( const std::string & strFilePath,
												SM_SHADERTYPE ShaderType,
												SM_SHADER_INDEX * outIndex )
{

	if( FAILED( CheckNoVShaderDecl( ShaderType, "LoadAndAssembleShader" ) ))
	{
		assert( false );
		*outIndex = 0;
		return( E_FAIL );
	}

	return( LoadAndAssembleShader(	strFilePath,
									NULL,
									NULL,
									ShaderType,
									outIndex	) );

}


HRESULT ShaderManager::LoadAndAssembleShader( const std::string & file_name,
												const DWORD * pDeclaration,
												DWORD Usage,
												SM_SHADERTYPE ShaderType,
												SM_SHADER_INDEX * outIndex )


{
	// assemble shader using DX runtime

	HRESULT hr = S_OK;
	FAIL_IF_NULL( m_pD3DDev );
	
	std::string strFilePath;
	strFilePath = GetFilePath( file_name );


/*
//@@@@@  this is mixing up the shaders for some reason
	// Check to see if shader has already been loaded & created
	SM_SHADER_INDEX index;
	if( IsShaderLoaded( strFilePath, & index ))
	{
		FDebug("Shader [%s] already assembled and has index %u\n", strFilePath.c_str(), index );	
		*outIndex = index;
		return( S_OK );
	}
//*/


	ShaderDescription	Desc;
    LPD3DXBUFFER pbufShader, pbufErrors;


#ifdef NV_USING_D3D9
	//flags:
	//	D3DXSHADER_DEBUG
	//	D3DXSHADER_SKIPVALIDATION
	//	D3DXSHADER_SKIPOPTIMIZATION

	hr = D3DXAssembleShaderFromFile( strFilePath.c_str(),
										NULL,			// D3DXMACRO defines
										NULL,			// LPD3DXINcLUDE include, NULL = do #include
										0,				// flags
										&pbufShader,	
										&pbufErrors );
#endif
#ifdef NV_USING_D3D8

	LPD3DXBUFFER pbufConstants;

	hr = D3DXAssembleShaderFromFile( strFilePath.c_str(),
										NULL,				// flags
										&pbufConstants,		// constants
										&pbufShader,
										&pbufErrors );
#endif

	if( FAILED( hr ))
	{
		FMsg("ShaderManager::LoadAndAssembleShader Errors:\n");
		if( pbufErrors != NULL )
		{
			HandleErrorString( (char*) pbufErrors->GetBufferPointer() );
		}
		else
		{
			FMsg("Error buffer is NULL!\n");
		}
	    SAFE_RELEASE( pbufShader );
		ASSERT_AND_RET_IF_FAILED( hr );
	}


	hr = CreateShader( pbufShader, pDeclaration, ShaderType, & Desc );


	Desc.m_ShaderDefinitionFile = strFilePath.c_str();
	Desc.m_ShaderObjectFile		= strFilePath.c_str();
	Desc.m_Type					= ShaderType;

	m_Shaders.push_back( Desc );
	* outIndex = m_Shaders.size() - 1;

	m_ShaderIndices.push_back( *outIndex );


	return( hr );
}







HRESULT ShaderManager::CheckNoVShaderDecl( SM_SHADERTYPE ShaderType, char * func_name )
{
#ifdef NV_USING_D3D8
	if( ShaderType == SM_SHADERTYPE_VERTEX )
	{
		FMsg("ShaderManager::%s(..)  This function can't be used with D3D8\n", func_name );
		FMsg("   D3D8 requires vertex shader declaration & usage parameters!\n");
		return( E_FAIL );
	}
#endif

	return( S_OK );
}


HRESULT ShaderManager::SetShader( SM_SHADER_INDEX index )
{
	// sets device's vertex or pixel shader
	HRESULT hr = S_OK;
	FAIL_IF_NULL( m_pD3DDev );

	if( index < m_Shaders.size() )
	{
		SM_SHADERTYPE ShaderType;
		ShaderType	= m_Shaders.at(index).m_Type;

		switch( ShaderType )
		{
#ifdef NV_USING_D3D9
		case SM_SHADERTYPE_VERTEX :
			m_pD3DDev->SetVertexShader( (IDirect3DVertexShader9*) (m_Shaders.at(index).m_pShader) );
			break;

		case SM_SHADERTYPE_PIXEL :
			m_pD3DDev->SetPixelShader( (IDirect3DPixelShader9*) (m_Shaders.at(index).m_pShader) );
			break;
#endif
#ifdef NV_USING_D3D8
		case SM_SHADERTYPE_VERTEX :
			m_pD3DDev->SetVertexShader( m_Shaders.at(index).m_dwShader );
			break;

		case SM_SHADERTYPE_PIXEL :
			m_pD3DDev->SetPixelShader( m_Shaders.at(index).m_dwShader );
			break;
#endif

		default:
			assert( false );
			hr = E_FAIL;
			break;

		}

	}
	else
	{
		FMsg("ShaderManager Index out of bounds! %u\n", index );
		assert( false );
		hr = E_FAIL;
	}

	return( hr );


}



/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////



HRESULT ShaderManager::HandleErrorString( const std::string & strError )
{
	HRESULT hr = S_OK;

	std::string::size_type i;
	i = strError.find( "error" );

	string err;

	int column_width = 90;

	if( i > 0 )
	{
		err = strError.substr( i, strError.size() );

		i = err.find( " ", column_width );

		while( i > 0 && i < err.size() )
		{
			string spacer = "\n     ";
			err.insert( i, spacer );
			i = err.find( " ", i + 2 + spacer.size() + column_width );
		}

	}
	else
	{
		err = strError;
	}

	FMsg("[\n%s]\n", err.c_str() );

	return( hr );
}



bool ShaderManager::IsShaderLoaded( const std::string & strFilePath, SM_SHADER_INDEX * out_index )
{
	// Checks if shader has already been loaded and created from the source file
	//  and returns the index of the created shader in out_index if it has.

	bool is_loaded = false;
	string filename;
	int i;


	for( i=0; i < m_Shaders.size(); i++ )
	{
		filename = m_Shaders.at(i).m_ShaderObjectFile;

		if( filename == strFilePath )
		{
			is_loaded = true;

			// find the index in m_ShaderIndices
			int ind;
			for( ind=0; ind < m_ShaderIndices.size(); ind++ )
			{
				if( m_ShaderIndices.at(ind) == i )
				{
					*out_index = ind;
					return( is_loaded );
				}
			}

			// if not found, keep searching
			is_loaded = false;
		}
	}

	return( false );
}


