
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			SkydomeDemo
//*				File:				Spline.h
//*				Description:		Spline template class
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

#ifndef			SPLINE_H_INCLUDED
#define			SPLINE_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> SPLINE.h\n")
#endif
#endif 

//------------- DEPENDENCIES -----------------------------------------------------------------------------------------

#include		"../../../../Engine/Shared/IGN_shared/Types/ign_types_shared.h"
#pragma warning	( push )
#pragma warning	( disable : 4786 )
#include		<vector>
#pragma warning	( pop )

//------------- FORWARD DECLARATIONS ---------------------------------------------------------------------------------

//------------- DEFINITIONS ------------------------------------------------------------------------------------------

//------------- DECLARATIONS -----------------------------------------------------------------------------------------

enum SplineInterpolationType
{
	SplineInterpolationType_None,
	SplineInterpolationType_Linear,
	SplineInterpolationType_Cubic,
	SplineInterpolationType_ForceDWORD = 0x7fffffff,
};

template< class T >
T HERMITE_SPLINE ( float t, T p0, T p1, T m0, T m1 ) 
{
	IGNFLOAT tempTQuadric = t * t;
	IGNFLOAT tempTCubic   = tempTQuadric * t;

	// Part 1

	IGNFLOAT T1Erg1 = tempTCubic * 2;
	IGNFLOAT T1Erg2 = tempTQuadric * 3;
	IGNFLOAT T1ErgT = T1Erg1 - T1Erg2 + 1;

	// Part 2

	IGNFLOAT T2ErgT = tempTCubic - ( 2 * tempTQuadric ) + t;

	// Part 3

	IGNFLOAT T3ErgT = tempTCubic - tempTQuadric;

	// Part 4

	IGNFLOAT T4ErgT = T1Erg2-T1Erg1;
		
	return ( ( p0 * T1ErgT ) + ( m0 * T2ErgT ) + ( m1 * T3ErgT ) + ( p1 * T4ErgT ) );
}

template< class T > class SPLINE
{

public:

	// Spline KnotInfo - template holding knot info

	template< class T > struct KnotInfo
	{
		IGNFLOAT	m_fLambda;
		T			m_Tangent;
		T			m_Point;
		IGNFLOAT	m_fAlpha;
		IGNFLOAT	m_fBeta;
	};

	typedef KnotInfo<T> KnotType;

protected:


	// Spline - interpolation type

	SplineInterpolationType	m_InterpolationType;

	// Spline - array of knot control points

	std::vector<KnotType>	m_KnotArray;

	// Spline find knots - get right two knots to actual lambda

	void FindKnots( IGNFLOAT m_fActLambda, KnotInfo<T>& a_Knot0, KnotInfo<T>& a_Knot1 )
	{
		for( IGNUINT iIndex = 1; iIndex < m_KnotArray.size(); ++iIndex )
		{
			if( m_KnotArray[iIndex].m_fLambda > m_fActLambda )
			{
				a_Knot0 = m_KnotArray[iIndex - 1];
				a_Knot1 = m_KnotArray[iIndex];

				return;
			}
		}

		a_Knot0 = m_KnotArray[m_KnotArray.size() - 2];
		a_Knot1 = m_KnotArray[m_KnotArray.size() - 1];
	}

public:

	// Spline constructor

	SPLINE	( void )
	{
		m_InterpolationType = SplineInterpolationType_Linear;
	}

	// Spline destructor

	~SPLINE	( void )
	{
	}

	// Spline calculate tangents - call this if using cubic interpolation

	void CalculateTangents ( void )
	{
		if( m_KnotArray.size() < 2 )
			return;

		memset( &m_KnotArray[0].m_Tangent, 0, sizeof(T) );
		memset( &m_KnotArray[m_KnotArray.size() - 1].m_Tangent, 0, sizeof(T) );

		for( IGNUINT iIndex = 1; iIndex < m_KnotArray.size() - 1; ++iIndex )
		{
			KnotInfo<T>* pKnot0 = &m_KnotArray[iIndex - 1];
			KnotInfo<T>* pKnot1 = &m_KnotArray[iIndex];
			KnotInfo<T>* pKnot2 = &m_KnotArray[iIndex + 1];

			IGNFLOAT fOma = 1.0f - pKnot1->m_fAlpha;
			IGNFLOAT fOmb = 1.0f - pKnot1->m_fBeta;
			IGNFLOAT fOpb = 1.0f + pKnot1->m_fBeta;

			pKnot1->m_Tangent = ( ( pKnot1->m_Point - pKnot0->m_Point ) * fOma * fOpb * 0.5f ) + 
								( ( pKnot2->m_Point - pKnot1->m_Point ) * fOma * fOmb * 0.5f );
		}
	}

	// Spline interpolate - return interpolated value at time a_fActLambda

	T Interpolate( IGNFLOAT a_fActLambda )
	{
		if( 1 == m_KnotArray.size() )
			return m_KnotArray[0].m_Point;

		KnotInfo<T> knot0, knot1;

		FindKnots( a_fActLambda, knot0, knot1 );

		IGNFLOAT fRealLambda = ( ( a_fActLambda - knot0.m_fLambda ) / ( knot1.m_fLambda - knot0.m_fLambda ) );

		T retValue;

		if( SplineInterpolationType_Linear == m_InterpolationType )
			retValue = Lerp( fRealLambda, knot0.m_Point, knot1.m_Point );
		else
			retValue = HERMITE_SPLINE( fRealLambda, knot0.m_Point, knot1.m_Point, knot0.m_Tangent, knot1.m_Tangent );

		return retValue;
	}

	// Spline add knot - add knot two spline at desired position lambda

	void AddKnot( T a_Item, IGNFLOAT a_fLambda )
	{
		KnotInfo<T> newKnot;

		newKnot.m_fLambda	= a_fLambda;
		newKnot.m_Point		= a_Item;
		newKnot.m_fAlpha	= 0.0f;
		newKnot.m_fBeta		= 0.0f;

		if( 0 == m_KnotArray.size() )
		{
			m_KnotArray.push_back(newKnot);

			return;
		}

		IGNINT iRef = -1;

		for( IGNUINT iIndex = 0; iIndex < m_KnotArray.size(); ++iIndex )
			if( m_KnotArray[iIndex].m_fLambda > newKnot.m_fLambda )
				iRef = iIndex;

		if( -1 == iRef )
		{
			m_KnotArray.push_back(newKnot);

			return;
		}

		std::vector<KnotType>::iterator i = m_KnotArray.begin() + iRef;
		m_KnotArray.insert( i, newKnot );
	}

	// Spline access knot array - return reference to knot array

	IGN_TEMPL_VALUELIST<KnotType>& AccessKnotArray( void )
	{
		return m_KnotArray;
	}

	// Spline set interpolation type

	void SetInterpolationType( SplineInterpolationType a_Type )
	{
		m_InterpolationType = a_Type;
	}

	// Spline return interpolation type

	SplineInterpolationType GetInterpolationType( void )
	{
		return m_InterpolationType;
	}

};

//------------- PREPROCESSOR END -------------------------------------------------------------------------------------

#endif // SPLINE_H_INCLUDED
