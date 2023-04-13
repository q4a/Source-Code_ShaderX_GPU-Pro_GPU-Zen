
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			WaterDemo
//*				File:				Water.h
//*				Description:		Water effect for water testing applications
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

#ifndef			WATER_H_INCLUDED
#define			WATER_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> Water.h\n")
#endif
#endif 




//------------- DEPENDENCIES -----------------------------------------------------------------------------------------

#include "DemoFrame.h"




//------------- FORWARD DECLARATIONS ---------------------------------------------------------------------------------

class TERRAIN;
class WATER;
class SKYDOME;
class CAMERA;




//------------- DEFINITIONS ------------------------------------------------------------------------------------------

#define		constReflectionTextureWidth				1024
#define		constReflectionTextureHeight			1024
#define		constRefractionTextureWidth				1024
#define		constRefractionTextureHeight			512




//------------- DECLARATIONS -----------------------------------------------------------------------------------------

struct	structVertexFormatWater
{
	IGN_STRUCT_3DVector			m_Position;
	IGN_STRUCT_3DVector			m_Normal;
	IGNFLOAT					m_TexCoord0_U;
	IGNFLOAT					m_TexCoord0_V;
	IGNFLOAT					m_TexCoord1_U;
	IGNFLOAT					m_TexCoord1_V;
	IGNFLOAT					m_TexCoord1_S;
	IGNFLOAT					m_TexCoord2_U;
	IGNFLOAT					m_TexCoord2_V;
	IGNFLOAT					m_TexCoord2_S;
	IGNFLOAT					m_TexCoord3_U;
	IGNFLOAT					m_TexCoord3_V;
};


class WATER
{
public:

	//
	// Constructors/Destructors
	//
	WATER		(	IGN_OBJIF_GfxContext*	a_pGfxContextInterface,
					IGN_OBJIF_GfxAssistant*	a_pAssistant,
					IGN_OBJIF_GfxMixer*		a_pMixer );
	~WATER		();

protected:

	//
	// Static data
	//
	static IGN_STRUCT_DclElement	ms_WaterVtxDeclaration[];
	static IGN_STRUCT_DclElement	ms_RefractionVtxDeclaration[];
	
	//
	// Handles of used dynamic link libraries
	//
	HMODULE						m_hTmrLibrary;

	//
	// Objects in IGN_graphic_sub.dll
	//
	IGN_OBJIF_GfxVisual*		m_pVisual;
	IGN_OBJIF_GfxVertexData*	m_pVertexData;
	IGN_OBJIF_GfxIndexData*		m_pIndexData;
	IGN_OBJIF_GfxShader*		m_pShader;
	IGN_OBJIF_GfxVertexProgDcl*	m_pWaterVertexProgDcl;
	IGN_OBJIF_GfxVertexProgDcl*	m_pRefractionVertexProgDcl;
	IGN_OBJIF_GfxVertexProgram*	m_pWaterVertexProgramDX8GF3;
	IGN_OBJIF_GfxVertexProgram*	m_pWaterVertexProgramDX8;
	IGN_OBJIF_GfxVertexProgram*	m_pWaterVertexProgramDX9;
	IGN_OBJIF_GfxPixelProgram*	m_pWaterPixelProgramDX8;
	IGN_OBJIF_GfxPixelProgram*	m_pWaterPixelProgramDX9;
	IGN_OBJIF_GfxVertexProgram*	m_pRefractionVertexProgramDX9;
	IGN_OBJIF_GfxPixelProgram*	m_pRefractionPixelProgramDX9;
	IGN_OBJIF_GfxMaterial*		m_pMaterial;
	IGN_OBJIF_GfxTextureBlock*	m_pTextureBlock0;
	IGN_OBJIF_GfxTextureBlock*	m_pTextureBlock1;
	IGN_OBJIF_GfxTextureBlock*	m_pTextureBlock2;
	IGN_OBJIF_GfxTextureBlock*	m_pTextureBlock3;
	IGN_OBJIF_GfxTextureBlock*	m_pTextureBlock4;
	IGN_OBJIF_GfxImageData*		m_pReflectionImage;
	IGN_OBJIF_GfxImageData*		m_pRefractionImage;
	IGN_OBJIF_GfxImageData*		m_pDetailImage;
	IGN_OBJIF_GfxImageData*		m_pBumpImage1;
	IGN_OBJIF_GfxImageData*		m_pBumpImage2;
	IGNHANDLE					m_hRefractionDepthBuffer;
	IGNHANDLE					m_hReflectionDepthBuffer;
	IGN_OBJIF_GfxCamera*		m_pMirrorCamera;

	//
	// Objects in IGN_timer_sub.dll
	//
	IGN_OBJIF_TmrInstance*		m_pTmrInstanceInterface;
	IGN_OBJIF_TmrContext*		m_pTmrContextInterface;
	IGN_OBJIF_TmrTimer*			m_pTimerInterface;

	//
	// Internal objects
	//
	IGN_OBJIF_GfxAssistant*		m_pAssistant;
	IGN_OBJIF_GfxMixer*			m_pMixer;
	CAMERA*						m_pOriginalCamera;
	IGNFLOAT					m_fWaterHeight;
	ENUM_TARGETCLASS			m_TargetClass;
	IGN_OBJIF_GfxVertexProgram*	m_pVertexProgram;
	IGN_OBJIF_GfxPixelProgram*	m_pPixelProgram;
	SKYDOME*					m_pSkydome;
	TERRAIN*					m_pTerrain;
	IGNBOOL						m_bWireframeModus;
	IGN_STRUCT_4x4Matrix		m_ReflectionTextureMatrix;
	IGN_STRUCT_4x4Matrix		m_RefractionTextureMatrix;

	//
	// Protected methods
	//
	void						Init					(	IGN_OBJIF_GfxContext*	a_pGfxContextInterface	);
	void						Finit					(	void											);
	IGNBOOL						BuildReflectionTexture	(	void											);
	IGNBOOL						BuildRefractionTexture	(	void											);
	IGNBOOL						ProceedWater			(	void											);


public:

	//
	// Public interface
	//
	IGNBOOL						CheckPosition			(	IGN_STRUCT_3DVector&	a_Position				);
	IGNBOOL						PreProcess				(	CAMERA*					a_pCamera,
															IGNFLOAT				a_fWaterHeight,
															ENUM_TARGETCLASS		a_TargetClass,
															SKYDOME*				a_pSkydome,
															TERRAIN*				a_pTerrain				);
	IGNBOOL						Proceed					(	IGNBOOL					a_bWireframeModus,
															ENUM_TARGETCLASS		a_TargetClass			);
};




//------------- PREPROCESSOR END -------------------------------------------------------------------------------------

#endif // WATER_H_INCLUDED