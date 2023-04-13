//-----------------------------------------------------------------------------
// File: main.h
//
// Desc: Header file main sample app
//-----------------------------------------------------------------------------
#pragma once

#include "CShadowVolume.h"

#define frand() ((FLOAT)rand()/(FLOAT)(RAND_MAX+1))

//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------


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

//-----------------------------------------------------------------------------
// Vertex declaration
//-----------------------------------------------------------------------------
D3DVERTEXELEMENT9 decl[] =
{
	{0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	0},
	{0, 24, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	{0, 32, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_LOOKUP,  D3DDECLUSAGE_SAMPLE,	0},
	D3DDECL_END()
};



// ----------------------------------------------------------------------------
// bullet
// ----------------------------------------------------------------------------
#define BULLET_SPEED     1.0f
#define BULLET_BURN_TIME 0.5f
#define BULLET_MAX 16	// The max count what we can shoot

#define BULLET_STATE_DEAD	0
#define BULLET_STATE_FLY	1
#define BULLET_STATE_BURN	2

class CBullet{
private:
	int				id;		 // unique id
	int				iState;	 // State of the bullet in the now
	int				iReq;	 // Request to state external
	FLOAT			fCnt;	 // Counter for effects
	BOOL			bSetPos; // Request to Set position
	D3DXVECTOR4		pos;	 // World position
	D3DXVECTOR4		velocity;// World velocity
	CD3DMesh		*pMesh;
	CShadowVolume	*pVolume;// path volume
public:
	// constructor
	CBullet():iState(BULLET_STATE_DEAD), bSetPos(FALSE){
		pMesh = new CD3DMesh();
		pVolume = new CShadowVolume();
	}
	~CBullet(){Delete();}

	void CreateMesh(LPDIRECT3DDEVICE9 pd3dDevice, TCHAR *filename);
	void RestoreDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice);
	void InvalidateDeviceObjects(){pMesh->InvalidateDeviceObjects();}
	void Destroy(){pMesh->Destroy();pVolume->Destroy();}
	void Delete(){SAFE_DELETE(pVolume);SAFE_DELETE(pMesh);}
	// Creation
	int Create(D3DXVECTOR4 &x, D3DXVECTOR4 &v);
	// Move and change state
	int Update(FLOAT dt);
	int Render( LPDIRECT3DDEVICE9 pd3dDevice );
	void RenderPathVolume( LPDIRECT3DDEVICE9 pd3dDevice ){
		pVolume->Render(pd3dDevice);
	}
	
	// ----------------------------------------------------------------------------
	// Geter & Seter
	inline void SetId(int i) {id=i;}
	inline int  GetState() const {return iState;}
	inline void ReqState(int state){iReq = state;}
	inline D3DXVECTOR4 &GetPosition(){return pos;}
	inline D3DXVECTOR4 &GetVelocity(){return velocity;}
	inline BOOL IsSetPosition() const {return bSetPos;}
	inline void SetPosition(BOOL val) {bSetPos = val;}
	inline CD3DMesh *GetMesh() {return pMesh;}

};
// ----------------------------------------------------------------------------
// Manager of bullets
// ----------------------------------------------------------------------------
class CBulletMgr{
private:
	CBullet		bullet[BULLET_MAX];
public:
	CBulletMgr(){for(DWORD i=0;i<BULLET_MAX;i++) bullet[i].SetId(i);}
	~CBulletMgr(){for(DWORD i=0;i<BULLET_MAX;i++) bullet[i].Delete();}
	int Add(D3DXVECTOR4 &x, D3DXVECTOR4 &v){
		DWORD i;
		for(i=0;i<BULLET_MAX;i++){
			if(0==bullet[i].Create(x,v)) return 0;
		}
		return -1;
	}
	void Update(FLOAT dt){
		for(DWORD i=0;i<BULLET_MAX;i++){
			bullet[i].Update(dt);
		}
	}
	void Render(LPDIRECT3DDEVICE9 pd3dDevice){
		for(DWORD i=0;i<BULLET_MAX;i++){
			bullet[i].Render( pd3dDevice );
		}
	}
	void CreateMesh(LPDIRECT3DDEVICE9 pd3dDevice, TCHAR *filename){
		for(DWORD i=0;i<BULLET_MAX;i++){
			bullet[i].CreateMesh(pd3dDevice, filename);
		}
	}
	void RestoreDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice){
		for(DWORD i=0;i<BULLET_MAX;i++){
			bullet[i].RestoreDeviceObjects(pd3dDevice);
		}
	}
	void InvalidateDeviceObjects(){
		for(DWORD i=0;i<BULLET_MAX;i++){
			bullet[i].InvalidateDeviceObjects();
		}
	}
	void Destroy(){
		for(DWORD i=0;i<BULLET_MAX;i++){
			bullet[i].Destroy();
		}
	}

	DWORD GetNum()const{return BULLET_MAX;}
	CBullet *GetObj(DWORD i){return &bullet[i];}
	int  GetState(DWORD i) const {return bullet[i].GetState();}
	D3DXVECTOR4 &GetPosition(DWORD i){return bullet[i].GetPosition();}
	D3DXVECTOR4 &GetVelocity(DWORD i){return bullet[i].GetVelocity();}
};
//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	LPDIRECT3DVERTEXDECLARATION9 m_pDecl;	// Vertex declaration
	CD3DMesh*				m_pMeshBg;		// Background model
	CBulletMgr*				m_pBullet;		// bullet
	FLOAT					m_TexWorldScale;

	// Shader
	LPD3DXEFFECT     m_pEffect;		// Effect
	D3DXHANDLE		 m_hTechnique;	// Technique
	D3DXHANDLE       m_hmWVPbg;		// local-projection
	D3DXHANDLE       m_hmWVP;		// local-projection
	D3DXHANDLE       m_hvVel;		// velocity
	D3DXHANDLE       m_hfDisplacement;// degree of displacement

	// Rendering target
	LPDIRECT3DSURFACE9		m_pZ;			// Z-buffer
	LPDIRECT3DTEXTURE9		m_pTex;			// texture
	LPDIRECT3DSURFACE9		m_pSurf;		// surface
	LPDIRECT3DTEXTURE9		m_pTotalTex;	// texture
	LPDIRECT3DSURFACE9		m_pTotalSurf;	// surface
	LPDIRECT3DTEXTURE9		m_pBurnTex;			// texture
	LPDIRECT3DSURFACE9		m_pBurnSurf;		// surface
	LPDIRECT3DTEXTURE9		m_pBlurTex;			// texture
	LPDIRECT3DSURFACE9		m_pBlurSurf;		// surface
	LPDIRECT3DTEXTURE9		m_pSeedTex;			// texture
	LPDIRECT3DTEXTURE9		m_pFireTex[2];		// texture
	LPDIRECT3DSURFACE9		m_pFireSurf[2];		// texture
	LPDIRECT3DTEXTURE9		m_pParticleTex;		// Particle map
	LPDIRECT3DSURFACE9		m_pParticleSurf;
	LPDIRECT3DTEXTURE9		m_pCoordTex;		// Texture Coordinate of the ground
	LPDIRECT3DSURFACE9		m_pCoordSurf;
	LPDIRECT3DTEXTURE9		m_pMinTex[5];		// Reduction m_pTex
	LPDIRECT3DSURFACE9		m_pMinSurf[5];
	LPDIRECT3DTEXTURE9		m_pNormalTex;		// Normal map of the ground
	LPDIRECT3DSURFACE9		m_pNormalSurf;
	LPDIRECT3DTEXTURE9		m_pCatTex[2];		// Catmap
	LPDIRECT3DSURFACE9		m_pCatSurf[2];
	DWORD					m_iCatId;

	IDirect3DQuery9*		m_pQuery[BULLET_MAX][2];	// 
	int						m_bCheck[BULLET_MAX][2];
	int						m_id;

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
	D3DXMATRIX				m_mUnderView;	// View matrix from seeing a below
	D3DXMATRIX				m_mUnderProj;	// Projection matrix from seeing a below
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
    virtual HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT, D3DFORMAT );

    HRESULT RenderText();

    void    UpdateInput( UserInput* pUserInput );
	void	UpdateParticleMap();
	void	UpdateAcceleration(DWORD id);

public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
    virtual ~CMyD3DApplication();
};

