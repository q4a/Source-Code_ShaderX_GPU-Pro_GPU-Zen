
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			WaterDemo
//*				File:				DemoFrame.h
//*				Description:		Entry point for water testing applications
//*
//********************************************************************************************************************
//*
//*				Author:				Kurt Pelzer
//*									Copyright (c) 2003 Kurt Pelzer
//*									All rights reserved.
//*
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Status:				In progress
//*
//********************************************************************************************************************
//*
//*				History:
//*
//*				01/14/03| 0.0.1		## Initial release	(kp) ##
//*
//********************************************************************************************************************
//********************************************************************************************************************




//------------- PREPROCESSOR -----------------------------------------------------------------------------------------

#ifndef			DEMOFRAME_H_INCLUDED
#define			DEMOFRAME_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> DemoFrame.h\n")
#endif
#endif 




//------------- DEFINITIONS ------------------------------------------------------------------------------------------

enum	ENUM_MOVE
{
	ENUM_MOVE_STOP					= 0x00,
	ENUM_MOVE_FOREWARD				= 0x01,
	ENUM_MOVE_BACKWARD				= 0x02,
	ENUM_MOVE_LEFT					= 0x04,
	ENUM_MOVE_RIGHT					= 0x08,
	ENUM_MOVE_UP					= 0x10,
	ENUM_MOVE_DOWN					= 0x20,
	ENUM_MOVE_FAST					= 0x40,
	ENUM_MOVE_BOUNDTOTERRAIN		= 0x80,
	ENUM_MOVE_FORCEDWORD			= 0x7fffffff
};

enum	ENUM_INFODISPLAY
{
	ENUM_INFODISPLAY_FPS			= 0x00,
	ENUM_INFODISPLAY_SHOWALL		= 0x01,
	ENUM_INFODISPLAY_HIDE			= 0x02,
	ENUM_INFODISPLAY_FORCEDWORD		= 0x7fffffff
};

enum	ENUM_TARGETCLASS
{
	ENUM_TARGETCLASS_DX7			= 0x00,
	ENUM_TARGETCLASS_DX8GF3			= 0x01,
	ENUM_TARGETCLASS_DX8			= 0x02,
	ENUM_TARGETCLASS_DX9			= 0x03,
	ENUM_TARGETCLASS_FORCEDWORD		= 0x7fffffff
};




//------------- DEPENDENCIES -----------------------------------------------------------------------------------------

#include "windows.h"
#include <math.h>

#include "../WinMain/Frame.h"
#include "../../../../Engine/Shared/IGN_shared/Modules/Subsystems/ign_graphic_sub_shared.h"
#include "../../../../Engine/Shared/IGN_shared/Modules/Subsystems/ign_input_sub_shared.h"
#include "../../../../Engine/Shared/IGN_shared/Modules/Subsystems/ign_timer_sub_shared.h"
#include "../../../../Engine/Shared/IGN_shared/Modules/Systems/ign_camera_sys_shared.h"
#include "Camera.h"
#include "Skydome.h"
#include "Terrain.h"
#include "Water.h"




//------------- FORWARD DECLARATIONS ---------------------------------------------------------------------------------




//------------- DECLARATIONS -----------------------------------------------------------------------------------------

class DEMOFRAME : public FRAME
{
public:

	//
	// Constructors/Destructors
	//
	DEMOFRAME	(	HWND		a_hWnd,
					IGNU32		a_u32APIType,
					IGNBOOL		a_bWindowed,
					IGNU32		a_u32Width,
					IGNU32		a_u32Height );
	~DEMOFRAME	();

protected:
	
	//
	// Handles of used dynamic link libraries
	//
	HMODULE						m_hGfxLibrary;
	HMODULE						m_hInpLibrary;
	HMODULE						m_hTmrLibrary;

	//
	// Objects in IGN_graphic_sub.dll
	//
	IGN_OBJIF_GfxInstance*		m_pGfxInstanceInterface;
	IGN_OBJIF_GfxContext*		m_pGfxContextInterface;
	IGN_OBJIF_GfxMixer*			m_pMixer;
	IGN_OBJIF_GfxAssistant*		m_pAssistant;
	IGN_OBJIF_GfxGauge*			m_pGauge;
	IGN_OBJIF_GfxPixelProgram*	m_pPixelProgram;

	//
	// Objects in IGN_input_sub.dll
	//
	IGN_OBJIF_InpInstance*		m_pInpInstanceInterface;
	IGN_OBJIF_InpContext*		m_pInpContextInterface;
	IGN_OBJIF_InpImmKeyboard*	m_pKeyboard;
	IGN_OBJIF_InpImmMouse*		m_pMouse;

	//
	// Objects in IGN_timer_sub.dll
	//
	IGN_OBJIF_TmrInstance*		m_pTmrInstanceInterface;
	IGN_OBJIF_TmrContext*		m_pTmrContextInterface;
	IGN_OBJIF_TmrTimer*			m_pTimerInterface;

	//
	// Internal objects
	//
	IGNBOOL						m_bWindowed;
	IGNU32						m_u32Width;
	IGNU32						m_u32Height;
	CAMERA*						m_pCamera;
	SKYDOME*					m_pSkydome;
	TERRAIN*					m_pTerrain;
	WATER*						m_pWater;
	IGNU32						m_u32CurrentFPS;
	IGNU32						m_u32PolysPerSecond;
	IGNU32						m_u32DrawCallsPerSecond;
	IGNFLOAT					m_fCurrentTimeStamp;
	IGNFLOAT					m_fLastFPSTimeStamp;
	ENUM_INFODISPLAY			m_ShowInfo;
	IGN_STRUCT_ImmKeyboardData	m_LastKeyboardData;
	IGNBOOL						m_bSkydomeWireframeModus;
	IGNBOOL						m_bWaterWireframeModus;
	ENUM_MOVE					m_MoveCamera;
	IGNFLOAT					m_fWaterHeight;
	ENUM_TARGETCLASS			m_TargetClass;
	
	//
	// Protected methods
	//
	IGN_OBJIF_GfxContext*		StartCompatibleContext	(	HWND					a_hWnd			);
	void						Init					(	HWND					a_hWnd,
															IGNU32					a_u32APIType	);
	void						Finit					(	void									);
	IGNBOOL						ExecuteKeyboard			(	void									);
	IGNBOOL						ExecuteMouse			(	void									);
	void						ShowInfo				(	void									);
	void						GetStatistics			(	void									);
	IGNBOOL						ProceedNextFrame		(	HWND					a_hWnd			);


public:

	//
	// Public interface
	//
	IGNBOOL						Proceed					(	HWND					a_hWnd,
															MSG*					a_pMsg			);
};




//------------- PREPROCESSOR END -------------------------------------------------------------------------------------

#endif // DEMOFRAME_H_INCLUDED