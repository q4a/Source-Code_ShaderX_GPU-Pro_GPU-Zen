///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e WindowSystem::CDemoMainFrame.
 * 
 * @file
 *  DemoMainFrm.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  11.10.2001
 * @brief
 *  Implementation of \e WindowSystem::CDemoMainFrame.
 */



#include "stdafx.h"
#include "resource.h"
#include "DemoMainFrm.h"
#include "Meshuggah.h"
#include "D3DInit.h"
#include "EffectSequencer.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



using namespace Application;
using namespace WindowSystem;
using namespace EffectSystem;
using namespace Shared;



const int c_iFrameWidth( 640 );
const int c_iFrameHeight( 480 );



BEGIN_MESSAGE_MAP( CDemoMainFrame, CFrameWnd )
    ON_WM_CREATE()
    ON_WM_SYSCOMMAND()
    ON_WM_KEYDOWN()
    ON_WM_DESTROY()
END_MESSAGE_MAP()



CDemoMainFrame::CDemoMainFrame( bool bEnableMusic, bool bLoopDemo )
: m_pkEffectSequencer( 0 )
, m_pD3DDev( 0 )
, m_sD3DPresParams()
, m_bEnableMusic( bEnableMusic)
, m_bLoopDemo( bLoopDemo )
{
    CMeshuggahApp::GetApp().GetD3DPresentParameters( m_sD3DPresParams );
}



CDemoMainFrame::~CDemoMainFrame()
{
    m_kDemoLoaderWnd.DestroyWindow();
}



int 
CDemoMainFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
    if( -1 == CFrameWnd::OnCreate( lpCreateStruct ) )
    {
        return( -1 );
    }

    CWaitCursor kWaitCursor;

    m_kDemoLoaderWnd.Create( this );
    m_kDemoLoaderWnd.ShowWindow( SW_SHOW );
    m_kDemoLoaderWnd.UpdateWindow();

    if( false == CD3DInit::GetInstance().InitializeD3D( GetSafeHwnd(), m_sD3DPresParams, m_pD3DDev ) )
    {   
        // release d3d device
        // otherwise the following error message will be invisible when running in fullscreen mode
        ReleaseD3DDevice(); 
        AfxMessageBox( IDS_ERROR_D3D );
        return( -1 );
    }

    if( 0 == ( m_pkEffectSequencer = CEffectSequencer::Create( m_hWnd, m_pD3DDev, 
                                                               m_bEnableMusic, m_bLoopDemo ) ) )
    {
        // release d3d device
        // otherwise the following error message will be invisible when running in fullscreen mode
        ReleaseD3DDevice();
        AfxMessageBox( IDS_DEMOERROR_EFFECTSYSTEM );
        return( -1 );
    }

    m_kDemoLoaderWnd.ShowWindow( SW_HIDE );
    return( 0 );
}



BOOL 
CDemoMainFrame::PreCreateWindow( CREATESTRUCT& cs )
{
    if( FALSE == CFrameWnd::PreCreateWindow( cs ) )
    {
        return( FALSE );
    }
    
    if( ::GetSystemMetrics( SM_CXSCREEN ) < c_iFrameWidth || 
        ::GetSystemMetrics( SM_CYSCREEN ) < c_iFrameHeight )
    {
        cs.x = 0;
        cs.y = 0;
        
        cs.cx = ::GetSystemMetrics( SM_CXSCREEN );
        cs.cy = ::GetSystemMetrics( SM_CYSCREEN );
    }
    else
    {
        cs.x = ( ::GetSystemMetrics( SM_CXSCREEN ) - c_iFrameWidth ) / 2;
        cs.y = ( ::GetSystemMetrics( SM_CYSCREEN ) - c_iFrameHeight ) / 2;
        
        cs.cx = c_iFrameWidth;
        cs.cy = c_iFrameHeight;
    }

    return( TRUE );
}



#ifdef _DEBUG
void 
CDemoMainFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}



void 
CDemoMainFrame::Dump( CDumpContext& dc ) const
{
    CFrameWnd::Dump( dc );
}
#endif //_DEBUG



void 
CDemoMainFrame::OnSysCommand( UINT nID, LPARAM lParam )
{
    // disable screensavers and display power management
    switch( nID )
    {
    case SC_SCREENSAVE:
    case SC_MONITORPOWER:
        {
            return;
        }
        break;
        
    default:
        {
        }
        break;
    }

    CFrameWnd::OnSysCommand( nID, lParam );
}



void 
CDemoMainFrame::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
    DBG_ASSERT( 0 != m_pkEffectSequencer );

    CFrameWnd::OnKeyDown( nChar, nRepCnt, nFlags );

    switch( nChar )
    {
    case VK_ESCAPE:
        {
            m_pkEffectSequencer->Stop();
            OnClose();
        }
        break;

    case VK_SPACE:
        {
            if( false != m_pkEffectSequencer->IsPlaying() )
            {
                m_pkEffectSequencer->Pause();
            }
            else
            {
                m_pkEffectSequencer->Play();
            }
        }
        break;

    case 'r':
    case 'R':
        {
            m_pkEffectSequencer->Reset();
        }
        break;

    case 's':
    case 'S':
        {
            m_pkEffectSequencer->Stop();
        }
        break;

    default:
        {
        }
        break;
    }
}



void 
CDemoMainFrame::ReleaseD3DDevice()
{
    if( 0 != m_pD3DDev )
    {
        m_pD3DDev->Release();
        m_pD3DDev = 0;
    }
}



void 
CDemoMainFrame::OnDestroy()
{
    CFrameWnd::OnDestroy();

    ReleaseD3DDevice();
    delete m_pkEffectSequencer;
}



void 
CDemoMainFrame::PlayDemo()
{
    DBG_ASSERT( 0 != m_pkEffectSequencer );

    while( 0 <= ShowCursor( FALSE ) );
    m_pkEffectSequencer->Play();
}



bool 
CDemoMainFrame::RenderFrame()
{
    DBG_ASSERT( 0 != m_pkEffectSequencer );

    bool bSuccess( m_pkEffectSequencer->FrameUpdate( m_sD3DPresParams ) );
    if( false == bSuccess )
    {       
        OnClose();
    }

    return( bSuccess );
}