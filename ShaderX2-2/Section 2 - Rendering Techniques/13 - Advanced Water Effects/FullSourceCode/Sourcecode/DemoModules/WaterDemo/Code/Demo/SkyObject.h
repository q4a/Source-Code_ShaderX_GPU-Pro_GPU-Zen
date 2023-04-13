
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			SkydomeDemo
//*				File:				SkyObject.h
//*				Description:		Sky object (e.g. sun or moon) base class
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
//*				03/04/03| 0.0.1		## Initial release	(ms) ##
//*
//********************************************************************************************************************
//********************************************************************************************************************

//------------- PREPROCESSOR -----------------------------------------------------------------------------------------

#ifndef			SKYOBJECT_H_INCLUDED
#define			SKYOBJECT_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> SkyObject.h\n")
#endif
#endif 

//------------- DEPENDENCIES -----------------------------------------------------------------------------------------

#include		"DemoFrame.h"

//------------- FORWARD DECLARATIONS ---------------------------------------------------------------------------------

//------------- DEFINITIONS ------------------------------------------------------------------------------------------

//------------- DECLARATIONS -----------------------------------------------------------------------------------------

class SKYOBJECT
{

public:

	//
	// Constructors/Destructors
	//
			SKYOBJECT	( IGN_OBJIF_GfxContext*	a_pGfxContextInterface );
	virtual	~SKYOBJECT	();

protected:

	//
	// Handles of used dynamic link libraries
	//

	HMODULE						m_hCamLibrary;

	//
	// Objects in IGN_camera_sys.dll
	//

	IGN_OBJIF_CamInstance*		m_pCamInstanceInterface;
	IGN_OBJIF_CamContext*		m_pCamContextInterface;
	IGN_OBJIF_CamFreeCamera*	m_pFreeCameraInterface;

	//
	// Basic sky object data
	//

	IGNDOUBLE					m_dTime;		// Current time
	IGN_STRUCT_3DVector			m_vecPosition;	// Position of the object
	IGN_STRUCT_3DVector			m_vecNormal;	// Normal vector of the object

	//
	// Protected helper methods
	//

	IGNDOUBLE							rev					( const IGNDOUBLE& x ) const;
	IGNDOUBLE							cbrt				( const IGNDOUBLE& x ) const;
	IGNDOUBLE							sind				( const IGNDOUBLE& x ) const;
	IGNDOUBLE							cosd				( const IGNDOUBLE& x ) const;
	IGNDOUBLE							tand				( const IGNDOUBLE& x ) const;
	IGNDOUBLE							arcsind				( const IGNDOUBLE& x ) const;
	IGNDOUBLE							arccosd				( const IGNDOUBLE& x ) const;
	IGNDOUBLE							arctand				( const IGNDOUBLE& x ) const;
	IGNDOUBLE							arctan2d			( const IGNDOUBLE& x
															, const IGNDOUBLE& y ) const;
	IGNINT								Clamp				( const	IGNINT& val
															, const	IGNINT& minVal
															, const	IGNINT& maxVal ) const;

public:

	//
	// Public interface
	//

	void								SetTime				( IGNINT				a_iYYYY
															, IGNINT				a_iMoMo
															, IGNINT				a_iDD
															, IGNINT				a_iHH
															, IGNINT				a_iMiMi );
	virtual void						SetTime				( const IGNDOUBLE&		a_dNewTime		);
	virtual const IGNDOUBLE&			GetTime				( void ) const	;
	virtual void						AdvanceTime			( const IGNDOUBLE&		a_dTimeOffset	);

	virtual const IGN_STRUCT_3DVector&	GetPosition			( void ) const	;
	virtual const IGN_STRUCT_3DVector&	GetNormal			( void ) const	;
	virtual void						GetProjectionMatrix	( IGN_STRUCT_4x4Matrix&	a_matProjection		) const	;
	virtual void						GetViewMatrix		( IGN_STRUCT_4x4Matrix&	a_matView			) const	;

};

//------------- INLINE IMPLEMENTATION --------------------------------------------------------------------------------

inline IGNDOUBLE SKYOBJECT::rev									( const IGNDOUBLE& x ) const
{
	//----------------------------------------------------------|------------------------------------------------rev-|
	// protected method
	// Clamps an angle

	return x - floor( x / 360.0 ) * 360.0;
}

inline IGNDOUBLE SKYOBJECT::sind								( const IGNDOUBLE& x ) const
{
	//----------------------------------------------------------|-----------------------------------------------sind-|
	// protected method
	// Computes the sinus from the specified degrees

	return sin( x * IGN_DegToRad );
}

inline IGNDOUBLE SKYOBJECT::cosd								( const IGNDOUBLE& x ) const
{
	//----------------------------------------------------------|-----------------------------------------------cosd-|
	// protected method
	// Computes the cosinus from the specified degrees

	return cos( x * IGN_DegToRad );
}

inline IGNDOUBLE SKYOBJECT::tand								( const IGNDOUBLE& x ) const
{
	//----------------------------------------------------------|-----------------------------------------------tand-|
	// protected method
	// Computes the tangens from the specified degrees

	return tan( x * IGN_DegToRad );
}

inline IGNDOUBLE SKYOBJECT::arcsind								( const IGNDOUBLE& x ) const
{
	//----------------------------------------------------------|--------------------------------------------arcsind-|
	// protected method
	// Computes the arcus sinus in degrees

	return IGN_RadToDeg * asin( x );
}

inline IGNDOUBLE SKYOBJECT::arccosd								( const IGNDOUBLE& x ) const
{
	//----------------------------------------------------------|--------------------------------------------arccosd-|
	// protected method
	// Computes the arcus cosinus in degrees

	return IGN_RadToDeg * acos( x );
}

inline IGNDOUBLE SKYOBJECT::arctand								( const IGNDOUBLE& x ) const
{
	//----------------------------------------------------------|--------------------------------------------arctand-|
	// protected method
	// Computes the arcus tangens in degrees

	return IGN_RadToDeg * atan( x );
}

inline IGNDOUBLE SKYOBJECT::arctan2d							( const IGNDOUBLE& x, const IGNDOUBLE& y ) const
{
	//----------------------------------------------------------|-------------------------------------------arctan2d-|
	// protected method
	// Computes the arcus tangens2 in degrees

	return IGN_RadToDeg * atan2( x, y );
}

inline IGNINT SKYOBJECT::Clamp									( const	IGNINT& val, const IGNINT& minVal, const IGNINT& maxVal ) const
{
	if( val<minVal )
		return minVal;
	if( val>=maxVal )
		return maxVal-1;
	return val;
}

inline void SKYOBJECT::SetTime									( IGNINT	a_iYYYY
																, IGNINT	a_iMoMo
																, IGNINT	a_iDD
																, IGNINT	a_iHH
																, IGNINT	a_iMiMi )
{
	//----------------------------------------------------------|--------------------------------------------SetTime-|
	// public method
	// Sets the sky object's current time

	m_dTime = ( 367 * a_iYYYY - 7 * ( a_iYYYY + ( a_iMoMo + 9 ) / 12 ) / 4 + 275 * a_iMoMo / 9 + a_iDD - 730530 )
			+ ( ( Clamp( a_iHH, 0, 24 ) * 100 + Clamp( a_iMiMi, 0, 60 ) ) / 100.0 ) / 24.0;
}

inline void SKYOBJECT::SetTime									( const IGNDOUBLE& a_dNewTime )
{
	//----------------------------------------------------------|--------------------------------------------SetTime-|
	// public method
	// Sets the specified time

	m_dTime = a_dNewTime;
}

inline const IGNDOUBLE& SKYOBJECT::GetTime						( void ) const
{
	//----------------------------------------------------------|--------------------------------------------GetTime-|
	// public method
	// Returns the sky object's current time

	return m_dTime;
}

inline void SKYOBJECT::AdvanceTime								( const IGNDOUBLE& a_dTimeOffset )
{
	//----------------------------------------------------------|----------------------------------------AdvanceTime-|
	// public method
	// Advances the sky object's current time by the specified offset

	m_dTime += a_dTimeOffset;
}

inline const IGN_STRUCT_3DVector& SKYOBJECT::GetPosition		( void ) const
{
	//----------------------------------------------------------|----------------------------------------GetPosition-|
	// public method
	// Returns the sky object's position

	return m_vecPosition;
}

inline const IGN_STRUCT_3DVector& SKYOBJECT::GetNormal			( void ) const
{
	//----------------------------------------------------------|------------------------------------------GetNormal-|
	// public method
	// Returns the sky object's normal

	return m_vecNormal;
}

inline void SKYOBJECT::GetProjectionMatrix						( IGN_STRUCT_4x4Matrix&	a_matProjection	) const
{
	//----------------------------------------------------------|-------------------------------GetProjectionMatrix-|
	// public method
	// Returns the sky object projector's projection matrix

	m_pFreeCameraInterface->GetProjectionMatrix( a_matProjection );
}

inline void SKYOBJECT::GetViewMatrix							( IGN_STRUCT_4x4Matrix&	a_matView ) const
{
	//----------------------------------------------------------|-------------------------------------GetViewMatrix-|
	// public method
	// Returns the sky object projector's view matrix

	m_pFreeCameraInterface->GetViewMatrix( a_matView );
}

//------------- PREPROCESSOR END -------------------------------------------------------------------------------------

#endif // SKYOBJECT_H_INCLUDED
