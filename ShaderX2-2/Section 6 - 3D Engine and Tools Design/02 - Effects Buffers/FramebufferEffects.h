//-----------------------------------------------------------------------------
// File: FramebufferEffects.h
//
// Desc: Header file FramebufferEffects sample app
//-----------------------------------------------------------------------------
#pragma once



#pragma warning ( push )


// Isn't it great when MS libs don't compile without producing warnings?
// In this case it's mostly stuff in xlocale


// unreferenced formal parameter
#pragma warning ( disable : 4100 )
// copy constructor could not be generated
#pragma warning ( disable : 4511 )
// assignment operator could not be generated
#pragma warning ( disable : 4512 )
// some C++ standard change
#pragma warning ( disable : 4663 )
// signed/unsigned mismatch
#pragma warning ( disable : 4018 )


#include <list>
// Stop turrning these warnings back on silly header!
#pragma warning ( disable : 4663 )
#pragma warning ( disable : 4018 )
#include <string>


#pragma warning ( pop )


#include "handy.h"




//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
// TODO: change "DirectX AppWizard Apps" to your name or the company name
#define DXAPP_KEY        TEXT("Software\\Tom Forsyth\\FramebufferEffects")

// DirectInput action mapper reports events only when buttons/axis change
// so we need to remember the present state of relevant axis/buttons for 
// each DirectInput device.  The CInputDeviceManager will store a 
// pointer for each device that points to this struct
struct InputDeviceState
{
    // TODO: change as needed
    FLOAT fAxisRotateLR;
    BOOL  bButtonRotateLeft;
    BOOL  bButtonRotateRight;

    FLOAT fAxisRotateUD;
    BOOL  bButtonRotateUp;
    BOOL  bButtonRotateDown;
};


// Struct to store the current input state
struct UserInput
{
    // TODO: change as needed
    FLOAT fAxisRotateUD;
    FLOAT fAxisRotateLR;
    BOOL bDoConfigureInput;
    BOOL bDoConfigureDisplay;
};



enum RenderingType
{
	RenderType_Normal,
	RenderType_Alpha,
	RenderType_Greyscale,
	RenderType_Distort,
	RenderType_Blur,
	RenderType_Heat,
	RenderType_HeatShow,
	RenderType_Background,

	RenderType_Last			// Always last.
};




// Returns pseudo-random number between 0.0 and 1.0 inclusive;
float Rand01 ( DWORD dwSeed )
{
	dwSeed ^= dwSeed<<5;
	dwSeed += 0x98649231;
	dwSeed *= 37;
	dwSeed = (dwSeed>>9)^(dwSeed>>3);
	return (1.0f / 65535.0f) * (float)( dwSeed & 0xffff );
}

// Returns pseudo-random number between 0 (inclusive) and iLimit (exclusive);
int RandLimit( DWORD dwSeed, int iLimit )
{
	dwSeed ^= dwSeed<<5;
	dwSeed += 0x98649231;
	dwSeed *= 37;
	dwSeed = (dwSeed>>9)^(dwSeed>>3);
	int iVal = (int)( dwSeed % (unsigned)iLimit );
	return iVal;
}


class RandomLerpF
{
private:
	float			fMin, fMax;
	DWORD			dwSeed;
public:
	RandomLerpF ( void )
	{
		fMin = 0.0f;
		fMax = 1.0f;
		dwSeed = 0;
	}

	RandomLerpF ( float fNewMin, float fNewMax, DWORD dwNewSeed )
	{
		Init ( fNewMin, fNewMax, dwNewSeed );
	}

	void Init ( float fNewMin, float fNewMax, DWORD dwNewSeed )
	{
		fMin = fNewMin;
		fMax = fNewMax;
		dwSeed = dwNewSeed;
	}

	float Value ( DWORD dwSeed2 )
	{
		dwSeed2 += dwSeed;
		float fLerp = Rand01 ( dwSeed2 );
		return fMin + ( fMax - fMin ) * fLerp;
	}

	float Max ( void )
	{
		return fMax;
	}
};

class RandomLerpI
{
private:
	int				iMin, iMax;
	DWORD			dwSeed;
public:
	RandomLerpI ( void )
	{
		iMin = 0;
		iMax = 65536;
		dwSeed = 0;
	}

	RandomLerpI ( int iNewMin, int iNewMax, DWORD dwNewSeed )
	{
		Init ( iNewMin, iNewMax, dwNewSeed );
	}

	void Init ( int iNewMin, int iNewMax, DWORD dwNewSeed )
	{
		iMin = iNewMin;
		iMax = iNewMax;
		dwSeed = dwNewSeed;
	}

	int Value ( DWORD dwSeed2 )
	{
		dwSeed2 += dwSeed;
		return iMin + RandLimit ( dwSeed2, iMax - iMin );
	}

	int Max ( void )
	{
		return iMax;
	}
};



class Object
{
public:
	D3DXMATRIX				mOrn;
	ID3DXMesh				*pD3DXMesh;
	DWORD					dwColour;
	float					fGreyScale;

	virtual void Render ( int iRenderTypeID );
};


class ObjectParticleSystem : public Object
{
public:
	int						iNumEmitters;
	int						iMaxNumEmitters;
	D3DXVECTOR3				*pvEmitterOrigin;
	DWORD					dwSeed;

	int						iNumParticles;

	RandomLerpI				rlEmitter;
	RandomLerpF				rlLifeTime;
	RandomLerpF				rlSize;
	RandomLerpF				rlShade;
	RandomLerpF				rlMoveX;
	RandomLerpF				rlMoveY;
	RandomLerpF				rlMoveZ;

	ObjectParticleSystem ( int iMaxNumEmitters );
	virtual ~ObjectParticleSystem();

	void AddEmitter ( const D3DXVECTOR3 &vPosition );

	virtual void Render ( int iRenderTypeID );
};



// All the info about a single render pass.
struct RenderPass
{
	// Static data.
	std::string				sName;					// The name of this effect.
	bool					bEnabled;				// Currently enabled?
	int						iNumChannels;			// How many channels this effect requires, 1-4.
	int						dwClearColour;			// The colour to clear to.


	struct RenderPassObject
	{
		// Holds the list of objects that use this effect
		Object				*pObject;				// The objects.
		int					iPassID;				// The ID that the object uses to know which effect to render.
	};
	std::list<RenderPassObject>		lRenderPassObjects;



	// Dynamic data.
	int						iTargetIndex;			// The target number.
	LPDIRECT3DSURFACE9		psurfTarget;			// The target for this rendering.
	LPDIRECT3DTEXTURE9		ptexTarget;				// The target for this rendering.
	DWORD					dwColourChannelMask;	// Combination of D3DCOLORWRITEENABLE_* flags.
	bool					bCanBeAlphaChannel;		// true if it's allowed to be an alpha channel.


	RenderPass ( std::string sName = "unknown", int iNumChannels = -1, DWORD dwClearColour = 0x00000000, bool bCanBeAlphaChannel = true );
	~RenderPass();

	// Add the object to this pass. When the object is called for rendering, it gets passed back iRenderTypeID,
	// so it knows which RP it was called for.
	void AddObject ( Object *pObject, int iRenderTypeID );
};






//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
    BOOL                    m_bLoadingApp;          // TRUE, if the app is loading
    CD3DFont*               m_pFont;                // Font for drawing text
    ID3DXMesh*              m_pD3DXMesh;            // D3DX mesh to store teapot

    CInputDeviceManager*    m_pInputDeviceManager;  // DirectInput device manager
    DIACTIONFORMAT          m_diafGame;             // Action format for game play
    LPDIRECT3DSURFACE9      m_pDIConfigSurface;     // Surface for config'ing DInput devices
    UserInput               m_UserInput;            // Struct for storing user input 

    //FLOAT                   m_fWorldRotX;           // World rotation state X-axis
    //FLOAT                   m_fWorldRotY;           // World rotation state Y-axis
	D3DXMATRIX				m_CameraOrn;				// NOTE! This is the orientation of the camera _object_. The inverse is fed to D3DTS_VIEW.



	int						m_iNumRenderTargets;
	LPDIRECT3DSURFACE9		*m_ppsurfRenderTargets;		// Array of the rendertargets.
	LPDIRECT3DTEXTURE9		*m_pptexRenderTargets;		// Array of the rendertargets.

	std::list<RenderPass*>	lprenderpassMasterList;		// The list of render passes possible.
	std::list<Object*>		lpobjectMasterList;			// The list of existing objects.

	ID3DXRenderToSurface	*pd3dxRenderToSurface;		// A helper interface to wrap the render-to-texture stuff up in.

	int						m_iRenderTargetWidth;
	int						m_iRenderTargetHeight;


	IDirect3DPixelShader9	*m_pCombiningPixelShader;
	DWORD					m_dwCombiningPixelShaderType;
	unsigned int			m_iNumSamplers;
	LPCSTR					m_pcSamplerNames[16];		// up to 16 samplers for PS3.0
	int						m_iSamplerIndex[16];		// For this sampler, the index of the rendertarget that should be bound to it.
	LPD3DXBUFFER			m_pShader;					// The shader itself - this is where m_pcSamplerNames[] point to, so it needs to hang around!




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
    VOID    Pause( bool bPause );

    HRESULT RenderText();

    HRESULT InitInput( HWND hWnd );
    void    UpdateInput( UserInput* pUserInput );
    void    CleanupDirectInput();

    VOID    ReadSettings();
    VOID    WriteSettings();



	// Finds a named renderpass. NULL if it doesn't exist.
	RenderPass *FindPass ( std::string sName );
	// Same as RenderPass::AddObject, but to a named renderpass. Return false if that renderpass doesn't exist.
	bool AddObject ( std::string sName, Object *pObject, int iRenderTypeID );



public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
    virtual ~CMyD3DApplication();

    HRESULT InputAddDeviceCB( CInputDeviceManager::DeviceInfo* pDeviceInfo, const DIDEVICEINSTANCE* pdidi );
    static HRESULT CALLBACK StaticInputAddDeviceCB( CInputDeviceManager::DeviceInfo* pDeviceInfo, const DIDEVICEINSTANCE* pdidi, LPVOID pParam );   
    BOOL    ConfigureInputDevicesCB( IUnknown* pUnknown );
    static BOOL CALLBACK StaticConfigureInputDevicesCB( IUnknown* pUnknown, VOID* pUserData );
};

