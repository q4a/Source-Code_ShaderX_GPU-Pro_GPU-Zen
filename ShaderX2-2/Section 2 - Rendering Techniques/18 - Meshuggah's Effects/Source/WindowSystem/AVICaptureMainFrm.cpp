///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e WindowSystem::CAVICaptureMainFrame.
 * 
 * @file
 *  DemoMainFrm.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  11.10.2001
 * @brief
 *  Implementation of \e WindowSystem::CAVICaptureMainFrame.
 */



#include "stdafx.h"
#include "resource.h"
#include "AVICaptureMainFrm.h"
#include "Meshuggah.h"
#include "D3DInit.h"
#include "EffectSequencer.h"
#include "AVICompressor.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



using namespace Application;
using namespace WindowSystem;
using namespace EffectSystem;
using namespace Shared;



const int c_iFrameWidth( 320 );
const int c_iFrameHeight( 240 );
const int c_iAVIFps( 25 );



BEGIN_MESSAGE_MAP( CAVICaptureMainFrame, CFrameWnd )
    ON_WM_CREATE()
    ON_WM_SYSCOMMAND()
    ON_WM_KEYDOWN()
    ON_WM_DESTROY()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



CAVICaptureMainFrame::CAVICaptureMainFrame()
: m_pkEffectSequencer( 0 )
, m_pD3DDev( 0 )
, m_pkAVICompressor( 0 )
, m_fTime( 0.0f )
, m_sD3DPresParams()
{
   memset( &m_sD3DPresParams, 0, sizeof( D3DPRESENT_PARAMETERS ) );    
}



CAVICaptureMainFrame::~CAVICaptureMainFrame()
{
}



int 
CAVICaptureMainFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
    if( -1 == CFrameWnd::OnCreate( lpCreateStruct ) )
    {
        return( -1 );
    }

    CWaitCursor kWaitCursor;
    
    if( false == SetupPresentParamsForCapture() )
    {
        return( -1 );
    }

    if( false == CD3DInit::GetInstance().InitializeD3D( GetSafeHwnd(), m_sD3DPresParams, m_pD3DDev ) )
    {
        AfxMessageBox( IDS_ERROR_D3D );
        return( -1 );
    }

    if( 0 == ( m_pkEffectSequencer = CEffectSequencer::Create( m_hWnd, m_pD3DDev, false, false ) ) )
    {
        AfxMessageBox( IDS_DEMOERROR_EFFECTSYSTEM );
        return( -1 );
    }

    try
    {
        m_pkAVICompressor = new CAVICompressor( "Meshuggah.avi", c_iAVIFps, c_iFrameWidth, c_iFrameHeight );
    }
    catch( ... )
    {
        return( -1 );
    }

    // adjust client window size to the requested capture size
    CRect rectClient;
    GetClientRect( rectClient );

    CRect rectWindow;
    GetWindowRect( rectWindow );

    rectWindow.InflateRect( 0, 
                            0, 
                            c_iFrameWidth - rectClient.Width(),
                            c_iFrameHeight - rectClient.Height()
                           );    
 
    // center window
    if( ::GetSystemMetrics( SM_CXSCREEN ) < rectWindow.Width() || 
        ::GetSystemMetrics( SM_CYSCREEN ) < rectWindow.Height() )
    {
        rectWindow.MoveToXY( 0, 0 );
    }
    else
    {
        rectWindow.MoveToXY( ( ::GetSystemMetrics( SM_CXSCREEN ) - rectWindow.Width() ) / 2, 
                             ( ::GetSystemMetrics( SM_CYSCREEN ) - rectWindow.Height() ) / 2 ); 
    }

    MoveWindow( rectWindow );

    return( 0 );
}



#ifdef _DEBUG
void 
CAVICaptureMainFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}



void 
CAVICaptureMainFrame::Dump( CDumpContext& dc ) const
{
    CFrameWnd::Dump( dc );
}
#endif //_DEBUG



void 
CAVICaptureMainFrame::OnDestroy()
{
    CFrameWnd::OnDestroy();

    if( 0 != m_pD3DDev )
    {
        m_pD3DDev->Release();
    }

    delete m_pkEffectSequencer;
    delete m_pkAVICompressor;
}



bool 
CAVICaptureMainFrame::RenderFrame()
{
    DBG_ASSERT( 0 != m_pkEffectSequencer );

    if( false == m_pkEffectSequencer->FrameUpdate( m_fTime ) ||
        false == m_pkAVICompressor->WriteVideoFrame( m_pD3DDev ) )
    {     
        OnClose();
        return( false );
    }

    m_fTime += 1.0f / (float) c_iAVIFps;
    return( true );
}



bool
CAVICaptureMainFrame::SetupPresentParamsForCapture()
{
    bool bRet( true );

    m_sD3DPresParams.BackBufferWidth                 = c_iFrameWidth;
    m_sD3DPresParams.BackBufferHeight                = c_iFrameHeight;
    m_sD3DPresParams.BackBufferCount                 = 1;
    m_sD3DPresParams.MultiSampleType                 = D3DMULTISAMPLE_NONE;     
    m_sD3DPresParams.SwapEffect                      = D3DSWAPEFFECT_COPY;   
    m_sD3DPresParams.Windowed                        = TRUE;
    m_sD3DPresParams.EnableAutoDepthStencil          = TRUE;
    m_sD3DPresParams.Flags                           = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER; 
    m_sD3DPresParams.FullScreen_RefreshRateInHz      = 0;        
    m_sD3DPresParams.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

    IDirect3D8* pD3D( CD3DInit::GetInstance().Direct3DCreate8() );
    if( 0 == pD3D )
    {
        AfxMessageBox( IDS_D3D_NODX81 );
        bRet = false;       
    }

    if( 0 != pD3D )
    {
        D3DDISPLAYMODE sDisplayMode;
        if( SUCCEEDED( pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &sDisplayMode ) ) )
        {
            m_sD3DPresParams.BackBufferFormat = sDisplayMode.Format;
        }
        else
        {
            AfxMessageBox( IDS_D3D_NO_ADAPTER_DISPLAYMODE );
            bRet = false;
        }

        if( false != bRet && 
            false == CD3DInit::GetInstance().GetZBufferFormat( pD3D, m_sD3DPresParams.BackBufferFormat, 
                                                               m_sD3DPresParams.AutoDepthStencilFormat )  )
        {
            AfxMessageBox( IDS_D3D_NO_SUITABLE_ZBUFFER );       
            bRet = false;
        }

        pD3D->Release();
    }

    return( bRet );
}