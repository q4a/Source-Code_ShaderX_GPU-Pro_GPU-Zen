///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Effects::CSoftObject.
 *
 * @file
 *  SoftObject.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  08.03.2002
 * @brief
 *  Implementation of \e Effects::CSoftObject.
 */



#include "StdAfx.h"
#include "SoftObject.h"



using namespace Effects;



void 
CSoftObject::UpdatePosition( float fTime, float fAttractor )
{
    int iInd( (int) fTime );
    float fBlend( fTime - (float) iInd );

    D3DXVECTOR3 vT1( m_pvKeys[ ( iInd + 1 ) & ( NUM_ANIMKEYS - 1 ) ] - 
                     m_pvKeys[ ( iInd - 1 ) & ( NUM_ANIMKEYS - 1 ) ] );
    D3DXVECTOR3 vT2( m_pvKeys[ ( iInd + 2 ) & ( NUM_ANIMKEYS - 1 ) ] - 
                     m_pvKeys[ ( iInd - 0 ) & ( NUM_ANIMKEYS - 1 ) ] );

    D3DXVec3Hermite( &m_vCurrentPos, 
                     &m_pvKeys[ iInd & ( NUM_ANIMKEYS - 1 ) ], 
                     &vT1,
                     &m_pvKeys[ ( iInd + 1 ) & ( NUM_ANIMKEYS - 1 ) ], 
                     &vT2, 
                     fBlend
                    );

    D3DXVec3Lerp( &m_vCurrentPos, &D3DXVECTOR3( 0.5f, 0.5f, 0.5f ), &m_vCurrentPos, fAttractor );
}