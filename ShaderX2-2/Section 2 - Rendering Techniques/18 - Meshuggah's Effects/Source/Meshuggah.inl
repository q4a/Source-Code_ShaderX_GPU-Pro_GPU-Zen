///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Application::CMeshuggahApp (inline methods).
 * 
 * @file
 *  Meshuggah.inl
 * @author
 *  Carsten Wenzel
 * @date
 *  21.10.2001
 * @brief
 *  Implementation of \e Application::CMeshuggahApp (inline methods).
 */



#ifndef _MESHUGGAH_INL_INCLUDED_
#define _MESHUGGAH_INL_INCLUDED_

#pragma once



inline CMeshuggahApp&
CMeshuggahApp::GetApp()
{
    return( s_kTheApp );
}



inline void
CMeshuggahApp::GetD3DPresentParameters( D3DPRESENT_PARAMETERS& sPresentParameters )
{
    memcpy( &sPresentParameters, &m_sD3DPresParams, sizeof( D3DPRESENT_PARAMETERS ) );
}



#endif // #define _MESHUGGAH_INL_INCLUDED_