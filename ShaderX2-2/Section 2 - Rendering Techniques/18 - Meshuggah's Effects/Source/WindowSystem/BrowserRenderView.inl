///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e WindowSystem::CBrowserRenderView (inline methods).
 *
 * @file
 *  BrowserRenderView.inl
 * @author
 *  Carsten Wenzel
 * @date
 *  03.03.2002
 * @brief
 *  Implementation of \e WindowSystem::CBrowserRenderView (inline methods).
 */



#ifndef _BROWSERRENDERVIEW_INL_INCLUDED_
#define _BROWSERRENDERVIEW_INL_INCLUDED_

#pragma once

    
    
inline bool 
CBrowserRenderView::EffectAvailable()
{
    return( 0 != m_pkCurrentEffect );
}



#endif // #ifndef _BROWSERRENDERVIEW_INL_INCLUDED_