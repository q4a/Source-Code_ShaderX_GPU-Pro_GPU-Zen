///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e EffectSystem::CEffect (inline methods).
 * 
 * @file
 *  EffectFactory.inl
 * @author
 *  Carste Wenzel
 * @date
 *  11.10.2001
 * @brief
 *  Implementation of \e EffectSystem::CEffect (inline methods).
 */



#ifndef _EFFECT_INL_INCLUDED_
#define _EFFECT_INL_INCLUDED_

#pragma once



inline bool
CEffect::IsRegistered() const
{
    return( m_bRegistered );
}



inline bool
CEffect::Register( float fEffectStart, float fEffectRuntime )
{
    if( false != IsRegistered() || 
        0.0f >= fEffectRuntime )
    {
        return( false );
    }

    m_fEffectStart = fEffectStart;
    m_fEffectEnd   = fEffectStart + fEffectRuntime;
    m_bRegistered  = true;

    return( true );
}



inline float 
CEffect::GetEffectStart() const
{
    return( m_fEffectStart );
}
    


inline float 
CEffect::GetEffectEnd() const
{
    return( m_fEffectEnd );
}



inline float
CEffect::GetEffectRuntime() const
{
    return( GetEffectEnd() - GetEffectStart() );
}



inline float
CEffect::GetRelativeTime( float fAbsoluteTime ) const
{
    return( fAbsoluteTime - GetEffectStart() );
}



template< typename T >
inline void
CEffect::SafeRelease( T*& pkInterface ) const
{
    if( 0 != pkInterface )
    {
        pkInterface->Release();
        pkInterface = 0;
    }
}



inline void
CEffect::GetWorldViewProjection( D3DXMATRIX& matWorldViewProj )
{
    D3DXMATRIX matTemp;
    D3DXMatrixMultiply( &matTemp, &m_matWorld, &m_matView );
    D3DXMatrixMultiply( &matTemp, &matTemp, &m_matProj );
    D3DXMatrixTranspose( &matWorldViewProj, &matTemp );
}



#endif // #ifndef _EFFECT_INL_INCLUDED_
