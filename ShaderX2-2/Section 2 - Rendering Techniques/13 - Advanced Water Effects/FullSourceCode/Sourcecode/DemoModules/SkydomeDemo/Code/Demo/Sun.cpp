//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			SkydomeDemo
//*				File:				Sun.cpp
//*
//********************************************************************************************************************
//*
//*				Author:				Marco Spoerl
//*									Copyright (c) 2003 Marco Spoerl
//*									All rights reserved.
//*
//********************************************************************************************************************
//********************************************************************************************************************

#include "Sun.h"

//********************************************************************************************************************
//*				SUN
//********************************************************************************************************************

SUN::SUN														( IGN_OBJIF_GfxContext*	a_pGfxContextInterface )
: SKYOBJECT														( a_pGfxContextInterface )
{
	//----------------------------------------------------------|------------------------------------------------SUN-|
	// public method
	// Constructor

	m_pFreeCameraInterface->SetFOV( static_cast<IGNFLOAT>(IGN_PI * 0.2), 1.0f );

	m_Color.SetInterpolationType( SplineInterpolationType_Cubic );

	m_Color.AddKnot( IGN_STRUCT_3DVector( 0.00f, 0.00f, 0.00f ), 0.00f );
	m_Color.AddKnot( IGN_STRUCT_3DVector( 0.70f, 0.70f, 0.70f ), 0.00f );
	m_Color.AddKnot( IGN_STRUCT_3DVector( 0.50f, 0.50f, 0.70f ), 0.10f );
	m_Color.AddKnot( IGN_STRUCT_3DVector( 0.60f, 0.45f, 0.40f ), 0.40f );
	m_Color.AddKnot( IGN_STRUCT_3DVector( 0.50f, 0.40f, 0.20f ), 0.45f );
	m_Color.AddKnot( IGN_STRUCT_3DVector( 0.40f, 0.20f, 0.10f ), 0.50f );
	m_Color.AddKnot( IGN_STRUCT_3DVector( 0.30f, 0.20f, 0.10f ), 0.60f );
	m_Color.AddKnot( IGN_STRUCT_3DVector( 0.10f, 0.10f, 0.10f ), 0.75f );
	m_Color.AddKnot( IGN_STRUCT_3DVector( 0.02f, 0.01f, 0.02f ), 0.80f );
	m_Color.AddKnot( IGN_STRUCT_3DVector( 0.01f, 0.01f, 0.01f ), 1.00f );

	m_Color.CalculateTangents();

	Prepare();
}

SUN::~SUN														()
{
	//----------------------------------------------------------|-----------------------------------------------~SUN-|
	// public method
	// Destructor
}

IGNDOUBLE SUN::ComputeGeocentricPosition								()
{
	//----------------------------------------------------------|--------------------------ComputeGeocentricPosition-|
	// private method
	// Computes the sun's geocentric position

	//
	// All angular values are given in degrees !!!
	//

	//
	// Orbital elements
	//

	IGNDOUBLE	w		= rev( 282.9404	+ 4.70935e-5 * m_dTime );	// Longitude of perihelion
	IGNDOUBLE	a		= 1.0;										// Mean distance in astronomical units
	IGNDOUBLE	e		= 0.016709 - 1.151e-9 * m_dTime;			// Eccentricity
				m_dM	= rev( 356.0470	+ 0.9856002585 * m_dTime );	// Mean anomaly

	//
	// Obliquity of the ecliptic
	//

	IGNDOUBLE	oblecl	= rev( 23.4393 - 3.563e-7 * m_dTime );

	//
	// Mean longitude
	//

				m_dL	= rev( w + m_dM);

	//
	// Eccentric anomaly
	//

	IGNDOUBLE	E		= rev( m_dM + IGN_RadToDeg * e * sind( m_dM ) * ( 1.0 + e * cosd( m_dM ) ) );

	//
	// Rectangular coordinates
	//

	IGNDOUBLE	x		= cosd( E ) - e;
	IGNDOUBLE	y		= sind( E ) * sqrt( 1.0 - e * e );

	//
	// Distance and true anomaly
	//

	IGNDOUBLE	r		= sqrt( x * x + y * y );
	IGNDOUBLE	v		= rev( arctan2d( y, x ) );

	//
	// Longitude
	//

	IGNDOUBLE	lon		= rev( v + w );

	//
	// Ecliptic rectangular coordinates
	//

	m_vecPosition.m_fX	= static_cast<IGNFLOAT>( r * cosd( lon ) );
    m_vecPosition.m_fY	= static_cast<IGNFLOAT>( r * sind( lon ) );
	m_vecPosition.m_fZ	= 0.0f;

	//
	// Rotate the position by the time of day
	//

	IGNDOUBLE dDays		= floor( m_dTime );
	IGNDOUBLE dRotation = ( m_dTime - dDays ) * IGN_2PI;

	return dRotation;
}

void SUN::Prepare												( void )
{
	//----------------------------------------------------------|---------------------------------------GetLongitude-|
	// public method
	// Computes the sun's parameter at the current time

	//
	// Compute current position
	//

	IGNDOUBLE dRotation;
	dRotation = ComputeGeocentricPosition();

	m_vecNormal	= m_vecPosition;
	m_vecNormal	.Normalize();

	IGN_STRUCT_3DVector vecStandardViewDirection;
	vecStandardViewDirection.m_fX	= 1.0f;
	vecStandardViewDirection.m_fY	= 0.0f;
	vecStandardViewDirection.m_fZ	= 0.0f;
	m_pFreeCameraInterface->SetViewDirection( vecStandardViewDirection );
	IGNFLOAT fPitchRotation = static_cast<IGNFLOAT>( IGN_DegToRad * rev( arccosd( m_vecNormal.m_fX ) ) );
	if( m_vecNormal.m_fY>=0.0f )
		fPitchRotation *= -1.0f;
	fPitchRotation += static_cast<IGNFLOAT>( dRotation );
	m_pFreeCameraInterface->SetAdditionalPitch( fPitchRotation );
	m_pFreeCameraInterface->Execute();

	IGN_STRUCT_4x4Matrix	matRotation;
							matRotation.GenerateRollRotate( static_cast<IGNFLOAT>(dRotation) );

	m_vecPosition.MultiplyMatrix( matRotation );

	m_vecNormal	= m_vecPosition;
	m_vecNormal	.Normalize();

	//
	// Compute current color
	//

	IGNFLOAT fDayTime = 1.0f - ( ( 1.0f + m_vecNormal.m_fY ) * 0.5f );

	IGN_STRUCT_3DVector vecTempColor = m_Color.Interpolate( fDayTime );

	m_CurrentColor.m_fRed	= ( vecTempColor.m_fX > 1.0f ? 1.0f : vecTempColor.m_fX );
	m_CurrentColor.m_fGreen = ( vecTempColor.m_fY > 1.0f ? 1.0f : vecTempColor.m_fY );
	m_CurrentColor.m_fBlue	= ( vecTempColor.m_fZ > 1.0f ? 1.0f : vecTempColor.m_fZ );
	m_CurrentColor.m_fAlpha = 1.0f;
}
