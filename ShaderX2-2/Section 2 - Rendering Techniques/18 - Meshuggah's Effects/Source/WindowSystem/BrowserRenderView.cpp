///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e WindowSystem::CBrowserRenderView.
 *
 * @file
 *  BrowserRenderView.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  11.10.2001
 * @brief
 *  Implementation of \e WindowSystem::CBrowserRenderView.
 */



#include "stdafx.h"
#include "resource.h"
#include "BrowserRenderView.h"
#include "Meshuggah.h"
#include "D3DInit.h"
#include "Effect.h"
#include "EffectFactory.h"
#include "EffectException.h"
#include "TextureManager.h"
#include "ScrambledArchive.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



using namespace Application;
using namespace WindowSystem;
using namespace EffectSystem;
using namespace Shared;
using namespace ScrambledArchive;
using namespace std;



const char c_pucDataArchive[] = "IDR_DATA";



IMPLEMENT_DYNCREATE( CBrowserRenderView, CView )



CBrowserRenderView::CBrowserRenderView()
: m_pD3DDev( 0 )
, m_sD3DPresParams()
, m_bForceReset( false )
, m_pkCurrentEffect( 0 )
, m_pkScrambledArchive( 0 )
, m_pkTextureManager( 0 )
{
    CMeshuggahApp::GetApp().GetD3DPresentParameters( m_sD3DPresParams );
}




CBrowserRenderView::~CBrowserRenderView()
{
}



BEGIN_MESSAGE_MAP( CBrowserRenderView, CView )
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_COMMAND( ID_VIEW_EFFECTSETTINGSDIALOG, OnEffectSettingsDialog )
    ON_UPDATE_COMMAND_UI( ID_VIEW_EFFECTSETTINGSDIALOG, OnUpdateEffectSettingsDialog )
    ON_COMMAND( ID_CONTEXT_HELP, OnContextHelp )
    ON_UPDATE_COMMAND_UI( ID_CONTEXT_HELP, OnUpdateContextHelp )
END_MESSAGE_MAP()



void
CBrowserRenderView::OnDraw( CDC* pDC )
{
    if( 0 != m_pkCurrentEffect )
    {
        // Blit back buffer to view. Since we use D3DSWAPEFFECT_COPY
        // for windowed mode rendering we can copy as many times as needed
        // without re-rendering the scene.
        m_pD3DDev->Present( 0, 0, 0, 0 );
    }
}



#ifdef _DEBUG
void
CBrowserRenderView::AssertValid() const
{
    CView::AssertValid();
}



void
CBrowserRenderView::Dump( CDumpContext& dc ) const
{
    CView::Dump( dc );
}
#endif //_DEBUG



int
CBrowserRenderView::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
    if( -1 == CView::OnCreate( lpCreateStruct ) )
    {
        return( -1 );
    }

    CWaitCursor kWaitCursor;

    if( false == CD3DInit::GetInstance().InitializeD3D( GetSafeHwnd(), m_sD3DPresParams, m_pD3DDev ) )
    {
        if( IDYES == AfxMessageBox( IDS_BROWSERWARNING_NOHWVERTEXSHADER,
                                    MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION ) )
        {
            if( false == CD3DInit::GetInstance().InitializeD3D( GetSafeHwnd(), m_sD3DPresParams, m_pD3DDev, false ) )
            {
                AfxMessageBox( IDS_ERROR_D3D );
                return( -1 );
            }
        }
        else
        {
            return( -1 );
        }
    }

    if( 0 == ( m_pkScrambledArchive = CScrambledArchive::CreateFromResource( c_pucDataArchive ) ) )
    {
        AfxMessageBox( IDS_BROWSERERROR_DATA );
        return( -1 );
    }

    if( 0 == ( m_pkTextureManager = CTextureManager::CreateAndBindToArchive( m_pD3DDev, m_pkScrambledArchive ) ) )
    {
        AfxMessageBox( IDS_BROWSERERROR_DATA );
        return( -1 );
    }

    return( 0 );
}



void
CBrowserRenderView::OnSize( UINT nType, int cx, int cy )
{
    CView::OnSize( nType, cx, cy );

    m_bForceReset = true;
}



void
CBrowserRenderView::OnDestroy()
{
    CView::OnDestroy();

    if( 0 != m_pD3DDev )
    {
        m_pD3DDev->Release();
    }

    if( 0 != m_pkCurrentEffect )
    {
        m_pkCurrentEffect->Destroy();
    }

    delete m_pkScrambledArchive;
    delete m_pkTextureManager;
}



void
CBrowserRenderView::OnEffectSettingsDialog()
{
    DBG_ASSERT( 0 != m_pkCurrentEffect );
    m_pkCurrentEffect->ShowEffectDialog( !m_pkCurrentEffect->IsEffectDialogVisible() );
}



void
CBrowserRenderView::OnUpdateEffectSettingsDialog( CCmdUI* pCmdUI )
{
    if( 0 != m_pkCurrentEffect )
    {
        pCmdUI->Enable();

        if( false != m_pkCurrentEffect->IsEffectDialogVisible() )
        {
            pCmdUI->SetCheck();
        }
        else
        {
            pCmdUI->SetCheck( 0 );
        }
    }
    else
    {
        pCmdUI->SetCheck( 0 );
        pCmdUI->Enable( FALSE );
    }
}



void
CBrowserRenderView::OnContextHelp()
{
    DBG_ASSERT( 0 != m_pkCurrentEffect );
    CMeshuggahApp::GetApp().HtmlHelp( m_pkCurrentEffect->GetHTMLHelpID() );
}



void
CBrowserRenderView::OnUpdateContextHelp( CCmdUI* pCmdUI )
{
    if( 0 != m_pkCurrentEffect )
    {
        pCmdUI->Enable();
    }
    else
    {
        pCmdUI->Enable( FALSE );
    }
}



bool
CBrowserRenderView::FrameUpdate()
{
    HRESULT hr;

    // test the cooperative level to see if it's okay to render
    // if "m_bForceReset" is true then the window has been resized
    if( FAILED( hr = m_pD3DDev->TestCooperativeLevel() ) || false != m_bForceReset )
    {
        // if the device was lost, do not render until we get it back
        if( D3DERR_DEVICELOST == hr )
        {
            return( true );
        }

        // check if the device needs to be resized.
        if( D3DERR_DEVICENOTRESET == hr || false != m_bForceReset )
        {
            // since the browser always runs in windowed mode
            // read the possibly new desktop mode

            IDirect3D8* pD3D( 0 );
            m_pD3DDev->GetDirect3D( &pD3D );

            D3DDISPLAYMODE sDisplayMode;
            if( FAILED( pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &sDisplayMode ) ) )
            {
                pD3D->Release();
                return( false );
            }
            m_sD3DPresParams.BackBufferFormat = sDisplayMode.Format;

            if( false == CD3DInit::GetInstance().GetZBufferFormat( pD3D,
                                                                   sDisplayMode.Format,
                                                                   m_sD3DPresParams.AutoDepthStencilFormat ) )
            {
                pD3D->Release();
                return( false );
            }
            pD3D->Release();

            if( 0 != m_pkCurrentEffect )
            {
                m_pkCurrentEffect->FreeAppManagedResources();
            }

            if( FAILED( m_pD3DDev->Reset( &m_sD3DPresParams ) ) )
            {
                DBG_ASSERTM( 0, "CBrowserRenderView::FrameUpdate() - d3d device rest failed!" );
                return( false );
            }

            if( 0 !=  m_pkCurrentEffect )
            {
                try
                {
                    m_pkCurrentEffect->InitAppManagedResources();
                }
                catch( CEffectException kException )
                {
                    kException.ReportError();
                    return( false );
                }
            }

            m_bForceReset = false;
        }
    }

    if( 0 != m_pkCurrentEffect )
    {
        m_pkCurrentEffect->Update();
    }

    return( true );
}




IEffect*
CBrowserRenderView::SetNewEffect( CEffectFactory* pkNewEffectFactory )
{
    DBG_ASSERT( 0 != pkNewEffectFactory );

    CWaitCursor kWaitCursor;

    // create temporary texture manager
    CTextureManager* pkTempTextureManager( CTextureManager::CreateAndBindToArchive( m_pD3DDev, m_pkScrambledArchive ) );
    if( 0 == pkTempTextureManager  )
    {
        return( 0 );
    }

    // try to create the new effect
    IEffect* pkNewEffect( pkNewEffectFactory->
        CreateInstance( m_pD3DDev, m_pkScrambledArchive, pkTempTextureManager ) );

    if( 0 != pkNewEffect )
    {
        if( 0 != m_pkCurrentEffect )
        {
            // release old effect
            m_pkCurrentEffect->Destroy();
        }

        // set the new effect
        m_pkCurrentEffect = pkNewEffect;

        // The temporary texture manager will become our new texture manager.
        // This effectivly purges all textures used by the previous effect and
        // prevents textures from being purged if effect creation failed.
        delete m_pkTextureManager;
        m_pkTextureManager = pkTempTextureManager;
    }
    else
    {
        // Effect creation failed, delete temporary texture manager.
        delete pkTempTextureManager;
    }

    return( pkNewEffect );
}
