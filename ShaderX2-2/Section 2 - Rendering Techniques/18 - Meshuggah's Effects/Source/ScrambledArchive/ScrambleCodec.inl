///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e ScrambledArchive::CScrambleCodec (inline methods).
 * 
 * @file
 *  ScrambleCodec.inl
 * @author
 *  Carsten Wenzel
 * @date
 *  31.10.2001
 * @brief
 *  Implementation of \e ScrambledArchive::CScrambleCodec (inline methods).
 */



#ifndef _SCRAMBLECODEC_INL_INCLUDED_
#define _SCRAMBLECODEC_INL_INCLUDED_

#pragma once



inline 
CScrambleCodec::SCryptoHeader::SCryptoHeader()
: m_uiTag( CHT_CRYPTOTAG )
, m_uiOrigDataSize( -1 )
{
}



inline 
CScrambleCodec::SCryptoHeader::SCryptoHeader( unsigned int uiOrigFileSize, const CMD5Generator::SMD5& sMD5 )
: m_uiTag( CHT_CRYPTOTAG )
, m_uiOrigDataSize( uiOrigFileSize )
{
    memcpy( &m_sMD5, &sMD5, sizeof( CMD5Generator::SMD5 ) );
}



#endif // #ifndef _SCRAMBLECODEC_INL_INCLUDED_
