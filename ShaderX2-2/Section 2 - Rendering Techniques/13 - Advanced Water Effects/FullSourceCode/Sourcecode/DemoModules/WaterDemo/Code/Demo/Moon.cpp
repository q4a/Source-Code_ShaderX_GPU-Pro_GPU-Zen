//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			SkydomeDemo
//*				File:				Moon.cpp
//*
//********************************************************************************************************************
//*
//*				Author:				Marco Spoerl
//*									Copyright (c) 2003 Marco Spoerl
//*									All rights reserved.
//*
//********************************************************************************************************************
//********************************************************************************************************************

#include	"Moon.h"

//********************************************************************************************************************
//*				MOON
//********************************************************************************************************************

MOON::MOON														( IGN_OBJIF_GfxContext*	a_pGfxContextInterface )
: SKYOBJECT														( a_pGfxContextInterface )
{
	//----------------------------------------------------------|-----------------------------------------------MOON-|
	// public method
	// Constructor

	m_pFreeCameraInterface->SetFOV( static_cast<IGNFLOAT>(IGN_PI * 0.05), 1.0f );
}

MOON::~MOON														()
{
	//----------------------------------------------------------|----------------------------------------------~MOON-|
	// public method
	// Destructor
}

IGNDOUBLE MOON::ComputeGeocentricPosition							( const IGNDOUBLE& a_dSunLongitude
																, const IGNDOUBLE& a_dSunMeanAnomaly )
{
	//----------------------------------------------------------|--------------------------ComputeGeocentricPosition-|
	// private method
	// Computes the moon's geocentric position

	//
	// All angular values are given in degrees !!!
	//

	//
	// Orbital elements
	//

	IGNDOUBLE	N		= rev( 125.1228 - 0.0529538083 * m_dTime );		// Longitude of the ascending node
	IGNDOUBLE	i		= 5.1454;										// Inclination
	IGNDOUBLE	w		= rev( 318.0634 + 0.1643573223 * m_dTime );		// Argument of perigee
	IGNDOUBLE	a		= 60.2666;										// Mean distance in Earth radii
	IGNDOUBLE	e		= 0.054900;										// Eccentricity
	IGNDOUBLE	M		= rev( 115.3654 + 13.0649929509 * m_dTime );	// Mean anomaly

	//
	// Eccentric anomaly
	//

	IGNDOUBLE	E0		= rev( M + IGN_RadToDeg * e * sind( M ) * ( 1.0 + e * cosd( M ) ) );
	IGNDOUBLE	E1		= rev( E0 - ( E0 - IGN_RadToDeg * e * sind( E0 ) - M ) / ( 1.0 - e * cosd( E0 ) ) );

	//
	// Rectangular coordinates
	//

	IGNDOUBLE	x		= a * ( cosd( E1 ) - e );
	IGNDOUBLE	y		= a * sqrt( 1.0 - e * e ) * sind( E1 );

	//
	// Distance and true anomaly
	//

	IGNDOUBLE	r		= sqrt( x * x + y * y );
	IGNDOUBLE	v		= rev( arctan2d( y, x ) );

	//
	// Ecliptic rectangular coordinates
	//

	IGNDOUBLE	xeclip	= r * ( cosd( N ) * cosd( v + w ) - sind( N ) * sind( v + w ) * cosd( i ) );
	IGNDOUBLE	yeclip	= r * ( sind( N ) * cosd( v + w ) + cosd( N ) * sind( v + w ) * cosd( i ) );
	IGNDOUBLE	zeclip	= r * sind( v + w ) * sind( i );

	//
	// Longitude and latitude
	//

	IGNDOUBLE	lon		= rev( arctan2d( yeclip, xeclip ) );
	IGNDOUBLE	lat		= rev( arctan2d( zeclip, sqrt( xeclip * xeclip + yeclip * yeclip ) ) );

	//
	// Perturbations of the Moon
	//

	IGNDOUBLE	Ls		= a_dSunLongitude;		// Sun's  mean longitude
	IGNDOUBLE	Lm		= rev( N + w + M );		// Moon's mean longitude
	IGNDOUBLE	Ms		= a_dSunMeanAnomaly;	// Sun's  mean anomaly
	IGNDOUBLE	Mm		= M;					// Moon's mean anomaly
	IGNDOUBLE	D		= rev( Lm - Ls );		// Moon's mean elongation
	IGNDOUBLE	F		= rev( Lm - N );		// Moon's argument of latitude

	lon = rev( lon +	-1.274 * sind( Mm - 2.0 * D )
						+0.658 * sind( 2.0 * D )
						-0.186 * sind( Ms )
						-0.059 * sind( 2.0 * Mm - 2.0 * D )
						-0.057 * sind( Mm - 2.0 * D + Ms )
						+0.053 * sind( Mm + 2.0 * D )
						+0.046 * sind( 2.0 * D - Ms )
						+0.041 * sind( Mm - Ms )
						-0.035 * sind( D)
						-0.031 * sind( Mm + Ms )
						-0.015 * sind( 2.0 * F - 2.0 * D )
						+0.011 * sind( Mm - 4 * D ) );

	lat = rev( lat +	-0.173 * sind( F - 2.0 * D )
						-0.055 * sind( Mm - F - 2.0 * D )
						-0.046 * sind( Mm + F - 2.0 * D )
						+0.033 * sind( F + 2.0 * D )
						+0.017 * sind( 2.0 * Mm + F ) );

	r = rev( r +	-0.58 * cosd( Mm - 2.0 * D )
					-0.46 * cosd( 2.0 * D ) );

	//
	// Recalc ecliptic rectangular coordinates with new distance
	//

	m_vecPosition.m_fX	= static_cast<IGNFLOAT>( r * ( cosd( N ) * cosd( v + w ) - sind( N ) * sind( v + w ) * cosd( i ) ) );
	m_vecPosition.m_fY	= static_cast<IGNFLOAT>( r * ( sind( N ) * cosd( v + w ) + cosd( N ) * sind( v + w ) * cosd( i ) ) );
	m_vecPosition.m_fZ	= static_cast<IGNFLOAT>( r * sind( v + w ) * sind( i ) );

	//
	// Elongation and phase angle
	//

	IGNDOUBLE	elong	= rev( arccosd( cosd( a_dSunLongitude - lon ) * cosd( lat ) ) );
				m_dFV	= rev( 180.0 - elong );

	//
	// Rotate the position by the time of day
	//

	IGNDOUBLE dDays		= floor( m_dTime );
	IGNDOUBLE dRotation = ( m_dTime - dDays ) * IGN_2PI;

	return dRotation;
}

void MOON::Prepare												( const IGNDOUBLE& a_dSunLongitude
																, const IGNDOUBLE& a_dSunMeanAnomaly )
{
	//----------------------------------------------------------|--------------------------------------------Prepare-|
	// public method
	// Computes the moon's parameter at the current time

	//
	// Compute current position
	//

	IGNDOUBLE dRotation;
	dRotation = ComputeGeocentricPosition( a_dSunLongitude, a_dSunMeanAnomaly );

	m_vecNormal	= m_vecPosition;
	m_vecNormal	.Normalize();

	//
	// Calculate angles for additional rotations
	//

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
}
