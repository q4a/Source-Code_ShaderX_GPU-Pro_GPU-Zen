///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Effects::CSoftObject (inline methods).
 *
 * @file
 *  SoftObject.inl
 * @author
 *  Carsten Wenzel
 * @date
 *  08.03.2002
 * @brief
 *  Implementation of \e Effects::CSoftObject (inline methods).
 */



#ifndef _SOFTOBJECT_INL_INCLUDED_
#define _SOFTOBJECT_INL_INCLUDED_

#pragma once



inline float 
CSoftObject::GetIsoValue( const D3DXVECTOR3& vPos ) const
{
    D3DXVECTOR3 vDist( vPos - m_vCurrentPos );
    return( 2e-3f / D3DXVec3LengthSq( &vDist ) );
}



inline void 
CSoftObject::GetIsoNormal( const D3DXVECTOR3& vPos, D3DXVECTOR3& vNormal ) const
{
    // given our iso formula
    //
    // F( vPos ) =               1 
    //             ----------------------------
    //             | vPos - m_vCurrentPos | ^ 2

    // the gradient is as follows...
    //
    // G( vPos ) =  ( dF( vPos ) / dx, dF( vPos ) / dy, dF( vPos ) / dz )              
    //
    // G( vPos ) =  2 * ( m_vCurrentPos - vPos )              
    //              ----------------------------
    //              | vPos - m_vCurrentPos | ^ 4

    // we want the normal which is -G( vPos ) hence...
    // 
    // N( vPos ) =  2 * ( vPos - m_vCurrentPos )              
    //              ----------------------------
    //              | vPos - m_vCurrentPos | ^ 4

    vNormal = vPos - m_vCurrentPos;
    float fLengthSq( D3DXVec3LengthSq( &vNormal ) );    
    vNormal *= 4e-3f / ( fLengthSq * fLengthSq );
}



inline const D3DXVECTOR3& 
CSoftObject::GetPosition() const
{
    return( m_vCurrentPos );
}



inline void 
CSoftObject::SetAnimKey( unsigned int iIndex, const D3DXVECTOR3& vKey )
{
    DBG_ASSERT( iIndex < NUM_ANIMKEYS );
    m_pvKeys[ iIndex ] = vKey;
}



#endif // #ifndef _SOFTOBJECT_INL_INCLUDED_