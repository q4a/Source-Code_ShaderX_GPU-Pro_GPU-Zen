
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			SkydomeDemo
//*				File:				Camera.h
//*				Description:		Camera for skydome testing applications
//*
//********************************************************************************************************************
//*
//*				Author:				Marco Spoerl
//*									Copyright (c) 2003 Marco Spoerl
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

#ifndef			CAMERA_H_INCLUDED
#define			CAMERA_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> Camera.h\n")
#endif
#endif 




//------------- DEPENDENCIES -----------------------------------------------------------------------------------------

#include "DemoFrame.h"




//------------- FORWARD DECLARATIONS ---------------------------------------------------------------------------------

class TERRAIN;
class SKYDOME;
class CAMERA;




//------------- DEFINITIONS ------------------------------------------------------------------------------------------




//------------- DECLARATIONS -----------------------------------------------------------------------------------------

class CAMERA
{
public:

	//
	// Constructors/Destructors
	//
	CAMERA		(	HWND					a_hWnd,
					IGN_OBJIF_GfxContext*	a_pGfxContextInterface );
	~CAMERA		();

protected:
	
	//
	// Handles of used dynamic link libraries
	//
	HMODULE						m_hCamLibrary;
	HMODULE						m_hTmrLibrary;

	//
	// Objects in IGN_camera_sys.dll
	//
	IGN_OBJIF_CamInstance*		m_pCamInstanceInterface;
	IGN_OBJIF_CamContext*		m_pCamContextInterface;
	IGN_OBJIF_CamFreeCamera*	m_pFreeCameraInterface;

	//
	// Objects in IGN_timer_sub.dll
	//
	IGN_OBJIF_TmrInstance*		m_pTmrInstanceInterface;
	IGN_OBJIF_TmrContext*		m_pTmrContextInterface;
	IGN_OBJIF_TmrTimer*			m_pTimerInterface;

	//
	// Internal objects
	//
	IGN_STRUCT_3DVector			m_Position;
	IGN_STRUCT_4x4Matrix		m_ViewMatrix;
	IGN_STRUCT_4x4Matrix		m_ProjectionMatrix;
	
	//
	// Protected methods
	//
	void						Init					(	HWND					a_hWnd,
															IGN_OBJIF_GfxContext*	a_pGfxContextInterface	);
	void						Finit					(	void											);
	IGNBOOL						ProceedCamera			(	ENUM_MOVE				a_MoveCamera,
															IGNFLOAT				a_fPitch,
															IGNFLOAT				a_fYaw,
															IGNFLOAT				a_fRoll,
															IGNFLOAT				a_fAdditionalPitch,
															IGNFLOAT				a_fAdditionalYaw,
															IGNFLOAT				a_fAdditionalRoll,
															IGNBOOL					a_bResetAdditionalValues,
															TERRAIN*				a_pTerrain				);


public:

	//
	// Public interface
	//
	IGNBOOL						GetPosition				(	IGN_STRUCT_3DVector&	a_Position				);
	IGNBOOL						GetViewMatrix			(	IGN_STRUCT_4x4Matrix&	a_Matrix				);
	IGNBOOL						GetProjectionMatrix		(	IGN_STRUCT_4x4Matrix&	a_Matrix				);
	IGNBOOL						ProcessAgain			(	void											);
	IGNBOOL						Proceed					(	IGN_OBJIF_InpImmMouse*	a_pMouse,
															ENUM_MOVE				a_MoveCamera,
															TERRAIN*				a_pTerrain				);
};




//------------- PREPROCESSOR END -------------------------------------------------------------------------------------

#endif // CAMERA_H_INCLUDED
