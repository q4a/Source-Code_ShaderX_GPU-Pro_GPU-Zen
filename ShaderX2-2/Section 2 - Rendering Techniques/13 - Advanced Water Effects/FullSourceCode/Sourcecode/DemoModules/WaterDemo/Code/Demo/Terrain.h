
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			WaterDemo
//*				File:				Terrain.h
//*				Description:		Terrain for water testing applications
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

#ifndef			TERRAIN_H_INCLUDED
#define			TERRAIN_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> Terrain.h\n")
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

#define		constTerrainWidth				128
#define		constTerrainDepth				128
#define		constTerrainBaseHeight			-5.0f
#define		constTerrainHeightmapScale		0.1f
#define		constTerrainTextureUScale		0.25f
#define		constTerrainTextureVScale		0.25f




//------------- DECLARATIONS -----------------------------------------------------------------------------------------

struct	structVertexFormatTerrain
{
	IGN_STRUCT_3DVector			m_Position;
	IGN_STRUCT_3DVector			m_Normal;
	IGNFLOAT					m_TexCoord0_U;
	IGNFLOAT					m_TexCoord0_V;
};


class TERRAIN
{
public:

	//
	// Constructors/Destructors
	//
	TERRAIN		(	IGN_OBJIF_GfxContext*	a_pGfxContextInterface,
					IGN_OBJIF_GfxAssistant*	a_pAssistant );
	~TERRAIN	();

protected:
	
	//
	// Objects in IGN_graphic_sub.dll
	//
	IGN_OBJIF_GfxVisual*		m_pVisual;
	IGN_OBJIF_GfxVertexData*	m_pVertexData;
	IGN_OBJIF_GfxIndexData*		m_pIndexData;
	IGN_OBJIF_GfxShader*		m_pShader;
	IGN_OBJIF_GfxMaterial*		m_pMaterial;
	IGN_OBJIF_GfxTextureBlock*	m_pTextureBlock0;
	IGN_OBJIF_GfxTextureBlock*	m_pTextureBlock1;
	IGN_OBJIF_GfxImageData*		m_pHeightMap;
	IGN_OBJIF_GfxImageData*		m_pTerrainImage;
	
	//
	// Internal objects
	//
	IGN_OBJIF_GfxAssistant*		m_pAssistant;
	IGNFLOAT					m_fWeightSum;

	//
	// Protected methods
	//
	void						Init					(	IGN_OBJIF_GfxContext*	a_pGfxContextInterface	);
	void						Finit					(	void											);
	IGNFLOAT					WeightingFactor			(	IGN_STRUCT_3DVector		a_PositionA,
															IGN_STRUCT_3DVector		a_PositionB				);
	IGNBOOL						ProceedTerrain			(	IGN_OBJIF_GfxVertexProgDcl*
																					a_pVertexProgDcl,
															IGN_OBJIF_GfxVertexProgram*
																					a_pVertexProgram,
															IGN_OBJIF_GfxPixelProgram*
																					a_pPixelProgram			);
	IGNBOOL						ProceedTerrain			(	void											);


public:

	//
	// Public interface
	//
	IGNBOOL						CheckPosition			(	IGN_STRUCT_3DVector&	a_Position				);
	IGNBOOL						Proceed					(	IGN_OBJIF_GfxVertexProgDcl*
																					a_pVertexProgDcl,
															IGN_OBJIF_GfxVertexProgram*
																					a_pVertexProgram,
															IGN_OBJIF_GfxPixelProgram*
																					a_pPixelProgram			);
	IGNBOOL						Proceed					(	void											);
};




//------------- PREPROCESSOR END -------------------------------------------------------------------------------------

#endif // TERRAIN_H_INCLUDED