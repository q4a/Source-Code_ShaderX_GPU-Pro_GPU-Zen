///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e WindowSystem::CBrowserTreeView.
 * 
 * @file
 *  BrowserTreeView.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  11.10.2001
 * @brief
 *  Implementation of \e WindowSystem::CBrowserTreeView.
 */



#include "stdafx.h"
#include "BrowserTreeView.h"
#include "BrowserRenderView.h"
#include "BrowserShaderFormView.h"
#include "Effect.h"
#include "EffectFactory.h"
#include "stdDeleteObject.h"
#include <algorithm>

#include "InfiniteZoom\InfiniteZoom.h"
#include "Hair\Hair.h"
#include "SoftObjects\SoftObjects.h"
#include "Beams\Beams.h"
#include "DisplacementMap\DisplacementMap.h"
#include "OceanScene\OceanScene.h"
#include "VolumetricLight\VolumetricLight.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif





using namespace WindowSystem;
using namespace EffectSystem;
using namespace Shared;
using namespace Effects;
using namespace std;



IMPLEMENT_DYNCREATE( CBrowserTreeView, CTreeView )



CBrowserTreeView::CBrowserTreeView()
: m_pkRenderView( 0 )
, m_pkShaderView( 0 )
, m_kImageList()
, m_colEffectFactories()
{
}



CBrowserTreeView::~CBrowserTreeView()
{
    for_each( m_colEffectFactories.begin(), m_colEffectFactories.end(), 
              Shared::TDeleteObject() );
}



BEGIN_MESSAGE_MAP( CBrowserTreeView, CTreeView) 
    ON_WM_CREATE()
    ON_NOTIFY_REFLECT( TVN_SELCHANGING, OnTvnSelChanging )
    ON_NOTIFY_REFLECT( TVN_ITEMEXPANDED, OnTvnItemExpanded )
END_MESSAGE_MAP()



#ifdef _DEBUG
void 
CBrowserTreeView::AssertValid() const
{
    CTreeView::AssertValid();
}



void 
CBrowserTreeView::Dump( CDumpContext& dc ) const
{
    CTreeView::Dump( dc );
}
#endif //_DEBUG



void 
CBrowserTreeView::OnInitialUpdate()
{
    GetTreeCtrl().ModifyStyle( 0, TVS_HASLINES | TVS_LINESATROOT | 
        TVS_HASBUTTONS | TVS_SHOWSELALWAYS  );

    CTreeView::OnInitialUpdate();
}



int 
CBrowserTreeView::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
    if( -1 == CTreeView::OnCreate( lpCreateStruct ) )
    {
        return( -1 );
    }

    // create and load image list
    m_kImageList.Create( 16, 16, ILC_COLOR32 | ILC_MASK, NUM_IMAGES, 0 );

    m_kImageList.Add( AfxGetApp()->LoadIcon( IDI_FOLDER ) );
    m_kImageList.Add( AfxGetApp()->LoadIcon( IDI_FOLDER_OPEN ) );
    m_kImageList.Add( AfxGetApp()->LoadIcon( IDI_EFFECT ) );
    m_kImageList.Add( AfxGetApp()->LoadIcon( IDI_EFFECT_PLAYING ) );

    GetTreeCtrl().SetImageList( &m_kImageList, TVSIL_NORMAL );
    
    // insert root
    HTREEITEM hRoot( InsertRoot() );

    // insert effects
    InsertEffect( TEffectFactory< CInfiniteZoom >::GetEffectName(), 
        new TEffectFactory< CInfiniteZoom >, hRoot );

    InsertEffect( TEffectFactory< CHair >::GetEffectName(), 
        new TEffectFactory< CHair >, hRoot );

    InsertEffect( TEffectFactory< CSoftObjects >::GetEffectName(), 
        new TEffectFactory< CSoftObjects >, hRoot );

    InsertEffect( TEffectFactory< CBeams >::GetEffectName(), 
        new TEffectFactory< CBeams >, hRoot );

    InsertEffect( TEffectFactory< CDisplacementMap >::GetEffectName(), 
        new TEffectFactory< CDisplacementMap >, hRoot );

    InsertEffect( TEffectFactory< COceanScene >::GetEffectName(), 
        new TEffectFactory< COceanScene >, hRoot );

    InsertEffect( TEffectFactory< CVolumetricLight >::GetEffectName(), 
        new TEffectFactory< CVolumetricLight >, hRoot );

    // expand effects tree
    GetTreeCtrl().Expand( hRoot, TVE_EXPAND );

    return( 0 );
}



HTREEITEM 
CBrowserTreeView::InsertRoot()
{
    return( GetTreeCtrl().InsertItem( _T( "Available effects" ), FOLDER_CLOSED, FOLDER_CLOSED ) );
}



void 
CBrowserTreeView::InsertEffect( const string& strEffectName, 
                                CEffectFactory* pkEffectFactory, 
                                HTREEITEM hRoot )
{
    TVINSERTSTRUCT sTvInsert;
    sTvInsert.hParent             = hRoot;
    sTvInsert.hInsertAfter        = TVI_LAST;
    sTvInsert.item.lParam         = (LPARAM) pkEffectFactory;
    sTvInsert.item.pszText        = (LPSTR) strEffectName.c_str();
    sTvInsert.item.iImage         = EFFECT;
    sTvInsert.item.iSelectedImage = EFFECT_PLAYING;
    sTvInsert.item.mask           = TVIF_TEXT | TVIF_PARAM | 
                                    TVIF_IMAGE | TVIF_SELECTEDIMAGE;

    GetTreeCtrl().InsertItem( &sTvInsert );

    m_colEffectFactories.push_back( pkEffectFactory );
}



void 
CBrowserTreeView::OnTvnSelChanging( NMHDR *pNMHDR, LRESULT *pResult )
{
    LPNMTREEVIEW pNMTreeView( reinterpret_cast< LPNMTREEVIEW >( pNMHDR ) );

    *pResult = 1;

    // set to old selection if new one doesn't have a valid
    // effect factory ptr
    if( 0 == pNMTreeView->itemNew.lParam )
    {
        return;
    }

    DBG_ASSERT( 0 != m_pkRenderView );
    DBG_ASSERT( 0 != m_pkShaderView );
    DBG_ASSERT( 0 != pNMTreeView->itemNew.lParam );

    if( pNMTreeView->itemNew.lParam != pNMTreeView->itemOld.lParam )    
    {
        CEffectFactory* pkNewEffectFactory( reinterpret_cast< CEffectFactory* >
                                                ( pNMTreeView->itemNew.lParam ) );

        IEffect* pkNewEffect( m_pkRenderView->SetNewEffect( pkNewEffectFactory ) );
        if( 0 != pkNewEffect )
        {
            m_pkShaderView->SetShaderSources( pkNewEffect );
            *pResult = 0;
        }
    }    
}



void 
CBrowserTreeView::OnTvnItemExpanded( NMHDR *pNMHDR, LRESULT *pResult )
{
    LPNMTREEVIEW pNMTreeView( reinterpret_cast< LPNMTREEVIEW >( pNMHDR ) );
    
    if( TVE_EXPAND == pNMTreeView->action )
    {
        GetTreeCtrl().SetItemImage( pNMTreeView->itemNew.hItem, FOLDER_OPEN, FOLDER_OPEN );
    }
    else
    if( TVE_COLLAPSE == pNMTreeView->action )
    {
        GetTreeCtrl().SetItemImage( pNMTreeView->itemNew.hItem, FOLDER_CLOSED, FOLDER_CLOSED );
    }

    *pResult = 0;
}




void
CBrowserTreeView::SetRenderView( CBrowserRenderView* pkRenderView )
{
    m_pkRenderView = pkRenderView;
}



void
CBrowserTreeView::SetShaderView( CBrowserShaderFormView* pkShaderView )
{
    m_pkShaderView = pkShaderView;
}