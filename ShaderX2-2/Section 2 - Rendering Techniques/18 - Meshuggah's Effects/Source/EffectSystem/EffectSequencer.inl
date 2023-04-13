///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e EffectSystem::CEffectSequencer (inline methods).
 *
 * @file
 *  EffectSequencer.inl
 * @author
 *  Carsten Wenzel
 * @date
 *  27.11.2001
 * @brief
 *  Implementation of \e EffectSystem::CEffectSequencer (inline methods).
 */



#ifndef _EFFECTSEQUENCER_INL_INCLUDED_
#define _EFFECTSEQUENCER_INL_INCLUDED_

#pragma once



inline bool
CEffectSequencer::IsPlaying()
{
    return( PLAYING == m_eStatus );
}



#endif // #ifndef _EFFECTSEQUENCER_INL_INCLUDED_