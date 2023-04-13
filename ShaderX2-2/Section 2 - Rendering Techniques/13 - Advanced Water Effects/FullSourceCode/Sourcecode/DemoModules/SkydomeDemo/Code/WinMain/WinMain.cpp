//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			SkydomeDemo
//*				File:				WinMain.cpp
//*
//********************************************************************************************************************
//*
//*				Author:				Kurt Pelzer
//*									Copyright (c) 2003 Kurt Pelzer
//*									All rights reserved.
//*
//********************************************************************************************************************
//********************************************************************************************************************




#include "stdafx.h"
#include "Frame.h"




//********************************************************************************************************************
//*				GLOBAL
//********************************************************************************************************************

LRESULT WINAPI MsgProc											(	HWND						a_hWnd, 
																	UINT						a_uiMsg, 
																	WPARAM						a_wParam, 
																	LPARAM						a_lParam )
{
	//----------------------------------------------------------|--------------------------------------------MsgProc-|
	// global method
	// Proceeds window essages.

	switch( a_uiMsg )
	{
		case WM_DESTROY:
			PostQuitMessage( 0 );
			return 0;

		case WM_PAINT:
			ValidateRect( a_hWnd, 
						  NULL );
			return 0;
		case WM_ACTIVATE:
			if( WA_INACTIVE != a_wParam )
			{
			}
			return 0;
	}
    return DefWindowProc( a_hWnd, 
						  a_uiMsg, 
						  a_wParam, 
						  a_lParam );
}


HWND CreateTestWindow											(	HINSTANCE					a_hInstance,
																	IGNU32						a_u32WindowWidth,
																	IGNU32						a_u32WindowHeight )
{
	//----------------------------------------------------------|-----------------------------------CreateTestWindow-|
	// global method
	// Creates a new test window.

	WNDCLASSEX wc = { sizeof(WNDCLASSEX), 
					  CS_CLASSDC, 
					  MsgProc, 
					  0L, 
					  0L,  
					  a_hInstance, 
					  NULL, 
					  NULL, 
					  NULL, 
					  NULL, 
					  "Skydome", 
					  NULL };
	RegisterClassEx( &wc );
    HWND hWnd = CreateWindow( "Skydome", "Skydome", 
							   WS_BORDER|WS_SYSMENU, 
							   300, 
							   300, 
							   a_u32WindowWidth, 
							   a_u32WindowHeight,
							   NULL, 
							   NULL, 
							   wc.hInstance, 
							   NULL );
	ShowWindow( hWnd, 
				SW_SHOWNORMAL);
	return hWnd;
}


IGNBOOL ParseCommandLine										(	LPSTR						a_pCmdLine,
																	IGNU32&						a_u32APIType,
																	IGNBOOL&					a_bWindowed,
																	IGNU32&						a_u32WindowWidth,
																	IGNU32&						a_u32WindowHeight )
{
	//----------------------------------------------------------|-----------------------------------ParseCommandLine-|
	// global method
	// Parses the command line input.

	a_u32APIType		= 1;
	a_bWindowed			= true;
	a_u32WindowWidth	= 800;
	a_u32WindowHeight	= 600;

	IGNBOOL bReadyToStart	= false;
	char seps[]   = " ,\t\n";
	char *token;
	token = strtok( a_pCmdLine, seps );
	while( token!=NULL )
	{
		if( 0==strncmp( token, "-s:", 3 ) )
		{
			if( 0==strncmp( token, "-s:fullscreen", 13 ) )
				a_bWindowed = false;
			else
			{
				if( 0==strncmp( token, "-s:windowed", 11 ) )
					a_bWindowed = true;
			}
		}
		if( 0==strncmp( token, "-a:", 3 ) )
		{
			if( 0==strncmp( token, "-a:D3D90", 8 ) )
				a_u32APIType = 1;
			else
			{
				if( 0==strncmp( token, "-a:D3D81", 8 ) )
					a_u32APIType = 2;
			}
		}
		if( 0==strncmp( token, "-r:", 3 ) )
		{
			if( 0==strncmp( token, "-r:640x480", 10 ) )
			{
				a_u32WindowWidth	= 640;
				a_u32WindowHeight	= 480;
			}
			else
			{
				if( 0==strncmp( token, "-r:800x600", 10 ) )
				{
					a_u32WindowWidth	= 800;
					a_u32WindowHeight	= 600;
				}
				else
				{
					if( 0==strncmp( token, "-r:1024x768", 11 ) )
					{
						a_u32WindowWidth	= 1024;
						a_u32WindowHeight	= 768;
					}
					else
					{
						if( 0==strncmp( token, "-r:1280x960", 11 ) )
						{
							a_u32WindowWidth	= 1280;
							a_u32WindowHeight	= 960;
						}
						else
						{
							if( 0==strncmp( token, "-r:1600x1200", 12 ) )
							{
								a_u32WindowWidth	= 1600;
								a_u32WindowHeight	= 1200;
							}
						}
					}
				}
			}
		}
		if( 0==strncmp( token, "skydome", 5 ) )
			bReadyToStart = true;
		token = strtok( NULL, seps );
	}

	return bReadyToStart;
}


IGNINT APIENTRY WinMain											(	HINSTANCE					a_hInstance,
																	HINSTANCE					a_hPrevInstance,
																	LPSTR						a_pCmdLine,
																	IGNINT						a_iCmdShow )
{
	//----------------------------------------------------------|--------------------------------------------WinMain-|
	// global method
	// Main function.

	//
	// Start the system ...
	//

	//
	// Examine the command line parameters
	//
	IGNU32	u32APIType		= 1;
	IGNBOOL	bWindowed		= true;
	IGNU32	u32WindowWidth	= 640;
	IGNU32	u32WindowHeight	= 480;

	IGNBOOL	bReadyToStart = ParseCommandLine(
		a_pCmdLine,
		u32APIType,
		bWindowed,
		u32WindowWidth,
		u32WindowHeight );

	//
	// Bound to demo browser or stand alone application (mask next two lines)
	//
/* !!! Stand alone !!!
	if( false==bReadyToStart )
		return 0;
*/

	//
	// Create the demo instance and display window
	//
	HWND hWnd;
	if( false==bWindowed )
		hWnd = CreateTestWindow( a_hInstance, 100, 100 );
	else
		hWnd = CreateTestWindow( a_hInstance, u32WindowWidth, u32WindowHeight );	
	FRAME* pFrame = new DEMOFRAME( hWnd, u32APIType, bWindowed, u32WindowWidth, u32WindowHeight );
	if( NULL==pFrame )
		return 0;

	//
	// Loop the message processing
	//
	MSG msg; 
	memset( &msg, 0, sizeof(msg) );
    while( msg.message!=WM_QUIT )
	{
		if( !pFrame->Proceed( hWnd, &msg ) )
			break;
		if( PeekMessage( &msg, 
						 NULL, 
						 0U, 
						 0U, 
						 PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	//
	// Turn down the system ...
	//
	delete pFrame;

	return 0;
}
