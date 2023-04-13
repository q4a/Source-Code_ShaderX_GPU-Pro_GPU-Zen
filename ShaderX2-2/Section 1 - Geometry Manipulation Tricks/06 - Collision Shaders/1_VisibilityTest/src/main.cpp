//-----------------------------------------------------------------------------
// File: main.cpp
//
// Desc: DirectX window application created by the DirectX AppWizard
//-----------------------------------------------------------------------------
#define STRICT
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <basetsd.h>
#include <math.h>
#include <stdio.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <tchar.h>
#include "DXUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DFile.h"
#include "D3DUtil.h"
#include "resource.h"
#include "main.h"


#define MAP_SIZE   512

//-----------------------------------------------------------------------------
// Global access to the app (needed for the global WndProc())
//-----------------------------------------------------------------------------
CMyD3DApplication* g_pApp  = NULL;
HINSTANCE          g_hInst = NULL;




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    CMyD3DApplication d3dApp;

    g_pApp  = &d3dApp;
    g_hInst = hInst;

    InitCommonControls();
    if( FAILED( d3dApp.Create( hInst ) ) )
        return 0;

    return d3dApp.Run();
}


// ----------------------------------------------------------------------------
int CBullet::Create(D3DXVECTOR4 &x, D3DXVECTOR4 &v){
	if(BULLET_STATE_DEAD!=iState) return -1;
	iReq = BULLET_STATE_FLY;
	pos = x;
	pos.w=0.0f;
	velosity = v;
	velosity.w = 0;
	return 0;
}

// ----------------------------------------------------------------------------
int CBullet::Update(FLOAT dt, CSmoke *pSmoke)
{
	int state = iState;
	switch(iState){
	case BULLET_STATE_DEAD:
		if(BULLET_STATE_FLY==iReq){
			iState = BULLET_STATE_FLY;
		}
		break;
	case BULLET_STATE_FLY:
		pos += velosity*(float)dt;
		pos.w=0.0f;
		// Dead check
		if(pos.x < -15.0f || 15.0f < pos.x
		|| pos.y < - 5.0f || 15.0f < pos.y
		|| pos.z < -15.0f || 15.0f < pos.z
		|| BULLET_STATE_BURN == iReq
		){
			iState = BULLET_STATE_BURN;
			fLastCnt = 0;
		}
		break;
	case BULLET_STATE_BURN:
	{
		if(fLastCnt+0.05f < fCnt){
			fLastCnt = fCnt;
			D3DXVECTOR3 vPos = D3DXVECTOR3(
				pos.x + 1.0f*(-0.5f+frand()),
				pos.y + 0.3f*(-0.5f+frand()),
				pos.z + 1.0f*(-0.5f+frand()));
			pSmoke->Add(vPos);
		}

		if(BULLET_BURN_TIME<=fCnt){
			iState = BULLET_STATE_DEAD;
		}
	}
		break;
	}
	fCnt += dt;
	if(state != iState){iReq = 0;fCnt = 0;}
	return -1;
}
// ----------------------------------------------------------------------------
int CBullet::Render(LPDIRECT3DDEVICE9 pd3dDevice, CD3DMesh* pMesh)
{
	D3DXMATRIX m;
	
	D3DXMatrixTranslation(&m, pos.x, pos.y, pos.z);
    pd3dDevice->SetTransform( D3DTS_WORLD,  &m );

	switch(iState){
	case BULLET_STATE_DEAD:
		return -1;
		break;
	case BULLET_STATE_FLY:
		pMesh->Render(pd3dDevice);
		break;
	case BULLET_STATE_BURN:
	{
		FLOAT t = fCnt/BULLET_BURN_TIME;

	    pd3dDevice->SetRenderState(  D3DRS_ALPHABLENDENABLE,   TRUE );
	    pd3dDevice->SetRenderState(  D3DRS_SRCBLEND,   D3DBLEND_INVSRCALPHA );
	    pd3dDevice->SetRenderState(  D3DRS_DESTBLEND,   D3DBLEND_SRCALPHA );
        for( DWORD i=0; i<pMesh->m_dwNumMaterials; i++ )
			pMesh->m_pMaterials[i].Diffuse.a = t;
		pMesh->Render(pd3dDevice);
	    pd3dDevice->SetRenderState(  D3DRS_ALPHABLENDENABLE,   FALSE );
	}
		break;
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Smoke
//-----------------------------------------------------------------------------
inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

struct POINTVERTEX {
	D3DXVECTOR3 v;
	D3DCOLOR    color;

    static const DWORD FVF;
};
const DWORD POINTVERTEX::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
CSmoke::CSmoke( )
{
	m_pTexture		= NULL;
    m_fRadius       = 50.0f;
	m_dwDiscard     = 2048;
    m_dwFlush       = 512;
    m_dwBase        = m_dwDiscard;

    m_dwParticlesLim = 2048;
	m_pParticles    = NULL;
	m_pParticlesFree = NULL;
	for(DWORD i=0; i<m_dwParticlesLim; i++){
		PARTICLE* p=new PARTICLE;
		p->m_pNext = m_pParticlesFree;
		m_pParticlesFree = p;
	}
}

//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
CSmoke::~CSmoke( )
{
    while( m_pParticles )
    {
        PARTICLE* pSpark = m_pParticles;
        m_pParticles = pSpark->m_pNext;
        delete pSpark;
    }

    while( m_pParticlesFree )
    {
        PARTICLE *pSpark = m_pParticlesFree;
        m_pParticlesFree = pSpark->m_pNext;
        delete pSpark;
    }
}

HRESULT CSmoke::Init( LPDIRECT3DDEVICE9 pd3dDevice, TCHAR* strName )
{
    HRESULT hr;

	if( FAILED( hr=D3DUtil_CreateTexture( pd3dDevice, strName,
                                       &m_pTexture ) ) )
        return DXTRACE_ERR( "Load Texture", hr );

	return S_OK;

}

//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CSmoke::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
    HRESULT hr;

    if(FAILED(hr = pd3dDevice->CreateVertexBuffer( m_dwDiscard * 
		sizeof(POINTVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS, 
        POINTVERTEX::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL )))
	{
        return E_FAIL;
	}

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CSmoke::InvalidateDeviceObjects()
{
    SAFE_RELEASE( m_pVB );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CSmoke::Add(D3DXVECTOR3 vPos)
{
	if(NULL==m_pParticlesFree)return -1;
	
	PARTICLE* p = m_pParticlesFree;
	m_pParticlesFree = p->m_pNext;
	p->m_pNext = m_pParticles;
	m_pParticles = p;

	p->m_vPos = vPos;
	p->m_vVel = D3DXVECTOR3(0,1,0);
	p->m_fTime = p->m_fTime0 = 3.0f;
	p->m_clrFade    = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0);
	p->m_clrDiffuse = D3DXCOLOR(2.0f, 2.0f, 2.0f, 0);
	
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CSmoke::Update(FLOAT dt)
{
    PARTICLE*    pParticle = m_pParticles;
    while( pParticle )
    {
		pParticle->m_vPos += pParticle->m_vVel*dt;
		pParticle->m_fTime -= dt;
		pParticle->m_fFade = pParticle->m_fTime / pParticle->m_fTime0;
		
        pParticle = pParticle->m_pNext;
	}

	PARTICLE* pParent=NULL;
	pParticle = m_pParticles;
    while( pParticle )
    {
		if(pParticle->m_fTime<0){
			if(NULL==pParent){
				m_pParticles = pParticle->m_pNext;
				pParticle->m_pNext = m_pParticlesFree;
				m_pParticlesFree = pParticle;
				pParticle = m_pParticles;
			}else{
				pParent->m_pNext = pParticle->m_pNext;
				pParticle->m_pNext = m_pParticlesFree;
				m_pParticlesFree = pParticle;
				pParticle = pParent->m_pNext;
			}
		}else{
			pParent = pParticle;
	        pParticle = pParticle->m_pNext;
		}
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CSmoke::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
    HRESULT hr;
    POINTVERTEX* pVertices;
    DWORD        dwNumParticlesToRender = 0;
    PARTICLE*    pParticle = m_pParticles;

    pd3dDevice->SetRenderState( D3DRS_LIGHTING,  FALSE );
    pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
    pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR  );
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR  );
    // Set the render states for using point sprites
    pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
    pd3dDevice->SetRenderState( D3DRS_POINTSIZE,     FtoDW(1.00f) );
    pd3dDevice->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(0.10f) );
    pd3dDevice->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.00f) );
    pd3dDevice->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.00f) );
    pd3dDevice->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(1.00f) );

    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	pd3dDevice->SetFVF( POINTVERTEX::FVF );
    pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(POINTVERTEX) );
    pd3dDevice->SetTexture( 0, m_pTexture );

    D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

	if(m_dwDiscard <= (m_dwBase+=m_dwFlush))	m_dwBase = 0;

	if( FAILED( hr = m_pVB->Lock( m_dwBase * sizeof(POINTVERTEX)
					, m_dwFlush * sizeof(POINTVERTEX), (void**) &pVertices
					, m_dwBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD ) ) )
    {
		return hr;
    }

    // Render each particle
    while( pParticle )
    {
        D3DXCOLOR clrDiffuse;
        D3DXColorLerp(&clrDiffuse, &pParticle->m_clrFade, &pParticle->m_clrDiffuse, pParticle->m_fFade);
        DWORD dwDiffuse = (DWORD) clrDiffuse;

        pVertices->v     = pParticle->m_vPos;
        pVertices->color = dwDiffuse;
        pVertices++;

        if( ++dwNumParticlesToRender == m_dwFlush )
        {
            m_pVB->Unlock();

            if(FAILED(hr = pd3dDevice->DrawPrimitive( D3DPT_POINTLIST
								, m_dwBase, dwNumParticlesToRender)))
				return hr;

			if(m_dwDiscard <= (m_dwBase+=m_dwFlush))	m_dwBase = 0;

			if( FAILED( hr = m_pVB->Lock( m_dwBase * sizeof(POINTVERTEX)
						, m_dwFlush * sizeof(POINTVERTEX), (void**) &pVertices
						, m_dwBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD ) ) )
            {
				return hr;
            }

            dwNumParticlesToRender = 0;
        }

        pParticle = pParticle->m_pNext;
    }
	
	// Unlock the vertex buffer
    m_pVB->Unlock();

    // Render any remaining particles
    if( dwNumParticlesToRender )
        if(FAILED(hr = pd3dDevice->DrawPrimitive( D3DPT_POINTLIST, m_dwBase, dwNumParticlesToRender )))
			return hr;

	// Reset render states
    pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
    pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );
    pd3dDevice->SetRenderState( D3DRS_LIGHTING,  TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	return S_OK;
}



VOID CSmoke::DeleteDeviceObjects( )
{
	SAFE_RELEASE( m_pTexture );
}



//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Application constructor.   Paired with ~CMyD3DApplication()
//       Member variables should be initialized to a known state here.  
//       The application window has not yet been created and no Direct3D device 
//       has been created, so any initialization that depends on a window or 
//       Direct3D should be deferred to a later stage. 
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
	m_pMesh						= new CD3DMesh();				
	m_pMeshBg					= new CD3DMesh();	
	m_pSmoke					= new CSmoke();
	m_pBullet					= new CBulletMgr();	
	m_fShootingStar				= 0;

	m_pTex						= NULL;
	m_pSurf						= NULL;
	m_pZ						= NULL;
	
	for(int i=0;i<BULLET_MAX;i++){
		m_pQuery[i][0]				= NULL;
		m_pQuery[i][1]				= NULL;
		m_bCheck[i][0]				= FALSE;
		m_bCheck[i][1]				= FALSE;
	}

	m_dwCreationWidth           = 500;
    m_dwCreationHeight          = 375;
    m_strWindowTitle            = TEXT( "main" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;

	m_id = 0;

    // Create a D3D font using d3dfont.cpp
    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;

	m_fMousePos[0]				= (FLOAT)m_dwCreationWidth/2;	
	m_fMousePos[1]				= (FLOAT)m_dwCreationHeight/2;

    ZeroMemory( &m_UserInput, sizeof(m_UserInput) );
    m_fWorldRotX                = -0.5f;
    m_fWorldRotY                =  0.5f*D3DX_PI;
	m_zoom						=  10.0f;
}




//-----------------------------------------------------------------------------
// Name: ~CMyD3DApplication()
// Desc: Application destructor.  Paired with CMyD3DApplication()
//-----------------------------------------------------------------------------
CMyD3DApplication::~CMyD3DApplication()
{
}




//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Paired with FinalCleanup().
//       The window has been created and the IDirect3D9 interface has been
//       created, but the device has not been created yet.  Here you can
//       perform application-related initialization and cleanup that does
//       not depend on a device.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
    // TODO: perform one time initialization

    // Drawing loading status message until app finishes loading
    SendMessage( m_hWnd, WM_PAINT, 0, 0 );

    m_bLoadingApp = FALSE;

    return S_OK;
}





//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device initialization, this code checks the display device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
                                          D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( Format );
    UNREFERENCED_PARAMETER( dwBehavior );
    UNREFERENCED_PARAMETER( pCaps );
    
    BOOL bCapsAcceptable;

    // TODO: Perform checks to see if these display caps are acceptable.
    bCapsAcceptable = TRUE;

    if( bCapsAcceptable )         
        return S_OK;
    else
        return E_FAIL;
}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Paired with DeleteDeviceObjects()
//       The device has been created.  Resources that are not lost on
//       Reset() can be created here -- resources in D3DPOOL_MANAGED,
//       D3DPOOL_SCRATCH, or D3DPOOL_SYSTEMMEM.  Image surfaces created via
//       CreateImageSurface are never lost and can be created here.  Vertex
//       shaders and pixel shaders can also be created here as they are not
//       lost on Reset().
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
	HRESULT hr;

	// Read model
	if( FAILED( hr = m_pMesh->Create(m_pd3dDevice, "ufo.x" )))
		return DXTRACE_ERR( "Load Mesh", hr );
	m_pBullet->SetMesh( m_pMesh );

	if( FAILED( hr = m_pMeshBg->Create(m_pd3dDevice, "map.x" )))
		return DXTRACE_ERR( "Load Mesh", hr );

	m_pSmoke->Init( m_pd3dDevice, _T("particle.bmp") );

    // Init the font
    m_pFont->InitDeviceObjects( m_pd3dDevice );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Paired with InvalidateDeviceObjects()
//       The device exists, but may have just been Reset().  Resources in
//       D3DPOOL_DEFAULT and any other device state that persists during
//       rendering should be set here.  Render states, matrices, textures,
//       etc., that don't change during rendering can be set once here to
//       avoid redundant state setting during Render() or FrameMove().
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
	HRESULT hr;
	
	// query
	for(int i=0;i<BULLET_MAX;i++){
		if (FAILED(hr = m_pd3dDevice->CreateQuery( D3DQUERYTYPE_OCCLUSION, &m_pQuery[i][0] )))
			return DXTRACE_ERR( "Create Query", hr );
		if (FAILED(hr = m_pd3dDevice->CreateQuery( D3DQUERYTYPE_OCCLUSION, &m_pQuery[i][1] )))
			return DXTRACE_ERR( "Create Query", hr );
	}

	// Mesh
	m_pMesh  ->RestoreDeviceObjects(m_pd3dDevice);
	m_pMeshBg->RestoreDeviceObjects(m_pd3dDevice);

	m_pSmoke->RestoreDeviceObjects(m_pd3dDevice);
	m_pBullet->RestoreDeviceObjects();
	m_UserInput.bClick = FALSE;
	
	// screen width
	D3DVIEWPORT9 viewport;
	m_pd3dDevice->GetViewport(&viewport);
	m_Width  = viewport.Width;
	m_Height = viewport.Height;

	// Creation of rendering target
	if (FAILED(m_pd3dDevice->CreateTexture(
						MAP_SIZE, MAP_SIZE, 1, 
						D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
						D3DPOOL_DEFAULT, &m_pTex, NULL)))
		return E_FAIL;
	if (FAILED(m_pTex->GetSurfaceLevel(0, &m_pSurf)))
		return E_FAIL;
	if (FAILED(m_pd3dDevice->CreateDepthStencilSurface(
						MAP_SIZE, MAP_SIZE, 
						D3DFMT_D16, D3DMULTISAMPLE_NONE, 0,
						TRUE, &m_pZ, NULL)))
		return E_FAIL;

	// Setup a material
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

    // Set up the textures
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

    // Set miscellaneous render states
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,        0x000F0F0F );

    // Set the world matrix
    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity( &matIdentity );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXMATRIX matView;
    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -m_zoom );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_mView, &vFromPt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_mView );

    // Set the projection matrix
    D3DXMATRIX matProj;
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &m_mProj, D3DX_PI/4, fAspect, 1.0f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_mProj );

    // Set up lighting states
    D3DLIGHT9 light;
    D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, -1.0f, -1.0f, 2.0f );
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // Restore the font
    m_pFont->RestoreDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
	HRESULT hr;
	DWORD pixels;

    // Update user input state
    UpdateInput( &m_UserInput );

    // Update the camera state according to user input
    D3DXMATRIX m;
    D3DXMATRIX mCamera;
    D3DXMATRIX matRotY;
    D3DXMATRIX matRotX;

    if( m_UserInput.bRotateLeft && !m_UserInput.bRotateRight )
        m_fWorldRotY += m_fElapsedTime;
    else if( m_UserInput.bRotateRight && !m_UserInput.bRotateLeft )
        m_fWorldRotY -= m_fElapsedTime;

    if( m_UserInput.bRotateUp && !m_UserInput.bRotateDown )
        m_fWorldRotX += m_fElapsedTime;
    else if( m_UserInput.bRotateDown && !m_UserInput.bRotateUp )
        m_fWorldRotX -= m_fElapsedTime;

    D3DXMatrixRotationX( &matRotX, m_fWorldRotX );
    D3DXMatrixRotationY( &matRotY, m_fWorldRotY );
    D3DXMatrixMultiply( &mCamera, &matRotY, &matRotX );

	// Zooming
	if(m_UserInput.bZ && !m_UserInput.bX)
		m_zoom += 0.01f;
	else if(m_UserInput.bX && !m_UserInput.bZ)
		m_zoom -= 0.01f;

    // View matrix
	D3DXVECTOR3 vEye    = D3DXVECTOR3( 0.0f, 0.0f, -m_zoom );
    D3DXVECTOR3 vLookat = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m, &vEye, &vLookat, &vUp );

	m_mView = mCamera * m;

	//-----------------------------------------------------------------------------
	// Sometimes shooting a star
	//-----------------------------------------------------------------------------
	m_fShootingStar -= m_fElapsedTime;
	if(m_fShootingStar<=0.0f){
		m_fShootingStar += 10.0f;
		D3DXVECTOR4 x, v;
		x.x = (10.0f/4096.0f)*(float)(rand()%0xfff)-5.0f;
		x.z = (10.0f/4096.0f)*(float)(rand()%0xfff)-5.0f;
		x.y = 10.0f;
		v.x = (10.0f/4096.0f)*(float)(rand()%0xfff)-5.0f;
		v.z = (10.0f/4096.0f)*(float)(rand()%0xfff)-5.0f;
		v.y = 0.0f;
		v = v - x;
		D3DXVec4Normalize(&v,&v);
		v *= BULLET_SPEED;
		m_pBullet->Add(x, v);
	}
	
	if( m_UserInput.bClick){ m_UserInput.bClick = 0;
		D3DXMATRIX mat = m_mView * m_mProj;
		// target point
		D3DXVECTOR4 c = D3DXVECTOR4(
					 2.0f*m_fMousePos[0]/(FLOAT)m_Width -1,
					-2.0f*m_fMousePos[1]/(FLOAT)m_Height+1,
					1.0f,1.0f);
		D3DXMatrixInverse(&mat, NULL, &mat);
		D3DXVec4Transform  (&c, &c, &mat);
		c /= c.w;

		D3DXVECTOR4 x = D3DXVECTOR4( 0, -1.0f, 0, 1);
		D3DXMatrixInverse(&mat, NULL, &m_mView);
		D3DXVec4Transform (&x, &x, &mat);
		// velocity
		D3DXVECTOR4 v = 100.0f*c-x;
		D3DXVec4Normalize(&v,&v);
		v *= BULLET_SPEED;

		m_pBullet->Add(x, v);
	}

	//-----------------------------------------------------------------------------
	// Check query
	//-----------------------------------------------------------------------------
	for(DWORD i=0;i<m_pBullet->GetNum();i++){
		if(m_bCheck[i][m_id]){m_bCheck[i][m_id]=FALSE;
			CBullet *pBullet = m_pBullet->GetObj(i);
			while(S_OK !=(hr = m_pQuery[i][m_id]->GetData(
						&pixels, sizeof(DWORD), D3DGETDATA_FLUSH ))){
				if(D3DERR_DEVICELOST == hr) break;
			}
			if(1000<=pixels){// This is magic number. Few pixels make no collision
				pBullet->ReqState(BULLET_STATE_BURN);
			}
		}
	}
	//-----------------------------------------------------------------------------
	// Move bullets
	//-----------------------------------------------------------------------------
	m_pBullet->Update(m_fElapsedTime, m_pSmoke);
	
	//-----------------------------------------------------------------------------
	// Move bullets
	//-----------------------------------------------------------------------------
	m_pSmoke->Update(m_fElapsedTime);

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UpdateInput()
// Desc: Update the user input.  Called once per frame 
//-----------------------------------------------------------------------------
void CMyD3DApplication::UpdateInput( UserInput* pUserInput )
{
    pUserInput->bRotateUp    = ( m_bActive && (GetAsyncKeyState( VK_UP )    & 0x8000) == 0x8000 );
    pUserInput->bRotateDown  = ( m_bActive && (GetAsyncKeyState( VK_DOWN )  & 0x8000) == 0x8000 );
    pUserInput->bRotateLeft  = ( m_bActive && (GetAsyncKeyState( VK_LEFT )  & 0x8000) == 0x8000 );
    pUserInput->bRotateRight = ( m_bActive && (GetAsyncKeyState( VK_RIGHT ) & 0x8000) == 0x8000 );

	pUserInput->bZ = ( m_bActive && (GetAsyncKeyState( 'Z' ) & 0x8000) == 0x8000 );
	pUserInput->bX = ( m_bActive && (GetAsyncKeyState( 'X' ) & 0x8000) == 0x8000 );
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
	DWORD i;
	D3DXMATRIX matIdentity;
    D3DXMATRIX mVP;
    D3DXMATRIX mView, mProj;
	LPDIRECT3DSURFACE9 pOldBackBuffer, pOldZBuffer;
	D3DVIEWPORT9 oldViewport;

	//---------------------------------------------------------
	// make matrix
	//---------------------------------------------------------
	D3DXVECTOR3 vEye	  = D3DXVECTOR3( 0.0f,-1.0f, 0.0f );
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp       = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
    D3DXMatrixLookAtLH( &mView, &vEye, &vLookatPt, &vUp );
    
	D3DXMatrixOrthoLH(&mProj
					, 9.8f, 9.8f			// width, height
					, -10.0f, 10.0f );		// min_z, max_z

	mVP = mView * mProj;


    // Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		//-------------------------------------------------
		// Back up of the frame buffer
		//-------------------------------------------------
		m_pd3dDevice->GetRenderTarget(0, &pOldBackBuffer);
		m_pd3dDevice->GetDepthStencilSurface(&pOldZBuffer);
		m_pd3dDevice->GetViewport(&oldViewport);

		//-------------------------------------------------
		// Change of the rendering target
		//-------------------------------------------------
		m_pd3dDevice->SetRenderTarget(0, m_pSurf);
		m_pd3dDevice->SetDepthStencilSurface(m_pZ);
		// Change viewport
		D3DVIEWPORT9 viewport = {0,0      // 
						, MAP_SIZE // Width
						, MAP_SIZE // Height
						, 0.0f,1.0f};     // near, far
		m_pd3dDevice->SetViewport(&viewport);

		// Clear the viewport
		m_pd3dDevice->Clear(0L, NULL, D3DCLEAR_ZBUFFER
						, 0xFFFFFFFF, 1.0f, 0L);

		m_pd3dDevice->SetTransform( D3DTS_VIEW, &mView );
	    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &mProj );
		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_NONE );
//		m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE , 0 );// Effect this comment makes FPS up

		//-------------------------------------------------
        // Render world
		//-------------------------------------------------
		// Set the world matrix
		D3DXMatrixIdentity( &matIdentity );
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

		m_pMeshBg->Render( m_pd3dDevice );

		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_CCW );
		m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

		// TODO: render bullet
		mVP = mView * mProj;
		
		for(i=0;i<m_pBullet->GetNum();i++){
			CBullet *pBullet = m_pBullet->GetObj(i);
			if(BULLET_STATE_FLY == pBullet->GetState()){
				m_pQuery[i][m_id]->Issue( D3DISSUE_BEGIN );
				pBullet->Render( m_pd3dDevice, m_pMesh );
				m_pQuery[i][m_id]->Issue( D3DISSUE_END );
				m_bCheck[i][m_id] = TRUE;
			}
		}
		m_id^=1;
        
		//-------------------------------------------------
		// restore the frame buffer
		//-------------------------------------------------
		m_pd3dDevice->SetRenderTarget(0, pOldBackBuffer);
		m_pd3dDevice->SetDepthStencilSurface(pOldZBuffer);
		m_pd3dDevice->SetViewport(&oldViewport);
		pOldBackBuffer->Release();
		pOldZBuffer->Release();
		m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE , 0xf );

		// Clear the viewport
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
							0x00000000, 1.0f, 0L );

		//-------------------------------------------------
        // TODO: render world
		//-------------------------------------------------
		m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_mView );
	    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_mProj );

		// Set the world matrix
		D3DXMatrixIdentity( &matIdentity );
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

		m_pMeshBg->Render( m_pd3dDevice );

        // TODO: render bullet

		m_pBullet->Render(m_pd3dDevice);

		m_pSmoke->Render( m_pd3dDevice );
        
#if 1 // For debugging, draw texture
		{
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,    D3DTOP_DISABLE);
		float scale = 128.0f;
		typedef struct {FLOAT p[4]; FLOAT tu, tv;} TVERTEX;
		TVERTEX Vertex[4] = {
			// x  y  z rhw tu tv
			{    0,    0,0, 1, 0, 0,},
			{scale,    0,0, 1, 1, 0,},
			{scale,scale,0, 1, 1, 1,},
			{    0,scale,0, 1, 0, 1,},
		};
		m_pd3dDevice->SetTexture( 0, m_pTex );
		m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, Vertex, sizeof( TVERTEX ) );
		}
#endif		

        // Render stats and help text  
        RenderText();

		// End the scene.
        m_pd3dDevice->EndScene();
	}

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: Renders stats and help text to the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,0);
    TCHAR szMsg[MAX_PATH] = TEXT("");

    // Output display stats
    FLOAT fNextLine = 40.0f; 

    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    // Output statistics & help
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height; 
    wsprintf( szMsg, TEXT("Arrow keys: Up=%d Down=%d Left=%d Right=%d" ), 
              m_UserInput.bRotateUp, m_UserInput.bRotateDown, m_UserInput.bRotateLeft, m_UserInput.bRotateRight );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    sprintf( szMsg, TEXT("World State: %0.3f, %0.3f"), 
                    m_fWorldRotX, m_fWorldRotY );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("Use arrow keys to update input") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("Press 'F2' to configure display") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Overrrides the main WndProc, so the sample can do custom message
//       handling (e.g. processing mouse, keyboard, or menu commands).
//-----------------------------------------------------------------------------
LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
{
    switch( msg )
    {
        case WM_PAINT:
        {
            if( m_bLoadingApp )
            {
                // Draw on the window tell the user that the app is loading
                // TODO: change as needed
                HDC hDC = GetDC( hWnd );
                TCHAR strMsg[MAX_PATH];
                wsprintf( strMsg, TEXT("Loading... Please wait") );
                RECT rct;
                GetClientRect( hWnd, &rct );
                DrawText( hDC, strMsg, -1, &rct, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
                ReleaseDC( hWnd, hDC );
            }
            break;
        }
		case WM_MOUSEMOVE:
		{
			long x = (lParam << 16) >> 16;
			long y = lParam >> 16;
			m_fMousePos[0] = (FLOAT)x;
			m_fMousePos[1] = (FLOAT)y;
            break;
		}
		case WM_LBUTTONDOWN:
		{
			m_UserInput.bClick = TRUE;
            break;
		}
    }

    return CD3DApplication::MsgProc( hWnd, msg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Invalidates device objects.  Paired with RestoreDeviceObjects()
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
    // TODO: Cleanup any objects created in RestoreDeviceObjects()
	SAFE_RELEASE(m_pSurf);
	SAFE_RELEASE(m_pTex);
	SAFE_RELEASE(m_pZ);

	for(int i=0;i<BULLET_MAX;i++){
		SAFE_RELEASE(m_pQuery[i][0]);
		SAFE_RELEASE(m_pQuery[i][1]);
	}

	// mesh
	m_pMeshBg->InvalidateDeviceObjects();
	m_pMesh  ->InvalidateDeviceObjects();

	m_pSmoke->InvalidateDeviceObjects();

    m_pFont->InvalidateDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Paired with InitDeviceObjects()
//       Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.  
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
    // TODO: Cleanup any objects created in InitDeviceObjects()
	m_pSmoke->DeleteDeviceObjects( );

	// Mesh
	m_pMeshBg->Destroy();
	m_pMesh  ->Destroy();

    m_pFont->DeleteDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Paired with OneTimeSceneInit()
//       Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
    // TODO: Perform any final cleanup needed

	// Mesh
	SAFE_DELETE( m_pMeshBg );
	SAFE_DELETE( m_pMesh );
	
	SAFE_DELETE( m_pBullet );
	SAFE_DELETE( m_pSmoke );

    // Cleanup D3D font
    SAFE_DELETE( m_pFont );

    return S_OK;
}




