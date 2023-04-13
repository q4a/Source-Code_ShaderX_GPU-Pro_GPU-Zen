#include "FiniteHLSL.h"

char g_szStencilModeMsg[128];


//-----------------------------------------------------------------------------
// WinMain()
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
#ifdef _DEBUG
  _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

  CFiniteHLSL d3dApp;

  InitCommonControls();

  if( FAILED( d3dApp.Create( hInst ) ) )
      return 0;

  return d3dApp.Run();
}


//-----------------------------------------------------------------------------
// CFiniteHLSL constructor
//-----------------------------------------------------------------------------
CFiniteHLSL::CFiniteHLSL()
{
  m_strWindowTitle                    = _T( "FiniteHLSL Shadow Volume Sample" );
  m_d3dEnumeration.AppUsesDepthBuffer = TRUE;
  m_d3dEnumeration.AppMinDepthBits    = 16;
  m_d3dEnumeration.AppMinStencilBits  = 4;

  m_bKeyInfo                          = FALSE;
  m_bShowObjects                      = TRUE;
  m_bTwoSidedStencil                  = TRUE;       // Assume DX9 compliant hw
  m_bRunTwoSidedStencil               = TRUE;       // Assume DX9 compliant hw
  m_bAnimateLight                     = FALSE;
  m_bShowShadowVol                    = FALSE;

  FillMode                            = SHADE_MODE;

  m_pFont                             = NULL;
  m_pWorldMesh                        = NULL;
  m_pPlatformMesh                     = NULL;
  m_pObjectMesh                       = NULL;
  m_pObjectMesh2                      = NULL;
  m_pShadow                           = NULL;
  m_pShadow2                          = NULL;
  m_pSphereMesh                       = NULL;

  m_vLightPos                         = D3DXVECTOR3( 3.0f, 7.0f,  3.0f );

  m_vAngularSpeed                     = D3DXVECTOR3( 0.0f, 0.0f,  0.0f );
  m_vPosition                         = D3DXVECTOR3( 0.0f, 4.0f,-10.0f );
  m_vDist                             = D3DXVECTOR3( 0.0f, 0.0f,  0.0f );
  m_vYPR                              = D3DXVECTOR3( 0.0f, 0.0f,  0.0f );
	m_vYPR_Dist                         = D3DXVECTOR3( 0.0f, 0.0f,  0.0f );
  m_fAngularSpeed                     = 0.12f;
  m_fSpeed                            = 0.75f;

  m_dwCreationWidth                   = 640;
  m_dwCreationHeight                  = 480;

  m_pEffect                           = NULL;
  m_hmClip                            = NULL;
  m_hvOLP                             = NULL;

  ZeroMemory(m_bKey, 256);
}


//-----------------------------------------------------------------------------
// ConfirmDevice()
// Check device caps here
//-----------------------------------------------------------------------------
HRESULT CFiniteHLSL::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
                        D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat )
{
  HRESULT hr;

  if ( ( dwBehavior&D3DCREATE_HARDWARE_VERTEXPROCESSING ) || 
        ( dwBehavior&D3DCREATE_MIXED_VERTEXPROCESSING ) )
  {
    if ( 0 == ( pCaps->VertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS ) )
      return DXTRACE_ERR( "ConfirmDevice", E_FAIL );

    // At least vs_1_1 as vs_1_0 is no longer supported by DX9
    if( pCaps->VertexShaderVersion < D3DVS_VERSION( 1, 1 ) )
      return DXTRACE_ERR( "ConfirmDevice", E_FAIL );
  }

  // Check required DX9 caps
  // Previously D3DPRASTERCAPS_ZBIAS in DX8.1 for D3DRS_ZBIAS
  // D3DRS_ZBIAS is replaced by D3DRS_DEPTHBIAS
  if ( !( ( pCaps->RasterCaps & D3DPRASTERCAPS_DEPTHBIAS ) && 
          ( pCaps->StencilCaps & D3DSTENCILCAPS_TWOSIDED ) ) )
    return DXTRACE_ERR( "ConfirmDevice", E_FAIL );

	if( FAILED( hr = m_pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, 
        pCaps->DeviceType, adapterFormat, 
        D3DUSAGE_RENDERTARGET | D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
				D3DRTYPE_SURFACE, backBufferFormat ) ) )
		return DXTRACE_ERR( "CheckDeviceFormat", hr );

  return S_OK;
}


//-----------------------------------------------------------------------------
// OneTimeSceneInit()
//-----------------------------------------------------------------------------
HRESULT CFiniteHLSL::OneTimeSceneInit()
{
  m_pFont         = new CD3DFont( _T("Arial"), 10, D3DFONT_BOLD );
  m_pWorldMesh    = new CD3DMesh();
  m_pPlatformMesh = new CD3DMesh();
  m_pObjectMesh   = new CD3DMesh();
  m_pObjectMesh2  = new CD3DMesh();
  m_pShadow       = new CShadow();
  m_pShadow2      = new CShadow();

  if( !m_pFont || !m_pWorldMesh || !m_pPlatformMesh || !m_pObjectMesh || 
      !m_pObjectMesh2 || !m_pShadow || !m_pShadow2 )
    return DXTRACE_ERR( "OneTimeSceneInit", E_OUTOFMEMORY );

  return S_OK;
}


//-----------------------------------------------------------------------------
// InitDeviceObjects()
//-----------------------------------------------------------------------------
HRESULT CFiniteHLSL::InitDeviceObjects()
{
  if ( !( m_d3dCaps.DeviceType & D3DDEVTYPE_HAL ) )
  {
    MessageBox( NULL, 
      _T( "The hardware does not support all the capabilities needed. Try using a graphics card that support DX9.\nNo hardware acceleration will be available.\nDepth biasing MAY NOT BE available." ), 
      _T( "Non-DX9 Compliant Hardware Detected" ), 
      MB_ICONEXCLAMATION|MB_OK );
  }

  if ( m_d3dCaps.StencilCaps & D3DSTENCILCAPS_TWOSIDED )
  {
    m_bTwoSidedStencil = m_bRunTwoSidedStencil = TRUE;
    sprintf( g_szStencilModeMsg, "%s (2-Sided Stencil Mode Supported)", m_strDeviceStats );
  }
  else
  {
    m_bTwoSidedStencil = m_bRunTwoSidedStencil = FALSE;
    sprintf( g_szStencilModeMsg, "%s (2-Sided Stencil Mode NOT Supported)", m_strDeviceStats );
  }

  HRESULT hr = S_OK;

  // Load meshes
  if( FAILED( m_pWorldMesh->Create( m_pd3dDevice, _T("../Media/"), 
        _T("SpaceSphere.x") ) ) )
    return D3DAPPERR_MEDIANOTFOUND;

  if( FAILED( m_pPlatformMesh->Create( m_pd3dDevice, _T("../Media/"), 
        _T("Platform.x") ) ) )
    return D3DAPPERR_MEDIANOTFOUND;

  if( FAILED( m_pObjectMesh->Create( m_pd3dDevice, _T("../Media/"), 
        _T("YK_frame.x") ) ) )
  return D3DAPPERR_MEDIANOTFOUND;

  if( FAILED( m_pObjectMesh2->Create( m_pd3dDevice, _T("../Media/"), 
        _T("YK_seats.x") ) ) )
    return D3DAPPERR_MEDIANOTFOUND;

  // Clones the meshs using our FVF
  m_pObjectMesh->SetFVF( m_pd3dDevice, D3DFVF_MESHVERTEX );
  m_pObjectMesh2->SetFVF( m_pd3dDevice, D3DFVF_MESHVERTEX );

  // Load the effects file
  if ( FAILED( hr = LoadEffectsFile() ) )
    return DXTRACE_ERR( "LoadEffectsFile", hr );

  // Restore the device-dependent objects
  if ( FAILED( m_pFont->InitDeviceObjects( m_pd3dDevice ) ) )
    return E_FAIL;

  // Create a big square for rendering the stencilbuffer contents
  if( FAILED( m_pd3dDevice->CreateVertexBuffer( 4*sizeof( BIGSQUAREVERTEX ), 
        D3DUSAGE_WRITEONLY, D3DFVF_BIGSQUAREVERTEX, D3DPOOL_MANAGED, 
            &m_pBigSquareVB, NULL ) ) )
    return E_FAIL;

  return S_OK;

}


//-----------------------------------------------------------------------------
// RestoreDeviceObjects()
//-----------------------------------------------------------------------------
HRESULT CFiniteHLSL::RestoreDeviceObjects()
{
  if ( FAILED( D3DXCreateSphere( m_pd3dDevice, 0.25f, 20, 20, &m_pSphereMesh, 
        NULL ) ) )
    return E_FAIL;

  	//Set the light material
	memset( &m_LightMaterial, 0, sizeof( D3DMATERIAL9 ) );
	m_LightMaterial.Emissive=D3DXCOLOR( 1.0f, 1.0f, 0.0f, 0.0f );

  m_pWorldMesh->RestoreDeviceObjects( m_pd3dDevice );
  m_pPlatformMesh->RestoreDeviceObjects( m_pd3dDevice );
  m_pObjectMesh->RestoreDeviceObjects( m_pd3dDevice );
  m_pObjectMesh2->RestoreDeviceObjects( m_pd3dDevice );
  m_pFont->RestoreDeviceObjects();

  // Reset effect
  if ( m_pEffect != NULL )
    m_pEffect->OnResetDevice();

  // Recreate shadow object for object 1
  if ( FAILED( m_pShadow->RestoreDeviceObjects( m_pd3dDevice, 
                m_pObjectMesh->GetSysMemMesh() ) ) )
    return E_FAIL;

  // Recreate shadow object for object 2
  if ( FAILED( m_pShadow2->RestoreDeviceObjects( m_pd3dDevice, 
                m_pObjectMesh2->GetSysMemMesh() ) ) )
    return E_FAIL;

  // World matrix
  D3DXMATRIX matScale; // For sphere
  D3DXMatrixScaling( &matScale, 10.0f, 10.0f, 10.0f );
  D3DXMatrixIdentity( &m_matWorld );
  D3DXMatrixMultiply( &m_matWorld, &m_matWorld, &matScale );

  // Platform matrix
  D3DXMatrixScaling( &matScale, 5.0f, 5.0f, 5.0f );
  D3DXMatrixIdentity( &m_matPlatform );
  D3DXMatrixMultiply( &m_matPlatform, &m_matPlatform, &matScale );

  // View matrix
  D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f,  0.0f  );
  D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f,  0.0f  );
  D3DXMatrixLookAtLH( &m_matView, &m_vPosition, &vLookatPt, &vUpVec );

  FLOAT fAspect = (FLOAT) m_d3dsdBackBuffer.Width / 
        (FLOAT) m_d3dsdBackBuffer.Height;
  FLOAT fFOV    = 60.0f * ( D3DX_PI / 180.0f );     // 60 degrees vertical
  FLOAT fNear   = 0.1f;
  FLOAT fFar    = 1000.0f;
  FLOAT h       = 1.0f / (FLOAT)tan( fFOV * 0.5f ); // 1/tan(x) == cot(x)
  FLOAT w       = h / fAspect;
  FLOAT Q       = fFar / ( fFar - fNear );    

  /*
  // Normal Projection matrix ~ left here for reference
  ZeroMemory( &m_matProject, sizeof( D3DXMATRIX ) );
  m_matProject(0, 0) = w;    
  m_matProject(1, 1) = h;    
  m_matProject(2, 2) = Q;    
  m_matProject(3, 2) = -Q*fNear;    
  m_matProject(2, 3) = 1;
  */

  /*
  // Infinite projection matrix, that forces geometries at infinity to render 
  // at 1.0-epsilon depth
  // This matrix is used if the silhouette edges are to be extruded to infinity
  ZeroMemory( &m_matProject, sizeof( D3DXMATRIX ) );
  FLOAT epsilon = 0.0001f;
  m_matProject(0, 0) = w;
  m_matProject(1, 1) = h;
  m_matProject(2, 2) = 1.0f-epsilon;
  m_matProject(3, 2) = fNear*(epsilon-1.0f);
  m_matProject(2, 3) = 1.0f;
  */

  
  // Infinite projection matrix, derived by taking the limits of fFar->infinity
  // This matrix is used if the silhouette edges are extruded by a finite value
  ZeroMemory( &m_matProject, sizeof( D3DXMATRIX ) );
  m_matProject(0, 0) = w;
  m_matProject(1, 1) = h;
  m_matProject(2, 2) = 1.0f;
  m_matProject(3, 2) = -1.0f*fNear;
  m_matProject(2, 3) = 1.0f;
  
  
  m_pd3dDevice->SetTransform( D3DTS_PROJECTION,  &m_matProject );
  m_pd3dDevice->SetTransform( D3DTS_WORLD,       &m_matWorld );
  m_pd3dDevice->SetTransform( D3DTS_VIEW,        &m_matView );

  // Set object matrices
  D3DXMatrixTranslation( &m_matObject, 0.0f, 0.0f, 0.0f );
  D3DXMatrixTranslation( &m_matObject2, 0.0f, 0.0f, 0.0f );

  D3DXMATRIX mat;
  D3DXMatrixScaling( &mat, 0.5f, 0.5f, 0.5f );
  D3DXMatrixTranslation( &m_matLight, m_vLightPos.x, m_vLightPos.y, 
                          m_vLightPos.z );
  D3DXMatrixMultiply( &m_matLight, &mat, &m_matLight );

  m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
  m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

  m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
  m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
  m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, TRUE );

  // Set up ambient light
  m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
  m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,  0x00808080 );

  // Set up point light
  ZeroMemory( &m_Light, sizeof( D3DLIGHT9 ) );
  m_Light.Type         = D3DLIGHT_POINT;
  m_Light.Range        = 100.0f;
  m_Light.Attenuation0 = 0.0f;
  m_Light.Attenuation1 = 1.00f;
  m_Light.Attenuation2 = 0.0f;
  m_Light.Diffuse.r    = 1.0f;
  m_Light.Diffuse.g    = 1.0f;
  m_Light.Diffuse.b    = 1.0f;
  m_Light.Position.x = m_vLightPos.x;
  m_Light.Position.y = m_vLightPos.y;
  m_Light.Position.z = m_vLightPos.z;
  m_pd3dDevice->SetLight( 0, &m_Light );
  m_pd3dDevice->LightEnable( 0, TRUE );

  // Set the size of the big square shadow
  BIGSQUAREVERTEX* v;
  FLOAT sx = (FLOAT)m_d3dsdBackBuffer.Width;
  FLOAT sy = (FLOAT)m_d3dsdBackBuffer.Height;
  m_pBigSquareVB->Lock(  0,  0, (VOID**)&v, 0 );
  v[0].p = D3DXVECTOR4( 0.0f, sy, 0.0f, 1.0f );
  v[1].p = D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );
  v[2].p = D3DXVECTOR4( sx, sy, 0.0f, 1.0f );
  v[3].p = D3DXVECTOR4( sx, 0.0f, 0.0f, 1.0f );
  v[0].color = 0x7f000000;
  v[1].color = 0x7f000000;
  v[2].color = 0x7f000000;
  v[3].color = 0x7f000000;
  m_pBigSquareVB->Unlock();

  return S_OK;

}


//-----------------------------------------------------------------------------
// InvalidateDeviceObjects()
//-----------------------------------------------------------------------------
HRESULT CFiniteHLSL::InvalidateDeviceObjects()
{
  SAFE_RELEASE( m_pSphereMesh );
  m_pWorldMesh->InvalidateDeviceObjects();
  m_pPlatformMesh->InvalidateDeviceObjects();
  m_pObjectMesh->InvalidateDeviceObjects();
  m_pObjectMesh2->InvalidateDeviceObjects();
  m_pFont->InvalidateDeviceObjects();

  m_pShadow->InvalidateDeviceObjects();
  m_pShadow2->InvalidateDeviceObjects();

  // Reset effect
  if ( m_pEffect != NULL )
    m_pEffect->OnLostDevice();

  return S_OK;
}


//-----------------------------------------------------------------------------
// DeleteDeviceObjects()
//-----------------------------------------------------------------------------
HRESULT CFiniteHLSL::DeleteDeviceObjects()
{
  m_pWorldMesh->Destroy();
  m_pPlatformMesh->Destroy();
  m_pObjectMesh->Destroy();
  m_pObjectMesh2->Destroy();
  m_pFont->DeleteDeviceObjects();

  m_pShadow->Destroy();
  m_pShadow2->Destroy();

  SAFE_RELEASE( m_pBigSquareVB );
  SAFE_RELEASE( m_pEffect );
  
  return S_OK;
}


//-----------------------------------------------------------------------------
// Render()
//-----------------------------------------------------------------------------
HRESULT CFiniteHLSL::Render()
{
  // Clear the backbuffer
  m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | 
                        D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0L );
  // Begin the scene
  if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
  {
    // Render the objects in the scene
    RenderScene();

    // Render the shadow volume for stencil counting
    RenderShadowVolume();

    // Draw the shadow by shading pixels using the stencil values as a guide
    DrawShadow();

    // Output text
    char str[128];
    if (m_bKeyInfo)
    {
      sprintf( str, "----------------- Controls -----------------" );
      m_pFont->DrawText( 2, 0, D3DCOLOR_ARGB(255,255,255,0), str );
      sprintf( str, "Forward ------------ W" );
      m_pFont->DrawText( 2, 15, D3DCOLOR_ARGB(255,180,180,0), str );
      sprintf( str, "Backward ---------- S" );
      m_pFont->DrawText( 2, 30, D3DCOLOR_ARGB(255,255,255,0), str );
      sprintf( str, "Slide Left ---------- A" );
      m_pFont->DrawText( 2, 45, D3DCOLOR_ARGB(255,180,180,0), str );
      sprintf( str, "Slide Right -------- D" );
      m_pFont->DrawText( 2, 60, D3DCOLOR_ARGB(255,255,255,0), str );
      sprintf( str, "Slide Up ------------ PgUp" );
      m_pFont->DrawText( 2, 75, D3DCOLOR_ARGB(255,180,180,0), str );
      sprintf( str, "Slide Down ------- PgDn" );
      m_pFont->DrawText( 2, 90, D3DCOLOR_ARGB(255,255,255,0), str );
      sprintf( str, "Turn Left ----------- Left Arrow" );
      m_pFont->DrawText( 2, 105, D3DCOLOR_ARGB(255,180,180,0), str );
      sprintf( str, "Turn Right --------- Right Arrow" );
      m_pFont->DrawText( 2, 120, D3DCOLOR_ARGB(255,255,255,0), str );
      sprintf( str, "Pitch Up ------------ Up Arrow" );
      m_pFont->DrawText( 2, 135, D3DCOLOR_ARGB(255,180,180,0), str );
      sprintf( str, "Pitch Dn ------------ Down Arrow" );
      m_pFont->DrawText( 2, 150, D3DCOLOR_ARGB(255,255,255,0), str );

      sprintf( str, "Cycle mode ------- M" );
      m_pFont->DrawText( 2, 195, D3DCOLOR_ARGB(255,180,180,0), str );
      sprintf( str, "Show Volume --- N" );
      m_pFont->DrawText( 2, 210, D3DCOLOR_ARGB(255,255,255,0), str );
      sprintf( str, "Toggle Obj -------- O" );
      m_pFont->DrawText( 2, 225, D3DCOLOR_ARGB(255,180,180,0), str );
      sprintf( str, "Pause ---------------- Enter" );
      m_pFont->DrawText( 2, 240, D3DCOLOR_ARGB(255,255,255,0), str );
      sprintf( str, "Step ------------------ SpaceBar" );
      m_pFont->DrawText( 2, 255, D3DCOLOR_ARGB(255,180,180,0), str );
      sprintf( str, "Animate Light --- L" );
      m_pFont->DrawText( 2, 270, D3DCOLOR_ARGB(255,255,255,0), str );
      sprintf( str, "Stencil Mode ---- K" );
      m_pFont->DrawText( 2, 285, D3DCOLOR_ARGB(255,180,180,0), str );

      sprintf( str, "Press F1 to toggle help.", m_strDeviceStats );
      m_pFont->DrawText( 2, 320, D3DCOLOR_ARGB( 255, 255, 128, 128 ), str );
    }
    else
    {
      m_pFont->DrawText( 2, 0, D3DCOLOR_ARGB( 255,255,255,0 ), g_szStencilModeMsg );

      if ( m_bRunTwoSidedStencil )
        sprintf( str, "%s (Running Two-Side Stencil Mode)", m_strFrameStats );
      else
        sprintf( str, "%s (Running Normal Stencil Mode)", m_strFrameStats );

      m_pFont->DrawText( 2, 15, D3DCOLOR_ARGB( 255,255,255,0 ), str );
      sprintf( str, "Camera        : %f, %f, %f", m_vPosition.x, 
            m_vPosition.y, m_vPosition.z );
      m_pFont->DrawText( 2, 30, D3DCOLOR_ARGB( 255, 255, 255, 0 ), str );
      sprintf( str, "Press F1 to toggle help.", m_strDeviceStats );
      m_pFont->DrawText( 2, 45, D3DCOLOR_ARGB( 255, 255, 128, 128 ), str );
    }

    // End the scene.
    m_pd3dDevice->EndScene();
  }

  return S_OK;
}


//-----------------------------------------------------------------------------
// RenderScene()
//-----------------------------------------------------------------------------
HRESULT CFiniteHLSL::RenderScene()
{
  if ( m_bShowObjects )
  {
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matObject );
    m_pObjectMesh->Render( m_pd3dDevice );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matObject2 );
    m_pObjectMesh2->Render( m_pd3dDevice );
  }

  m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
  m_pWorldMesh->Render( m_pd3dDevice );

  m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matPlatform );
  m_pPlatformMesh->Render( m_pd3dDevice );

  m_pd3dDevice->SetMaterial( &m_LightMaterial );
  m_pd3dDevice->SetTexture( 0, NULL );
  m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matLight );
  m_pSphereMesh->DrawSubset( 0 );

  return S_OK;
}


//-----------------------------------------------------------------------------
// RenderShadowVolume()
// This function is driven by the techniques defined in the effects file 
// HLSL_ShadowVolume.fx. There are 2 techniques, one implements the old 1-sided
// stencil mode while the other implements the new 2-sided stencil mode. For
// 2-sided stencil mode, a DX9 compliant device is required.
//-----------------------------------------------------------------------------
HRESULT CFiniteHLSL::RenderShadowVolume()
{
  // Show shadow volume?
  if ( m_bShowShadowVol )
  {
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTALPHA  );
  }
  else
    m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );


  UINT nPasses;
  UINT iPass;
  D3DXHANDLE hTechnique = NULL;

  if( m_pEffect != NULL )
  {
    // Select between 1-sided and 2-sided stencil mode
    if ( m_bRunTwoSidedStencil )
      hTechnique = m_pEffect->GetTechniqueByName( "T1" );
    else
      hTechnique = m_pEffect->GetTechniqueByName( "T0" );

    m_pEffect->SetTechnique( hTechnique );
    m_pEffect->Begin( &nPasses, 0 );

    D3DXMATRIX  mClip, mInvObj;
    ZeroMemory( &mClip, sizeof( D3DXMATRIX ) );
    ZeroMemory( &mInvObj, sizeof( D3DXMATRIX ) );
    D3DXVECTOR4 currLightPos = D3DXVECTOR4( m_Light.Position.x, 
                                            m_Light.Position.y, 
                                            m_Light.Position.z, 
                                            1.0f );

    for( iPass = 0; iPass < nPasses; iPass ++ )
    {
      m_pEffect->Pass( iPass );

      // Shadow volume of object 1
      mClip = m_matObject * m_matView * m_matProject;
      D3DXMatrixInverse( &mInvObj, NULL, &m_matObject );
      D3DXVec4Transform( &m_vOLP, &currLightPos, &mInvObj );
      m_pEffect->SetMatrix( m_hmClip, &mClip );
      m_pEffect->SetVector( m_hvOLP, &m_vOLP );
      m_pShadow->RenderShadow();

      // Shadow volume of object 2
      mClip = m_matObject2 * m_matView * m_matProject;
      D3DXMatrixInverse( &mInvObj, NULL, &m_matObject2 );
      D3DXVec4Transform( &m_vOLP, &currLightPos, &mInvObj );
      m_pEffect->SetMatrix( m_hmClip, &mClip );
      m_pEffect->SetVector( m_hvOLP, &m_vOLP );
      m_pShadow2->RenderShadow();
    }

    m_pEffect->End();
  }

  m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0x0000000F ); 
  m_pd3dDevice->SetRenderState( D3DRS_SHADEMODE,        D3DSHADE_GOURAUD );
  m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,     TRUE );
  m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
  m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

  return S_OK;
}


//-----------------------------------------------------------------------------
// DrawShadow()
// Draws a big quad while testing the stencil buffer for the appropiate pixels
// to shade. 
//-----------------------------------------------------------------------------
HRESULT CFiniteHLSL::DrawShadow()
{
  // Disable z-buffering, enable stencil, enable alphablending, rule the world
  m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          FALSE );
  m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    TRUE );
  m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
  m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
  m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

  m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
  m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
  m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
  m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
  m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
  m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

  // Only write where stencil val >= 1 
  m_pd3dDevice->SetRenderState( D3DRS_STENCILREF,  0x1 );
  m_pd3dDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL );
  m_pd3dDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );

  // Draw a big quad
  m_pd3dDevice->SetVertexShader( NULL );
  m_pd3dDevice->SetFVF( D3DFVF_BIGSQUAREVERTEX );
  m_pd3dDevice->SetStreamSource( 0, m_pBigSquareVB, 0, sizeof(BIGSQUAREVERTEX) );
  m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

  // Restore render states
  m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
  m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
  m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

  return S_OK;
}


//-----------------------------------------------------------------------------
// FrameMove()
//-----------------------------------------------------------------------------
HRESULT CFiniteHLSL::FrameMove()
{
  D3DXMATRIX mat; // For sphere

  // Animate world
  D3DXMatrixIdentity( &m_matWorld );
  D3DXMatrixRotationY( &mat, 0.01f * m_fTime );
  D3DXMatrixMultiply( &m_matWorld, &m_matWorld, &mat );
  D3DXMatrixScaling( &mat, 10.0f, 10.0f, 10.0f );
  D3DXMatrixMultiply( &m_matWorld, &m_matWorld, &mat );

  // Animate the occluders
  D3DXMatrixIdentity( &m_matObject );
  D3DXMatrixScaling( &mat, 0.05f, 0.05f, 0.05f );
  D3DXMatrixMultiply( &m_matObject, &m_matObject, &mat );
  D3DXMatrixTranslation ( &mat, 0.0f, 0.0f, 0.0f );
  D3DXMatrixMultiply( &m_matObject, &m_matObject, &mat );

  D3DXVECTOR3 vPivot = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
  D3DXMatrixIdentity( &m_matObject2 );
  D3DXMatrixScaling( &mat, 0.05f, 0.05f, 0.05f );
  D3DXMatrixMultiply( &m_matObject2, &m_matObject2, &mat );
  D3DXMatrixRotationAxis( &mat, &vPivot, (float)sin( m_fTime ) );
  D3DXMatrixMultiply( &m_matObject2, &m_matObject2, &mat );
  D3DXMatrixTranslation ( &mat, 0.0f, 4.24f, 0.0f );
  D3DXMatrixMultiply( &m_matObject2, &m_matObject2, &mat );

  // Animate the light
  if ( m_bAnimateLight )
  {
    D3DXMatrixIdentity( &m_matLight );
    D3DXMatrixTranslation ( &mat, m_vLightPos.x, m_vLightPos.y, 
          m_vLightPos.z );
    D3DXMatrixMultiply( &m_matLight, &m_matLight, &mat );
    D3DXMatrixRotationY( &mat, -2.1f * m_fTime );
    D3DXMatrixMultiply( &m_matLight, &m_matLight, &mat );

    // Set light position
    m_Light.Position.x = m_matLight._41;
    m_Light.Position.y = m_matLight._42;
    m_Light.Position.z = m_matLight._43;
    m_pd3dDevice->SetLight( 0, &m_Light );
  }

  // Camera movements
  FLOAT fDist         = m_fSpeed * m_fElapsedTime;
  FLOAT fAngularDist  = m_fAngularSpeed * m_fElapsedTime;

  // De-accelerate the camera movement (for smooth motion)
  m_vDist     *= 0.9f;
  m_vYPR_Dist *= 0.9f;

  // Process keyboard input
  if( m_bKey['W'] )       m_vDist.z      += fDist;        // Move Forward
  if( m_bKey['S'] )       m_vDist.z      -= fDist;        // Move Backward
  if( m_bKey['D'] )       m_vDist.x      += fDist;        // Slide Right
  if( m_bKey['A'] )       m_vDist.x      -= fDist;        // Slide Left
  if( m_bKey[VK_PRIOR] )  m_vDist.y      += fDist;        // Slide Up
  if( m_bKey[VK_NEXT] )   m_vDist.y      -= fDist;        // Slide Down

  if( m_bKey[VK_RIGHT] )  m_vYPR_Dist.x  += fAngularDist; // Turn Right
  if( m_bKey[VK_LEFT] )   m_vYPR_Dist.x  -= fAngularDist; // Turn Left
  if( m_bKey[VK_DOWN] )   m_vYPR_Dist.y  += fAngularDist; // Turn Down
  if( m_bKey[VK_UP] )     m_vYPR_Dist.y  -= fAngularDist; // Turn Up

  // Update the position vector
  D3DXVECTOR3 vT = m_vDist;
  D3DXVec3TransformNormal( &vT, &vT, &m_matOrientation );
  m_vPosition += vT;

  // Update the yaw-pitch-rotation vector
  m_vYPR.x   += m_vYPR_Dist.x;
  m_vYPR.y   += m_vYPR_Dist.y;

  // Set the view matrix
  D3DXQUATERNION qR;
  D3DXQuaternionRotationYawPitchRoll( &qR, m_vYPR.x, m_vYPR.y, 0.0f );
  D3DXMatrixAffineTransformation( &m_matOrientation, 1.25f, NULL, &qR, &m_vPosition );
  D3DXMatrixInverse( &m_matView, NULL, &m_matOrientation );

  m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );

  return S_OK;
}


//-----------------------------------------------------------------------------
// FinalCleanup()
//-----------------------------------------------------------------------------
HRESULT CFiniteHLSL::FinalCleanup()
{
  SAFE_DELETE( m_pWorldMesh );
  SAFE_DELETE( m_pPlatformMesh );
  SAFE_DELETE( m_pObjectMesh );
  SAFE_DELETE( m_pObjectMesh2 );
  SAFE_DELETE( m_pShadow );
  SAFE_DELETE( m_pShadow2 );
  SAFE_DELETE( m_pFont );

  return S_OK;
}


//-----------------------------------------------------------------------------
// LoadEffectsFile()
// Loads the .fx file and create the techniques coded
//-----------------------------------------------------------------------------
HRESULT CFiniteHLSL::LoadEffectsFile()
{
  HRESULT hr = S_OK;
  if( FAILED( hr = D3DXCreateEffectFromFile( m_pd3dDevice, 
              "./Effects/HLSL_ShadowVolume.fx", NULL, NULL, 
              0, NULL, &m_pEffect, NULL  ) ) )
    return DXTRACE_ERR( "D3DXCreateEffectFromFile", hr );

  // Get the handle for the effects. Its faster than passing in string later.
  D3DXEFFECT_DESC m_EffectDesc;
  D3DXPARAMETER_DESC ParamDesc;
  D3DXHANDLE hParam;
  m_pEffect->GetDesc( &m_EffectDesc );
  for( UINT iParam = 0; iParam < m_EffectDesc.Parameters; iParam++ )
  {
    hParam = m_pEffect->GetParameter ( NULL, iParam );
    m_pEffect->GetParameterDesc( hParam, &ParamDesc );

    if ( strcmpi( ParamDesc.Name, "mClip" ) == 0 )
      m_hmClip = hParam;
    else
    if ( strcmpi( ParamDesc.Name, "vObjLightPos" ) == 0 )
      m_hvOLP = hParam;
  }

  return hr;
}


//-----------------------------------------------------------------------------
// MsgProc()
//-----------------------------------------------------------------------------
LRESULT CFiniteHLSL::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
  if ( WM_KEYDOWN == uMsg )
  {
    m_bKey[wParam] = 1;
  }
  if ( WM_KEYUP == uMsg )
  {
    m_bKey[wParam] = 0;
    if ( wParam == VK_F1 )
      m_bKeyInfo = !m_bKeyInfo;
    if ( wParam == 'N' )
      m_bShowShadowVol = !m_bShowShadowVol;
    if ( wParam == 'O' )
      m_bShowObjects            = !m_bShowObjects;
    if ( wParam == 'L' )
      m_bAnimateLight           = !m_bAnimateLight;
    if ( ( wParam == 'K' ) && m_bTwoSidedStencil )
        m_bRunTwoSidedStencil   = !m_bRunTwoSidedStencil;
    if ( wParam == 'M' )
    {
      switch ( FillMode )
      {
      case SHADE_MODE:
        m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
        FillMode = WIRE_MODE;
        break;
      case WIRE_MODE:
        m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_POINT );
        FillMode = POINT_MODE;
        break;
      case POINT_MODE:
        m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
        FillMode = SHADE_MODE;
      }
    }
  }

  // Pass remaining messages to default handler
  return CD3DApplication::MsgProc( hWnd, uMsg, wParam, lParam );
}

