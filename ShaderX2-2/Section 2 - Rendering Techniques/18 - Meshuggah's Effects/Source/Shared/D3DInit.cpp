///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Shared::CD3DInit.
 * 
 * @file
 *  D3DInit.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  21.10.2001
 * @brief
 *  Implementation of \e Shared::CD3DInit.
 */



#include "stdafx.h"
#include "D3DInit.h"



using namespace Shared;



const unsigned int c_uiAdapter( D3DADAPTER_DEFAULT );
const D3DDEVTYPE   c_d3DDevType( D3DDEVTYPE_HAL );



CD3DInit::CD3DInit()
: m_hD3D8( ::LoadLibrary( "d3d8.dll" ) )
, m_pfCreate( 0 )
{
    if( 0 != m_hD3D8 )
    {
        m_pfCreate = (FPDirect3DCreate8) GetProcAddress( m_hD3D8, "Direct3DCreate8" );
    }
}   



CD3DInit::~CD3DInit()
{
    if( 0 != m_hD3D8 )
    {
        ::FreeLibrary( m_hD3D8 );
    }
}



CD3DInit& 
CD3DInit::GetInstance()
{
    static CD3DInit s_kInstance;
    return( s_kInstance );
}



IDirect3D8* 
CD3DInit::Direct3DCreate8()
{
    IDirect3D8* pD3D( 0 );
    if( 0 != m_pfCreate )
    {
        pD3D = m_pfCreate( D3D_SDK_VERSION );
    }     

    return( pD3D );
}



bool 
CD3DInit::Get32bitColorBufferFormat( IDirect3D8* pD3D, D3DFORMAT &eColBufFormat )
{
    const unsigned int c_uiNum32bitFormats( 3 );
    static D3DFORMAT s_col32ARGB[ c_uiNum32bitFormats ] = 
    {               
        D3DFMT_A8R8G8B8,
        D3DFMT_X8R8G8B8,
        D3DFMT_R8G8B8        
    };

    for( unsigned int i( 0 ); i < c_uiNum32bitFormats; ++i )
    {       
        if( FAILED( pD3D->CheckDeviceFormat( 
                                    c_uiAdapter, 
                                    c_d3DDevType, 
                                    s_col32ARGB[ i ], 
                                    D3DUSAGE_RENDERTARGET, 
                                    D3DRTYPE_SURFACE, 
                                    s_col32ARGB[ i ] 
                                    ) ) )
        {           
            continue;
        }

        break;
    }
    
    if( c_uiNum32bitFormats == i )
    {
        return( false );
    }

    eColBufFormat = s_col32ARGB[ i ];

    return( true );
}



bool 
CD3DInit::GetZBufferFormat( IDirect3D8* pD3D, D3DFORMAT eColBufFormat, D3DFORMAT &eZBufFormat )
{
    const unsigned int c_uiNumDepthFormats( 6 );
    static D3DFORMAT s_col6bitDepthStencil[ c_uiNumDepthFormats ] = 
    { 
        // depth formats that match the color depth of eColFormat come first
        D3DFMT_D16,
        D3DFMT_D15S1,

        D3DFMT_D32,
        D3DFMT_D24S8,
        D3DFMT_D24X4S4,
        D3DFMT_D24X8
    };
    
    static D3DFORMAT s_col32bitDepthStencil[ c_uiNumDepthFormats ] = 
    { 
        // depth formats that match the color depth of eColFormat come first
        D3DFMT_D32,
        D3DFMT_D24S8,
        D3DFMT_D24X4S4,
        D3DFMT_D24X8,

        D3DFMT_D16,
        D3DFMT_D15S1
    };
    
    D3DFORMAT* pDepthSencil( 0 );    
    switch( eColBufFormat )
    {
    case D3DFMT_R5G6B5:
    case D3DFMT_X1R5G5B5:   
    case D3DFMT_A1R5G5B5:   
        {
            pDepthSencil = s_col6bitDepthStencil;
        }
        break;
        
    case D3DFMT_R8G8B8:
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:   
    default:
        {
            pDepthSencil = s_col32bitDepthStencil;
        }
        break;
    }
    
    for( unsigned int i( 0 ); i < c_uiNumDepthFormats; ++i )
    {       
        if( FAILED( pD3D->CheckDeviceFormat( 
                                c_uiAdapter, 
                                c_d3DDevType, 
                                eColBufFormat,
                                D3DUSAGE_DEPTHSTENCIL, 
                                D3DRTYPE_SURFACE, 
                                pDepthSencil[ i ] 
                                ) ) )
        {
            continue;
        }
        
        if( FAILED( pD3D->CheckDepthStencilMatch( 
                                c_uiAdapter, 
                                c_d3DDevType, 
                                eColBufFormat, 
                                eColBufFormat, 
                                pDepthSencil[ i ] 
                                ) ) )
        {
            continue;
        }
        
        break;
    }
    
    if( c_uiNumDepthFormats == i )
    {
        return( false );
    }

    eZBufFormat = pDepthSencil[ i ];

    return( true );
}



bool
CD3DInit::InitializeD3D( HWND hWndFocus, D3DPRESENT_PARAMETERS& sD3DPresParams, 
                         IDirect3DDevice8*& pD3DDev, bool bHardwareVertexProcessing )
{
    IDirect3D8* pD3D( CD3DInit::GetInstance().Direct3DCreate8() );
    if( 0 == pD3D  )
    {       
        return( false );
    }

    unsigned int uiD3DBehaviorFlags( D3DCREATE_SOFTWARE_VERTEXPROCESSING );  
#ifndef _DEBUG
    if( false != bHardwareVertexProcessing ) 
    {
        uiD3DBehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;  
    }
#endif

    if( FAILED( pD3D->CreateDevice( 
                                    c_uiAdapter, 
                                    c_d3DDevType, 
                                    hWndFocus,              
                                    uiD3DBehaviorFlags, 
                                    &sD3DPresParams, 
                                    &pD3DDev 
                                   ) ) )
    {
        pD3D->Release();
        return( false );
    }

    if( FALSE == sD3DPresParams.Windowed )
    {
        pD3DDev->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x0, 1.0f, 0 );
        pD3DDev->Present( 0, 0, 0, 0 );
    }

    pD3D->Release();
    return( true );
}
