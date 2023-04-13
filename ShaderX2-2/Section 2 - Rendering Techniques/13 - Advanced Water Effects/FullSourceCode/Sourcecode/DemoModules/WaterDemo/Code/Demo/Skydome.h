
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			SkydomeDemo
//*				File:				Skydome.h
//*				Description:		Skydome for skydome testing applications
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

#ifndef			SKYDOME_H_INCLUDED
#define			SKYDOME_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> Skydome.h\n")
#endif
#endif 

//------------- DEPENDENCIES -----------------------------------------------------------------------------------------

#include		"DemoFrame.h"

//------------- FORWARD DECLARATIONS ---------------------------------------------------------------------------------

class			TERRAIN;
class			WATER;
class			SKYDOME;
class			CAMERA;
class			SUN;
class			MOON;

//------------- DEFINITIONS ------------------------------------------------------------------------------------------

//------------- DECLARATIONS -----------------------------------------------------------------------------------------

struct	structVertexFormatSkydome
{
	IGN_STRUCT_3DVector	m_vecPosition;
	IGN_STRUCT_3DVector	m_vecNormal;
};

class SKYDOME
{

public:

	//
	// Constructors/Destructors
	//

	SKYDOME		(	IGN_OBJIF_GfxContext*	a_pGfxContextInterface,
					IGN_OBJIF_GfxAssistant*	a_pAssistant );
	~SKYDOME	();

protected:

	//
	// Static data
	//

	static IGN_STRUCT_DclElement	ms_Declaration[];

	//
	// Handles of used dynamic link libraries
	//

	HMODULE							m_hTmrLibrary;

	//
	// Objects in IGN_graphic_sub.dll
	//

	IGN_OBJIF_GfxVisual*			m_pVisual;
	IGN_OBJIF_GfxVertexData*		m_pVertexData;
	IGN_OBJIF_GfxShader*			m_pShader;
	IGN_OBJIF_GfxVertexProgDcl*		m_pVertexProgDcl;
	IGN_OBJIF_GfxVertexProgram*		m_pVertexProgram;
	IGN_OBJIF_GfxPixelProgram*		m_pPixelProgram;
	IGN_OBJIF_GfxMaterial*			m_pMaterial;
	IGN_OBJIF_GfxTextureBlock*		m_pTextureBlock0;
	IGN_OBJIF_GfxTextureBlock*		m_pTextureBlock1;
	IGN_OBJIF_GfxTextureBlock*		m_pTextureBlock2;
	IGN_OBJIF_GfxImageData*			m_pImageSun;
	IGN_OBJIF_GfxImageData*			m_pImageMoon;
	IGN_OBJIF_GfxImageData*			m_pImageMoonNormals;
	IGN_OBJIF_GfxLight*				m_pLight;

	//
	// Objects in IGN_timer_sub.dll
	//

	IGN_OBJIF_TmrInstance*			m_pTmrInstanceInterface;
	IGN_OBJIF_TmrContext*			m_pTmrContextInterface;
	IGN_OBJIF_TmrClock*				m_pClockInterface;

	//
	// Internal objects
	//

	IGN_OBJIF_GfxAssistant*			m_pAssistant;
	IGNBOOL							m_bWireframeModus;
	IGN_STRUCT_LightData			m_LightData;						

	//
	// Sky objects
	//

	SUN*							m_pSun;
	MOON*							m_pMoon;
	IGNDOUBLE						m_dLastPhaseAngle;

	//
	// Protected methods
	//

	void							Init				(	IGN_OBJIF_GfxContext*		a_pGfxContextInterface	);
	void							Finit				(	void												);

	IGNBOOL							ProceedSkyDome		(	const IGN_STRUCT_4x4Matrix&	a_matViewMatrix
														,	const IGN_STRUCT_4x4Matrix&	a_matProjectionMatrix
														,	const IGN_STRUCT_3DVector&	a_vecCameraPosition		);

public:

	//
	// Public interface
	//

	void							AdvanceTime			( const IGNDOUBLE&				a_dTimeOffset			);

	IGNBOOL							Proceed				(	CAMERA*						a_pCamera
														,	IGNBOOL						a_bWireframeModus		);
	IGNBOOL							Proceed				(	IGN_OBJIF_GfxCamera*		a_pCamera				);

};

//------------- PREPROCESSOR END -------------------------------------------------------------------------------------

#endif // SKYDOME_H_INCLUDED
