
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			SkydomeDemo
//*				File:				Sun.h
//*				Description:		Sun sky object
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
//*				03/15/03| 0.0.1		## Initial release	(ms) ##
//*
//********************************************************************************************************************
//********************************************************************************************************************

//------------- PREPROCESSOR -----------------------------------------------------------------------------------------

#ifndef			SUN_H_INCLUDED
#define			SUN_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> Sun.h\n")
#endif
#endif 

//------------- DEPENDENCIES -----------------------------------------------------------------------------------------

#include		"SkyObject.h"
#include		"Spline.h"

//------------- FORWARD DECLARATIONS ---------------------------------------------------------------------------------

//------------- DEFINITIONS ------------------------------------------------------------------------------------------

//------------- DECLARATIONS -----------------------------------------------------------------------------------------

class SUN : public SKYOBJECT
{

public:

	//
	// Constructors/Destructors
	//

	SUN		( IGN_OBJIF_GfxContext*	a_pGfxContextInterface );
	~SUN	();

private:

	//
	// Sun data
	//

	IGNDOUBLE						m_dM;			// Mean anomaly
	IGNDOUBLE						m_dL;			// Mean longitude
	SPLINE<IGN_STRUCT_3DVector>		m_Color;		// Color interpolation spline
	IGN_STRUCT_ColorValue			m_CurrentColor;	// Current object color sampled form the spline

	//
	// Protected methods
	//

	IGNDOUBLE						ComputeGeocentricPosition	( void );

public:

	//
	// Public interface
	//

	void							Prepare						( void );

	const IGNDOUBLE&				GetLongitude				( void ) const;
	const IGNDOUBLE&				GetMeanAnomaly				( void ) const;
	const IGN_STRUCT_ColorValue&	GetColor					( void ) const;

};

//------------- INLINE IMPLEMENTATION --------------------------------------------------------------------------------

inline const IGNDOUBLE& SUN::GetLongitude						( void ) const
{
	//----------------------------------------------------------|---------------------------------------GetLongitude-|
	// public method
	// Returns the sky current mean longitude

	return m_dL;
}

inline const IGNDOUBLE& SUN::GetMeanAnomaly						( void ) const
{
	//----------------------------------------------------------|-------------------------------------GetMeanAnomaly-|
	// public method
	// Returns the sun's current mean anomaly

	return m_dM;
}

inline const IGN_STRUCT_ColorValue& SUN::GetColor				( void ) const
{
	//----------------------------------------------------------|-------------------------------------------GetColor-|
	// public method
	// Returns the sun's current color

	return m_CurrentColor;
}

//------------- PREPROCESSOR END -------------------------------------------------------------------------------------

#endif // SUN_H_INCLUDED
