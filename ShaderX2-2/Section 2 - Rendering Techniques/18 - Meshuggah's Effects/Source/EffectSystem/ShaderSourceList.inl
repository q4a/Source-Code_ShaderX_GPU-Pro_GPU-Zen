///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e EffectSystem::SShaderSourceList (inline methods).
 *
 * @file
 *  ShaderSourceList.inl
 * @author
 *  Carsten Wenzel
 * @date
 *  27.11.2001
 * @brief
 *  Implementation of \e EffectSystem::SShaderSourceList (inline methods).
 */



#ifndef _SHADERSOURCELIST_INL_INCLUDED_
#define _SHADERSOURCELIST_INL_INCLUDED_

#pragma once



inline
SShaderSourceList::SShaderSource::SShaderSource( 
    const std::string& strShaderName,
    const std::string& strVertexCode,
    const std::string& strPixelCode )
: m_strShaderName( strShaderName )
, m_strVertexCode( strVertexCode )
, m_strPixelCode( strPixelCode )
{
}



#endif // #ifndef _SHADERSOURCELIST_INL_INCLUDED_