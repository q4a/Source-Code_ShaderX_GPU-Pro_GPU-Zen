//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			WaterDemo
//*				File:				Water.cpp
//*
//********************************************************************************************************************
//*
//*				Author:				Kurt Pelzer
//*									Copyright (c) 2003 Kurt Pelzer
//*									All rights reserved.
//*
//********************************************************************************************************************
//*
//*				History:			02/21/2004		- Fixed invisible water plane bug (on GeForceFX cards)		(kp)
//*
//********************************************************************************************************************
//********************************************************************************************************************




#include		"Water.h"
#include		<direct.h>
#include		"d3dx8.h"




//********************************************************************************************************************
//*				WATER STATIC DATA
//********************************************************************************************************************

IGN_STRUCT_DclElement WATER::ms_WaterVtxDeclaration[] =
{	
	//----------------------------------------------------------|---------------------------------VERTEX-DECLARATION-|
	// Vertex declaration for water plane
	//

	{	
		//
		// Vertex position
		//
		0, 
		0, 
		IGN_ENUM_DECLTYPE_FLOAT3,
		IGN_ENUM_DECLMETHOD_DEFAULT,
		IGN_ENUM_DECLUSAGE_POSITION,
		0
	},
	{	
		//
		// Vertex normal
		//
		0, 
		12, 
		IGN_ENUM_DECLTYPE_FLOAT3,
		IGN_ENUM_DECLMETHOD_DEFAULT,
		IGN_ENUM_DECLUSAGE_NORMAL,
		0
	},
	{	
		//
		// Vertex texture coordinates (Index 0)
		//
		0, 
		24, 
		IGN_ENUM_DECLTYPE_FLOAT2,
		IGN_ENUM_DECLMETHOD_DEFAULT,
		IGN_ENUM_DECLUSAGE_TEXCOORD,
		0
	},
	{	
		//
		// Vertex texture coordinates (Index 1)
		//
		0, 
		32, 
		IGN_ENUM_DECLTYPE_FLOAT3,
		IGN_ENUM_DECLMETHOD_DEFAULT,
		IGN_ENUM_DECLUSAGE_TEXCOORD,
		1
	},
	{	
		//
		// Vertex texture coordinates (Index 2)
		//
		0, 
		44, 
		IGN_ENUM_DECLTYPE_FLOAT3,
		IGN_ENUM_DECLMETHOD_DEFAULT,
		IGN_ENUM_DECLUSAGE_TEXCOORD,
		2
	},
	{	
		//
		// Vertex texture coordinates (Index 3)
		//
		0, 
		56, 
		IGN_ENUM_DECLTYPE_FLOAT2,
		IGN_ENUM_DECLMETHOD_DEFAULT,
		IGN_ENUM_DECLUSAGE_TEXCOORD,
		3
	},
	IGNDECL_END()
};


IGN_STRUCT_DclElement WATER::ms_RefractionVtxDeclaration[] =
{	
	//----------------------------------------------------------|---------------------------------VERTEX-DECLARATION-|
	// Vertex declaration for refraction objects
	//

	{	
		//
		// Vertex position
		//
		0, 
		0, 
		IGN_ENUM_DECLTYPE_FLOAT3,
		IGN_ENUM_DECLMETHOD_DEFAULT,
		IGN_ENUM_DECLUSAGE_POSITION,
		0
	},
	{	
		//
		// Vertex normal
		//
		0, 
		12, 
		IGN_ENUM_DECLTYPE_FLOAT3,
		IGN_ENUM_DECLMETHOD_DEFAULT,
		IGN_ENUM_DECLUSAGE_NORMAL,
		0
	},
	{	
		//
		// Vertex texture coordinates (Index 0)
		//
		0, 
		24, 
		IGN_ENUM_DECLTYPE_FLOAT2,
		IGN_ENUM_DECLMETHOD_DEFAULT,
		IGN_ENUM_DECLUSAGE_TEXCOORD,
		0
	},
	IGNDECL_END()
};


//********************************************************************************************************************
//*				WATER
//********************************************************************************************************************

WATER::WATER													(	IGN_OBJIF_GfxContext*	a_pGfxContextInterface,
																	IGN_OBJIF_GfxAssistant*	a_pAssistant,
																	IGN_OBJIF_GfxMixer*		a_pMixer )
{
	//----------------------------------------------------------|----------------------------------------------WATER-|
	// public method
	// Constructor
	
	m_hTmrLibrary					= NULL;
	m_pTmrInstanceInterface			= NULL;
	m_pTmrContextInterface			= NULL;
	m_pTimerInterface				= NULL;
	m_pAssistant					= a_pAssistant;
	m_pMixer						= a_pMixer;
	m_pVisual						= NULL;
	m_pVertexData					= NULL;
	m_pIndexData					= NULL;
	m_pShader						= NULL;
	m_pWaterVertexProgDcl			= NULL;
	m_pRefractionVertexProgDcl		= NULL;
	m_pVertexProgram				= NULL;
	m_pWaterVertexProgramDX8GF3		= NULL;
	m_pWaterVertexProgramDX8		= NULL;
	m_pWaterVertexProgramDX9		= NULL;
	m_pPixelProgram					= NULL;
	m_pWaterPixelProgramDX8			= NULL;
	m_pWaterPixelProgramDX9			= NULL;
	m_pRefractionVertexProgramDX9	= NULL;
	m_pRefractionPixelProgramDX9	= NULL;
	m_pMaterial						= NULL;
	m_pTextureBlock0				= NULL;
	m_pTextureBlock1				= NULL;
	m_pTextureBlock2				= NULL;
	m_pTextureBlock3				= NULL;
	m_pTextureBlock4				= NULL;
	m_pReflectionImage				= NULL;
	m_pRefractionImage				= NULL;
	m_pDetailImage					= NULL;
	m_pBumpImage1					= NULL;
	m_pBumpImage2					= NULL;
	m_hRefractionDepthBuffer		= NULL;
	m_hReflectionDepthBuffer		= NULL;
	m_pOriginalCamera				= NULL;
	m_pMirrorCamera					= NULL;
	m_fWaterHeight					= 1.75f;
	m_TargetClass					= ENUM_TARGETCLASS_DX9;
	m_pSkydome						= NULL;
	m_pTerrain						= NULL;
	m_bWireframeModus				= false;

	//
	// Initialize the system
	//
	Init( a_pGfxContextInterface );
}


WATER::~WATER													()
{
	//----------------------------------------------------------|---------------------------------------------~WATER-|
	// public method
	// Destructor

	//
	// Finalize the system
	//
	Finit();
}


void WATER::Init												(	IGN_OBJIF_GfxContext*	a_pGfxContextInterface )
{
	//----------------------------------------------------------|-----------------------------------------------Init-|
	// public method
	// Initializes the tester.

	_chdir( "System" );
	IGNRESULT hResult = 0;

	//
	// Load IGN_timer_sub.dll and create objects
	//
	IGN_STRUCT_IFSubTimer* pSubTimer = NULL;
	m_hTmrLibrary = LoadLibrary( "Ign_timer_sub.dll" );
	IGN_METHOD_GetSubHookTimer* 
			TmrGetInterface = reinterpret_cast<IGN_METHOD_GetSubHookTimer*>( GetProcAddress( m_hTmrLibrary, "GetInterface" ) );
	if( NULL!=TmrGetInterface )
		hResult = TmrGetInterface( &pSubTimer );
	if( NULL!=pSubTimer )
		hResult = pSubTimer->m_pCreateInstance( IGN_ENUM_TMRINSTANCETYPE_WINDOWSMULTIMEDIA, m_pTmrInstanceInterface );
	if( NULL!=m_pTmrInstanceInterface )
		m_pTmrInstanceInterface->CreateContext( 1, m_pTmrContextInterface );
	if( NULL!=m_pTmrContextInterface )
		m_pTmrContextInterface->CreateTimer( m_pTimerInterface );
	if( NULL!=m_pTimerInterface )
		m_pTimerInterface->Resume();

	_chdir( "..\\" );

	//
	// Create objects in IGN_graphic_sub.dll
	//
	if( NULL!=a_pGfxContextInterface )
	{		
		_chdir( "Media" );
		_chdir( "Textures" );
		a_pGfxContextInterface->CreateCamera( m_pMirrorCamera );
		a_pGfxContextInterface->CreateImageData( m_pBumpImage1 );
		if( NULL!=m_pBumpImage1 )
		{
			IGN_OBJIF_GfxImageData*	pWaterBumpsImage;
			a_pGfxContextInterface->CreateImageData( pWaterBumpsImage );
			if( NULL!=pWaterBumpsImage )
			{
				IGN_STRUCT_LayersDesc LayerDesc;
				LayerDesc.m_uiWidth		= 0;
				LayerDesc.m_uiHeight	= 0;
				LayerDesc.m_uiDepth		= 0;
				LayerDesc.m_bCubeMap	= false;
				LayerDesc.m_uiLevels	= 0;
				LayerDesc.m_dwUsage		= 0;
				LayerDesc.m_Format		= IGN_ENUM_PIXELFORMAT_UNKNOWN;
				LayerDesc.m_Pool		= IGN_ENUM_POOLTYPE_MANAGED;
				if( NULL!=m_pAssistant )
				{
					m_pAssistant->FillImageFromFile( pWaterBumpsImage, LayerDesc, "WaterBumps1.tga" );
					m_pAssistant->FillImageFromFile( m_pBumpImage1, LayerDesc, "WaterBumps1.tga" );
					hResult = m_pAssistant->ComputeNormalMap( m_pBumpImage1, pWaterBumpsImage, IGN_ENUM_NORMALMAPGEN_DEFAULT, IGN_ENUM_CHANNEL_RED, 20.0f );
				}
				pWaterBumpsImage->Release();
			}
		}
		a_pGfxContextInterface->CreateImageData( m_pBumpImage2 );
		if( NULL!=m_pBumpImage2 )
		{
			IGN_OBJIF_GfxImageData*	pWaterBumpsImage;
			a_pGfxContextInterface->CreateImageData( pWaterBumpsImage );
			if( NULL!=pWaterBumpsImage )
			{
				IGN_STRUCT_LayersDesc LayerDesc;
				LayerDesc.m_uiWidth		= 0;
				LayerDesc.m_uiHeight	= 0;
				LayerDesc.m_uiDepth		= 0;
				LayerDesc.m_bCubeMap	= false;
				LayerDesc.m_uiLevels	= 0;
				LayerDesc.m_dwUsage		= 0;
				LayerDesc.m_Format		= IGN_ENUM_PIXELFORMAT_UNKNOWN;
				LayerDesc.m_Pool		= IGN_ENUM_POOLTYPE_MANAGED;
				if( NULL!=m_pAssistant )
				{
					m_pAssistant->FillImageFromFile( pWaterBumpsImage, LayerDesc, "WaterBumps2.tga" );
					m_pAssistant->FillImageFromFile( m_pBumpImage2, LayerDesc, "WaterBumps2.tga" );
					hResult = m_pAssistant->ComputeNormalMap( m_pBumpImage2, pWaterBumpsImage, IGN_ENUM_NORMALMAPGEN_DEFAULT, IGN_ENUM_CHANNEL_RED, 20.0f );
				}
				pWaterBumpsImage->Release();
			}
		}
		a_pGfxContextInterface->CreateImageData( m_pReflectionImage );
		if( NULL!=m_pReflectionImage )
		{
			IGN_STRUCT_LayersDesc LayerDesc;
			LayerDesc.m_uiWidth		= constReflectionTextureWidth;
			LayerDesc.m_uiHeight	= constReflectionTextureHeight;
			LayerDesc.m_uiDepth		= 0;
			LayerDesc.m_bCubeMap	= false;
			LayerDesc.m_uiLevels	= 1;
			LayerDesc.m_dwUsage		= IGNUSAGE_RENDERTARGET;
			LayerDesc.m_Format		= IGN_ENUM_PIXELFORMAT_A8R8G8B8;
			LayerDesc.m_Pool		= IGN_ENUM_POOLTYPE_DEFAULT;
			m_pReflectionImage->CreateLayers( LayerDesc );
		}
		a_pGfxContextInterface->CreateImageData( m_pRefractionImage );
		if( NULL!=m_pRefractionImage )
		{
			IGN_STRUCT_LayersDesc LayerDesc;
			LayerDesc.m_uiWidth		= constRefractionTextureWidth;
			LayerDesc.m_uiHeight	= constRefractionTextureHeight;
			LayerDesc.m_uiDepth		= 0;
			LayerDesc.m_bCubeMap	= false;
			LayerDesc.m_uiLevels	= 1;
			LayerDesc.m_dwUsage		= IGNUSAGE_RENDERTARGET;
			LayerDesc.m_Format		= IGN_ENUM_PIXELFORMAT_A8R8G8B8;
			LayerDesc.m_Pool		= IGN_ENUM_POOLTYPE_DEFAULT;
			m_pRefractionImage->CreateLayers( LayerDesc );
		}
		a_pGfxContextInterface->CreateImageData( m_pDetailImage );
		if( NULL!=m_pDetailImage )
		{
			IGN_STRUCT_LayersDesc LayerDesc;
			LayerDesc.m_uiWidth		= 0;
			LayerDesc.m_uiHeight	= 0;
			LayerDesc.m_uiDepth		= 0;
			LayerDesc.m_bCubeMap	= false;
			LayerDesc.m_uiLevels	= 0;
			LayerDesc.m_dwUsage		= 0;
			LayerDesc.m_Format		= IGN_ENUM_PIXELFORMAT_UNKNOWN;
			LayerDesc.m_Pool		= IGN_ENUM_POOLTYPE_MANAGED;
			if( NULL!=m_pAssistant )
				m_pAssistant->FillImageFromFile( m_pDetailImage, LayerDesc, "WaterDetails.tga" );
		}
		m_pMixer->CreateSpecialDepthBuffer( constReflectionTextureWidth, constReflectionTextureHeight, IGN_ENUM_DEPTHFORMAT_D24X8, m_hReflectionDepthBuffer );
		m_pMixer->CreateSpecialDepthBuffer( constRefractionTextureWidth, constRefractionTextureHeight, IGN_ENUM_DEPTHFORMAT_D24X8, m_hRefractionDepthBuffer );
		
		_chdir( "..\\" );
		_chdir( "Shaders" );

		a_pGfxContextInterface->CreateVertexProgDcl( m_pWaterVertexProgDcl );
		if( NULL!=m_pWaterVertexProgDcl )
		{
			m_pWaterVertexProgDcl->CreateDeclaration( ms_WaterVtxDeclaration );
		}
		a_pGfxContextInterface->CreateVertexProgDcl( m_pRefractionVertexProgDcl );
		if( NULL!=m_pRefractionVertexProgDcl )
		{
			m_pRefractionVertexProgDcl->CreateDeclaration( ms_RefractionVtxDeclaration );
		}
/*		a_pGfxContextInterface->CreateVertexProgram( m_pWaterVertexProgramDX8GF3 );
		if( NULL!=m_pWaterVertexProgramDX8GF3 )
		{
			m_pAssistant->AssembleProgramFromFile( "DX8WaterForGF3VS.txt" );
			IGNHANDLE hBinary;
			IGNU32 u32BinaryLength;
			m_pAssistant->GetAuxBuffer( IGN_ENUM_AUXBUFFERTYPE_BINARY, &hBinary, u32BinaryLength );
			m_pWaterVertexProgramDX8GF3->CreateProgram( hBinary, false );
		}
		a_pGfxContextInterface->CreateVertexProgram( m_pWaterVertexProgramDX8 );
		if( NULL!=m_pWaterVertexProgramDX8 )
		{
			m_pAssistant->AssembleProgramFromFile( "DX8WaterVS.txt" );
			IGNHANDLE hBinary;
			IGNU32 u32BinaryLength;
			m_pAssistant->GetAuxBuffer( IGN_ENUM_AUXBUFFERTYPE_BINARY, &hBinary, u32BinaryLength );
			m_pWaterVertexProgramDX8->CreateProgram( hBinary, false );
		}
*/		a_pGfxContextInterface->CreateVertexProgram( m_pWaterVertexProgramDX9 );
		if( NULL!=m_pWaterVertexProgramDX9 )
		{
			m_pAssistant->AssembleProgramFromFile( "DX9WaterVS.txt" );
			IGNHANDLE hBinary;
			IGNU32 u32BinaryLength;
			m_pAssistant->GetAuxBuffer( IGN_ENUM_AUXBUFFERTYPE_BINARY, &hBinary, u32BinaryLength );
			m_pWaterVertexProgramDX9->CreateProgram( hBinary, false );
		}
		a_pGfxContextInterface->CreateVertexProgram( m_pRefractionVertexProgramDX9 );
		if( NULL!=m_pRefractionVertexProgramDX9 )
		{
			m_pAssistant->AssembleProgramFromFile( "DX9RefractionVS.txt" );
			IGNHANDLE hBinary;
			IGNU32 u32BinaryLength;
			m_pAssistant->GetAuxBuffer( IGN_ENUM_AUXBUFFERTYPE_BINARY, &hBinary, u32BinaryLength );
			m_pRefractionVertexProgramDX9->CreateProgram( hBinary, false );
		}
/*		a_pGfxContextInterface->CreatePixelProgram( m_pWaterPixelProgramDX8 );
		if( NULL!=m_pWaterPixelProgramDX8 )
		{
			m_pAssistant->AssembleProgramFromFile( "DX8WaterPS.txt" );
			IGNHANDLE hBinary;
			IGNU32 u32BinaryLength;
			m_pAssistant->GetAuxBuffer( IGN_ENUM_AUXBUFFERTYPE_BINARY, &hBinary, u32BinaryLength );
			m_pWaterPixelProgramDX8->CreateProgram( hBinary );
		}
*/		a_pGfxContextInterface->CreatePixelProgram( m_pWaterPixelProgramDX9 );
		if( NULL!=m_pWaterPixelProgramDX9 )
		{
			m_pAssistant->AssembleProgramFromFile( "DX9WaterPS.txt" );
			IGNHANDLE hBinary;
			IGNU32 u32BinaryLength;
			m_pAssistant->GetAuxBuffer( IGN_ENUM_AUXBUFFERTYPE_BINARY, &hBinary, u32BinaryLength );
			m_pWaterPixelProgramDX9->CreateProgram( hBinary );
		}
		a_pGfxContextInterface->CreatePixelProgram( m_pRefractionPixelProgramDX9 );
		if( NULL!=m_pRefractionPixelProgramDX9 )
		{
			m_pAssistant->AssembleProgramFromFile( "DX9RefractionPS.txt" );
			IGNHANDLE hBinary;
			IGNU32 u32BinaryLength;
			m_pAssistant->GetAuxBuffer( IGN_ENUM_AUXBUFFERTYPE_BINARY, &hBinary, u32BinaryLength );
			m_pRefractionPixelProgramDX9->CreateProgram( hBinary );
		}
		a_pGfxContextInterface->CreateTextureBlock( m_pTextureBlock0 );
		if( NULL!=m_pTextureBlock0 )
		{
			m_pTextureBlock0->AttachImageData( m_pBumpImage1 );
			m_pTextureBlock0->SetTextureCoordinateIndex( 0 );
			m_pTextureBlock0->SetColorCalc( IGN_ENUM_TEXOPERATION_SELECTARG1, IGN_ENUM_TEXARGUMENT_TEXTURE, IGN_ENUM_TEXARGUMENT_DIFFUSE );
			m_pTextureBlock0->SetAlphaCalc( IGN_ENUM_TEXOPERATION_SELECTARG1, IGN_ENUM_TEXARGUMENT_TEXTURE, IGN_ENUM_TEXARGUMENT_DIFFUSE );
			m_pTextureBlock0->SetTextureAdressing( IGN_ENUM_TEXADDRESS_WRAP, IGN_ENUM_TEXADDRESS_WRAP, IGN_ENUM_TEXADDRESS_WRAP );
			m_pTextureBlock0->SetTextureFiltering( IGN_ENUM_TEXFILTER_LINEAR, IGN_ENUM_TEXFILTER_LINEAR, IGN_ENUM_TEXFILTER_LINEAR );
		}
		a_pGfxContextInterface->CreateTextureBlock( m_pTextureBlock1 );
		if( NULL!=m_pTextureBlock1 )
		{			
			m_pTextureBlock1->AttachImageData( m_pReflectionImage );
			m_pTextureBlock1->SetTextureCoordinateIndex( 1 );
			m_pTextureBlock1->SetColorCalc( IGN_ENUM_TEXOPERATION_SELECTARG1, IGN_ENUM_TEXARGUMENT_TEXTURE, IGN_ENUM_TEXARGUMENT_CURRENT );
			m_pTextureBlock1->SetAlphaCalc( IGN_ENUM_TEXOPERATION_SELECTARG1, IGN_ENUM_TEXARGUMENT_TEXTURE, IGN_ENUM_TEXARGUMENT_DIFFUSE );

			//
			// DX9 Retail Version + DX8 Vertex-Shader needs this line (remove it for DX9 Debug Version):
			//
			m_pTextureBlock1->SetTextureCoordinateTrafo( IGNTEXCOORDTRAFO_PROJECTED );

			m_pTextureBlock1->SetTextureAdressing( IGN_ENUM_TEXADDRESS_CLAMP, IGN_ENUM_TEXADDRESS_CLAMP, IGN_ENUM_TEXADDRESS_CLAMP );
			m_pTextureBlock1->SetTextureFiltering( IGN_ENUM_TEXFILTER_LINEAR, IGN_ENUM_TEXFILTER_LINEAR, IGN_ENUM_TEXFILTER_LINEAR );
		}
		a_pGfxContextInterface->CreateTextureBlock( m_pTextureBlock2 );
		if( NULL!=m_pTextureBlock2 )
		{			
			m_pTextureBlock2->AttachImageData( m_pRefractionImage );
			m_pTextureBlock2->SetTextureCoordinateIndex( 2 );
			m_pTextureBlock2->SetColorCalc( IGN_ENUM_TEXOPERATION_BLENDFACTORALPHA, IGN_ENUM_TEXARGUMENT_TEXTURE, IGN_ENUM_TEXARGUMENT_CURRENT );
			m_pTextureBlock2->SetAlphaCalc( IGN_ENUM_TEXOPERATION_SELECTARG1, IGN_ENUM_TEXARGUMENT_TEXTURE, IGN_ENUM_TEXARGUMENT_DIFFUSE );

			//
			// DX9 Retail Version + DX8 Vertex-Shader needs this line (remove it for DX9 Debug Version):
			//
			m_pTextureBlock2->SetTextureCoordinateTrafo( IGNTEXCOORDTRAFO_PROJECTED );

			m_pTextureBlock2->SetTextureAdressing( IGN_ENUM_TEXADDRESS_CLAMP, IGN_ENUM_TEXADDRESS_CLAMP, IGN_ENUM_TEXADDRESS_CLAMP );
			m_pTextureBlock2->SetTextureFiltering( IGN_ENUM_TEXFILTER_ANISOTROPIC, IGN_ENUM_TEXFILTER_LINEAR, IGN_ENUM_TEXFILTER_LINEAR );
		}
		a_pGfxContextInterface->CreateTextureBlock( m_pTextureBlock3 );
		if( NULL!=m_pTextureBlock3 )
		{
			m_pTextureBlock3->SetTextureCoordinateIndex( 3 );
			m_pTextureBlock3->AttachImageData( m_pDetailImage );
			m_pTextureBlock3->SetColorCalc( IGN_ENUM_TEXOPERATION_BLENDTEXTUREALPHA, IGN_ENUM_TEXARGUMENT_TEXTURE, IGN_ENUM_TEXARGUMENT_CURRENT );
			m_pTextureBlock3->SetAlphaCalc( IGN_ENUM_TEXOPERATION_SELECTARG1, IGN_ENUM_TEXARGUMENT_CURRENT, IGN_ENUM_TEXARGUMENT_DIFFUSE );
			m_pTextureBlock3->SetTextureAdressing( IGN_ENUM_TEXADDRESS_WRAP, IGN_ENUM_TEXADDRESS_WRAP, IGN_ENUM_TEXADDRESS_WRAP );
			m_pTextureBlock3->SetTextureFiltering( IGN_ENUM_TEXFILTER_LINEAR, IGN_ENUM_TEXFILTER_LINEAR, IGN_ENUM_TEXFILTER_LINEAR );
		}
		a_pGfxContextInterface->CreateTextureBlock( m_pTextureBlock4 );
		if( NULL!=m_pTextureBlock4 )
		{
			m_pTextureBlock4->AttachImageData( m_pBumpImage2 );
			m_pTextureBlock4->SetTextureCoordinateIndex( 4 );
			m_pTextureBlock4->SetColorCalc( IGN_ENUM_TEXOPERATION_DISABLE, IGN_ENUM_TEXARGUMENT_TEXTURE, IGN_ENUM_TEXARGUMENT_DIFFUSE );
			m_pTextureBlock4->SetAlphaCalc( IGN_ENUM_TEXOPERATION_DISABLE, IGN_ENUM_TEXARGUMENT_TEXTURE, IGN_ENUM_TEXARGUMENT_DIFFUSE );
			m_pTextureBlock4->SetTextureAdressing( IGN_ENUM_TEXADDRESS_WRAP, IGN_ENUM_TEXADDRESS_WRAP, IGN_ENUM_TEXADDRESS_WRAP );
			m_pTextureBlock4->SetTextureFiltering( IGN_ENUM_TEXFILTER_LINEAR, IGN_ENUM_TEXFILTER_LINEAR, IGN_ENUM_TEXFILTER_LINEAR );
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
			m_pMaterial->SetColorFactor( 0xAf000000 );
			m_pMaterial->SetBlending( IGN_ENUM_BLENDING_SRCALPHA, IGN_ENUM_BLENDING_INVSRCALPHA );
			m_pMaterial->AttachTextureBlock( 0, m_pTextureBlock0 );
			m_pMaterial->AttachTextureBlock( 1, m_pTextureBlock1 );
			m_pMaterial->AttachTextureBlock( 2, m_pTextureBlock2 );
			m_pMaterial->AttachTextureBlock( 3, m_pTextureBlock3 );
			m_pMaterial->AttachTextureBlock( 4, m_pTextureBlock4 );
		}
		a_pGfxContextInterface->CreateShader( m_pShader );
		if( NULL!=m_pShader )
		{
			m_pShader->AttachMaterial( m_pMaterial );
			m_pShader->AttachVertexProgDcl( m_pWaterVertexProgDcl );
		}	
		a_pGfxContextInterface->CreateVertexData( m_pVertexData );
		if( NULL!=m_pVertexData )
		{
			IGN_STRUCT_VertexBufDesc VertexBufferDescription;
			VertexBufferDescription.m_uiLength	= 128*128*64;
			VertexBufferDescription.m_dwUsage	= 0;
			VertexBufferDescription.m_dwFVF		= IGNFVF_XYZ|IGNFVF_NORMAL|IGNFVF_TEX4|IGNFVF_TEXCOORDSIZE2_0|IGNFVF_TEXCOORDSIZE3_1|IGNFVF_TEXCOORDSIZE3_2|IGNFVF_TEXCOORDSIZE2_3;
			VertexBufferDescription.m_Pool		= IGN_ENUM_POOLTYPE_MANAGED;
			m_pVertexData->CreateBuffer( VertexBufferDescription );
			unsigned char* pData = NULL;
			m_pVertexData->LockBuffer( 0, 0, 0, &pData );
			if( NULL!=pData )
			{
				structVertexFormatWater* pVertexData = reinterpret_cast<structVertexFormatWater*>( pData );
				for( IGNU32 u32ZIndex = 0; u32ZIndex<128; u32ZIndex++ )
				{
					for( IGNU32 u32XIndex = 0; u32XIndex<128; u32XIndex++ )
					{
						pVertexData->m_Position.m_fX	=	-128.0f + u32XIndex*2;
						pVertexData->m_Position.m_fY	=	0.0f;
						pVertexData->m_Position.m_fZ	=	-128.0f + u32ZIndex*2;
						pVertexData->m_Normal.m_fX		=	0.0f;
						pVertexData->m_Normal.m_fY		=	1.0f;
						pVertexData->m_Normal.m_fZ		=	0.0f;
						pVertexData->m_TexCoord0_U		=	static_cast<IGNFLOAT>( u32XIndex );
						pVertexData->m_TexCoord0_V		=	static_cast<IGNFLOAT>( u32ZIndex );
						pVertexData->m_TexCoord1_U		=	0.0f;
						pVertexData->m_TexCoord1_V		=	0.0f;
						pVertexData->m_TexCoord1_S		=	0.0f;
						pVertexData->m_TexCoord2_U		=	0.0f;
						pVertexData->m_TexCoord2_V		=	0.0f;
						pVertexData->m_TexCoord2_S		=	0.0f;
						pVertexData->m_TexCoord3_U		=	u32XIndex/32.f;
						pVertexData->m_TexCoord3_V		=	u32ZIndex/32.f;
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
			IndexBufferDescription.m_uiLength	= (127*(128*2+2))*4;
			IndexBufferDescription.m_dwUsage	= IGNUSAGE_WRITEONLY;
			IndexBufferDescription.m_Format		= IGN_ENUM_INDEXFORMAT_INDEX32;
			IndexBufferDescription.m_Pool		= IGN_ENUM_POOLTYPE_MANAGED;
			m_pIndexData->CreateBuffer( IndexBufferDescription );
			unsigned char* pData = NULL;
			m_pIndexData->LockBuffer( 0, 0, 0, &pData );
			if( NULL!=pData )
			{
				unsigned int data[127*(128*2+2)];
				IGNU32 u32Index = 0;
				for( IGNU32 u32IZ = 0; u32IZ<127; u32IZ++ )
				{
					for( IGNU32 u32IX = 0; u32IX<128; u32IX++ )
					{
						data[u32Index]	= u32IZ*128 + u32IX;
						u32Index++;
						data[u32Index]	= u32IZ*128 + u32IX + 128;
						u32Index++;
					}
					data[u32Index]	= u32IZ*128 + u32IX-1 + 128;
					u32Index++;
					data[u32Index]	= u32IZ*128 + u32IX-1 + 1;
					u32Index++;
				}
				memcpy( pData, data, (127*(128*2+2))*4 );
			}
			m_pIndexData->UnlockBuffer();
		}
		a_pGfxContextInterface->CreateVisual( m_pVisual );
		if( NULL!=m_pVisual )
		{
			IGN_STRUCT_DrawCallDesc	DrawCallDesc;
			DrawCallDesc.m_bIndexedDrawCall		= true;
			DrawCallDesc.m_PrimitiveType		= IGN_ENUM_PRIMITIVETYPE_TRIANGLESTRIP;
			DrawCallDesc.m_u32PrimitiveCount	= 127*(128*2+2)-2;
			DrawCallDesc.m_u32StartVertex		= 0;
			DrawCallDesc.m_u32MinIndex			= 0;
			DrawCallDesc.m_u32NumberOfVertices	= 128*128;
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


void WATER::Finit												(	void )
{
	//----------------------------------------------------------|----------------------------------------------Finit-|
	// public method
	// Finalizes the tester.

	//
	// Release objects in IGN_timer_sub.dll
	//
	if( NULL!=m_pTimerInterface )
		m_pTimerInterface->Release();
	if( NULL!=m_pTmrContextInterface )
		m_pTmrContextInterface->Release();
	if( NULL!=m_pTmrInstanceInterface )
		m_pTmrInstanceInterface->Release();
	if( NULL!=m_hTmrLibrary )
		FreeLibrary( m_hTmrLibrary );

	//
	// Release objects in IGN_graphic_sub.dll
	//
	if( NULL!=m_pMirrorCamera )
		m_pMirrorCamera->Release();
	if( NULL!=m_pShader )
		m_pShader->Release();
	if( NULL!=m_pMaterial )
		m_pMaterial->Release();
	if( NULL!=m_pTextureBlock0 )
		m_pTextureBlock0->Release();
	if( NULL!=m_pTextureBlock1 )
		m_pTextureBlock1->Release();
	if( NULL!=m_pTextureBlock2 )
		m_pTextureBlock2->Release();
	if( NULL!=m_pTextureBlock3 )
		m_pTextureBlock3->Release();
	if( NULL!=m_pTextureBlock4 )
		m_pTextureBlock4->Release();
	if( NULL!=m_pWaterVertexProgDcl )
		m_pWaterVertexProgDcl->Release();
	if( NULL!=m_pRefractionVertexProgDcl )
		m_pRefractionVertexProgDcl->Release();
	if( NULL!=m_pWaterVertexProgramDX8GF3 )
		m_pWaterVertexProgramDX8GF3->Release();
	if( NULL!=m_pWaterVertexProgramDX8 )
		m_pWaterVertexProgramDX8->Release();
	if( NULL!=m_pWaterVertexProgramDX9 )
		m_pWaterVertexProgramDX9->Release();
	if( NULL!=m_pWaterPixelProgramDX8 )
		m_pWaterPixelProgramDX8->Release();
	if( NULL!=m_pWaterPixelProgramDX9 )
		m_pWaterPixelProgramDX9->Release();
	if( NULL!=m_pRefractionVertexProgramDX9 )
		m_pRefractionVertexProgramDX9->Release();
	if( NULL!=m_pRefractionPixelProgramDX9 )
		m_pRefractionPixelProgramDX9->Release();
	if( NULL!=m_pReflectionImage )
		m_pReflectionImage->Release();
	if( NULL!=m_pRefractionImage )
		m_pRefractionImage->Release();
	if( NULL!=m_pDetailImage )
		m_pDetailImage->Release();
	if( NULL!=m_pBumpImage1 )
		m_pBumpImage1->Release();
	if( NULL!=m_pBumpImage2 )
		m_pBumpImage2->Release();
	if( NULL!=m_pVertexData )
		m_pVertexData->Release();
	if( NULL!=m_pIndexData )
		m_pIndexData->Release();
	if( NULL!=m_pVisual )
		m_pVisual->Release();
	m_pMixer->DestroySpecialDepthBuffer( m_hReflectionDepthBuffer );
	m_pMixer->DestroySpecialDepthBuffer( m_hRefractionDepthBuffer );
}


IGNBOOL WATER::BuildReflectionTexture							(	void )
{
	//----------------------------------------------------------|-----------------------------BuildReflectionTexture-|
	// public method
	// Proceeds the water.

	if( NULL!=m_pMixer )
	{
		//
		// Get the current view and projection matrix
		//
		IGN_STRUCT_4x4Matrix OriginalViewMatrix;
		m_pOriginalCamera->GetViewMatrix( OriginalViewMatrix );
		IGN_STRUCT_4x4Matrix OriginalProjectionMatrix;
		m_pOriginalCamera->GetProjectionMatrix( OriginalProjectionMatrix );

		//
		// Set mirror camera
		//
		IGN_STRUCT_4x4Matrix MirrorViewMatrix;
		MirrorViewMatrix = OriginalViewMatrix;
		IGN_STRUCT_4x4Matrix TrafoMatrix;
		TrafoMatrix.SetIdentity();
		TrafoMatrix.m_Elements.m_f22 = -1.0f;
		TrafoMatrix.m_Elements.m_f42 = 2.0f*m_fWaterHeight;
		MirrorViewMatrix.MultiplyMatrixFromLeftSide( TrafoMatrix );
		m_pMirrorCamera->SetViewMatrix( MirrorViewMatrix );
		IGN_STRUCT_4x4Matrix MirrorProjectionMatrix;
		MirrorProjectionMatrix = OriginalProjectionMatrix;
		m_pMirrorCamera->SetProjectionMatrix( MirrorProjectionMatrix );
		m_pMirrorCamera->Execute();
		
		//
		// Get the current viewport
		//
		IGN_STRUCT_Viewport OriginalViewport;
		m_pMixer->GetViewport( OriginalViewport );

		//
		// Set new render target
		//
		IGNHANDLE hReflectionRenderTargetHandle;
		m_pReflectionImage->GetRenderTargetHandle( IGN_ENUM_CUBEMAPFACE_NONE, hReflectionRenderTargetHandle );
		IGNHANDLE hOriginalRenderTargetHandle;
		IGNHANDLE m_hOriginalDepthBuffer;
		m_pMixer->GetRenderTarget( hOriginalRenderTargetHandle, m_hOriginalDepthBuffer );
		m_pMixer->SetRenderTarget( hReflectionRenderTargetHandle, m_hReflectionDepthBuffer );

		//
		// Set new viewport
		//
		IGN_STRUCT_Viewport CameraViewport;
		CameraViewport.m_u32X		= 0;
		CameraViewport.m_u32Y		= 0;
		CameraViewport.m_u32Width	= constReflectionTextureWidth;
		CameraViewport.m_u32Height	= constReflectionTextureHeight;
		CameraViewport.m_fMinZ		= 0.0f;
		CameraViewport.m_fMaxZ		= 1.0f;
		m_pMixer->SetViewport( CameraViewport );
		m_pMixer->ClearViewport( static_cast<IGN_ENUM_VIEWPORTCLEAR>( 
			IGN_ENUM_VIEWPORTCLEAR_COLOR|IGN_ENUM_VIEWPORTCLEAR_DEPTH ), 
			0x00000000, 
			1.0f, 
			0 );

		//
		// Change culling mode
		//
		IGN_ENUM_MASTERCULLINGMODE OriginalCullingMode;
		m_pMixer->GetMasterCullingMode( OriginalCullingMode );
		m_pMixer->SetMasterCullingMode( IGN_ENUM_MASTERCULLINGMODE_CW );

		//
		// Enable clip plane
		//
		m_pMixer->SetClipPlane( 1, true );
		IGN_STRUCT_PlaneData ClipPlane;
		ClipPlane.m_fA	= 0.0f;
		ClipPlane.m_fB	= 1.0f;
		ClipPlane.m_fC	= 0.0f;
		if( ENUM_TARGETCLASS_DX9==m_TargetClass )
			ClipPlane.m_fD	= -m_fWaterHeight;
		else
			//
			// Additive bias to avoid filter artefacts
			//
			ClipPlane.m_fD	= -m_fWaterHeight + 0.25f;
		m_pMixer->SetClipPlaneData( 1, ClipPlane );

		//
		// Proceed with the terrain
		//
		if( NULL!=m_pTerrain )
			m_pTerrain->Proceed();
		else 
			return false;

		//
		// Disable clip plane
		//
		m_pMixer->SetClipPlane( 1, false );

		//
		// Reset culling mode
		//
		m_pMixer->SetMasterCullingMode( OriginalCullingMode );

		//
		// Proceed with the skydome
		//
		if( NULL!=m_pSkydome )
			m_pSkydome->Proceed( m_pMirrorCamera );
		else 
			return false;
		
		//
		// Reset render target
		//
		m_pMixer->SetRenderTarget( hOriginalRenderTargetHandle, m_hOriginalDepthBuffer );

		//
		// Reset viewport
		//
		m_pMixer->SetViewport( OriginalViewport );

		//
		// Set original view and projection matrix
		//
		m_pMirrorCamera->SetViewMatrix( OriginalViewMatrix );
		m_pMirrorCamera->SetProjectionMatrix( OriginalProjectionMatrix );
		m_pMirrorCamera->Execute();

		//
		// Calculate and set texture trafo matrix
		//
		m_ReflectionTextureMatrix.Clear();
		m_ReflectionTextureMatrix.m_Elements.m_f11	= 0.5f;
		m_ReflectionTextureMatrix.m_Elements.m_f22	= -0.5f;
		m_ReflectionTextureMatrix.m_Elements.m_f41	= 0.5f;
		m_ReflectionTextureMatrix.m_Elements.m_f42	= 0.5f;
		m_ReflectionTextureMatrix.m_Elements.m_f43	= 1.0f;
		m_ReflectionTextureMatrix.m_Elements.m_f44	= 1.0f;
		m_ReflectionTextureMatrix.MultiplyMatrixFromLeftSide( MirrorProjectionMatrix );
		m_ReflectionTextureMatrix.MultiplyMatrixFromLeftSide( MirrorViewMatrix );
	}
	return true;
}


IGNBOOL WATER::BuildRefractionTexture							(	void )
{
	//----------------------------------------------------------|-----------------------------BuildRefractionTexture-|
	// public method
	// Proceeds the water.

	if( NULL!=m_pMixer )
	{
		//
		// Get the current viewport
		//
		IGN_STRUCT_Viewport OriginalViewport;
		m_pMixer->GetViewport( OriginalViewport );

		//
		// Set new render target
		//
		IGNHANDLE hRefractionRenderTargetHandle;
		m_pRefractionImage->GetRenderTargetHandle( IGN_ENUM_CUBEMAPFACE_NONE, hRefractionRenderTargetHandle );
		IGNHANDLE hOriginalRenderTargetHandle;
		IGNHANDLE m_hOriginalDepthBuffer;
		m_pMixer->GetRenderTarget( hOriginalRenderTargetHandle, m_hOriginalDepthBuffer );
		m_pMixer->SetRenderTarget( hRefractionRenderTargetHandle, m_hRefractionDepthBuffer );

		//
		// Set new viewport
		//
		IGN_STRUCT_Viewport CameraViewport;
		CameraViewport.m_u32X		= 0;
		CameraViewport.m_u32Y		= 0;
		CameraViewport.m_u32Width	= constRefractionTextureWidth;
		CameraViewport.m_u32Height	= constRefractionTextureHeight;
		CameraViewport.m_fMinZ		= 0.0f;
		CameraViewport.m_fMaxZ		= 1.0f;
		m_pMixer->SetViewport( CameraViewport );
		m_pMixer->ClearViewport( static_cast<IGN_ENUM_VIEWPORTCLEAR>( 
			IGN_ENUM_VIEWPORTCLEAR_COLOR|IGN_ENUM_VIEWPORTCLEAR_DEPTH ), 
			0x00000000, 
			1.0f, 
			0 );

		//
		// Enable clip plane
		//
		m_pMixer->SetClipPlane( 1, true );
		IGN_STRUCT_PlaneData ClipPlane;
		ClipPlane.m_fA	= 0.0f;
		ClipPlane.m_fB	= -1.0f;
		ClipPlane.m_fC	= 0.0f;
		ClipPlane.m_fD	= m_fWaterHeight + 0.25f; // Additive bias to avoid filter artefacts
		m_pMixer->SetClipPlaneData( 1, ClipPlane );

		//
		// Proceed with the terrain
		//
		if( NULL!=m_pTerrain )
			m_pTerrain->Proceed();
		else 
			return false;

		//
		// Disable clip plane
		//
		m_pMixer->SetClipPlane( 1, false );

		//
		// Set special refraction shaders
		//
		switch( m_TargetClass )
		{
		case	ENUM_TARGETCLASS_DX9:
				IGN_STRUCT_4x4Matrix ViewMatrix;
				m_pOriginalCamera->GetViewMatrix( ViewMatrix );
				IGN_STRUCT_4x4Matrix ProjectionMatrix;
				m_pOriginalCamera->GetProjectionMatrix( ProjectionMatrix );
				IGN_STRUCT_4x4Matrix ViewProjectionMatrix;
				ViewProjectionMatrix = ViewMatrix;
				ViewProjectionMatrix.MultiplyMatrixFromRightSide( ProjectionMatrix );
				ViewProjectionMatrix.Transpose();
				m_pRefractionVertexProgramDX9->SetProgramRegister( 0, reinterpret_cast<IGNFLOAT*>( &ViewProjectionMatrix ), 4 );
				IGNFLOAT fConstRegister1[4];
				fConstRegister1[0] = 0.0f;
				fConstRegister1[1] = m_fWaterHeight;
				fConstRegister1[2] = 0.0f;
				fConstRegister1[3] = 0.0f;
				m_pRefractionPixelProgramDX9->SetProgramRegister( 1, fConstRegister1, 1 );
				m_pMixer->SetMasterBlending( IGN_ENUM_MASTERBLENDING_DESTCOLOR, IGN_ENUM_MASTERBLENDING_ZERO );
				if( NULL!=m_pTerrain )
					m_pTerrain->Proceed(
						m_pRefractionVertexProgDcl,
						m_pRefractionVertexProgramDX9,
						m_pRefractionPixelProgramDX9 );
				m_pMixer->SetMasterBlending( IGN_ENUM_MASTERBLENDING_MATERIAL, IGN_ENUM_MASTERBLENDING_MATERIAL );
				break;
		}
		
		//
		// Reset render target
		//
		m_pMixer->SetRenderTarget( hOriginalRenderTargetHandle, m_hOriginalDepthBuffer );

		//
		// Reset viewport
		//
		m_pMixer->SetViewport( OriginalViewport );

		//
		// Calculate and set texture trafo matrix
		//
		IGN_STRUCT_4x4Matrix ViewMatrix;
		m_pOriginalCamera->GetViewMatrix( ViewMatrix );
		IGN_STRUCT_4x4Matrix ProjectionMatrix;
		m_pOriginalCamera->GetProjectionMatrix( ProjectionMatrix );
		m_RefractionTextureMatrix.Clear();
		m_RefractionTextureMatrix.m_Elements.m_f11	= 0.5f;
		m_RefractionTextureMatrix.m_Elements.m_f22	= -0.5f;
		m_RefractionTextureMatrix.m_Elements.m_f41	= 0.5f;
		m_RefractionTextureMatrix.m_Elements.m_f42	= 0.5f;
		m_RefractionTextureMatrix.m_Elements.m_f43	= 1.0f;
		m_RefractionTextureMatrix.m_Elements.m_f44	= 1.0f;
		m_RefractionTextureMatrix.MultiplyMatrixFromLeftSide( ProjectionMatrix );
		m_RefractionTextureMatrix.MultiplyMatrixFromLeftSide( ViewMatrix );
	}
	return true;
}


IGNBOOL WATER::ProceedWater										(	void )
{
	//----------------------------------------------------------|---------------------------------------ProceedWater-|
	// public method
	// Proceeds the water.

	//
	// Render Water plane with reflection and refraction
	//
	if( NULL!=m_pVisual )
	{
		//
		// If the user switched to wireframe mode ...
		//
		if( true==m_bWireframeModus )
			m_pVisual->SetRenderMode( IGN_ENUM_RENDERMODE_WIREFRAME );
		else
			m_pVisual->SetRenderMode( IGN_ENUM_RENDERMODE_SOLID );

		IGN_STRUCT_4x4Matrix ViewMatrix;
		m_pOriginalCamera->GetViewMatrix( ViewMatrix );
		IGN_STRUCT_4x4Matrix ProjectionMatrix;
		m_pOriginalCamera->GetProjectionMatrix( ProjectionMatrix );
		IGN_STRUCT_4x4Matrix ViewProjectionMatrix;
		ViewProjectionMatrix = ViewMatrix;
		ViewProjectionMatrix.MultiplyMatrixFromRightSide( ProjectionMatrix );

		switch( m_TargetClass )
		{
		case	ENUM_TARGETCLASS_DX7:
				//
			    // !!! You have to force software vertex processing for DX7 hardware !!!
				//
				m_pVertexProgram	= m_pWaterVertexProgramDX8;
				m_pPixelProgram		= NULL;
				break;
		case	ENUM_TARGETCLASS_DX8GF3:
				m_pVertexProgram	= m_pWaterVertexProgramDX8GF3;
				m_pPixelProgram		= m_pWaterPixelProgramDX8;
				break;
		case	ENUM_TARGETCLASS_DX8:
				m_pVertexProgram	= m_pWaterVertexProgramDX8;
				m_pPixelProgram		= m_pWaterPixelProgramDX8;
				break;
		case	ENUM_TARGETCLASS_DX9:
				m_pVertexProgram	= m_pWaterVertexProgramDX9;
				m_pPixelProgram		= m_pWaterPixelProgramDX9;
				break;
		}
		if( NULL!=m_pVertexProgram )
		{
			IGNFLOAT fTime[4];
			fTime[0] = 1.0f;
			if( NULL!=m_pTimerInterface )
			{
				m_pTimerInterface->GetTime( fTime[0] );
			}
			m_pVertexProgram->SetProgramRegister( 13, &(fTime[0]), 1 );
		}
		if( m_pShader )
		{
			m_pShader->AttachVertexProgram( m_pVertexProgram );
			m_pShader->AttachPixelProgram( m_pPixelProgram );
		}

		if( NULL!=m_pVertexProgram )
		{
			ViewProjectionMatrix.Transpose();
			m_ReflectionTextureMatrix.Transpose();
			m_RefractionTextureMatrix.Transpose();
			m_pVertexProgram->SetProgramRegister( 0, reinterpret_cast<IGNFLOAT*>( &ViewProjectionMatrix ), 4 );
			m_pVertexProgram->SetProgramRegister( 4, reinterpret_cast<IGNFLOAT*>( &m_ReflectionTextureMatrix ), 4 );
			m_pVertexProgram->SetProgramRegister( 8, reinterpret_cast<IGNFLOAT*>( &m_RefractionTextureMatrix ), 4 );
			IGNFLOAT PositionAndWaterHeight[4];
			IGN_STRUCT_3DVector CamPositionVector;
			m_pOriginalCamera->GetPosition( CamPositionVector );
			PositionAndWaterHeight[0] = CamPositionVector.m_fX;
			PositionAndWaterHeight[1] = CamPositionVector.m_fY;
			PositionAndWaterHeight[2] = CamPositionVector.m_fZ;
			PositionAndWaterHeight[3] = m_fWaterHeight;
			m_pVertexProgram->SetProgramRegister( 12, PositionAndWaterHeight, 1 );
		}

		//
		// Render the water
		//
		IGNBOOL bOriginalLighting;
		m_pMixer->GetMasterLighting( bOriginalLighting );
		m_pMixer->SetMasterLighting( false );
		m_pVisual->Execute();
		m_pMixer->SetMasterLighting( bOriginalLighting );

		return true;
	}

	return false;
}


IGNBOOL WATER::CheckPosition									(	IGN_STRUCT_3DVector&	a_Position )
{
	//----------------------------------------------------------|--------------------------------------CheckPosition-|
	// public method
	// Checks the position for water collision.

	if( (m_fWaterHeight+1.5f)>a_Position.m_fY )
		a_Position.m_fY = m_fWaterHeight+1.5f;

	return true;
}


IGNBOOL WATER::PreProcess										(	CAMERA*					a_pCamera,
																	IGNFLOAT				a_fWaterHeight,
																	ENUM_TARGETCLASS		a_TargetClass,
																	SKYDOME*				a_pSkydome,
																	TERRAIN*				a_pTerrain )
{
	//----------------------------------------------------------|--------------------------------------------Proceed-|
	// public method
	// Proceeds the water.

	m_pOriginalCamera		= a_pCamera;
	m_fWaterHeight			= a_fWaterHeight;
	m_TargetClass			= a_TargetClass;
	m_pSkydome				= a_pSkydome;
	m_pTerrain				= a_pTerrain;

	//
	// Fill the reflection und the refraction textures
	//
	BuildReflectionTexture();
	BuildRefractionTexture();

	return true;
}


IGNBOOL WATER::Proceed											(	IGNBOOL					a_bWireframeModus,
																	ENUM_TARGETCLASS		a_TargetClass )
{
	//----------------------------------------------------------|--------------------------------------------Proceed-|
	// public method
	// Proceeds the water.

	m_bWireframeModus		= a_bWireframeModus;
	m_TargetClass			= a_TargetClass;

	//
	// Proceed with the water
	//
	return ProceedWater();
}
