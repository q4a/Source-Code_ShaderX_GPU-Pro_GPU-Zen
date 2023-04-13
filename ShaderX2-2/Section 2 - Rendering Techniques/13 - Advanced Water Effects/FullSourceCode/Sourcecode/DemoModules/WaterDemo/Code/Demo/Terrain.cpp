//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			WaterDemo
//*				File:				Terrain.cpp
//*
//********************************************************************************************************************
//*
//*				Author:				Kurt Pelzer
//*									Copyright (c) 2003 Kurt Pelzer
//*									All rights reserved.
//*
//********************************************************************************************************************
//********************************************************************************************************************




#include		"Terrain.h"
#include		<direct.h>




//********************************************************************************************************************
//*				TERRAIN
//********************************************************************************************************************

TERRAIN::TERRAIN												(	IGN_OBJIF_GfxContext*	a_pGfxContextInterface,
																	IGN_OBJIF_GfxAssistant*	a_pAssistant )
{
	//----------------------------------------------------------|--------------------------------------------SKYDOME-|
	// public method
	// Constructor
	
	m_pAssistant			= a_pAssistant;
	m_pVisual				= NULL;
	m_pVertexData			= NULL;
	m_pIndexData			= NULL;
	m_pShader				= NULL;
	m_pVertexData			= NULL;
	m_pMaterial				= NULL;
	m_pTextureBlock0		= NULL;
	m_pTextureBlock1		= NULL;
	m_pHeightMap			= NULL;
	m_pTerrainImage			= NULL;
	m_fWeightSum			= 0.0f;

	//
	// Initialize the system
	//
	Init( a_pGfxContextInterface );
}


TERRAIN::~TERRAIN												()
{
	//----------------------------------------------------------|-------------------------------------------~SKYDOME-|
	// public method
	// Destructor

	//
	// Finalize the system
	//
	Finit();
}


void TERRAIN::Init												(	IGN_OBJIF_GfxContext*	a_pGfxContextInterface )
{
	//----------------------------------------------------------|-----------------------------------------------Init-|
	// public method
	// Initializes the tester.

	//
	// Create objects in IGN_graphic_sub.dll
	//
	if( NULL!=a_pGfxContextInterface )
	{		
		_chdir( "Media" );
		_chdir( "Textures" );
		a_pGfxContextInterface->CreateImageData( m_pTerrainImage );
		IGN_STRUCT_LayersDesc LayerDesc;
		LayerDesc.m_uiWidth		= 0;
		LayerDesc.m_uiHeight	= 0;
		LayerDesc.m_uiDepth		= 0;
		LayerDesc.m_bCubeMap	= false;
		LayerDesc.m_uiLevels	= 0;
		LayerDesc.m_dwUsage		= 0;
		LayerDesc.m_Format		= IGN_ENUM_PIXELFORMAT_UNKNOWN;
		LayerDesc.m_Pool		= IGN_ENUM_POOLTYPE_MANAGED;
		if( NULL!=m_pTerrainImage )
		{
			m_pTerrainImage->SetUnitNumber( 0 );
			if( NULL!=m_pAssistant )
				m_pAssistant->FillImageFromFile( m_pTerrainImage, LayerDesc, "TerrainMaterial.tga" );
		}
		a_pGfxContextInterface->CreateTextureBlock( m_pTextureBlock0 );
		if( NULL!=m_pTextureBlock0 )
		{
			m_pTextureBlock0->AttachImageData( m_pTerrainImage );
			m_pTextureBlock0->SetColorCalc( IGN_ENUM_TEXOPERATION_MODULATE, IGN_ENUM_TEXARGUMENT_TEXTURE, IGN_ENUM_TEXARGUMENT_CURRENT );
			m_pTextureBlock0->SetAlphaCalc( IGN_ENUM_TEXOPERATION_DISABLE, IGN_ENUM_TEXARGUMENT_TEXTURE, IGN_ENUM_TEXARGUMENT_DIFFUSE );
			m_pTextureBlock0->SetTextureAdressing( IGN_ENUM_TEXADDRESS_WRAP, IGN_ENUM_TEXADDRESS_WRAP, IGN_ENUM_TEXADDRESS_CLAMP );
			m_pTextureBlock0->SetTextureFiltering( IGN_ENUM_TEXFILTER_ANISOTROPIC, IGN_ENUM_TEXFILTER_LINEAR, IGN_ENUM_TEXFILTER_LINEAR );
		}
		a_pGfxContextInterface->CreateTextureBlock( m_pTextureBlock1 );
		if( NULL!=m_pTextureBlock1 )
		{
			m_pTextureBlock1->SetColorCalc( IGN_ENUM_TEXOPERATION_DISABLE, IGN_ENUM_TEXARGUMENT_COLORFACTOR, IGN_ENUM_TEXARGUMENT_DIFFUSE );
			m_pTextureBlock1->SetAlphaCalc( IGN_ENUM_TEXOPERATION_DISABLE, IGN_ENUM_TEXARGUMENT_COLORFACTOR, IGN_ENUM_TEXARGUMENT_DIFFUSE );
		}
		a_pGfxContextInterface->CreateMaterial( m_pMaterial );
		if( NULL!=m_pMaterial )
		{
			m_pMaterial->SetAmbientMaterialSource( IGN_ENUM_MATERIALCOLORSRC_MATERIAL );
			m_pMaterial->SetDiffuseMaterialSource( IGN_ENUM_MATERIALCOLORSRC_MATERIAL );
			m_pMaterial->SetSpecularMaterialSource( IGN_ENUM_MATERIALCOLORSRC_MATERIAL );
			m_pMaterial->SetEmissiveMaterialSource( IGN_ENUM_MATERIALCOLORSRC_MATERIAL );
			IGN_STRUCT_ColorValue Color;
			Color.m_fRed	=	1.0f;
			Color.m_fGreen	=	1.0f;
			Color.m_fBlue	=	1.0f;
			Color.m_fAlpha	=	1.0f;
			m_pMaterial->SetAmbientReflection( Color );
			m_pMaterial->SetDiffuseReflection( Color );
			Color.m_fRed	=	0.0f;
			Color.m_fGreen	=	0.0f;
			Color.m_fBlue	=	0.0f;
			Color.m_fAlpha	=	0.0f;
			m_pMaterial->SetSpecularReflection( Color, 0.0f );
			m_pMaterial->SetEmission( Color );
			m_pMaterial->AttachTextureBlock( 0, m_pTextureBlock0 );
			m_pMaterial->AttachTextureBlock( 1, m_pTextureBlock1 );
		}
		a_pGfxContextInterface->CreateShader( m_pShader );
		if( NULL!=m_pShader )
		{
			m_pShader->AttachMaterial( m_pMaterial );
		}		
		a_pGfxContextInterface->CreateImageData( m_pHeightMap );
		LayerDesc.m_Pool		= IGN_ENUM_POOLTYPE_SYSTEMMEM;
		if( NULL!=m_pHeightMap )
		{
			if( NULL!=m_pAssistant )
				m_pAssistant->FillImageFromFile( m_pHeightMap, LayerDesc, "TerrainHeightmap.tga" );
		}
		a_pGfxContextInterface->CreateVertexData( m_pVertexData );
		if( NULL!=m_pVertexData )
		{
			IGN_STRUCT_VertexBufDesc VertexBufferDescription;
			VertexBufferDescription.m_uiLength	= constTerrainWidth*constTerrainDepth*32;
			VertexBufferDescription.m_dwUsage	= 0;
			VertexBufferDescription.m_dwFVF		= IGNFVF_XYZ|IGNFVF_NORMAL|IGNFVF_TEX1|IGNFVF_TEXCOORDSIZE2_0;
			VertexBufferDescription.m_Pool		= IGN_ENUM_POOLTYPE_MANAGED;
			m_pVertexData->CreateBuffer( VertexBufferDescription );

			//
			// Use the heigth map for terrain generation
			//
			structVertexFormatTerrain Terrain[constTerrainWidth][constTerrainDepth];
			IGNINT iRowPitch;
			IGNINT iSlicePitch;
			IGNU32* pBits;
			if( NULL!=m_pHeightMap )
			{
				//
				// Fill position and texture coordiantes
				//
				m_pHeightMap->LockLayer( 0, IGN_ENUM_CUBEMAPFACE_NONE, IGNLOCKFLAG_READONLY, iRowPitch, iSlicePitch, reinterpret_cast<void**>( &pBits ) );
				for( IGNU32 u32ZIndex = 0; u32ZIndex<constTerrainDepth; u32ZIndex++ )
				{
					for( IGNU32 u32XIndex = 0; u32XIndex<constTerrainWidth; u32XIndex++ )
					{
						Terrain[u32XIndex][u32ZIndex].m_Position.m_fX	=	-constTerrainWidth + 2.0f*u32XIndex;
						Terrain[u32XIndex][u32ZIndex].m_Position.m_fY	=	constTerrainBaseHeight + (0x000000ff&(*(pBits+u32ZIndex*(iRowPitch/4)+u32XIndex))) * constTerrainHeightmapScale;
						Terrain[u32XIndex][u32ZIndex].m_Position.m_fZ	=	-constTerrainDepth + 2.0f*u32ZIndex;
						Terrain[u32XIndex][u32ZIndex].m_TexCoord0_U		=	u32XIndex * constTerrainTextureUScale;
						Terrain[u32XIndex][u32ZIndex].m_TexCoord0_V		=	u32ZIndex * constTerrainTextureVScale;
					}
				}

				//
				// Calculate all face normals
				//
				for( u32ZIndex = 0; u32ZIndex<constTerrainDepth; u32ZIndex++ )
				{
					for( IGNU32 u32XIndex = 0; u32XIndex<constTerrainWidth; u32XIndex++ )
					{
						if( constTerrainWidth-1==u32XIndex )
						{
							Terrain[u32XIndex][u32ZIndex].m_Normal.m_fX		=	Terrain[u32XIndex-1][u32ZIndex].m_Normal.m_fX;
							Terrain[u32XIndex][u32ZIndex].m_Normal.m_fY		=	Terrain[u32XIndex-1][u32ZIndex].m_Normal.m_fY;
							Terrain[u32XIndex][u32ZIndex].m_Normal.m_fZ		=	Terrain[u32XIndex-1][u32ZIndex].m_Normal.m_fZ;
						}
						else if( constTerrainDepth-1==u32ZIndex )
						{
							Terrain[u32XIndex][u32ZIndex].m_Normal.m_fX		=	Terrain[u32XIndex][u32ZIndex-1].m_Normal.m_fX;
							Terrain[u32XIndex][u32ZIndex].m_Normal.m_fY		=	Terrain[u32XIndex][u32ZIndex-1].m_Normal.m_fY;
							Terrain[u32XIndex][u32ZIndex].m_Normal.m_fZ		=	Terrain[u32XIndex][u32ZIndex-1].m_Normal.m_fZ;
						}
						else
						{
							IGN_STRUCT_3DVector	VectorX;
							VectorX.m_fX	= Terrain[u32XIndex+1][u32ZIndex].m_Position.m_fX - Terrain[u32XIndex][u32ZIndex].m_Position.m_fX;
							VectorX.m_fY	= Terrain[u32XIndex+1][u32ZIndex].m_Position.m_fY - Terrain[u32XIndex][u32ZIndex].m_Position.m_fY;
							VectorX.m_fZ	= Terrain[u32XIndex+1][u32ZIndex].m_Position.m_fZ - Terrain[u32XIndex][u32ZIndex].m_Position.m_fZ;
							IGN_STRUCT_3DVector	VectorZ;
							VectorZ.m_fX	= Terrain[u32XIndex][u32ZIndex+1].m_Position.m_fX - Terrain[u32XIndex][u32ZIndex].m_Position.m_fX;
							VectorZ.m_fY	= Terrain[u32XIndex][u32ZIndex+1].m_Position.m_fY - Terrain[u32XIndex][u32ZIndex].m_Position.m_fY;
							VectorZ.m_fZ	= Terrain[u32XIndex][u32ZIndex+1].m_Position.m_fZ - Terrain[u32XIndex][u32ZIndex].m_Position.m_fZ;
							Terrain[u32XIndex][u32ZIndex].m_Normal.m_fX		=	VectorZ.m_fY*VectorX.m_fZ - VectorZ.m_fZ*VectorX.m_fY;
							Terrain[u32XIndex][u32ZIndex].m_Normal.m_fY		=	VectorZ.m_fZ*VectorX.m_fX - VectorZ.m_fX*VectorX.m_fZ;
							Terrain[u32XIndex][u32ZIndex].m_Normal.m_fZ		=	VectorZ.m_fX*VectorX.m_fY - VectorZ.m_fY*VectorX.m_fX;
						}
						Terrain[u32XIndex][u32ZIndex].m_Normal.Normalize();
					}
				}

				//
				// Smooth all normals
				//
				IGN_STRUCT_3DVector SmoothedAreaNormal[constTerrainWidth][constTerrainDepth];
				for( u32ZIndex = 2; u32ZIndex<constTerrainDepth-2; u32ZIndex++ )
				{
					for( IGNU32 u32XIndex = 2; u32XIndex<constTerrainWidth-2; u32XIndex++ )
					{
						SmoothedAreaNormal[u32XIndex][u32ZIndex].m_fX
														=	(	Terrain[u32XIndex-1][u32ZIndex-1].m_Normal.m_fX +
																Terrain[u32XIndex+0][u32ZIndex-1].m_Normal.m_fX + 
																Terrain[u32XIndex+1][u32ZIndex-1].m_Normal.m_fX + 
																Terrain[u32XIndex-1][u32ZIndex+0].m_Normal.m_fX + 
																Terrain[u32XIndex+1][u32ZIndex+0].m_Normal.m_fX +
																Terrain[u32XIndex-1][u32ZIndex+1].m_Normal.m_fX + 
																Terrain[u32XIndex+0][u32ZIndex+1].m_Normal.m_fX + 
																Terrain[u32XIndex+1][u32ZIndex+1].m_Normal.m_fX + 
																Terrain[u32XIndex-2][u32ZIndex-2].m_Normal.m_fX +
																Terrain[u32XIndex-1][u32ZIndex-2].m_Normal.m_fX + 
																Terrain[u32XIndex+0][u32ZIndex-2].m_Normal.m_fX + 
																Terrain[u32XIndex+1][u32ZIndex-2].m_Normal.m_fX + 
																Terrain[u32XIndex+2][u32ZIndex-2].m_Normal.m_fX +
																Terrain[u32XIndex-2][u32ZIndex+2].m_Normal.m_fX +
																Terrain[u32XIndex-1][u32ZIndex+2].m_Normal.m_fX + 
																Terrain[u32XIndex+0][u32ZIndex+2].m_Normal.m_fX + 
																Terrain[u32XIndex+1][u32ZIndex+2].m_Normal.m_fX + 
																Terrain[u32XIndex+2][u32ZIndex+2].m_Normal.m_fX + 
																Terrain[u32XIndex-2][u32ZIndex-1].m_Normal.m_fX + 
																Terrain[u32XIndex+2][u32ZIndex-1].m_Normal.m_fX + 
																Terrain[u32XIndex-2][u32ZIndex+0].m_Normal.m_fX + 
																Terrain[u32XIndex+2][u32ZIndex+0].m_Normal.m_fX + 
																Terrain[u32XIndex-2][u32ZIndex+1].m_Normal.m_fX + 
																Terrain[u32XIndex+2][u32ZIndex+1].m_Normal.m_fX );
						SmoothedAreaNormal[u32XIndex][u32ZIndex].m_fY
														=	(	Terrain[u32XIndex-1][u32ZIndex-1].m_Normal.m_fY +
																Terrain[u32XIndex+0][u32ZIndex-1].m_Normal.m_fY + 
																Terrain[u32XIndex+1][u32ZIndex-1].m_Normal.m_fY + 
																Terrain[u32XIndex-1][u32ZIndex+0].m_Normal.m_fY + 
																Terrain[u32XIndex+1][u32ZIndex+0].m_Normal.m_fY +
																Terrain[u32XIndex-1][u32ZIndex+1].m_Normal.m_fY + 
																Terrain[u32XIndex+0][u32ZIndex+1].m_Normal.m_fY + 
																Terrain[u32XIndex+1][u32ZIndex+1].m_Normal.m_fY + 
																Terrain[u32XIndex-2][u32ZIndex-2].m_Normal.m_fY +
																Terrain[u32XIndex-1][u32ZIndex-2].m_Normal.m_fY + 
																Terrain[u32XIndex+0][u32ZIndex-2].m_Normal.m_fY + 
																Terrain[u32XIndex+1][u32ZIndex-2].m_Normal.m_fY + 
																Terrain[u32XIndex+2][u32ZIndex-2].m_Normal.m_fY +
																Terrain[u32XIndex-2][u32ZIndex+2].m_Normal.m_fY +
																Terrain[u32XIndex-1][u32ZIndex+2].m_Normal.m_fY + 
																Terrain[u32XIndex+0][u32ZIndex+2].m_Normal.m_fY + 
																Terrain[u32XIndex+1][u32ZIndex+2].m_Normal.m_fY + 
																Terrain[u32XIndex+2][u32ZIndex+2].m_Normal.m_fY + 
																Terrain[u32XIndex-2][u32ZIndex-1].m_Normal.m_fY + 
																Terrain[u32XIndex+2][u32ZIndex-1].m_Normal.m_fY + 
																Terrain[u32XIndex-2][u32ZIndex+0].m_Normal.m_fY + 
																Terrain[u32XIndex+2][u32ZIndex+0].m_Normal.m_fY + 
																Terrain[u32XIndex-2][u32ZIndex+1].m_Normal.m_fY + 
																Terrain[u32XIndex+2][u32ZIndex+1].m_Normal.m_fY );
						SmoothedAreaNormal[u32XIndex][u32ZIndex].m_fZ
														=	(	Terrain[u32XIndex-1][u32ZIndex-1].m_Normal.m_fZ +
																Terrain[u32XIndex+0][u32ZIndex-1].m_Normal.m_fZ + 
																Terrain[u32XIndex+1][u32ZIndex-1].m_Normal.m_fZ + 
																Terrain[u32XIndex-1][u32ZIndex+0].m_Normal.m_fZ + 
																Terrain[u32XIndex+1][u32ZIndex+0].m_Normal.m_fZ +
																Terrain[u32XIndex-1][u32ZIndex+1].m_Normal.m_fZ + 
																Terrain[u32XIndex+0][u32ZIndex+1].m_Normal.m_fZ + 
																Terrain[u32XIndex+1][u32ZIndex+1].m_Normal.m_fZ + 
																Terrain[u32XIndex-2][u32ZIndex-2].m_Normal.m_fZ +
																Terrain[u32XIndex-1][u32ZIndex-2].m_Normal.m_fZ + 
																Terrain[u32XIndex+0][u32ZIndex-2].m_Normal.m_fZ + 
																Terrain[u32XIndex+1][u32ZIndex-2].m_Normal.m_fZ + 
																Terrain[u32XIndex+2][u32ZIndex-2].m_Normal.m_fZ +
																Terrain[u32XIndex-2][u32ZIndex+2].m_Normal.m_fZ +
																Terrain[u32XIndex-1][u32ZIndex+2].m_Normal.m_fZ + 
																Terrain[u32XIndex+0][u32ZIndex+2].m_Normal.m_fZ + 
																Terrain[u32XIndex+1][u32ZIndex+2].m_Normal.m_fZ + 
																Terrain[u32XIndex+2][u32ZIndex+2].m_Normal.m_fZ + 
																Terrain[u32XIndex-2][u32ZIndex-1].m_Normal.m_fZ + 
																Terrain[u32XIndex+2][u32ZIndex-1].m_Normal.m_fZ + 
																Terrain[u32XIndex-2][u32ZIndex+0].m_Normal.m_fZ + 
																Terrain[u32XIndex+2][u32ZIndex+0].m_Normal.m_fZ + 
																Terrain[u32XIndex-2][u32ZIndex+1].m_Normal.m_fZ + 
																Terrain[u32XIndex+2][u32ZIndex+1].m_Normal.m_fZ );
						SmoothedAreaNormal[u32XIndex][u32ZIndex].Normalize();
					}
				}
				for( u32ZIndex = 2; u32ZIndex<constTerrainDepth-2; u32ZIndex++ )
				{
					for( IGNU32 u32XIndex = 2; u32XIndex<constTerrainWidth-2; u32XIndex++ )
					{
						Terrain[u32XIndex][u32ZIndex].m_Normal.m_fX += SmoothedAreaNormal[u32XIndex][u32ZIndex].m_fX * 2.0f;
						Terrain[u32XIndex][u32ZIndex].m_Normal.m_fY += SmoothedAreaNormal[u32XIndex][u32ZIndex].m_fY * 2.0f;
						Terrain[u32XIndex][u32ZIndex].m_Normal.m_fZ += SmoothedAreaNormal[u32XIndex][u32ZIndex].m_fZ * 2.0f;
						Terrain[u32XIndex][u32ZIndex].m_Normal.Normalize();
					}
				}
				m_pHeightMap->UnlockLayer( IGN_ENUM_CUBEMAPFACE_NONE, 0 );
			}

			//
			// Transfer terrain data to vertex buffer
			//
			unsigned char* pData = NULL;
			m_pVertexData->LockBuffer( 0, 0, 0, &pData );
			if( NULL!=pData )
			{
				structVertexFormatTerrain* pVertexData = reinterpret_cast<structVertexFormatTerrain*>( pData );
				for( IGNU32 u32ZIndex = 0; u32ZIndex<constTerrainDepth; u32ZIndex++ )
				{
					for( IGNU32 u32XIndex = 0; u32XIndex<constTerrainWidth; u32XIndex++ )
					{
						*pVertexData = Terrain[u32XIndex][u32ZIndex];
						pVertexData++;
					}
				}
			}
			m_pVertexData->UnlockBuffer();
			m_pVertexData->SetStreamNumber( 0 );
		}
		a_pGfxContextInterface->CreateIndexData( m_pIndexData );
		if( NULL!=m_pIndexData )
		{
			IGN_STRUCT_IndexBufDesc IndexBufferDescription;
			IndexBufferDescription.m_uiLength	= (constTerrainDepth-1)*((constTerrainWidth*2+2))*4;
			IndexBufferDescription.m_dwUsage	= IGNUSAGE_WRITEONLY;
			IndexBufferDescription.m_Format		= IGN_ENUM_INDEXFORMAT_INDEX32;
			IndexBufferDescription.m_Pool		= IGN_ENUM_POOLTYPE_MANAGED;
			m_pIndexData->CreateBuffer( IndexBufferDescription );
			unsigned char* pData = NULL;
			m_pIndexData->LockBuffer( 0, 0, 0, &pData );
			if( NULL!=pData )
			{
				unsigned int data[(constTerrainDepth-1)*((constTerrainWidth*2+2))];
				IGNU32 u32Index = 0;
				for( IGNU32 u32IZ = 0; u32IZ<constTerrainDepth-1; u32IZ++ )
				{
					for( IGNU32 u32IX = 0; u32IX<constTerrainWidth; u32IX++ )
					{
						data[u32Index]	= u32IZ*constTerrainWidth + u32IX;
						u32Index++;
						data[u32Index]	= u32IZ*constTerrainWidth + u32IX + constTerrainWidth;
						u32Index++;
					}
					data[u32Index]	= u32IZ*constTerrainWidth + u32IX-1 + constTerrainWidth;
					u32Index++;
					data[u32Index]	= u32IZ*constTerrainWidth + u32IX-1 + 1;
					u32Index++;
				}
				memcpy( pData, data, (constTerrainDepth-1)*((constTerrainWidth*2+2))*4 );
			}
			m_pIndexData->UnlockBuffer();
		}
		a_pGfxContextInterface->CreateVisual( m_pVisual );
		if( NULL!=m_pVisual )
		{
			IGN_STRUCT_DrawCallDesc	DrawCallDesc;
			DrawCallDesc.m_bIndexedDrawCall		= true;
			DrawCallDesc.m_PrimitiveType		= IGN_ENUM_PRIMITIVETYPE_TRIANGLESTRIP;
			DrawCallDesc.m_u32PrimitiveCount	= (constTerrainDepth-1)*((constTerrainWidth*2+2))-2;
			DrawCallDesc.m_u32StartVertex		= 0;
			DrawCallDesc.m_u32MinIndex			= 0;
			DrawCallDesc.m_u32NumberOfVertices	= constTerrainWidth*constTerrainDepth;
			DrawCallDesc.m_u32StartIndex		= 0;
			DrawCallDesc.m_u32BaseVertexIndex	= 0;
			m_pVisual->SetDrawCallDescription( DrawCallDesc );
			m_pVisual->SetCullingMode( IGN_ENUM_CULLINGMODE_CCW );
			m_pVisual->SetDepthTesting( IGN_ENUM_CMPFUNC_LESSEQUAL );
			m_pVisual->AttachVertexData( m_pVertexData );
			m_pVisual->AttachIndexData( m_pIndexData );
			m_pVisual->AttachShader( m_pShader );
		}
		_chdir( "..\\" );
		_chdir( "..\\" );
	}
}


void TERRAIN::Finit												(	void )
{
	//----------------------------------------------------------|----------------------------------------------Finit-|
	// public method
	// Finalizes the tester.

	//
	// Release objects in IGN_graphic_sub.dll
	//
	if( NULL!=m_pShader )
		m_pShader->Release();
	if( NULL!=m_pMaterial )
		m_pMaterial->Release();
	if( NULL!=m_pTextureBlock0 )
		m_pTextureBlock0->Release();
	if( NULL!=m_pTextureBlock1 )
		m_pTextureBlock1->Release();
	if( NULL!=m_pHeightMap )
		m_pHeightMap->Release();
	if( NULL!=m_pTerrainImage )
		m_pTerrainImage->Release();
	if( NULL!=m_pVertexData )
		m_pVertexData->Release();
	if( NULL!=m_pIndexData )
		m_pIndexData->Release();
	if( NULL!=m_pVisual )
		m_pVisual->Release();
}


IGNBOOL TERRAIN::ProceedTerrain									(	IGN_OBJIF_GfxVertexProgDcl*
																							a_pVertexProgDcl,
																	IGN_OBJIF_GfxVertexProgram*
																							a_pVertexProgram,
																	IGN_OBJIF_GfxPixelProgram*
																							a_pPixelProgram )
{
	//----------------------------------------------------------|-------------------------------------ProceedTerrain-|
	// public method
	// Proceeds the terrain with special shaders.

	if( NULL!=m_pVisual )
	{
		//
		// Render the terrain
		//
		m_pShader->AttachVertexProgDcl( a_pVertexProgDcl );
		m_pShader->AttachVertexProgram( a_pVertexProgram );
		m_pShader->AttachPixelProgram( a_pPixelProgram );
		m_pVisual->Execute();
		m_pShader->AttachVertexProgDcl( NULL );
		m_pShader->AttachVertexProgram( NULL );
		m_pShader->AttachPixelProgram( NULL );

		return true;
	}

	return false;
}


IGNBOOL TERRAIN::ProceedTerrain									(	void )
{
	//----------------------------------------------------------|-------------------------------------ProceedTerrain-|
	// public method
	// Proceeds the terrain.

	if( NULL!=m_pVisual )
	{
		//
		// Render the terrain
		//
		m_pVisual->Execute();

		return true;
	}

	return false;
}


IGNFLOAT TERRAIN::WeightingFactor								(	IGN_STRUCT_3DVector		a_PositionA,
																	IGN_STRUCT_3DVector		a_PositionB )
{
	//----------------------------------------------------------|------------------------------------WeightingFactor-|
	// public method
	// Calculates the weighting factor.

	IGNFLOAT fDistance = static_cast<IGNFLOAT>( sqrt(
		(a_PositionA.m_fX - a_PositionB.m_fX)*(a_PositionA.m_fX - a_PositionB.m_fX) +
		(a_PositionA.m_fZ - a_PositionB.m_fZ)*(a_PositionA.m_fZ - a_PositionB.m_fZ) ) );
	IGNFLOAT fWeight = -1.0f*fDistance + 5.0f;
	if( 0.0f>fWeight )
		fWeight = 0.0f;
	m_fWeightSum += fWeight;

	return fWeight;
}


IGNBOOL TERRAIN::CheckPosition									(	IGN_STRUCT_3DVector&	a_Position )
{
	//----------------------------------------------------------|--------------------------------------CheckPosition-|
	// public method
	// Checks the position for terrain collision.

	if( NULL!=m_pVertexData )
	{
		IGNUCHAR* pucharBytes;
		m_pVertexData->LockBuffer(
			0,
			0,
			IGNLOCKFLAG_READONLY,
			&pucharBytes );
		structVertexFormatTerrain* pVertexData = reinterpret_cast<structVertexFormatTerrain*>( pucharBytes );
		if( NULL!=pVertexData )
		{
			if( -110>a_Position.m_fX )
			{
				a_Position.m_fX = -110.0f;
			}
			else if( 110<a_Position.m_fX )
			{
				a_Position.m_fX = 110.0f;
			}
			if( -110>a_Position.m_fZ )
			{
				a_Position.m_fZ = -110.0f;
			}
			else if( 110<a_Position.m_fZ )
			{
				a_Position.m_fZ = 110.0f;
			}
			IGNINT iXIndex;
			IGNINT iZIndex;
			FloatToInt( &iXIndex, (a_Position.m_fX + 128.0f)/2 );
			FloatToInt( &iZIndex, (a_Position.m_fZ + 128.0f)/2 );
			pVertexData += iZIndex*128 + iXIndex;
			a_Position.m_fY =(
				pVertexData		->m_Position.m_fY * WeightingFactor( pVertexData->m_Position, a_Position ) +

				(pVertexData+1)		->m_Position.m_fY * WeightingFactor( (pVertexData+1)->m_Position, a_Position ) +
				(pVertexData-1)		->m_Position.m_fY * WeightingFactor( (pVertexData-1)->m_Position, a_Position ) +
				(pVertexData+127)	->m_Position.m_fY * WeightingFactor( (pVertexData+127)->m_Position, a_Position ) +
				(pVertexData+128)	->m_Position.m_fY * WeightingFactor( (pVertexData+128)->m_Position, a_Position ) +
				(pVertexData+129)	->m_Position.m_fY * WeightingFactor( (pVertexData+129)->m_Position, a_Position ) +
				(pVertexData-127)	->m_Position.m_fY * WeightingFactor( (pVertexData-127)->m_Position, a_Position ) +
				(pVertexData-128)	->m_Position.m_fY * WeightingFactor( (pVertexData-128)->m_Position, a_Position ) +
				(pVertexData-129)	->m_Position.m_fY * WeightingFactor( (pVertexData-129)->m_Position, a_Position ) +

				(pVertexData+2)		->m_Position.m_fY * WeightingFactor( (pVertexData+2)->m_Position, a_Position ) +
				(pVertexData-2)		->m_Position.m_fY * WeightingFactor( (pVertexData-2)->m_Position, a_Position ) +
				(pVertexData+126)	->m_Position.m_fY * WeightingFactor( (pVertexData+126)->m_Position, a_Position ) +
				(pVertexData+130)	->m_Position.m_fY * WeightingFactor( (pVertexData+130)->m_Position, a_Position ) +
				(pVertexData-126)	->m_Position.m_fY * WeightingFactor( (pVertexData-126)->m_Position, a_Position ) +
				(pVertexData-130)	->m_Position.m_fY * WeightingFactor( (pVertexData-130)->m_Position, a_Position ) +
				(pVertexData+254)	->m_Position.m_fY * WeightingFactor( (pVertexData+254)->m_Position, a_Position ) +
				(pVertexData+255)	->m_Position.m_fY * WeightingFactor( (pVertexData+255)->m_Position, a_Position ) +
				(pVertexData+256)	->m_Position.m_fY * WeightingFactor( (pVertexData+256)->m_Position, a_Position ) +
				(pVertexData+257)	->m_Position.m_fY * WeightingFactor( (pVertexData+257)->m_Position, a_Position ) +
				(pVertexData+258)	->m_Position.m_fY * WeightingFactor( (pVertexData+258)->m_Position, a_Position ) +
				(pVertexData-254)	->m_Position.m_fY * WeightingFactor( (pVertexData-254)->m_Position, a_Position ) +
				(pVertexData-255)	->m_Position.m_fY * WeightingFactor( (pVertexData-255)->m_Position, a_Position ) +
				(pVertexData-256)	->m_Position.m_fY * WeightingFactor( (pVertexData-256)->m_Position, a_Position ) +
				(pVertexData-257)	->m_Position.m_fY * WeightingFactor( (pVertexData-257)->m_Position, a_Position ) +
				(pVertexData-258)	->m_Position.m_fY * WeightingFactor( (pVertexData-258)->m_Position, a_Position ) +

				(pVertexData+3)		->m_Position.m_fY * WeightingFactor( (pVertexData+3)->m_Position, a_Position ) +
				(pVertexData-3)		->m_Position.m_fY * WeightingFactor( (pVertexData-3)->m_Position, a_Position ) +
				(pVertexData+125)	->m_Position.m_fY * WeightingFactor( (pVertexData+125)->m_Position, a_Position ) +
				(pVertexData+131)	->m_Position.m_fY * WeightingFactor( (pVertexData+131)->m_Position, a_Position ) +
				(pVertexData-125)	->m_Position.m_fY * WeightingFactor( (pVertexData-125)->m_Position, a_Position ) +
				(pVertexData-131)	->m_Position.m_fY * WeightingFactor( (pVertexData-131)->m_Position, a_Position ) +
				(pVertexData+253)	->m_Position.m_fY * WeightingFactor( (pVertexData+253)->m_Position, a_Position ) +
				(pVertexData+259)	->m_Position.m_fY * WeightingFactor( (pVertexData+259)->m_Position, a_Position ) +
				(pVertexData-253)	->m_Position.m_fY * WeightingFactor( (pVertexData-253)->m_Position, a_Position ) +
				(pVertexData-259)	->m_Position.m_fY * WeightingFactor( (pVertexData-259)->m_Position, a_Position ) +
				(pVertexData+509)	->m_Position.m_fY * WeightingFactor( (pVertexData+509)->m_Position, a_Position ) +
				(pVertexData+510)	->m_Position.m_fY * WeightingFactor( (pVertexData+510)->m_Position, a_Position ) +
				(pVertexData+511)	->m_Position.m_fY * WeightingFactor( (pVertexData+511)->m_Position, a_Position ) +
				(pVertexData+512)	->m_Position.m_fY * WeightingFactor( (pVertexData+512)->m_Position, a_Position ) +
				(pVertexData+513)	->m_Position.m_fY * WeightingFactor( (pVertexData+513)->m_Position, a_Position ) +
				(pVertexData+514)	->m_Position.m_fY * WeightingFactor( (pVertexData+514)->m_Position, a_Position ) +
				(pVertexData+515)	->m_Position.m_fY * WeightingFactor( (pVertexData+515)->m_Position, a_Position ) +
				(pVertexData-509)	->m_Position.m_fY * WeightingFactor( (pVertexData-509)->m_Position, a_Position ) +
				(pVertexData-510)	->m_Position.m_fY * WeightingFactor( (pVertexData-510)->m_Position, a_Position ) +
				(pVertexData-511)	->m_Position.m_fY * WeightingFactor( (pVertexData-511)->m_Position, a_Position ) +
				(pVertexData-512)	->m_Position.m_fY * WeightingFactor( (pVertexData-512)->m_Position, a_Position ) +
				(pVertexData-513)	->m_Position.m_fY * WeightingFactor( (pVertexData-513)->m_Position, a_Position ) +
				(pVertexData-514)	->m_Position.m_fY * WeightingFactor( (pVertexData-514)->m_Position, a_Position ) +
				(pVertexData-515)	->m_Position.m_fY * WeightingFactor( (pVertexData-515)->m_Position, a_Position ) +

				(pVertexData+4)		->m_Position.m_fY * WeightingFactor( (pVertexData+4)->m_Position, a_Position ) +
				(pVertexData-4)		->m_Position.m_fY * WeightingFactor( (pVertexData-4)->m_Position, a_Position ) +
				(pVertexData+124)	->m_Position.m_fY * WeightingFactor( (pVertexData+124)->m_Position, a_Position ) +
				(pVertexData+132)	->m_Position.m_fY * WeightingFactor( (pVertexData+132)->m_Position, a_Position ) +
				(pVertexData-124)	->m_Position.m_fY * WeightingFactor( (pVertexData-124)->m_Position, a_Position ) +
				(pVertexData-132)	->m_Position.m_fY * WeightingFactor( (pVertexData-132)->m_Position, a_Position ) +
				(pVertexData+252)	->m_Position.m_fY * WeightingFactor( (pVertexData+252)->m_Position, a_Position ) +
				(pVertexData+260)	->m_Position.m_fY * WeightingFactor( (pVertexData+260)->m_Position, a_Position ) +
				(pVertexData-252)	->m_Position.m_fY * WeightingFactor( (pVertexData-252)->m_Position, a_Position ) +
				(pVertexData-260)	->m_Position.m_fY * WeightingFactor( (pVertexData-260)->m_Position, a_Position ) +
				(pVertexData+508)	->m_Position.m_fY * WeightingFactor( (pVertexData+508)->m_Position, a_Position ) +
				(pVertexData+516)	->m_Position.m_fY * WeightingFactor( (pVertexData+516)->m_Position, a_Position ) +
				(pVertexData-508)	->m_Position.m_fY * WeightingFactor( (pVertexData-508)->m_Position, a_Position ) +
				(pVertexData-516)	->m_Position.m_fY * WeightingFactor( (pVertexData-516)->m_Position, a_Position ) +
				(pVertexData+764)	->m_Position.m_fY * WeightingFactor( (pVertexData+764)->m_Position, a_Position ) +
				(pVertexData+765)	->m_Position.m_fY * WeightingFactor( (pVertexData+765)->m_Position, a_Position ) +
				(pVertexData+766)	->m_Position.m_fY * WeightingFactor( (pVertexData+766)->m_Position, a_Position ) +
				(pVertexData+767)	->m_Position.m_fY * WeightingFactor( (pVertexData+767)->m_Position, a_Position ) +
				(pVertexData+768)	->m_Position.m_fY * WeightingFactor( (pVertexData+768)->m_Position, a_Position ) +
				(pVertexData+769)	->m_Position.m_fY * WeightingFactor( (pVertexData+769)->m_Position, a_Position ) +
				(pVertexData+770)	->m_Position.m_fY * WeightingFactor( (pVertexData+770)->m_Position, a_Position ) +
				(pVertexData+771)	->m_Position.m_fY * WeightingFactor( (pVertexData+771)->m_Position, a_Position ) +
				(pVertexData+772)	->m_Position.m_fY * WeightingFactor( (pVertexData+772)->m_Position, a_Position ) +
				(pVertexData-764)	->m_Position.m_fY * WeightingFactor( (pVertexData-764)->m_Position, a_Position ) +
				(pVertexData-765)	->m_Position.m_fY * WeightingFactor( (pVertexData-765)->m_Position, a_Position ) +
				(pVertexData-766)	->m_Position.m_fY * WeightingFactor( (pVertexData-766)->m_Position, a_Position ) +
				(pVertexData-767)	->m_Position.m_fY * WeightingFactor( (pVertexData-767)->m_Position, a_Position ) +
				(pVertexData-768)	->m_Position.m_fY * WeightingFactor( (pVertexData-768)->m_Position, a_Position ) +
				(pVertexData-769)	->m_Position.m_fY * WeightingFactor( (pVertexData-769)->m_Position, a_Position ) +
				(pVertexData-770)	->m_Position.m_fY * WeightingFactor( (pVertexData-770)->m_Position, a_Position ) +
				(pVertexData-771)	->m_Position.m_fY * WeightingFactor( (pVertexData-771)->m_Position, a_Position ) +
				(pVertexData-772)	->m_Position.m_fY * WeightingFactor( (pVertexData-772)->m_Position, a_Position ) +

				(pVertexData+5)		->m_Position.m_fY * WeightingFactor( (pVertexData+5)->m_Position, a_Position ) +
				(pVertexData-5)		->m_Position.m_fY * WeightingFactor( (pVertexData-5)->m_Position, a_Position ) +
				(pVertexData+123)	->m_Position.m_fY * WeightingFactor( (pVertexData+123)->m_Position, a_Position ) +
				(pVertexData+133)	->m_Position.m_fY * WeightingFactor( (pVertexData+133)->m_Position, a_Position ) +
				(pVertexData-123)	->m_Position.m_fY * WeightingFactor( (pVertexData-123)->m_Position, a_Position ) +
				(pVertexData-133)	->m_Position.m_fY * WeightingFactor( (pVertexData-133)->m_Position, a_Position ) +
				(pVertexData+251)	->m_Position.m_fY * WeightingFactor( (pVertexData+251)->m_Position, a_Position ) +
				(pVertexData+261)	->m_Position.m_fY * WeightingFactor( (pVertexData+261)->m_Position, a_Position ) +
				(pVertexData-251)	->m_Position.m_fY * WeightingFactor( (pVertexData-251)->m_Position, a_Position ) +
				(pVertexData-261)	->m_Position.m_fY * WeightingFactor( (pVertexData-261)->m_Position, a_Position ) +
				(pVertexData+507)	->m_Position.m_fY * WeightingFactor( (pVertexData+507)->m_Position, a_Position ) +
				(pVertexData+517)	->m_Position.m_fY * WeightingFactor( (pVertexData+517)->m_Position, a_Position ) +
				(pVertexData-507)	->m_Position.m_fY * WeightingFactor( (pVertexData-507)->m_Position, a_Position ) +
				(pVertexData-517)	->m_Position.m_fY * WeightingFactor( (pVertexData-517)->m_Position, a_Position ) +
				(pVertexData+763)	->m_Position.m_fY * WeightingFactor( (pVertexData+763)->m_Position, a_Position ) +
				(pVertexData+773)	->m_Position.m_fY * WeightingFactor( (pVertexData+773)->m_Position, a_Position ) +
				(pVertexData-763)	->m_Position.m_fY * WeightingFactor( (pVertexData-763)->m_Position, a_Position ) +
				(pVertexData-773)	->m_Position.m_fY * WeightingFactor( (pVertexData-773)->m_Position, a_Position ) +
				(pVertexData+1019)	->m_Position.m_fY * WeightingFactor( (pVertexData+1019)->m_Position, a_Position ) +
				(pVertexData+1020)	->m_Position.m_fY * WeightingFactor( (pVertexData+1020)->m_Position, a_Position ) +
				(pVertexData+1021)	->m_Position.m_fY * WeightingFactor( (pVertexData+1021)->m_Position, a_Position ) +
				(pVertexData+1022)	->m_Position.m_fY * WeightingFactor( (pVertexData+1022)->m_Position, a_Position ) +
				(pVertexData+1023)	->m_Position.m_fY * WeightingFactor( (pVertexData+1023)->m_Position, a_Position ) +
				(pVertexData+1024)	->m_Position.m_fY * WeightingFactor( (pVertexData+1024)->m_Position, a_Position ) +
				(pVertexData+1025)	->m_Position.m_fY * WeightingFactor( (pVertexData+1025)->m_Position, a_Position ) +
				(pVertexData+1026)	->m_Position.m_fY * WeightingFactor( (pVertexData+1026)->m_Position, a_Position ) +
				(pVertexData+1027)	->m_Position.m_fY * WeightingFactor( (pVertexData+1027)->m_Position, a_Position ) +
				(pVertexData+1028)	->m_Position.m_fY * WeightingFactor( (pVertexData+1028)->m_Position, a_Position ) +
				(pVertexData+1029)	->m_Position.m_fY * WeightingFactor( (pVertexData+1029)->m_Position, a_Position ) +
				(pVertexData-1019)	->m_Position.m_fY * WeightingFactor( (pVertexData-1019)->m_Position, a_Position ) +
				(pVertexData-1020)	->m_Position.m_fY * WeightingFactor( (pVertexData-1020)->m_Position, a_Position ) +
				(pVertexData-1021)	->m_Position.m_fY * WeightingFactor( (pVertexData-1021)->m_Position, a_Position ) +
				(pVertexData-1022)	->m_Position.m_fY * WeightingFactor( (pVertexData-1022)->m_Position, a_Position ) +
				(pVertexData-1023)	->m_Position.m_fY * WeightingFactor( (pVertexData-1023)->m_Position, a_Position ) +
				(pVertexData-1024)	->m_Position.m_fY * WeightingFactor( (pVertexData-1024)->m_Position, a_Position ) +
				(pVertexData-1025)	->m_Position.m_fY * WeightingFactor( (pVertexData-1025)->m_Position, a_Position ) +
				(pVertexData-1026)	->m_Position.m_fY * WeightingFactor( (pVertexData-1026)->m_Position, a_Position ) +
				(pVertexData-1027)	->m_Position.m_fY * WeightingFactor( (pVertexData-1027)->m_Position, a_Position ) +
				(pVertexData-1028)	->m_Position.m_fY * WeightingFactor( (pVertexData-1028)->m_Position, a_Position ) +
				(pVertexData-1029)	->m_Position.m_fY * WeightingFactor( (pVertexData-1029)->m_Position, a_Position ) )/m_fWeightSum +
				2.5f;
			m_fWeightSum = 0.0f;
		}
		m_pVertexData->UnlockBuffer();
	}

	return true;
}


IGNBOOL TERRAIN::Proceed										(	IGN_OBJIF_GfxVertexProgDcl*
																							a_pVertexProgDcl,
																	IGN_OBJIF_GfxVertexProgram*
																							a_pVertexProgram,
																	IGN_OBJIF_GfxPixelProgram*
																							a_pPixelProgram )
{
	//----------------------------------------------------------|--------------------------------------------Proceed-|
	// public method
	// Proceeds the terrain with special shaders.

	//
	// Proceed with the terrain
	//
	return ProceedTerrain(
		a_pVertexProgDcl,
		a_pVertexProgram,
		a_pPixelProgram );
}


IGNBOOL TERRAIN::Proceed										(	void )
{
	//----------------------------------------------------------|--------------------------------------------Proceed-|
	// public method
	// Proceeds the terrain.

	//
	// Proceed with the terrain
	//
	return ProceedTerrain();
}
