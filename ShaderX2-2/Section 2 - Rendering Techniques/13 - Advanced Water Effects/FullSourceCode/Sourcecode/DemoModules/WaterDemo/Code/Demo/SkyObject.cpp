//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			SkydomeDemo
//*				File:				SkyObject.cpp
//*
//********************************************************************************************************************
//*
//*				Author:				Marco Spoerl
//*									Copyright (c) 2003 Marco Spoerl
//*									All rights reserved.
//*
//********************************************************************************************************************
//********************************************************************************************************************

#include	"SkyObject.h"

//********************************************************************************************************************
//*				SKYOBJECT
//********************************************************************************************************************

SKYOBJECT::SKYOBJECT											( IGN_OBJIF_GfxContext*	a_pGfxContextInterface )
{
	//----------------------------------------------------------|--------------------------------------------SKYDOME-|
	// protected method
	// Constructor

	//
	// Load IGN_camera_sys.dll and create objects
	//

	IGN_STRUCT_IFSysCamera* pSysCamera = NULL;

	m_hCamLibrary = LoadLibrary( "Ign_camera_sys.dll" );

	IGN_METHOD_GetSysHookCamera* CamGetInterface = reinterpret_cast<IGN_METHOD_GetSysHookCamera*>( GetProcAddress( m_hCamLibrary, "GetInterface" ) );

	if( NULL != CamGetInterface )
		CamGetInterface( &pSysCamera );

	if( NULL != pSysCamera )
		pSysCamera->m_pCreateInstance( IGN_ENUM_CAMINSTANCETYPE_STANDARD, m_pCamInstanceInterface );

	if( NULL != m_pCamInstanceInterface )
		m_pCamInstanceInterface->CreateContext( 1, m_pCamContextInterface );

	if( NULL != m_pCamContextInterface )
		m_pCamContextInterface->CreateFreeCamera( m_pFreeCameraInterface );

	//
	// Initialize the free camera
	//

	if( NULL != m_pFreeCameraInterface )
	{
		m_pFreeCameraInterface->AttachGfxContext( a_pGfxContextInterface );
		m_pFreeCameraInterface->SetNearAndFarClipping( 1.0f, 1000.0f );
		m_pFreeCameraInterface->SetPosition( IGN_STRUCT_3DVector( 0.0f, 0.0f, 0.0f ) );
		m_pFreeCameraInterface->SetViewDirection( IGN_STRUCT_3DVector( 0.0f, 0.0f, 1.0f ) );
		m_pFreeCameraInterface->SetViewUp( IGN_STRUCT_3DVector( 0.0f, 1.0f, 0.0f ) );
		m_pFreeCameraInterface->FixViewUp( true );
	}

	//
	// Initialize basic sky object data
	//

	m_dTime = 0.0;
}

SKYOBJECT::~SKYOBJECT											()
{
	//----------------------------------------------------------|-------------------------------------------~SKYDOME-|
	// public method
	// Destructor

	//
	// Release objects in IGN_camera_sys.dll
	//

	if( NULL != m_pFreeCameraInterface )
		m_pFreeCameraInterface->Release();

	if( NULL != m_pCamContextInterface )
		m_pCamContextInterface->Release();

	if( NULL != m_pCamInstanceInterface )
		m_pCamInstanceInterface->Release();

	if( NULL != m_hCamLibrary )
		FreeLibrary( m_hCamLibrary );
}

IGNDOUBLE SKYOBJECT::cbrt										( const IGNDOUBLE& x ) const
{
	//----------------------------------------------------------|-----------------------------------------------cbrt-|
	// public method
	// Computes the cube root

	if( x > 0.0 )
		return exp( log( x ) / 3.0 );
	else if( x < 0.0 )
		return -cbrt( -x );
	else
		return 0.0;
}
