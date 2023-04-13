//-----------------------------------------------------------------------------
// File: main.h
//
// Desc: Header file main sample app
//-----------------------------------------------------------------------------
#pragma once


#define frand() ((FLOAT)rand()/(FLOAT)(RAND_MAX+1))

//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
// TODO: change "DirectX AppWizard Apps" to your name or the company name
#define DXAPP_KEY        TEXT("Software\\DirectX AppWizard Apps\\main")

// Struct to store the current input state
struct UserInput
{
    // TODO: change as needed
    BOOL bRotateUp;
    BOOL bRotateDown;
    BOOL bRotateLeft;
    BOOL bRotateRight;
    BOOL bZ;
    BOOL bX;
	BOOL bClick;
};


// ----------------------------------------------------------------------------
// smoke
// ----------------------------------------------------------------------------
struct PARTICLE
{
    BOOL        m_bSpark;     // Sparks are less energetic particles that
                              // are generated where/when the main particles
                              // hit the ground

    D3DXVECTOR3 m_vPos;       // Current position
    D3DXVECTOR3 m_vVel;       // Current velocity
    FLOAT       m_fTime;      // Current Time

    D3DXVECTOR3 m_vPos0;      // Initial position
    D3DXVECTOR3 m_vVel0;      // Initial velocity
    FLOAT       m_fTime0;     // Time of creation

    D3DXCOLOR   m_clrDiffuse; // Initial diffuse color
    D3DXCOLOR   m_clrFade;    // Faded diffuse color
    FLOAT       m_fFade;      // Fade progression

    PARTICLE*   m_pNext;      // Next particle in list
};

class CSmoke{
protected:
    LPDIRECT3DTEXTURE9 m_pTexture;
    LPDIRECT3DVERTEXBUFFER9 m_pVB;

    FLOAT     m_fRadius;
    DWORD     m_dwBase;
	DWORD     m_dwFlush;
    DWORD     m_dwDiscard;

    DWORD     m_dwParticlesLim;
    PARTICLE* m_pParticles;
    PARTICLE* m_pParticlesFree;

public:
	CSmoke();
	~CSmoke();
	HRESULT Init( LPDIRECT3DDEVICE9 pd3dDevice, TCHAR* strName );
	HRESULT Update(FLOAT dt);
	HRESULT Render(LPDIRECT3DDEVICE9 pd3dDevice);
    HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT InvalidateDeviceObjects();
	VOID DeleteDeviceObjects();

	HRESULT Add(D3DXVECTOR3 vPos);
};


// ----------------------------------------------------------------------------
// bullet
// ----------------------------------------------------------------------------
#define BULLET_SPEED     5.0f
#define BULLET_BURN_TIME 2.0f
#define BULLET_MAX 10	// The max count what we can shoot

#define BULLET_STATE_DEAD	0
#define BULLET_STATE_FLY	1
#define BULLET_STATE_BURN	2

class CBullet{
private:
	int				iState;	// State of the bullet in the now
	int				iReq;	// Request to state external
	FLOAT			fCnt;	// Counter for effects
	FLOAT			fLastCnt;
	D3DXVECTOR4		pos;	// World position
	D3DXVECTOR4		velosity;// World velocity
public:
	// constructor
	CBullet():iState(BULLET_STATE_DEAD){}
	// Dead enforce
	void Reset(){iState=BULLET_STATE_DEAD;iReq = 0;}
	// Creation
	int Create(D3DXVECTOR4 &x, D3DXVECTOR4 &v);
	// Move and change state
	int Update(FLOAT dt, CSmoke *pSmoke);
	int Render(LPDIRECT3DDEVICE9 pd3dDevice, CD3DMesh* pMesh);
	
	// ----------------------------------------------------------------------------
	// Geter & Seter
	int  GetState() const {return iState;}
	void ReqState(int state){iReq = state;}
	D3DXVECTOR4 &GetPosition(){return pos;}
	D3DXVECTOR4 &GetVelosity(){return velosity;}
};
// ----------------------------------------------------------------------------
// Manager of bullets
// ----------------------------------------------------------------------------
class CBulletMgr{
private:
	CD3DMesh*	m_pMesh;
	CBullet		bullet[BULLET_MAX];
public:
	int Add(D3DXVECTOR4 &x, D3DXVECTOR4 &v){
		DWORD i;
		for(i=0;i<BULLET_MAX;i++){
			if(0==bullet[i].Create(x,v)) return 0;
		}
		return -1;
	}
	void Update(FLOAT dt, CSmoke *pSmoke){
		for(DWORD i=0;i<BULLET_MAX;i++){
			bullet[i].Update(dt,pSmoke);
		}
	}
	void Render(LPDIRECT3DDEVICE9 pd3dDevice){
		for(DWORD i=0;i<BULLET_MAX;i++){
			bullet[i].Render(pd3dDevice, m_pMesh);
		}
	}
	void RestoreDeviceObjects(){
		for(DWORD i=0;i<BULLET_MAX;i++){
			bullet[i].Reset();
		}
	}
	DWORD GetNum()const{return BULLET_MAX;}
	CBullet *GetObj(DWORD i){return &bullet[i];}
	int  GetState(DWORD i) const {return bullet[i].GetState();}
	D3DXVECTOR4 &GetPosition(DWORD i){return bullet[i].GetPosition();}
	D3DXVECTOR4 &GetVelosity(DWORD i){return bullet[i].GetVelosity();}
	VOID SetMesh( CD3DMesh*	pMesh ){m_pMesh = pMesh;}
};
//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	CD3DMesh*				m_pMesh;		// flying object
	CD3DMesh*				m_pMeshBg;		// Background model
	CSmoke*					m_pSmoke;		// particle manager
	CBulletMgr*				m_pBullet;		// bullet
	int						m_id;

	// Rendering target
	LPDIRECT3DTEXTURE9		m_pTex;			// texture
	LPDIRECT3DSURFACE9		m_pSurf;		// surface
	LPDIRECT3DSURFACE9		m_pZ;			// Z-buffer

	IDirect3DQuery9*		m_pQuery[BULLET_MAX][2];	// 
	BOOL					m_bCheck[BULLET_MAX][2];

	FLOAT					m_fShootingStar;// some times falls a bullet
	FLOAT					m_fMousePos[2];	// cursol position


    BOOL                    m_bLoadingApp;  // TRUE, if the app is loading
    CD3DFont*               m_pFont;        // Font for drawing text
    UserInput               m_UserInput;    // Struct for storing user input 

    FLOAT                   m_fWorldRotX;   // World rotation state X-axis
    FLOAT                   m_fWorldRotY;   // World rotation state Y-axis
	FLOAT					m_zoom;			// Amount of track back of the camera

	D3DXMATRIX				m_mView;		// View matrix
	D3DXMATRIX				m_mProj;		// Projection matrix
	DWORD                   m_Width;		// Width of the viewport
	DWORD                   m_Height;		// Height of the viewport


protected:
    virtual HRESULT OneTimeSceneInit();
    virtual HRESULT InitDeviceObjects();
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
    virtual HRESULT FinalCleanup();
    virtual HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT );

    HRESULT RenderText();

    void    UpdateInput( UserInput* pUserInput );

public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
    virtual ~CMyD3DApplication();
};

