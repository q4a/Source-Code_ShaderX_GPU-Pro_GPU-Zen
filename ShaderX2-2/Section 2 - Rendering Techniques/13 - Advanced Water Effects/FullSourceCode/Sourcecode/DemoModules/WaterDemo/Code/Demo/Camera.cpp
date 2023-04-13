//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			WaterDemo
//*				File:				Camera.cpp
//*
//********************************************************************************************************************
//*
//*				Author:				Kurt Pelzer
//*									Copyright (c) 2003 Kurt Pelzer
//*									All rights reserved.
//*
//********************************************************************************************************************
//********************************************************************************************************************




#include		"Camera.h"
#include		<direct.h>




//********************************************************************************************************************
//*				CAMERA
//********************************************************************************************************************

CAMERA::CAMERA													(	HWND					a_hWnd,
																	IGN_OBJIF_GfxContext*	a_pGfxContextInterface )
{
	//----------------------------------------------------------|---------------------------------------------CAMERA-|
	// public method
	// Constructor
	
	m_hCamLibrary			= NULL;
	m_hTmrLibrary			= NULL;
	m_pCamInstanceInterface	= NULL;
	m_pCamContextInterface	= NULL;
	m_pFreeCameraInterface	= NULL;
	m_pTmrInstanceInterface	= NULL;
	m_pTmrContextInterface	= NULL;
	m_pTimerInterface		= NULL;

	//
	// Initialize the system
	//
	Init( a_hWnd, a_pGfxContextInterface );
}


CAMERA::~CAMERA											()
{
	//----------------------------------------------------------|--------------------------------------------~CAMERA-|
	// public method
	// Destructor

	//
	// Finalize the system
	//
	Finit();
}


void CAMERA::Init												(	HWND					a_hWnd,
																	IGN_OBJIF_GfxContext*	a_pGfxContextInterface )
{
	//----------------------------------------------------------|-----------------------------------------------Init-|
	// public method
	// Initializes the tester.

	_chdir( "System" );
	IGNRESULT hResult = 0;

	//
	// Load IGN_camera_sys.dll and create objects
	//
	IGN_STRUCT_IFSysCamera* pSysCamera = NULL;
	m_hCamLibrary = LoadLibrary( "Ign_camera_sys.dll" );
	IGN_METHOD_GetSysHookCamera* 
			CamGetInterface = reinterpret_cast<IGN_METHOD_GetSysHookCamera*>( GetProcAddress( m_hCamLibrary, "GetInterface" ) );
	if( NULL!=CamGetInterface )
		hResult = CamGetInterface( &pSysCamera );
	if( NULL!=pSysCamera )
		hResult = pSysCamera->m_pCreateInstance( IGN_ENUM_CAMINSTANCETYPE_STANDARD, m_pCamInstanceInterface );
	if( NULL!=m_pCamInstanceInterface )
		m_pCamInstanceInterface->CreateContext( 1, m_pCamContextInterface );
	if( NULL!=m_pCamContextInterface )
		m_pCamContextInterface->CreateFreeCamera( m_pFreeCameraInterface );
	if( NULL!=m_pFreeCameraInterface )
	{
		//
		// Initialize the free camera
		//
		RECT Rect;
		GetClientRect( a_hWnd, &Rect );
		m_pFreeCameraInterface->AttachGfxContext( a_pGfxContextInterface );
		IGNFLOAT fAspectRatio = static_cast<IGNFLOAT>(Rect.right - Rect.left)/static_cast<IGNFLOAT>(Rect.bottom- Rect.top);
		m_pFreeCameraInterface->SetFOV( 3.1416f/3.0f, fAspectRatio );
		m_pFreeCameraInterface->SetNearAndFarClipping( 1.0f, 1000.0f );

		//
		// You may want a fix up direction (positive y-axis)
		//
		IGN_STRUCT_3DVector PositionVector;
		PositionVector.m_fX	= -39.0f;
		PositionVector.m_fY	= 3.0f;
		PositionVector.m_fZ	= -34.0f;
		m_pFreeCameraInterface->SetPosition( PositionVector );
		IGN_STRUCT_3DVector DirectionVector;
		DirectionVector.m_fX	= 0.96f;
		DirectionVector.m_fY	= 0.20f;
		DirectionVector.m_fZ	= -0.15f;
		m_pFreeCameraInterface->SetViewDirection( DirectionVector );
		IGN_STRUCT_3DVector UpVector;
		UpVector.m_fX	= 0.0f;
		UpVector.m_fY	= 1.0f;
		UpVector.m_fZ	= 0.0f;
		m_pFreeCameraInterface->SetViewUp( UpVector );
		m_pFreeCameraInterface->FixViewUp( true );
	}

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
	{
		m_pTimerInterface->Reset();
		m_pTimerInterface->Resume();
	}

	_chdir( "..\\" );
}


void CAMERA::Finit												(	void )
{
	//----------------------------------------------------------|----------------------------------------------Finit-|
	// public method
	// Finalizes the tester.

	//
	// Release objects in IGN_camera_sys.dll
	//
	if( NULL!=m_pCamInstanceInterface )
		m_pCamInstanceInterface->Release();
	if( NULL!=m_pCamContextInterface )
		m_pCamContextInterface->Release();
	if( NULL!=m_pFreeCameraInterface )
		m_pFreeCameraInterface->Release();
	if( NULL!=m_hCamLibrary )
		FreeLibrary( m_hCamLibrary );

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
}


IGNBOOL CAMERA::ProceedCamera									(	ENUM_MOVE				a_MoveCamera,
																	IGNFLOAT				a_fPitch,
																	IGNFLOAT				a_fYaw,
																	IGNFLOAT				a_fRoll,
																	IGNFLOAT				a_fAdditionalPitch,
																	IGNFLOAT				a_fAdditionalYaw,
																	IGNFLOAT				a_fAdditionalRoll,
																	IGNBOOL					a_bResetAdditionalValues,
																	TERRAIN*				a_pTerrain,
																	WATER*					a_pWater )
{
	//----------------------------------------------------------|--------------------------------------ProceedCamera-|
	// public method
	// Proceeds the camera.

	//
	// Add pitch, yaw & roll and execute the camera
	//
	if( NULL!=m_pFreeCameraInterface )
	{
		IGNFLOAT fTime;
		if( NULL!=m_pTimerInterface )
		{
			IGNFLOAT fTimeDiff;
			m_pTimerInterface->GetTimeDifference( fTimeDiff );
			m_pTimerInterface->GetTime( fTime );
			fTimeDiff *= 100.0f;
			if( ENUM_MOVE_STOP!=a_MoveCamera )
			{
				IGNFLOAT fMoveStep = 0.05f;
				if( a_MoveCamera&ENUM_MOVE_FAST )
					fMoveStep = 0.25f;
				fMoveStep *= fTimeDiff;
				if( a_MoveCamera&ENUM_MOVE_FOREWARD )
					m_pFreeCameraInterface->MoveForward( fMoveStep );
				if( a_MoveCamera&ENUM_MOVE_BACKWARD )
					m_pFreeCameraInterface->MoveForward( -fMoveStep );
				if( a_MoveCamera&ENUM_MOVE_RIGHT )
					m_pFreeCameraInterface->MoveSideways( fMoveStep );
				if( a_MoveCamera&ENUM_MOVE_LEFT )
					m_pFreeCameraInterface->MoveSideways( -fMoveStep );
				if( a_MoveCamera&ENUM_MOVE_UP )
					m_pFreeCameraInterface->MoveUpwards( fMoveStep );
				if( a_MoveCamera&ENUM_MOVE_DOWN )
					m_pFreeCameraInterface->MoveUpwards( -fMoveStep );
				if( a_MoveCamera&ENUM_MOVE_BOUNDTOTERRAIN )
				{
					IGN_STRUCT_3DVector Position;
					m_pFreeCameraInterface->GetPosition( Position );
					a_pTerrain->CheckPosition( Position );
					a_pWater->CheckPosition( Position );
					m_pFreeCameraInterface->SetPosition( Position );
				}
			}
			m_pFreeCameraInterface->SetPitch( a_fPitch*fTimeDiff );
			m_pFreeCameraInterface->SetYaw( a_fYaw*fTimeDiff );
			m_pFreeCameraInterface->SetRoll( a_fRoll*fTimeDiff );
			m_pFreeCameraInterface->AddAdditionalPitch( a_fAdditionalPitch*fTimeDiff );
			m_pFreeCameraInterface->AddAdditionalYaw( a_fAdditionalYaw*fTimeDiff );
			m_pFreeCameraInterface->AddAdditionalRoll( a_fAdditionalRoll*fTimeDiff );
			if( true==a_bResetAdditionalValues )
			{
				m_pFreeCameraInterface->SetAdditionalPitch( 0.0f );
				m_pFreeCameraInterface->SetAdditionalYaw( 0.0f );
				m_pFreeCameraInterface->SetAdditionalRoll( 0.0f );
			}
			m_pFreeCameraInterface->Execute();
			m_pFreeCameraInterface->GetPosition( m_Position );
			m_pFreeCameraInterface->GetViewMatrix( m_ViewMatrix );
			m_pFreeCameraInterface->GetProjectionMatrix( m_ProjectionMatrix );
			return true;
		}
	}

	return false;
}


IGNBOOL CAMERA::GetPosition 									(	IGN_STRUCT_3DVector&	a_Position )
{
	//----------------------------------------------------------|----------------------------------------GetPosition-|
	// public method
	// Retrieves the current camera position.

	//
	// Get the matrix
	//
	a_Position = m_Position;

	return true;
}


IGNBOOL CAMERA::GetViewMatrix 									(	IGN_STRUCT_4x4Matrix&	a_Matrix )
{
	//----------------------------------------------------------|--------------------------------------GetViewMatrix-|
	// public method
	// Retrieves the current view matrix.

	//
	// Get the matrix
	//
	a_Matrix = m_ViewMatrix;

	return true;
}


IGNBOOL CAMERA::GetProjectionMatrix 							(	IGN_STRUCT_4x4Matrix&	a_Matrix )
{
	//----------------------------------------------------------|--------------------------------GetProjectionMatrix-|
	// public method
	// Proceeds the camera.

	//
	// Get the matrix
	//
	a_Matrix = m_ProjectionMatrix;

	return true;
}


IGNBOOL CAMERA::ProcessAgain									(	void )
{
	//----------------------------------------------------------|---------------------------------------ProcessAgain-|
	// public method
	// Process the camera again without any updates.

	//
	// Execute the camera
	//
	if( NULL!=m_pFreeCameraInterface )
	{
		if( IGN_OK==m_pFreeCameraInterface->ExecuteWithoutUpdate() )
			return true;
	}

	return false;
}


IGNBOOL CAMERA::Proceed 										(	IGN_OBJIF_InpImmMouse*		a_pMouse,
																	ENUM_MOVE					a_MoveCamera,
																	TERRAIN*					a_pTerrain,
																	WATER*						a_pWater )
{
	//----------------------------------------------------------|--------------------------------------------Proceed-|
	// public method
	// Proceeds the camera.

	//
	// Determinate the additional camera pitch, yaw and roll
	//
	IGNFLOAT fPitch				= 0.0f;
	IGNFLOAT fYaw				= 0.0f;
	IGNFLOAT fRoll				= 0.0f;
	IGNFLOAT fAdditionalPitch	= 0.0f;
	IGNFLOAT fAdditionalYaw		= 0.0f;
	IGNFLOAT fAdditionalRoll	= 0.0f;
	IGNBOOL	 bResetAdditionalValues = false;
	if( NULL!=a_pMouse )
	{
		IGN_STRUCT_ImmMouseData Data;
		a_pMouse->GetData( Data );
		if( true==Data.m_bButtonDown[1] )
			bResetAdditionalValues = true;
		if( true==Data.m_bButtonDown[0] )
		{
			fAdditionalPitch	= 0.001f * Data.m_iY;
			fAdditionalYaw		= 0.001f * Data.m_iX;
			fAdditionalRoll		= 0.001f * Data.m_iZ;
		}
		else
		{
			fPitch	= 0.001f * Data.m_iY;
			fYaw	= 0.001f * Data.m_iX;
			fRoll	= 0.001f * Data.m_iZ;
		}
	}

	//
	// Proceed with the camera
	//
	return ProceedCamera(	
		a_MoveCamera,
		fPitch,
		fYaw,
		fRoll,
		fAdditionalPitch,
		fAdditionalYaw,
		fAdditionalRoll,
		bResetAdditionalValues,
		a_pTerrain,
		a_pWater );
}
