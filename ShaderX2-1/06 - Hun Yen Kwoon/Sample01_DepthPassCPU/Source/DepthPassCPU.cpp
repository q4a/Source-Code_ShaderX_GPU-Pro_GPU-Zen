#include <DepthPassCPU.h>

D3DXVECTOR3 g_vecUp( 0, 1, 0);
D3DXVECTOR3 g_vecAt( 0, 0, 0);


//-----------------------------------------------------------------------------
// WinMain()
// Entry to program
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
  CDepthPass d3dApp;

  if( FAILED( d3dApp.Create( hInst ) ) )
    return 0;

  return d3dApp.Run();
}


//-----------------------------------------------------------------------------
// CDepthPass()
// Constructor of our d3dapp object
//-----------------------------------------------------------------------------
CDepthPass::CDepthPass()
{
  m_strWindowTitle          = _T( "DepthPassCPU Sample" );
  m_bUseDepthBuffer         = TRUE;
  m_dwMinDepthBits          = 16;
  m_dwMinStencilBits        = 4;

  m_bKeyInfo                = FALSE;
  m_eFillMode               = SHADE_MODE;

  m_pFont                   = NULL;
  m_pWorldMesh              = NULL;
  m_pPlatformMesh           = NULL;
  m_pSphereMesh             = NULL;
  m_pObjectMesh             = NULL;
  m_pShadowObjectMesh       = NULL;
  m_pShadowObjectMesh2      = NULL;
  m_pBigSquareVB            = NULL;
  m_pShadow                 = NULL;
  m_pShadow2                = NULL;

  m_vLightPos               = D3DXVECTOR3( 3.0f, 7.0f,  3.0f );

  m_vPosition               = D3DXVECTOR3( 0.0f, 4.0f,-10.0f );
  m_vDist                   = D3DXVECTOR3( 0.0f, 0.0f,  0.0f );
  m_vYPR                    = D3DXVECTOR3( 0.0f, 0.0f,  0.0f );
	m_vYPR_Dist               = D3DXVECTOR3( 0.0f, 0.0f,  0.0f );
  m_fAngularSpeed           = 0.1f;
  m_fSpeed                  = 0.6f;

  m_bShowShadowVolFrontFace = FALSE;
  m_bShowShadowVolBackFace  = FALSE;
  m_bShowObjects            = TRUE;
  m_bAnimateLight           = FALSE;
  
  m_dwCreationWidth         = 640;
  m_dwCreationHeight        = 480;

  ZeroMemory(m_bKey, 256);
}


//-----------------------------------------------------------------------------
// ConfirmDevice()
// Checks the capabilities of the device
//-----------------------------------------------------------------------------
HRESULT CDepthPass::ConfirmDevice( D3DCAPS8* pCaps, DWORD dwBehavior, 
                                      D3DFORMAT Format )
{
  if ( ( dwBehavior&D3DCREATE_HARDWARE_VERTEXPROCESSING ) || 
        ( dwBehavior&D3DCREATE_MIXED_VERTEXPROCESSING ) )
  {
    if ( 0==( pCaps->VertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS ) )
      return E_FAIL;
  }

  if ( !( pCaps->RasterCaps & D3DPRASTERCAPS_ZBIAS ) )
    return E_FAIL;

  return S_OK;
}


//-----------------------------------------------------------------------------
// OneTimeSceneInit()
// Called once during startup to initialize objects
//-----------------------------------------------------------------------------
HRESULT CDepthPass::OneTimeSceneInit()
{
  m_pFont               = new CD3DFont( _T( "Arial "), 10, D3DFONT_BOLD );
  m_pWorldMesh          = new CD3DMesh();
  m_pPlatformMesh       = new CD3DMesh();
  m_pObjectMesh         = new CD3DMesh();
  m_pObjectMesh2        = new CD3DMesh();
  m_pShadowObjectMesh   = new CD3DMesh();       // Welded meshes for shadow vol
  m_pShadowObjectMesh2  = new CD3DMesh();       // Welded meshes for shadow vol
  m_pShadow             = new CShadow( 2000 );  // Amount of memory needed
  m_pShadow2            = new CShadow( 9000 );  // Amount of memory needed

  if( !m_pFont || !m_pWorldMesh || !m_pPlatformMesh || !m_pObjectMesh || 
      !m_pObjectMesh2 || !m_pShadow || !m_pShadow2 || 
      !m_pShadowObjectMesh || !m_pShadowObjectMesh2 )
    return E_OUTOFMEMORY;

  return S_OK;
}


//-----------------------------------------------------------------------------
// InitDeviceObjects()
// Device dependednt object are initialized here
//-----------------------------------------------------------------------------
HRESULT CDepthPass::InitDeviceObjects()
{
  // Load meshes
  if( FAILED( m_pWorldMesh->Create( m_pd3dDevice, _T("../Media/"), 
        _T("SpaceSphere.x"), FALSE ) ) )
    return D3DAPPERR_MEDIANOTFOUND;

  if( FAILED( m_pPlatformMesh->Create( m_pd3dDevice, _T("../Media/"), 
        _T("Platform.x"), FALSE ) ) )
    return D3DAPPERR_MEDIANOTFOUND;

  if( FAILED( m_pObjectMesh->Create( m_pd3dDevice, _T("../Media/"), 
        _T("YK_frame.x"), FALSE ) ) )
  return D3DAPPERR_MEDIANOTFOUND;

  if( FAILED( m_pObjectMesh2->Create( m_pd3dDevice, _T("../Media/"), 
        _T("YK_seats.x"), FALSE ) ) )
    return D3DAPPERR_MEDIANOTFOUND;

  if( FAILED( m_pShadowObjectMesh->Create( m_pd3dDevice, _T( "../Media/" ), 
        _T( "YK_frame_weld.x" ), FALSE ) ) )
    return D3DAPPERR_MEDIANOTFOUND;

  if( FAILED( m_pShadowObjectMesh2->Create( m_pd3dDevice, _T( "../Media/" ), 
        _T( "YK_seats_weld.x" ), FALSE ) ) )
    return D3DAPPERR_MEDIANOTFOUND;

  // Clones the meshs using our FVF
  m_pShadowObjectMesh->SetFVF( m_pd3dDevice, D3DFVF_MESHVERTEX );
  m_pShadowObjectMesh2->SetFVF( m_pd3dDevice, D3DFVF_MESHVERTEX );

  // Restore the device-dependent objects
  m_pFont->InitDeviceObjects( m_pd3dDevice );

  // Create a big square for rendering the stencilbuffer contents
  if( FAILED( m_pd3dDevice->CreateVertexBuffer( 4*sizeof(BIGSQUAREVERTEX), 
        D3DUSAGE_WRITEONLY, D3DFVF_BIGSQUAREVERTEX, D3DPOOL_MANAGED, 
            &m_pBigSquareVB ) ) )
    return E_FAIL;

  return S_OK;
}


//-----------------------------------------------------------------------------
// RestoreDeviceObjects()
// Restores device dependent obejcts
//-----------------------------------------------------------------------------
HRESULT CDepthPass::RestoreDeviceObjects()
{
  if ( FAILED( D3DXCreateSphere( m_pd3dDevice, 0.25f, 20, 20, &m_pSphereMesh, 
        NULL ) ) )
    return E_FAIL;

  	//Set the light material
	memset( &m_LightMaterial, 0, sizeof( D3DMATERIAL8 ) );
	m_LightMaterial.Emissive=D3DXCOLOR( 1.0f, 1.0f, 0.0f, 0.0f );

  // Set shadow volume front face material ~ use only for showing shadow volume
  memset( &m_ShadowVolFrontFaceMaterial, 0, sizeof( D3DMATERIAL8 ) );
  m_ShadowVolFrontFaceMaterial.Ambient = D3DXCOLOR( 0.0f, 0.4f, 0.4f, 0.0f );

  // Set shadow volume back face material ~ use only for showing shadow volume
  memset( &m_ShadowVolBackFaceMaterial, 0, sizeof( D3DMATERIAL8 ) );
  m_ShadowVolBackFaceMaterial.Ambient = D3DXCOLOR( 0.4f, 0.4f, 0.0f, 0.0f );

  m_pWorldMesh->RestoreDeviceObjects( m_pd3dDevice );
  m_pPlatformMesh->RestoreDeviceObjects( m_pd3dDevice );
  m_pObjectMesh->RestoreDeviceObjects( m_pd3dDevice );
  m_pObjectMesh2->RestoreDeviceObjects( m_pd3dDevice );
  m_pShadowObjectMesh->RestoreDeviceObjects( m_pd3dDevice );
  m_pShadowObjectMesh2->RestoreDeviceObjects( m_pd3dDevice );
  m_pFont->RestoreDeviceObjects();

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

  // Normal projection matrix
  ZeroMemory(&m_matProject, sizeof(D3DXMATRIX));    
  m_matProject(0, 0) = w;    
  m_matProject(1, 1) = h;    
  m_matProject(2, 2) = Q;    
  m_matProject(3, 2) = -Q*fNear;    
  m_matProject(2, 3) = 1;

  m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
  m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );
  m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProject );

  // Set object matrices
  D3DXMatrixTranslation(&m_matObject, 0.0f, 0.0f, 0.0f);
  D3DXMatrixTranslation(&m_matObject2, 0.0f, 0.0f, 0.0f);

  D3DXMATRIX mat;
  D3DXMatrixScaling(&mat, 0.5f, 0.5f, 0.5f);
  D3DXMatrixTranslation( &m_matLight, m_vLightPos.x, m_vLightPos.y, 
                          m_vLightPos.z );
  D3DXMatrixMultiply( &m_matLight, &mat, &m_matLight );

  m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
  m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );

  m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
  m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
  m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, TRUE );

  // Set up ambient light
  m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
  m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,  0x00808080 );

  // Set up point light
  ZeroMemory( &m_Light, sizeof( D3DLIGHT8 ) );
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
  m_pBigSquareVB->Lock( 0, 0, (BYTE**)&v, 0 );
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
// Invalidates or releases device dependent objects
//-----------------------------------------------------------------------------
HRESULT CDepthPass::InvalidateDeviceObjects()
{
  SAFE_RELEASE( m_pSphereMesh );
  m_pWorldMesh->InvalidateDeviceObjects();
  m_pPlatformMesh->InvalidateDeviceObjects();
  m_pObjectMesh->InvalidateDeviceObjects();
  m_pObjectMesh2->InvalidateDeviceObjects();
  m_pShadowObjectMesh->InvalidateDeviceObjects();
  m_pShadowObjectMesh2->InvalidateDeviceObjects();
  m_pFont->InvalidateDeviceObjects();

  return S_OK;
}


//-----------------------------------------------------------------------------
// DeleteDeviceObjects()
// Destroys and delete device dependent objects
//-----------------------------------------------------------------------------
HRESULT CDepthPass::DeleteDeviceObjects()
{
  m_pWorldMesh->Destroy();
  m_pPlatformMesh->Destroy();
  m_pObjectMesh->Destroy();
  m_pObjectMesh2->Destroy();
  m_pShadowObjectMesh->Destroy();
  m_pShadowObjectMesh2->Destroy();
  m_pFont->DeleteDeviceObjects();

  SAFE_RELEASE( m_pBigSquareVB );
  
  return S_OK;
}


//-----------------------------------------------------------------------------
// FrameMove()
// Called every frame
//-----------------------------------------------------------------------------
HRESULT CDepthPass::FrameMove()
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


  // Transform the light vector to object space before building shadow volume
  D3DXVECTOR3 vLight;
  D3DXMATRIX m;
  D3DXMatrixInverse( &m, NULL, &m_matObject );
  vLight.x = m_matLight._41 * m._11 + m_matLight._42 * m._21 + m_matLight._43 
        * m._31 + m._41;
  vLight.y = m_matLight._41 * m._12 + m_matLight._42 * m._22 + m_matLight._43 
        * m._32 + m._42;
  vLight.z = m_matLight._41 * m._13 + m_matLight._42 * m._23 + m_matLight._43 
        * m._33 + m._43;

  // Build the shadow volume
  m_pShadow->Reset();
  m_pShadow->BuildShadowVolume( m_pShadowObjectMesh->GetSysMemMesh(), vLight );

  // Transform the light vector to object space before building shadow volume
  D3DXMatrixInverse( &m, NULL, &m_matObject2 );
  vLight.x = m_matLight._41 * m._11 + m_matLight._42 * m._21 + m_matLight._43 
        * m._31 + m._41;
  vLight.y = m_matLight._41 * m._12 + m_matLight._42 * m._22 + m_matLight._43 
        * m._32 + m._42;
  vLight.z = m_matLight._41 * m._13 + m_matLight._42 * m._23 + m_matLight._43 
        * m._33 + m._43;

  // Build the shadow volume
  m_pShadow2->Reset();
  m_pShadow2->BuildShadowVolume( m_pShadowObjectMesh2->GetSysMemMesh(), 
        vLight );

  return S_OK;
}


//-----------------------------------------------------------------------------
// Render()
// Renders the scene and shadow volume through different function calls
//-----------------------------------------------------------------------------
HRESULT CDepthPass::Render()
{
  // Clear the backbuffer
  m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | 
        D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0L );
  // Begin the scene
  if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
  {
    // Render the scene
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
      sprintf( str, "Front Volume ---- N" );
      m_pFont->DrawText( 2, 210, D3DCOLOR_ARGB(255,255,255,0), str );
      sprintf( str, "Back Volume ----- B" );
      m_pFont->DrawText( 2, 225, D3DCOLOR_ARGB(255,180,180,0), str );
      sprintf( str, "Toggle Obj -------- O" );
      m_pFont->DrawText( 2, 240, D3DCOLOR_ARGB(255,255,255,0), str );
      sprintf( str, "Pause ---------------- Enter" );
      m_pFont->DrawText( 2, 255, D3DCOLOR_ARGB(255,180,180,0), str );
      sprintf( str, "Step ------------------ SpaceBar" );
      m_pFont->DrawText( 2, 270, D3DCOLOR_ARGB(255,255,255,0), str );
      sprintf( str, "Animate Light --- L" );
      m_pFont->DrawText( 2, 285, D3DCOLOR_ARGB(255,180,180,0), str );

      sprintf( str, "Press F1 to toggle help.", m_strDeviceStats );
      m_pFont->DrawText( 2, 315, D3DCOLOR_ARGB( 255, 255, 128, 128 ), str );
    }
    else
    {
      sprintf( str, "%s", m_strDeviceStats );
      m_pFont->DrawText( 2, 0, D3DCOLOR_ARGB( 255,255,255,0 ), str );
      m_pFont->DrawText( 2, 15, D3DCOLOR_ARGB( 255,255,255,0 ), 
            m_strFrameStats );
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
// Renders the object meshes in the scene
//-----------------------------------------------------------------------------
HRESULT CDepthPass::RenderScene()
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
// FinalCleanup()
// Called once before program exit to cleanup
//-----------------------------------------------------------------------------
HRESULT CDepthPass::FinalCleanup()
{
  SAFE_DELETE( m_pWorldMesh );
  SAFE_DELETE( m_pPlatformMesh );
  SAFE_DELETE( m_pObjectMesh );
  SAFE_DELETE( m_pObjectMesh2 );
  SAFE_DELETE( m_pShadowObjectMesh );
  SAFE_DELETE( m_pShadowObjectMesh2 );
  SAFE_DELETE( m_pFont );
  SAFE_DELETE( m_pShadow );
  SAFE_DELETE( m_pShadow2 );
  return S_OK;
}


//-----------------------------------------------------------------------------
// RenderShadowVolume()
// Sets up the stencil operations for the depth-pass algorithm
// The front facing geometries of the shadow volume are first rendered with the
// stencil operations set to increment the stencil values with depth pass.
// Then the back facing geometries are rendered with the stencil operations set
// to decrement the stencil values with depth pass.
//-----------------------------------------------------------------------------
HRESULT CDepthPass::RenderShadowVolume()
{
  // Disable z-buffer writes (note: z-testing still occurs), and enable stencil
  // buffering
  m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
  m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );

  // Dont bother with interpolating color
  m_pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );

  // Set up stencil compare fuction, reference value, and masks.
  // Stencil test passes if ((ref & mask) cmpfn (stencil & mask)) is true.
  // Note: since we set up the stencil-test to always pass, the STENCILFAIL 
  // renderstate is really not needed.
  m_pd3dDevice->SetRenderState( D3DRS_STENCILFUNC,  D3DCMP_ALWAYS );
  m_pd3dDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
  m_pd3dDevice->SetRenderState( D3DRS_STENCILFAIL,  D3DSTENCILOP_KEEP );

  // Sets up stencil reference value and stencil masks
  m_pd3dDevice->SetRenderState( D3DRS_STENCILREF, 0x1 );
  m_pd3dDevice->SetRenderState( D3DRS_STENCILMASK, 0xffffffff );
  m_pd3dDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );

  // Increment stencil buffer value if depth test passes
  m_pd3dDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_INCR );

  // Show shadow volume front faces?
  if ( m_bShowShadowVolFrontFace )
  {
    m_pd3dDevice->SetMaterial( &m_ShadowVolFrontFaceMaterial );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,         D3DBLEND_SRCCOLOR );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,        D3DBLEND_DESTALPHA );
  }
  else
    m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );

  // Draw front-side of shadow volume in stencil/z only
  m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matObject);
  m_pShadow->RenderShadowVolume( m_pd3dDevice );
  m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matObject2);
  m_pShadow2->RenderShadowVolume( m_pd3dDevice );

  // Now reverse cull order so back sides of shadow volume are written.
  m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );

  // Decrement stencil buffer value if depth test passes
  m_pd3dDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_DECR );

  // Show shadow volume back faces?
  if ( m_bShowShadowVolBackFace )
  {
    m_pd3dDevice->SetMaterial( &m_ShadowVolBackFaceMaterial );
    m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0x0000000F );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTALPHA  );
  }
  else
    m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );

  // Draw back-side of shadow volume in stencil/z only
  m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matObject);
  m_pShadow->RenderShadowVolume( m_pd3dDevice );
  m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matObject2);
  m_pShadow2->RenderShadowVolume( m_pd3dDevice );

  // Restore render states
  m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0x0000000F ); 
  m_pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
  m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
  m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
  m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );
  m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

  return S_OK;
}


//-----------------------------------------------------------------------------
// DrawShadow()
// Draws a big quad while testing the stencil buffer for the appropiate pixels
// to shade. 
//-----------------------------------------------------------------------------
HRESULT CDepthPass::DrawShadow()
{
  // Set renderstates: disable z-buffering, enable stencil, and turn on 
  // alphablending
  m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
  m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );
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

  // Draw a big, gray square
  m_pd3dDevice->SetVertexShader( D3DFVF_BIGSQUAREVERTEX );
  m_pd3dDevice->SetStreamSource( 0, m_pBigSquareVB, sizeof(BIGSQUAREVERTEX) );
  m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

  // Restore render states
  m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
  m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
  m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

  return S_OK;
}


//-----------------------------------------------------------------------------
// MsgProc()
// Handle windows messages 
//-----------------------------------------------------------------------------
LRESULT CDepthPass::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
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
      m_bShowShadowVolFrontFace = !m_bShowShadowVolFrontFace;
    if ( wParam == 'B' )
      m_bShowShadowVolBackFace  = !m_bShowShadowVolBackFace;
    if ( wParam == 'O' )
      m_bShowObjects            = !m_bShowObjects;
    if ( wParam == 'L' )
      m_bAnimateLight           = !m_bAnimateLight;
    if ( wParam == 'M' )
    {
      switch ( m_eFillMode )
      {
      case SHADE_MODE:
        m_eFillMode = WIRE_MODE;
        m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
        break;
      case WIRE_MODE:
        m_eFillMode = POINT_MODE;
        m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_POINT );
        break;
      case POINT_MODE:
        m_eFillMode = SHADE_MODE;
        m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
      }
    }
  }

  // Pass remaining messages to default handler
  return CD3DApplication::MsgProc( hWnd, uMsg, wParam, lParam );
}
