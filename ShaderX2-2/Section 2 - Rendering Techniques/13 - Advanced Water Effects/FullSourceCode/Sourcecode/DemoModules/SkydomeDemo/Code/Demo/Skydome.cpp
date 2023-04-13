//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			SkydomeDemo
//*				File:				Skydome.cpp
//*
//********************************************************************************************************************
//*
//*				Author:				Marco Spoerl
//*									Copyright (c) 2003 Marco Spoerl
//*									All rights reserved.
//*
//********************************************************************************************************************
//********************************************************************************************************************

#include	"Skydome.h"
#include	"Sun.h"
#include	"Moon.h"
#include	<direct.h>

//********************************************************************************************************************
//*				SKYDOME STATIC DATA
//********************************************************************************************************************

IGN_STRUCT_DclElement SKYDOME::ms_Declaration[] =
{	
	{
		0, 
		0, 
		IGN_ENUM_DECLTYPE_FLOAT3,
		IGN_ENUM_DECLMETHOD_DEFAULT,
		IGN_ENUM_DECLUSAGE_POSITION,
		0
	},
	{
		0, 
		12, 
		IGN_ENUM_DECLTYPE_FLOAT3,
		IGN_ENUM_DECLMETHOD_DEFAULT,
		IGN_ENUM_DECLUSAGE_NORMAL,
		0
	},
	IGNDECL_END()
};


//********************************************************************************************************************
//*				SKYDOME
//********************************************************************************************************************

#include	"Sun.h"
#include	"Moon.h"

SKYDOME::SKYDOME												(	IGN_OBJIF_GfxContext*	a_pGfxContextInterface
																,	IGN_OBJIF_GfxAssistant*	a_pAssistant )
{
	//----------------------------------------------------------|--------------------------------------------SKYDOME-|
	// public method
	// Constructor

	m_hTmrLibrary			= NULL;
	m_pTmrInstanceInterface	= NULL;
	m_pTmrContextInterface	= NULL;
	m_pClockInterface		= NULL;
	m_pAssistant			= a_pAssistant;
	m_pVisual				= NULL;
	m_pVertexData			= NULL;
	m_pShader				= NULL;
	m_pVertexProgDcl		= NULL;
	m_pVertexProgram		= NULL;
	m_pPixelProgram			= NULL;
	m_pMaterial				= NULL;
	m_pTextureBlock0		= NULL;
	m_pTextureBlock1		= NULL;
	m_pTextureBlock2		= NULL;
	m_pImageSun				= NULL;
	m_pImageMoon			= NULL;
	m_pImageMoonNormals		= NULL;
	m_pLight				= NULL;
	m_pSun					= NULL;
	m_pMoon					= NULL;

	//
	// Initialize the system
	//

	Init( a_pGfxContextInterface );
}


SKYDOME::~SKYDOME												()
{
	//----------------------------------------------------------|-------------------------------------------~SKYDOME-|
	// public method
	// Destructor

	//
	// Finalize the system
	//
	Finit();
}


void SKYDOME::Init												(	IGN_OBJIF_GfxContext*	a_pGfxContextInterface )
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

	IGN_METHOD_GetSubHookTimer* TmrGetInterface = reinterpret_cast<IGN_METHOD_GetSubHookTimer*>( GetProcAddress( m_hTmrLibrary, "GetInterface" ) );

	if( NULL!=TmrGetInterface )
		hResult = TmrGetInterface( &pSubTimer );

	if( NULL!=pSubTimer )
		hResult = pSubTimer->m_pCreateInstance( IGN_ENUM_TMRINSTANCETYPE_WINDOWSMULTIMEDIA, m_pTmrInstanceInterface );

	if( NULL!=m_pTmrInstanceInterface )
		m_pTmrInstanceInterface->CreateContext( 1, m_pTmrContextInterface );

	if( NULL!=m_pTmrContextInterface )
		m_pTmrContextInterface->CreateClock( m_pClockInterface );

	_chdir( "..\\" );

	//
	// Create objects in IGN_graphic_sub.dll
	//

	if( NULL != a_pGfxContextInterface )
	{
		//
		// Initial sun or moon light
		//

		a_pGfxContextInterface->CreateLight( m_pLight );

		if( NULL != m_pLight )
		{
			m_pLight->SetAmbientLighting( 0xff101010 );

			m_LightData.m_Type				= IGN_ENUM_LIGHTTYPE_DIRECTIONAL;
			m_LightData.m_Diffuse.m_fRed	= 1.0f;
			m_LightData.m_Diffuse.m_fGreen	= 1.0f;
			m_LightData.m_Diffuse.m_fBlue	= 1.0f;
			m_LightData.m_Diffuse.m_fAlpha	= 0.0f;
			m_LightData.m_Ambient.m_fRed	= 0.0f;
			m_LightData.m_Ambient.m_fGreen	= 0.0f;
			m_LightData.m_Ambient.m_fBlue	= 0.0f;
			m_LightData.m_Ambient.m_fAlpha	= 0.0f;
			m_LightData.m_Specular.m_fRed	= 0.0f;
			m_LightData.m_Specular.m_fGreen	= 0.0f;
			m_LightData.m_Specular.m_fBlue	= 0.0f;
			m_LightData.m_Specular.m_fAlpha	= 0.0f;
			m_LightData.m_Direction.m_fX	= -1.0f;
			m_LightData.m_Direction.m_fY	= -0.5f;
			m_LightData.m_Direction.m_fZ	= -1.0f;
			m_LightData.m_Direction.Normalize();
			m_LightData.m_fRange			= 10000.0f;
			m_LightData.m_fAttenuation0		= 1.0f;
			m_LightData.m_fAttenuation1		= 0.0f;

			m_pLight->SetVertexLighting( 0, true );
			m_pLight->SetVertexLightingData( 0, m_LightData );
			m_pLight->Execute();
		}

		//
		// Number of rings (vertical subdivisions) and segments (horizontal subdivisions) in the skydome
		//

		IGNU32	u32NumSphereRings		= 40;
		IGNU32	u32NumSphereSegments	= 60;

		//
		// Vertexbuffer containing the skydome
		//

		a_pGfxContextInterface->CreateVertexData( m_pVertexData );

		if( NULL != m_pVertexData )
		{
			IGN_STRUCT_VertexBufDesc VertexBufferDescription;

			VertexBufferDescription.m_uiLength	= 2 * u32NumSphereRings * u32NumSphereSegments * sizeof(structVertexFormatSkydome);
			VertexBufferDescription.m_dwUsage	= IGNUSAGE_WRITEONLY;
			VertexBufferDescription.m_dwFVF		= IGNFVF_XYZ|IGNFVF_NORMAL|IGNFVF_TEX0;
			VertexBufferDescription.m_Pool		= IGN_ENUM_POOLTYPE_MANAGED;

			m_pVertexData->CreateBuffer( VertexBufferDescription );

			structVertexFormatSkydome* pVertex = NULL;
			m_pVertexData->LockBuffer( 0, 0, 0, reinterpret_cast<unsigned char**>( &pVertex ) );

			//
			// Dummy radius (shader renders the skydome on the far clipping plane)
			//

			IGNFLOAT fRadius	= 50.0f;
			IGNFLOAT fDesHeight	= 50.0f; // You may want to flatten the dome ... choose a value < fRadius
			IGNFLOAT fDesHFac	= fDesHeight / fRadius;

			//
			// Enclose the dome (75% vertical (starting at the top) and 100% horizontal)
			//

			IGNFLOAT fDeltaRingAngle	= ( 0.75f * (1.0f * 3.1416f) / u32NumSphereRings );
			IGNFLOAT fDeltaSegAngle		= ( 1.00f * (2.0f * 3.1416f) / u32NumSphereSegments );

			//
			// Build the dome
			//

			for( IGNU32 u32RingIndex = 0; u32RingIndex<u32NumSphereRings; ++u32RingIndex )
			{
				IGNFLOAT fR0 = fRadius * sinf( (u32RingIndex+0) * fDeltaRingAngle );
				IGNFLOAT fR1 = fRadius * sinf( (u32RingIndex+1) * fDeltaRingAngle );
				IGNFLOAT fY0 = fRadius * cosf( (u32RingIndex+0) * fDeltaRingAngle );
				IGNFLOAT fY1 = fRadius * cosf( (u32RingIndex+1) * fDeltaRingAngle );

				for( IGNU32 u32SegmentIndex = 0; u32SegmentIndex<u32NumSphereSegments; ++u32SegmentIndex )
				{
					IGNFLOAT fLength = 0.0f;

					IGNFLOAT fX0 = fR0 * sinf( u32SegmentIndex * fDeltaSegAngle );
					IGNFLOAT fZ0 = fR0 * cosf( u32SegmentIndex * fDeltaSegAngle );
					IGNFLOAT fX1 = fR1 * sinf( u32SegmentIndex * fDeltaSegAngle );
					IGNFLOAT fZ1 = fR1 * cosf( u32SegmentIndex * fDeltaSegAngle );

					pVertex->m_vecNormal.m_fX = pVertex->m_vecPosition.m_fX	= fX0;
					pVertex->m_vecNormal.m_fY = pVertex->m_vecPosition.m_fY	= fY0 * fDesHFac;
					pVertex->m_vecNormal.m_fZ = pVertex->m_vecPosition.m_fZ	= fZ0;
					pVertex->m_vecNormal.Normalize();

					++pVertex;

					pVertex->m_vecNormal.m_fX = pVertex->m_vecPosition.m_fX	= fX1;
					pVertex->m_vecNormal.m_fY = pVertex->m_vecPosition.m_fY	= fY1 * fDesHFac;
					pVertex->m_vecNormal.m_fZ = pVertex->m_vecPosition.m_fZ	= fZ1;
					pVertex->m_vecNormal.Normalize();

					++pVertex;
				}
			}

			m_pVertexData->UnlockBuffer();
			m_pVertexData->SetStreamNumber( 0 );
		}

		//
		// Vertex shader declaration
		//

		a_pGfxContextInterface->CreateVertexProgDcl( m_pVertexProgDcl );

		if( NULL != m_pVertexProgDcl )
		{
			m_pVertexProgDcl->CreateDeclaration( ms_Declaration );
		}

		_chdir( "Media" );
		_chdir( "Shaders" );

		//
		// Vertex program
		//

		a_pGfxContextInterface->CreateVertexProgram( m_pVertexProgram );

		if( NULL != m_pVertexProgram )
		{
			m_pAssistant->AssembleProgramFromFile( "DX8SkydomeVS.txt" );

			IGNHANDLE	hBinary;
			IGNU32		u32BinaryLength;

			m_pAssistant->GetAuxBuffer( IGN_ENUM_AUXBUFFERTYPE_BINARY, &hBinary, u32BinaryLength );
			m_pVertexProgram->CreateProgram( hBinary, false );
		}

		//
		// Pixel program
		//

		a_pGfxContextInterface->CreatePixelProgram( m_pPixelProgram );

		if( NULL != m_pPixelProgram )
		{
			m_pAssistant->AssembleProgramFromFile( "DX8SkydomePS.txt" );

			IGNHANDLE hBinary;
			IGNU32 u32BinaryLength;

			m_pAssistant->GetAuxBuffer( IGN_ENUM_AUXBUFFERTYPE_BINARY, &hBinary, u32BinaryLength );
			m_pPixelProgram->CreateProgram( hBinary );
		}

		_chdir( "..\\" );
		_chdir( "Textures" );

		//
		// Sun image
		//

		a_pGfxContextInterface->CreateImageData( m_pImageSun );

		if( NULL != m_pImageSun )
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

			m_pImageSun->SetUnitNumber( 0 );

			if( NULL != m_pAssistant )
				m_pAssistant->FillImageFromFile( m_pImageSun, LayerDesc, "SkySun.tga" );
		}

		//
		// Moon image
		//

		a_pGfxContextInterface->CreateImageData( m_pImageMoon );

		if( NULL != m_pImageMoon )
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

			m_pImageMoon->SetUnitNumber( 1 );

			if( NULL != m_pAssistant )
				m_pAssistant->FillImageFromFile( m_pImageMoon, LayerDesc, "SkyMoon.tga" );
		}

		//
		// Moon normals image
		//

		a_pGfxContextInterface->CreateImageData( m_pImageMoonNormals );

		if( NULL != m_pImageMoonNormals )
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

			m_pImageMoonNormals->SetUnitNumber( 2 );

			if( NULL != m_pAssistant )
				m_pAssistant->FillImageFromFile( m_pImageMoonNormals, LayerDesc, "SkyMoonNormals.bmp" );
		}

		//
		// Sun texture
		//

		a_pGfxContextInterface->CreateTextureBlock( m_pTextureBlock0 );

		if( NULL != m_pTextureBlock0 )
		{
			m_pTextureBlock0->AttachImageData( m_pImageSun );
			m_pTextureBlock0->SetColorCalc( IGN_ENUM_TEXOPERATION_SELECTARG1, IGN_ENUM_TEXARGUMENT_TEXTURE, IGN_ENUM_TEXARGUMENT_DIFFUSE );
			m_pTextureBlock0->SetAlphaCalc( IGN_ENUM_TEXOPERATION_SELECTARG1, IGN_ENUM_TEXARGUMENT_TEXTURE, IGN_ENUM_TEXARGUMENT_DIFFUSE );
			//
			// DX9 Retail Version + DX8 Vertex-Shader needs this line (remove it for DX9 Debug Version):
			//
			m_pTextureBlock0->SetTextureCoordinateTrafo( IGNTEXCOORDTRAFO_COUNT3|IGNTEXCOORDTRAFO_PROJECTED );

			m_pTextureBlock0->SetTextureAdressing( IGN_ENUM_TEXADDRESS_BORDER, IGN_ENUM_TEXADDRESS_BORDER, IGN_ENUM_TEXADDRESS_CLAMP );
			m_pTextureBlock0->SetTextureFiltering( IGN_ENUM_TEXFILTER_LINEAR, IGN_ENUM_TEXFILTER_LINEAR, IGN_ENUM_TEXFILTER_LINEAR );
		}

		//
		// Moon texture
		//

		a_pGfxContextInterface->CreateTextureBlock( m_pTextureBlock1 );

		if( NULL != m_pTextureBlock1 )
		{
			m_pTextureBlock1->AttachImageData( m_pImageMoon );
			m_pTextureBlock1->SetColorCalc( IGN_ENUM_TEXOPERATION_SELECTARG1, IGN_ENUM_TEXARGUMENT_TEXTURE, IGN_ENUM_TEXARGUMENT_CURRENT );
			m_pTextureBlock1->SetAlphaCalc( IGN_ENUM_TEXOPERATION_SELECTARG1, IGN_ENUM_TEXARGUMENT_TEXTURE, IGN_ENUM_TEXARGUMENT_CURRENT );

			//
			// DX9 Retail Version + DX8 Vertex-Shader needs this line (remove it for DX9 Debug Version):
			//
			m_pTextureBlock1->SetTextureCoordinateTrafo( IGNTEXCOORDTRAFO_COUNT3|IGNTEXCOORDTRAFO_PROJECTED );

			m_pTextureBlock1->SetTextureAdressing( IGN_ENUM_TEXADDRESS_BORDER, IGN_ENUM_TEXADDRESS_BORDER, IGN_ENUM_TEXADDRESS_CLAMP );
			m_pTextureBlock1->SetTextureFiltering( IGN_ENUM_TEXFILTER_LINEAR, IGN_ENUM_TEXFILTER_LINEAR, IGN_ENUM_TEXFILTER_LINEAR );
		}

		//
		// Moon normals texture
		//

		a_pGfxContextInterface->CreateTextureBlock( m_pTextureBlock2 );

		if( NULL != m_pTextureBlock2 )
		{
			m_pTextureBlock2->AttachImageData( m_pImageMoonNormals );
			m_pTextureBlock2->SetColorCalc( IGN_ENUM_TEXOPERATION_SELECTARG1, IGN_ENUM_TEXARGUMENT_TEXTURE, IGN_ENUM_TEXARGUMENT_CURRENT );
			m_pTextureBlock2->SetAlphaCalc( IGN_ENUM_TEXOPERATION_SELECTARG1, IGN_ENUM_TEXARGUMENT_TEXTURE, IGN_ENUM_TEXARGUMENT_CURRENT );
			//
			// DX9 Retail Version + DX8 Vertex-Shader needs this line (remove it for DX9 Debug Version):
			//
			m_pTextureBlock2->SetTextureCoordinateTrafo( IGNTEXCOORDTRAFO_COUNT3|IGNTEXCOORDTRAFO_PROJECTED );

			m_pTextureBlock2->SetTextureAdressing( IGN_ENUM_TEXADDRESS_BORDER, IGN_ENUM_TEXADDRESS_BORDER, IGN_ENUM_TEXADDRESS_CLAMP );
			m_pTextureBlock2->SetTextureFiltering( IGN_ENUM_TEXFILTER_LINEAR, IGN_ENUM_TEXFILTER_LINEAR, IGN_ENUM_TEXFILTER_LINEAR );
		}

		//
		// Skydome material
		//

		a_pGfxContextInterface->CreateMaterial( m_pMaterial );

		if( NULL != m_pMaterial )
		{
			m_pMaterial->SetAmbientMaterialSource( IGN_ENUM_MATERIALCOLORSRC_COLOR1 );
			m_pMaterial->AttachTextureBlock( 0, m_pTextureBlock0 );
			m_pMaterial->AttachTextureBlock( 1, m_pTextureBlock1 );
			m_pMaterial->AttachTextureBlock( 2, m_pTextureBlock2 );
		}

		//
		// Skydome shader
		//

		a_pGfxContextInterface->CreateShader( m_pShader );

		if( NULL != m_pShader )
		{
			m_pShader->AttachMaterial( m_pMaterial );
			m_pShader->AttachVertexProgDcl( m_pVertexProgDcl );
			m_pShader->AttachVertexProgram( m_pVertexProgram );
			m_pShader->AttachPixelProgram( m_pPixelProgram );
		}

		//
		// Skydome visual
		//

		a_pGfxContextInterface->CreateVisual( m_pVisual );

		if( NULL != m_pVisual )
		{
			IGN_STRUCT_DrawCallDesc	DrawCallDesc;

			DrawCallDesc.m_bIndexedDrawCall		= false;
			DrawCallDesc.m_PrimitiveType		= IGN_ENUM_PRIMITIVETYPE_TRIANGLESTRIP;
			DrawCallDesc.m_u32PrimitiveCount	= 2 * u32NumSphereRings * u32NumSphereSegments - 2;
			DrawCallDesc.m_u32StartVertex		= 0;
			DrawCallDesc.m_u32MinIndex			= 0;
			DrawCallDesc.m_u32NumberOfVertices	= 2 * u32NumSphereRings * u32NumSphereSegments;
			DrawCallDesc.m_u32StartIndex		= 0;
			DrawCallDesc.m_u32BaseVertexIndex	= 0;

			m_pVisual->SetDrawCallDescription( DrawCallDesc );
			m_pVisual->SetCullingMode( IGN_ENUM_CULLINGMODE_NONE );
			m_pVisual->SetDepthTesting( IGN_ENUM_CMPFUNC_LESSEQUAL );
			m_pVisual->AttachVertexData( m_pVertexData );
			m_pVisual->AttachShader( m_pShader );
		}

		_chdir( "..\\" );
		_chdir( "..\\" );

	}

	//
	// Sky data
	//

	m_pSun	= new SUN( a_pGfxContextInterface );
	m_pSun->SetTime( 1990, 11, 28, 12, 0 );
	m_pSun->Prepare();

	m_pMoon	= new MOON( a_pGfxContextInterface );
	m_pMoon->SetTime( 1990, 11, 28, 12, 0 );
	m_pMoon->Prepare( m_pSun->GetLongitude(), m_pSun->GetMeanAnomaly() );

	m_dLastPhaseAngle = m_pMoon->GetPhaseAngle();
}

void SKYDOME::Finit												( void )
{
	//----------------------------------------------------------|----------------------------------------------Finit-|
	// public method
	// Finalizes the tester.

	//
	// Clear sky data
	//

	delete m_pSun;
	delete m_pMoon;

	//
	// Release objects in IGN_timer_sub.dll
	//

	if( NULL != m_pClockInterface )
		m_pClockInterface->Release();

	if( NULL != m_pTmrContextInterface )
		m_pTmrContextInterface->Release();

	if( NULL != m_pTmrInstanceInterface )
		m_pTmrInstanceInterface->Release();

	if( NULL != m_hTmrLibrary )
		FreeLibrary( m_hTmrLibrary );

	//
	// Release objects in IGN_graphic_sub.dll
	//

	if( NULL != m_pShader )
		m_pShader->Release();

	if( NULL != m_pMaterial )
		m_pMaterial->Release();

	if( NULL != m_pTextureBlock0 )
		m_pTextureBlock0->Release();

	if( NULL != m_pTextureBlock1 )
		m_pTextureBlock1->Release();

	if( NULL != m_pTextureBlock2 )
		m_pTextureBlock2->Release();

	if( NULL != m_pImageSun )
		m_pImageSun->Release();

	if( NULL != m_pImageMoon )
		m_pImageMoon->Release();

	if( NULL != m_pImageMoonNormals )
		m_pImageMoonNormals->Release();

	if( NULL != m_pVertexProgDcl )
		m_pVertexProgDcl->Release();

	if( NULL != m_pVertexProgram )
		m_pVertexProgram->Release();

	if( NULL != m_pPixelProgram )
		m_pPixelProgram->Release();

	if( NULL != m_pVertexData )
		m_pVertexData->Release();

	if( NULL != m_pVisual )
		m_pVisual->Release();
}

IGNBOOL SKYDOME::ProceedSkyDome									( const IGN_STRUCT_4x4Matrix& a_matViewMatrix
																, const IGN_STRUCT_4x4Matrix& a_matProjectionMatrix
																, const IGN_STRUCT_3DVector& a_vecCameraPosition )
{
	//----------------------------------------------------------|-------------------------------------ProceedSkydome-|
	// public method
	// Proceeds the skydome.

	if( NULL != m_pVisual && NULL != m_pVertexProgram )
	{
		//
		// Compute sun/moon position and color
		//

		if( NULL != m_pSun && NULL != m_pMoon )
		{
			m_pSun->Prepare();
			m_pMoon->Prepare( m_pSun->GetLongitude(), m_pSun->GetMeanAnomaly() );
		}

		//
		// Build the transformation matrix for the skydome
		//

		IGN_STRUCT_4x4Matrix	matViewProjection = a_matViewMatrix;
								matViewProjection.m_Elements.m_f41	= 0.0f;
								matViewProjection.m_Elements.m_f42	= 0.0f;
								matViewProjection.m_Elements.m_f43	= 0.0f;
								matViewProjection.MultiplyMatrixFromRightSide( const_cast<IGN_STRUCT_4x4Matrix&>(a_matProjectionMatrix) );

		//
		// Build the projection matrices for the sky objects
		//

		IGN_STRUCT_4x4Matrix matTexSpace;

		matTexSpace.Clear();
		matTexSpace.m_Elements.m_f11	=  0.5f;
		matTexSpace.m_Elements.m_f22	= -0.5f;
		matTexSpace.m_Elements.m_f41	=  0.5f;
		matTexSpace.m_Elements.m_f42	=  0.5f;
		matTexSpace.m_Elements.m_f43	=  1.0f;
		matTexSpace.m_Elements.m_f44	=  1.0f;

		IGN_STRUCT_4x4Matrix matProjection, matView;

		if( NULL != m_pSun )
		{
			m_pSun->GetProjectionMatrix( matProjection );
			m_pSun->GetViewMatrix( matView );
		}

		IGN_STRUCT_4x4Matrix matViewProjTexSun = matTexSpace;

		matViewProjTexSun.MultiplyMatrixFromLeftSide( matProjection );
		matViewProjTexSun.MultiplyMatrixFromLeftSide( matView );

		if( NULL != m_pMoon )
		{
			m_pMoon->GetProjectionMatrix( matProjection );
			m_pMoon->GetViewMatrix( matView );
		}

		IGN_STRUCT_4x4Matrix matViewProjTexMoon = matTexSpace;

		matViewProjTexMoon.MultiplyMatrixFromLeftSide( matProjection );
		matViewProjTexMoon.MultiplyMatrixFromLeftSide( matView );

		//
		// Transpose the matrices for the vertex shader
		//

		matViewProjection	.Transpose();
		matViewProjTexSun	.Transpose();
		matViewProjTexMoon	.Transpose();

		//
		// Prepare vertex shader constants
		//

		IGN_STRUCT_4DVector	C11;
		IGN_STRUCT_4DVector	C12;
		IGN_STRUCT_4DVector	C13;
		IGN_STRUCT_4DVector	C14;
		IGN_STRUCT_4DVector	C15;
		IGN_STRUCT_4DVector	C16;
		IGN_STRUCT_4DVector	C17;

		if( NULL != m_pSun )
		{
			C11.m_fX = m_pSun->GetColor().m_fRed;
			C11.m_fY = m_pSun->GetColor().m_fGreen;
			C11.m_fZ = m_pSun->GetColor().m_fBlue;
			C11.m_fW = 0.0f;

			C12.m_fX = m_pSun->GetNormal().m_fX;
			C12.m_fY = m_pSun->GetNormal().m_fY;
			C12.m_fZ = m_pSun->GetNormal().m_fZ;
			C12.m_fW = 0.0f;
		}

		C13.m_fX = 0.0005f;
		C13.m_fY = 0.00025f;
		C13.m_fZ = 0.0003f;
		C13.m_fW = 0.0f;

		C14.m_fX = 0.00001f;
		C14.m_fY = 0.0004f;
		C14.m_fZ = 0.0008f;
		C14.m_fW = 0.0f;

		C15.m_fX = 0.0f;
		C15.m_fY = 1.0f;
		C15.m_fZ = 0.65f;
		C15.m_fW = 0.9f;

		IGNFLOAT fInvR = 1.0f / 50.0f;

		C16.m_fX = -1000.0f * fInvR * fInvR;
		C16.m_fY = 3000.0f;
		C16.m_fZ = 0.0f;
		C16.m_fW = 2.0f;

		IGN_STRUCT_3DVector vecLight(0.0f, 0.0f, -1.0f);

		if( NULL != m_pMoon )
		{
			IGNDOUBLE dPhaseAngle = m_pMoon->GetPhaseAngle();

			if( dPhaseAngle <= m_dLastPhaseAngle )
				dPhaseAngle = 360.0 - dPhaseAngle;
			
			m_dLastPhaseAngle = m_pMoon->GetPhaseAngle();

			IGN_STRUCT_4x4Matrix	matRotation;
									matRotation.GenerateYawRotate( static_cast<IGNFLOAT>(dPhaseAngle * IGN_DegToRad) );

			vecLight.MultiplyMatrix( matRotation );
			vecLight.Normalize();
		}

		C17.m_fX = vecLight.m_fX;
		C17.m_fY = vecLight.m_fY;
		C17.m_fZ = vecLight.m_fZ;
		C17.m_fW = 0.5f;

		m_pVertexProgram->SetProgramRegister( 0, reinterpret_cast<IGNFLOAT*>( &matViewProjection ), 4 );
		m_pVertexProgram->SetProgramRegister( 5, reinterpret_cast<IGNFLOAT*>( &matViewProjTexSun ), 3 );
		m_pVertexProgram->SetProgramRegister( 8, reinterpret_cast<IGNFLOAT*>( &matViewProjTexMoon ), 3 );
		m_pVertexProgram->SetProgramRegister( 11, reinterpret_cast<IGNFLOAT*>( &C11 ), 1 );
		m_pVertexProgram->SetProgramRegister( 12, reinterpret_cast<IGNFLOAT*>( &C12 ), 1 );
		m_pVertexProgram->SetProgramRegister( 13, reinterpret_cast<IGNFLOAT*>( &C13 ), 1 );
		m_pVertexProgram->SetProgramRegister( 14, reinterpret_cast<IGNFLOAT*>( &C14 ), 1 );
		m_pVertexProgram->SetProgramRegister( 15, reinterpret_cast<IGNFLOAT*>( &C15 ), 1 );
		m_pVertexProgram->SetProgramRegister( 16, reinterpret_cast<IGNFLOAT*>( &C16 ), 1 );
		m_pVertexProgram->SetProgramRegister( 17, reinterpret_cast<IGNFLOAT*>( &C17 ), 1 );

		//
		// Prepare pixel shader constants
		//

		IGN_STRUCT_4DVector C0;

		C0.m_fX = 1.0f;
		C0.m_fY = 1.0f;
		C0.m_fZ = 1.0f;
		C0.m_fW = 1.0f;

		if( NULL != m_pSun )
		{
			if( m_pSun->GetNormal().m_fY > 0.0f )
			{
				C0.m_fX -= m_pSun->GetNormal().m_fY;
				C0.m_fY -= m_pSun->GetNormal().m_fY;
				C0.m_fZ -= m_pSun->GetNormal().m_fY;
				C0.m_fW -= m_pSun->GetNormal().m_fY;
			}
		}

		m_pPixelProgram->SetProgramRegister( 0, reinterpret_cast<IGNFLOAT*>( &C0 ), 1 );

		//
		// If the user switched to wireframe mode ...
		//

		if( true == m_bWireframeModus )
			m_pVisual->SetRenderMode( IGN_ENUM_RENDERMODE_WIREFRAME );
		else
			m_pVisual->SetRenderMode( IGN_ENUM_RENDERMODE_SOLID );

		//
		// Render the skydome
		//

		m_pVisual->Execute();

		//
		// Set the light
		//

		if( NULL != m_pSun )
		{
			if( NULL != m_pLight )
			{
				m_LightData.m_Ambient.m_fRed	= m_pSun->GetColor().m_fRed		* 0.1f;
				m_LightData.m_Ambient.m_fGreen	= m_pSun->GetColor().m_fGreen	* 0.1f;
				m_LightData.m_Ambient.m_fBlue	= m_pSun->GetColor().m_fBlue	* 0.1f;
				m_LightData.m_Diffuse.m_fRed	= m_pSun->GetColor().m_fRed		* 0.9f;
				m_LightData.m_Diffuse.m_fGreen	= m_pSun->GetColor().m_fGreen	* 0.9f;
				m_LightData.m_Diffuse.m_fBlue	= m_pSun->GetColor().m_fBlue	* 0.9f;
				m_LightData.m_Position			= m_pSun->GetPosition();
				m_LightData.m_Direction.m_fX	= -m_pSun->GetNormal().m_fX;
				m_LightData.m_Direction.m_fY	= -m_pSun->GetNormal().m_fY;
				m_LightData.m_Direction.m_fZ	= -m_pSun->GetNormal().m_fZ;
				m_LightData.m_Direction.Normalize();

				m_pLight->SetVertexLightingData( 0, m_LightData );
				m_pLight->Execute();
			}
		}

		return true;
	}

	return false;
}

void SKYDOME::AdvanceTime										( const IGNDOUBLE& a_dTimeOffset )
{
	//----------------------------------------------------------|----------------------------------------AdvanceTime-|
	// public method
	// Advances the skydome's time. Only affects sun and moon.

	if( NULL != m_pSun && NULL != m_pMoon )
	{
		m_pSun->AdvanceTime( a_dTimeOffset );
		m_pMoon->AdvanceTime( a_dTimeOffset );
	}
}

IGNBOOL SKYDOME::Proceed										(	CAMERA*					a_pCamera,
																	IGNBOOL					a_bWireframeModus )
{
	//----------------------------------------------------------|--------------------------------------------Proceed-|
	// public method
	// Proceeds the skydome.

	m_bWireframeModus	= a_bWireframeModus;

	IGN_STRUCT_4x4Matrix	matViewMatrix;
	IGN_STRUCT_4x4Matrix	matProjectionMatrix;
	IGN_STRUCT_3DVector		vecCameraPosition;

	a_pCamera->GetViewMatrix		( matViewMatrix );
	a_pCamera->GetProjectionMatrix	( matProjectionMatrix );
	a_pCamera->GetPosition			( vecCameraPosition );

	//
	// Proceed with the skydome
	//

	return ProceedSkyDome( matViewMatrix, matProjectionMatrix, vecCameraPosition );
}

IGNBOOL SKYDOME::Proceed										(	IGN_OBJIF_GfxCamera*	a_pCamera )
{
	//----------------------------------------------------------|--------------------------------------------Proceed-|
	// public method
	// Proceeds the skydome.

	m_bWireframeModus	= false;

	IGN_STRUCT_4x4Matrix	matViewMatrix;
	IGN_STRUCT_4x4Matrix	matProjectionMatrix;
	IGN_STRUCT_3DVector		vecCameraPosition	(0.0f, 0.0f, 0.0f);

	a_pCamera->GetViewMatrix		( matViewMatrix );
	a_pCamera->GetProjectionMatrix	( matProjectionMatrix );

	//
	// Proceed with the skydome
	//

	return ProceedSkyDome( matViewMatrix, matProjectionMatrix, vecCameraPosition );
}
