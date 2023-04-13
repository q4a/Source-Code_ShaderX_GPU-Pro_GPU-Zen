//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			SkydomeDemo
//*				File:				DemoFrame.cpp
//*
//********************************************************************************************************************
//*
//*				Author:				Marco Spoerl
//*									Copyright (c) 2003 Marco Spoerl
//*									All rights reserved.
//*
//********************************************************************************************************************
//********************************************************************************************************************




#include		"DemoFrame.h"
#include		<direct.h>
#include		<stdio.h>




//********************************************************************************************************************
//*				DEMOFRAME
//********************************************************************************************************************

DEMOFRAME::DEMOFRAME											(	HWND						a_hWnd,
																	IGNU32						a_u32APIType,
																	IGNBOOL						a_bWindowed,
																	IGNU32						a_u32Width,
																	IGNU32						a_u32Height )
{
	//----------------------------------------------------------|------------------------------------------DEMOFRAME-|
	// public method
	// Constructor
	
	m_bWindowed					= a_bWindowed;
	m_u32Width					= a_u32Width;
	m_u32Height					= a_u32Height;
	m_hGfxLibrary				= NULL;
	m_hInpLibrary				= NULL;
	m_hTmrLibrary				= NULL;
	m_pGfxInstanceInterface		= NULL;
	m_pGfxContextInterface		= NULL;
	m_pMixer					= NULL;
	m_pAssistant				= NULL;
	m_pGauge					= NULL;
	m_pPixelProgram				= NULL;
	m_pInpInstanceInterface		= NULL;
	m_pInpContextInterface		= NULL;
	m_pKeyboard					= NULL;
	m_pMouse					= NULL;
	m_pTmrInstanceInterface		= NULL;
	m_pTmrContextInterface		= NULL;
	m_pTimerInterface			= NULL;
	m_pCamera					= NULL;
	m_pSkydome					= NULL;
	m_u32CurrentFPS				= 100;
	m_u32PolysPerSecond			= 0;
	m_u32DrawCallsPerSecond		= 0;
	m_fCurrentTimeStamp			= 0.0f;
	m_fLastFPSTimeStamp			= 0.0f;
	m_ShowInfo					= ENUM_INFODISPLAY_FPS;
	m_bSkydomeWireframeModus	= false;
	m_MoveCamera				= ENUM_MOVE_BOUNDTOTERRAIN;
	m_fTimeFactor				= 0.0005f;

	//
	// Initialize the system
	//
	Init( a_hWnd, a_u32APIType );
}


DEMOFRAME::~DEMOFRAME											()
{
	//----------------------------------------------------------|-----------------------------------------~DEMOFRAME-|
	// public method
	// Destructor

	//
	// Finalize the system
	//
	Finit();
}


IGN_OBJIF_GfxContext* DEMOFRAME::StartCompatibleContext			(	HWND						a_hWnd )
{
	//----------------------------------------------------------|-----------------------------StartCompatibleContext-|
	// public method
	// Start the subsystem with a compatible context.

	//
	// Get device caps
	//
	HDC hdc					= GetDC( 0 );
	IGNU32 u32DesktopWidth	= GetDeviceCaps( hdc, HORZRES );
	IGNU32 u32DesktopHeight	= GetDeviceCaps( hdc, VERTRES );
	IGNU32 u32DesktopBPP	= GetDeviceCaps( hdc, BITSPIXEL );
	if( false==m_bWindowed )
	{
		u32DesktopWidth		= m_u32Width;
		u32DesktopHeight	= m_u32Height;
	}
	IGN_ENUM_PIXELFORMAT pixelFormat;
	if( 32==u32DesktopBPP )
		pixelFormat = IGN_ENUM_PIXELFORMAT_X8R8G8B8;
	else
		pixelFormat = IGN_ENUM_PIXELFORMAT_R5G6B5;
	ReleaseDC( 0, hdc );

	//
	// Search and create context
	//
	if( NULL!=m_pGfxInstanceInterface )
	{
		IGNU32 u32ContextCount;
		m_pGfxInstanceInterface->GetContextCount( u32ContextCount );
		IGN_OBJIF_GfxContext* pGfxContextInterface = NULL;
		for( IGNU32 i = 0; i<u32ContextCount; i++ )
		{
			IGN_STRUCT_GfxContextDesc contextDesc;
			m_pGfxInstanceInterface->EnumContext( i, contextDesc );
			if( ( u32DesktopWidth==contextDesc.m_ModeDesc.m_u32Width )&&
				( u32DesktopHeight==contextDesc.m_ModeDesc.m_u32Height )&&
				( pixelFormat==contextDesc.m_ModeDesc.m_Format ) )
			{
				m_pGfxInstanceInterface->CreateContext( i, 
														a_hWnd,
														IGN_ENUM_GFXCONTEXTCREATE_MIXVERTEXPROCESSING,
														m_bWindowed,
														IGN_ENUM_MULTISAMPLE_NONE,
														pGfxContextInterface );
				break;
			}
		}
		return pGfxContextInterface;
	}

	return NULL;
}


void DEMOFRAME::Init											(	HWND						a_hWnd,
																	IGNU32						a_u32APIType )
{
	//----------------------------------------------------------|-----------------------------------------------Init-|
	// public method
	// Initializes the tester.

	_chdir( "System" );
	IGNRESULT hResult = 0;

	//
	// Load IGN_graphic_sub.dll and create objects
	//
	IGN_ENUM_GFXINSTANCETYPE instanceType;
	switch( a_u32APIType )
	{
	case	1:
			instanceType = IGN_ENUM_GFXINSTANCETYPE_DIRECTX90;
			break;
	case	2:
			instanceType = IGN_ENUM_GFXINSTANCETYPE_DIRECTX81;
			break;
	}
	IGN_STRUCT_IFSubGraphic* pSubGraphic = NULL;
	m_hGfxLibrary = LoadLibrary( "IGN_graphic_sub.dll" );
	IGN_METHOD_GetSubHookGraphic* 
			GfxGetInterface = reinterpret_cast<IGN_METHOD_GetSubHookGraphic*>( GetProcAddress( m_hGfxLibrary, "GetInterface" ) );
	if( NULL!=GfxGetInterface )
		hResult = GfxGetInterface( &pSubGraphic );
	if( NULL!=pSubGraphic )
		hResult = pSubGraphic->m_pCreateInstance( instanceType, m_pGfxInstanceInterface );
	if( NULL!=m_pGfxInstanceInterface )
		m_pGfxContextInterface	= StartCompatibleContext( a_hWnd );
	if( NULL!=m_pGfxContextInterface )
	{
		m_pGfxContextInterface->CreateMixer( m_pMixer );
		if( NULL!=m_pMixer )
		{
			m_pMixer->SetMasterLighting( true );
			m_pMixer->SetMasterAlphaBlending( true );
		}
		if( NULL!=m_pGfxContextInterface )
			m_pGfxContextInterface->CreateAssistant( m_pAssistant );
		m_pGfxContextInterface->CreateGauge( m_pGauge );
		if( NULL!=m_pGauge )
			m_pGauge->SetModus( IGN_ENUM_GAUGEMODUS_STANDARD ); //IGN_ENUM_GAUGEMODUS_SIMPLE //IGN_ENUM_GAUGEMODUS_ADVANCED //IGN_ENUM_GAUGEMODUS_DISABLED
		m_pGfxContextInterface->CreatePixelProgram( m_pPixelProgram );
	}

	//
	// Load IGN_input_sub.dll and create objects
	//
	IGN_STRUCT_IFSubInput* pSubInput = NULL;
	m_hInpLibrary = LoadLibrary( "IGN_input_sub.dll" );
	IGN_METHOD_GetSubHookInput* 
			InpGetInterface = reinterpret_cast<IGN_METHOD_GetSubHookInput*>( GetProcAddress( m_hInpLibrary, "GetInterface" ) );
	if( NULL!=InpGetInterface )
		hResult = InpGetInterface( &pSubInput );
	if( NULL!=pSubInput )
		hResult = pSubInput->m_pCreateInstance( IGN_ENUM_INPINSTANCETYPE_STANDARD, m_pInpInstanceInterface );
	if( NULL!=m_pInpInstanceInterface )
		m_pInpInstanceInterface->CreateContext( 1, a_hWnd, m_pInpContextInterface );
	if( NULL!=m_pInpContextInterface )
	{
		m_pInpContextInterface->CreateImmediateKeyboard( static_cast<IGN_ENUM_COOPLEVEL>( IGN_ENUM_COOPLEVEL_EXCLUSIVE|IGN_ENUM_COOPLEVEL_FOREGROUND ), m_pKeyboard );
		m_pInpContextInterface->CreateImmediateMouse( static_cast<IGN_ENUM_COOPLEVEL>( IGN_ENUM_COOPLEVEL_EXCLUSIVE|IGN_ENUM_COOPLEVEL_FOREGROUND ), m_pMouse );
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

	//
	// Create camera, skydome and terrain
	//
	m_pCamera	= new CAMERA( a_hWnd, m_pGfxContextInterface );
	m_pSkydome	= new SKYDOME( m_pGfxContextInterface, m_pAssistant );
	m_pTerrain	= new TERRAIN( m_pGfxContextInterface, m_pAssistant );
}


void DEMOFRAME::Finit											(	void )
{
	//----------------------------------------------------------|----------------------------------------------Finit-|
	// public method
	// Finalizes the tester.

	//
	// Delete camera, skydome and terrain
	//
	if( NULL!=m_pCamera )
		delete m_pCamera;
	if( NULL!=m_pSkydome )
		delete m_pSkydome;
	if( NULL!=m_pTerrain )
		delete m_pTerrain;

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
	// Release objects in IGN_input_sub.dll
	//
	if( NULL!=m_pMouse )
		m_pMouse->Release();
	if( NULL!=m_pKeyboard )
		m_pKeyboard->Release();
	if( NULL!=m_pInpContextInterface )
		m_pInpContextInterface->Release();
	if( NULL!=m_pInpInstanceInterface )
		m_pInpInstanceInterface->Release();
	if( NULL!=m_hInpLibrary )
		FreeLibrary( m_hInpLibrary );

	//
	// Release objects in IGN_graphic_sub.dll
	//
	if( NULL!=m_pPixelProgram )
		m_pPixelProgram->Release();
	if( NULL!=m_pGauge )
		m_pGauge->Release();
	if( NULL!=m_pAssistant )
		m_pAssistant->Release();
	if( NULL!=m_pMixer )
		m_pMixer->Release();
	if( NULL!=m_pGfxContextInterface )
		m_pGfxContextInterface->Release();
	if( NULL!=m_pGfxInstanceInterface )
		m_pGfxInstanceInterface->Release();
	if( NULL!=m_hGfxLibrary )
		FreeLibrary( m_hGfxLibrary );
}


IGNBOOL DEMOFRAME::ExecuteKeyboard								(	void )
{
	//----------------------------------------------------------|------------------------------------ExecuteKeyboard-|
	// public method
	// Executes the keyboard and checks pressed keys.

	if( NULL!=m_pKeyboard )
	{		
		//
		// Execute keyboard
		//
		m_pKeyboard->Execute();

		//
		// Get current keyboard data and check for pressed keys ...
		//
		IGN_STRUCT_ImmKeyboardData Data;
		m_pKeyboard->GetData( Data );

		//
		// ESC
		//
		if( true==Data.m_bKeyDown[IGN_ENUM_INPKEY_ESCAPE] )
			return false;

		//
		// F1
		//
		if( (true==Data.m_bKeyDown[IGN_ENUM_INPKEY_F1])&&(false==m_LastKeyboardData.m_bKeyDown[IGN_ENUM_INPKEY_F1]) )
		{
			switch( m_ShowInfo )
			{
			case	ENUM_INFODISPLAY_SHOWALL:
					m_ShowInfo = ENUM_INFODISPLAY_HIDE;
					break;
			case	ENUM_INFODISPLAY_HIDE:
					m_ShowInfo = ENUM_INFODISPLAY_FPS;
					break;
			case	ENUM_INFODISPLAY_FPS:
			default:
					m_ShowInfo = ENUM_INFODISPLAY_SHOWALL;
			}
		}

		//
		// F2
		//
		if( (true==Data.m_bKeyDown[IGN_ENUM_INPKEY_F2])&&(false==m_LastKeyboardData.m_bKeyDown[IGN_ENUM_INPKEY_F2]) )
		{
			if( false==m_bSkydomeWireframeModus )
				m_bSkydomeWireframeModus = true;
			else
				m_bSkydomeWireframeModus = false;
		}

		//
		// F4
		//
		if( m_MoveCamera&ENUM_MOVE_BOUNDTOTERRAIN )
			m_MoveCamera = ENUM_MOVE_BOUNDTOTERRAIN;
		else
			m_MoveCamera = ENUM_MOVE_STOP;
		if( (true==Data.m_bKeyDown[IGN_ENUM_INPKEY_F4])&&(false==m_LastKeyboardData.m_bKeyDown[IGN_ENUM_INPKEY_F4]) )
		{
			if( m_MoveCamera&ENUM_MOVE_BOUNDTOTERRAIN )
				m_MoveCamera = ENUM_MOVE_STOP;
			else
				m_MoveCamera = ENUM_MOVE_BOUNDTOTERRAIN;
		}

		//
		// Arrows (up, down, left, right)
		//
		if( true==Data.m_bKeyDown[IGN_ENUM_INPKEY_RCONTROL] )
		{
			m_MoveCamera = static_cast<ENUM_MOVE>( m_MoveCamera|ENUM_MOVE_FAST );
		}
		if( true==Data.m_bKeyDown[IGN_ENUM_INPKEY_UP] )
		{
			m_MoveCamera = static_cast<ENUM_MOVE>( m_MoveCamera|ENUM_MOVE_FOREWARD );
		}
		else if( true==Data.m_bKeyDown[IGN_ENUM_INPKEY_DOWN] )
		{
			m_MoveCamera = static_cast<ENUM_MOVE>( m_MoveCamera|ENUM_MOVE_BACKWARD );
		}
		if( true==Data.m_bKeyDown[IGN_ENUM_INPKEY_LEFT] )
		{
			m_MoveCamera = static_cast<ENUM_MOVE>( m_MoveCamera|ENUM_MOVE_LEFT );
		}
		else if( true==Data.m_bKeyDown[IGN_ENUM_INPKEY_RIGHT] )
		{
			m_MoveCamera = static_cast<ENUM_MOVE>( m_MoveCamera|ENUM_MOVE_RIGHT );
		}

		//
		// Page up and down
		//
		if( true==Data.m_bKeyDown[IGN_ENUM_INPKEY_NEXT] )
		{
			m_MoveCamera = static_cast<ENUM_MOVE>( m_MoveCamera|ENUM_MOVE_DOWN );
		}
		else if( true==Data.m_bKeyDown[IGN_ENUM_INPKEY_PRIOR] )
		{
			m_MoveCamera = static_cast<ENUM_MOVE>( m_MoveCamera|ENUM_MOVE_UP );
		}

		// A, S and D
		if( true==Data.m_bKeyDown[IGN_ENUM_INPKEY_A] )
		{
			m_fTimeFactor += 0.00001f;
		}
		else if( true==Data.m_bKeyDown[IGN_ENUM_INPKEY_S] )
		{
			m_fTimeFactor = 0.0f;
		}
		else if( true==Data.m_bKeyDown[IGN_ENUM_INPKEY_D] )
		{
			m_fTimeFactor -= 0.00001f;

			if( m_fTimeFactor < 0.0f )
				m_fTimeFactor = 0.0f;
		}

		m_LastKeyboardData = Data;
	}

	return true;
}


IGNBOOL DEMOFRAME::ExecuteMouse									(	void )
{
	//----------------------------------------------------------|--------------------------------------------Proceed-|
	// public method
	// Executes the mouse.

	//
	// Execute mouse
	//
	if( NULL!=m_pMouse )
		m_pMouse->Execute();

	return true;
}


void DEMOFRAME::ShowInfo										(	void )
{
	//----------------------------------------------------------|--------------------------------------------ShowFPS-|
	// public method
	// Presents the current FPS data.

	if( ENUM_INFODISPLAY_HIDE==m_ShowInfo )
		return;
	RECT	rect;
	rect.left = rect.top = 25;
	rect.right = rect.bottom = 0;

	//
	// Build the text string
	//
	char string[2048];
	if( ENUM_INFODISPLAY_SHOWALL==m_ShowInfo )
	{
		strcpy( string, "HELP MENU \n\n" );
		strcat( string, "\nTarget Hardware: DX8-Class" );
		strcat( string, "\n" );
		strcat( string, "\nKeys:" );
		strcat( string, "\n F1  -  Display Info (FPS-Only/Help-Menu/Hide Display)" );
		strcat( string, "\n F2  -  Skydome Rendermode (Solid/Wireframe)" );
		strcat( string, "\n F4  -  Camera Navigation (Free/Bound-To-Terrain)" );
		strcat( string, "\n A   -  Accelerate sky object rotation" );
		strcat( string, "\n S   -  Stop sky object rotation" );
		strcat( string, "\n D   -  Decelerate sky object rotation" );
		strcat( string, "\n Use Mouse, Cursor and Page Keys to navigate the camera" );
		strcat( string, "\n (Additionally press Right Control Key to speed up)" );
		strcat( string, "\n" );
		strcat( string, "\nCurrent Data:" );
		strcat( string, "\n   Camera Position:   ( " );
		IGN_STRUCT_3DVector CamPosition;
		m_pCamera->GetPosition( CamPosition );
		char stringFloat[16];
		sprintf( stringFloat, "%f", CamPosition.m_fX );
		strcat( string, stringFloat );
		strcat( string, ", " );
		sprintf( stringFloat, "%f", CamPosition.m_fY );
		strcat( string, stringFloat );
		strcat( string, ", " );
		sprintf( stringFloat, "%f", CamPosition.m_fZ );
		strcat( string, stringFloat );
		strcat( string, " )" );
		strcat( string, "\n   Camera Direction:  ( " );
		IGN_STRUCT_3DVector CamDirection;
		CamDirection.m_fX = 0.0f;
		CamDirection.m_fY = 0.0f;
		CamDirection.m_fZ = 1.0f;
		IGN_STRUCT_4x4Matrix CamViewMatrix;
		m_pCamera->GetViewMatrix( CamViewMatrix );
		CamDirection.MultiplyMatrix( CamViewMatrix );
		stringFloat[16];
		sprintf( stringFloat, "%f", CamDirection.m_fX );
		strcat( string, stringFloat );
		strcat( string, ", " );
		sprintf( stringFloat, "%f", CamDirection.m_fY );
		strcat( string, stringFloat );
		strcat( string, ", " );
		sprintf( stringFloat, "%f", CamDirection.m_fZ );
		strcat( string, stringFloat );
		strcat( string, " )" );
		char stringDezimal[64];
		sprintf( stringDezimal, "\n   PolygonsPerSecond:   %d", m_u32PolysPerSecond );
		strcat( string, stringDezimal );
		sprintf( stringDezimal, "\n   DrawCallsPerSecond:  %d", m_u32DrawCallsPerSecond );
		strcat( string, stringDezimal );
	}
	else
	{
		strcpy( string, "FPS: " );
		IGNU32 u32Digit = 0;
		IGNU32 u32Divisor;
		for( IGNU32 i = 4; i > 0; i-- )
		{
			switch( i )
			{
			case	1:
					u32Divisor = 1;
					break;
			case	2:
					u32Divisor = 10;
					break;
			case	3:
					u32Divisor = 100;
					break;
			case	4:
					u32Divisor = 1000;
					break;
			}
			if( u32Divisor<=m_u32CurrentFPS )
			{
				u32Digit = (m_u32CurrentFPS%(u32Divisor*10))/u32Divisor;
				switch( u32Digit )
				{
				case	0:
						strcat( string, "0" );
						break;
				case	1:
						strcat( string, "1" );
						break;
				case	2:
						strcat( string, "2" );
						break;
				case	3:
						strcat( string, "3" );
						break;
				case	4:
						strcat( string, "4" );
						break;
				case	5:
						strcat( string, "5" );
						break;
				case	6:
						strcat( string, "6" );
						break;
				case	7:
						strcat( string, "7" );
						break;
				case	8:
						strcat( string, "8" );
						break;
				case	9:
						strcat( string, "9" );
						break;
				default:
						strcat( string, "*" );
				}
			}
			else
				strcat( string, " " );
		}
	}
	strcat( string, "\0" );

	//
	// Reset the pixel shader and draw the text
	//
	if( NULL!=m_pPixelProgram )
		m_pPixelProgram->Execute();
	if( NULL!=m_pAssistant )
		m_pAssistant->DrawText( string, 0xffff0000, rect );
}


void DEMOFRAME::GetStatistics									(	void )
{
	//----------------------------------------------------------|--------------------------------------GetStatistics-|
	// public method
	// Gets the current statistics.

	if( NULL!=m_pTimerInterface )
	{
		m_pTimerInterface->GetTime( m_fCurrentTimeStamp );
	}
	if( m_fCurrentTimeStamp>m_fLastFPSTimeStamp+1.0f )
	{
		m_fLastFPSTimeStamp	= m_fCurrentTimeStamp;
		IGN_STRUCT_GfxGaugeValues gaugeValues;
		m_pGauge->GetValues( gaugeValues );
		m_u32CurrentFPS			= gaugeValues.m_Simple.m_u32ScreenSwapCalls;
		m_u32PolysPerSecond		= gaugeValues.m_Standard.m_u32PolyCount;
		m_u32DrawCallsPerSecond	= gaugeValues.m_Standard.m_u32VisualExecuteCalls;
		m_pGauge->Reset();
	}
}


IGNBOOL DEMOFRAME::ProceedNextFrame								(	HWND						a_hWnd )
{
	//----------------------------------------------------------|-----------------------------------ProceedNextFrame-|
	// public method
	// Proceeds the next frame.

	if( NULL!=m_pMixer )
	{
		//
		// Get the client rect of the application's window
		//
		RECT Rect;
		GetClientRect( a_hWnd, &Rect );
		Rect.right	= Rect.right - Rect.left;
		Rect.bottom	= Rect.bottom - Rect.top;
		Rect.left	= 0;
		Rect.top	= 0;

		if( (0<Rect.right)&&(0<Rect.bottom) )
		{
			//
			// Set and clear the current view port
			//
			IGN_STRUCT_Viewport Viewport;
			Viewport.m_u32X			= 0;
			Viewport.m_u32Y			= 0;
			Viewport.m_u32Width		= Rect.right;
			Viewport.m_u32Height	= Rect.bottom;
			Viewport.m_fMinZ		= 0.0f;
			Viewport.m_fMaxZ		= 1.0f;
			m_pMixer->SetViewport( Viewport );
			m_pMixer->ClearViewport( 
				static_cast<IGN_ENUM_VIEWPORTCLEAR>( IGN_ENUM_VIEWPORTCLEAR_COLOR|IGN_ENUM_VIEWPORTCLEAR_DEPTH|IGN_ENUM_VIEWPORTCLEAR_STENCIL ), 
				0x00000000, 
				1.0f, 
				0 );

			//
			// Proceed with the camera
			//
			if( NULL!=m_pCamera )
				m_pCamera->Proceed(	
					m_pMouse,
					static_cast<ENUM_MOVE>( m_MoveCamera ),
					m_pTerrain );
			else
				return false;

			//
			// Begin a new scene
			//
			m_pMixer->BeginScene();

			//
			// Proceed with the terrain
			//
			if( NULL!=m_pTerrain )
				m_pTerrain->Proceed();
			else 
				return false;

			//
			// Proceed with the skydome
			//
			if( NULL!=m_pSkydome )
			{
				m_pSkydome->AdvanceTime( m_fTimeFactor * 100.0f/static_cast<IGNFLOAT>( m_u32CurrentFPS ) );
				m_pSkydome->Proceed( 
					m_pCamera, 
					m_bSkydomeWireframeModus );
			}
			else 
			{
				return false;
			}

			//
			// Display the system info
			//
			GetStatistics();
			ShowInfo();

			//
			// End the scene and swap the screen
			//
			m_pMixer->EndScene();
			RECT* pRect = NULL;
			if( true==m_bWindowed )
				pRect = &Rect;
			m_pMixer->SwapScreen( NULL, 
								  pRect, 
								  NULL );
		}
		return true;
	}

	return false;
}


IGNBOOL DEMOFRAME::Proceed										(	HWND						a_hWnd,
																	MSG*						a_pMsg )
{
	//----------------------------------------------------------|--------------------------------------------Proceed-|
	// public method
	// Proceeds the next frame.

	//
	// Execute the keyboard
	//
	if(	false==ExecuteKeyboard() )
		return false;

	//
	// Execute the mouse
	//
	if( false==ExecuteMouse() )
		return false;
		
	//
	// Proceed the complete next frame
	//
	return ProceedNextFrame( a_hWnd );
}
