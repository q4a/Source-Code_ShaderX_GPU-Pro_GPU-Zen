///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Shared::CMP3Player (inline methods).
 *
 * @file
 *  MP3Player.inl
 * @author
 *  Carsten Wenzel
 * @date
 *  27.02.2002
 * @brief
 *  Implementation of \e Shared::CMP3Player (inline methods).
 */



#ifndef _MP3PLAYER_INL_INCLUDED_
#define _MP3PLAYER_INL_INCLUDED_

#pragma once



inline bool 
CMP3Player::IsPlaying() const
{
    return( PLAYING == m_eStatus );
}



#endif // #ifndef _MP3PLAYER_INL_INCLUDED_