/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
//
// IEB_Effect.h: interface for the EBEffect class
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __IEBEFFECT_H
#define __IEBEFFECT_H

#include <windows.h>
#include <NVEffectsBrowser/Registry.h>

#pragma warning(disable: 4786)
#include <string.h>
#include <stdarg.h>

#include <NVEffectsBrowser/eb_string.h>
#include <NVEffectsBrowser/eb_timer.h>
#include <NVEffectsBrowser/eb_property.h>
#include <nv_renderdevice/nv_renderdevice.h>

////////////////////////////////////////////////////////////////////////////////
// EBEffect API versioning
//
// These defines declare the current major and minor version of the effect API.
// When changing the EBEffect class (the API), if your change is backward
// compatible, only increment the minor version number.  If your change requires
// that effects compiled against the prior API be recompiled or updated, zero
// the minor version number and increment the major version number.  That is,
// if an effect is compiled with API version X.Y, then it will run with browsers
// supporting any API numbered X.Z where Y <= Z.  Also remember that you needn't
// change the API at all, if your change will not disrupt the EBEffect interface
// at all.
//
// NOTE: The versioning information will be 'burned into' each DLL as it is
// compiled (see __DECLARE_EFFECT_VERSION() below).
//
// This versioning system outdates the old EBEFFECT_VERSION system.  This
// version provides the ability update the API and explicitly retain or remove
// backwards compatibility.  It also changes the version to be associated with
// the entire DLL rather than each individual effect defined therein.  This
// allows for more optimal DLL loading in the browser.  Finally, this
// implementation is not upset by changes to the EBEffect class structure or
// virtual function table.

#define EBEFFECT_APIVERSION_MAJOR 8
#define EBEFFECT_APIVERSION_MINOR 0


#define MAKE_SHADER_VERSION(major, minor) ((major << 8) | (minor) | (0xFFFF << 16))

////////////////////////////////////////////////////////////////////////////////
// EBEffect public function prototypes and macros
//
// Each effect deriving from EBEffect must include the DECLARE_EFFECT_MAIN()
// macro.  This macro defines some mandatory exports and the DLL's DllMain()
// function.  Additionally, each effect must export "C" style functions named
// 'CreateEffect' and 'GetEffectNum' that match the LPCREATEEFFECT and
// LPGETEFFECTNUM prototypes respectively.  The four macros named below can be
// used to simplify this process.

typedef class IEBEffect* (*LPCREATEEFFECT)(unsigned int);
typedef unsigned int (*LPGETEFFECTNUM)();

#if STANDALONE
#define DECLARE_EFFECT_MAIN(effectClass, width, height)\
	__DECLARE_EFFECT_STATICS()\
	__DECLARE_EFFECT_APIVERSION()\
	int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int)\
	{\
		effectClass* effect = new effectClass;\
		int ret = effect->Main(hInstance, (width), (height));\
		delete effect;\
		return ret;\
	}
#else
#define DECLARE_EFFECT_MAIN()				\
	__DECLARE_EFFECT_STATICS()				\
	__DECLARE_EFFECT_APIVERSION()			\
	__DECLARE_EFFECT_SCANFUNCS()			\
	__DECLARE_EFFECT_DLLMAIN()
#endif

// These macros provide a simple way to define the list of effects that can
// be created by your DLL.  These macros can be used as follows:
//
// DECLARE_EFFECT_COUNT(3)
// DECLARE_EFFECT_CREATE_BEG()
// DECLARE_EFFECT_CREATE(0, MyFirstEffectClass())
// DECLARE_EFFECT_CREATE(1, MyOtherEffectClass(ARG_EFFECT_ONE, ARG_EFFECT_TWO))
// DECLARE_EFFECT_CREATE(2, MyOtherEffectClass(ARG_EFFECT))
// DECLARE_EFFECT_CREATE_END()
//
//    OR
//
// DECLARE_EFFECT_JUST_ONE(MyOnlyEffect(Arg1, Arg2, Arg3))
//
// You can, of course, simply declare the functions yourself.

#define DECLARE_EFFECT_COUNT(num)			\
extern "C" {								\
	__declspec(dllexport) unsigned int		\
	GetNumEffects()							\
	{										\
		return num;							\
	}										\
}

#define DECLARE_EFFECT_CREATE_BEG()			\
extern "C" {								\
	__declspec(dllexport) EBEffect*			\
	CreateEffect(unsigned int num)			\
	{										\
		switch (num) {

#define DECLARE_EFFECT_CREATE(n,c)			\
		case n: return new c; break;

#define DECLARE_EFFECT_CREATE_END()			\
		}									\
		return NULL;						\
	}										\
}

#define DECLARE_EFFECT_JUST_ONE(c)			\
	DECLARE_EFFECT_COUNT(1)					\
	DECLARE_EFFECT_CREATE_BEG()				\
	DECLARE_EFFECT_CREATE(0,c)				\
	DECLARE_EFFECT_CREATE_END()


////////////////////////////////////////////////////////////////////////////////
// EBEffect enums

typedef enum tagButtonID
{
	MOUSE_LEFTBUTTON = 0,
	MOUSE_RIGHTBUTTON = 1,
	MOUSE_MIDDLEBUTTON = 2
} eButtonID;

typedef enum tagEBGFXAPI
{
	GFXAPI_D3D = 0,
	GFXAPI_OPENGL = 1,
    GFXAPI_OGLCG = 2,
    GFXAPI_D3DCG = 3,
	GFXAPI_D3D9 = 4,
	GFXAPI_D3D9CG = 5
} EBGFXAPI;

typedef enum tagEBKeyAction
{
	// Required
	EB_HELP = 0,		// Show help
	EB_WIREFRAME,		// toggle wireframe
	EB_RESET,			// Reset to start conditions

	// Reserved
	EB_QUIT,			// Quits - not used in effects browser
	EB_HUD,				// toggles HUD
	EB_CONSOLE,			// toggles console window
	EB_PAUSE,			// toggles Animation
	EB_ADD,				// Increase something
	EB_SUBTRACT,		// Decrease something
	EB_MULTIPLY,		// Multiply something
	EB_DIVIDE,			// Divide something
	EB_PAGEUP,			// Page up to move through options
	EB_PAGEDOWN,		// Page down to move through options
	EB_POINTS,			// sets point rendering mode

	// Suggested
	EB_CUBEMAP,			// cycles cubemaps			
	EB_TEXTURES,		// toggles textures
	EB_NORMALMAP,		// cycles normal maps
	EB_MIPMAP,			// toggles mipmapping
	EB_LIGHTING,		// toggles lighting

	// Not recognized
	EB_UNKNOWN = 0xFFFFFFFF
} eEBKeyAction;

#define EBEFFECT_DIRTY_PUBLICSTATE   (1 << 0)
#define EBEFFECT_DIRTY_PIXELSHADERS  (1 << 1)
#define EBEFFECT_DIRTY_VERTEXSHADERS (1 << 2)
#define EBEFFECT_DIRTY_REGCOMBSTATE  (1 << 3)

// Defined below
//
extern "C" __declspec(dllexport) void GetEffectApiVersion(unsigned int*, unsigned int*);


////////////////////////////////////////////////////////////////////////////////
// EBEffect class definition

class IEBEffect : public IPropertySet
{
public:

	// Destroy this effect (deletes it from memory)	
	virtual void DestroyEffect() = 0;

	// This method informs the browser of which graphics API the effect supports
	// It returns D3D by default so that all of the current D3D
	// effects will continue to function without code changes.
	//
	virtual EBGFXAPI API() const = 0;

	// Must override 
	virtual HRESULT ConfirmDevice(nv_renderdevice::INVRenderDevice* pRenderDevice) = 0;
	virtual HRESULT Initialize(nv_renderdevice::INVRenderDevice* pRenderDevice) = 0;
	virtual HRESULT Free() = 0;
	virtual HRESULT Start() = 0;
	virtual HRESULT Tick(EBTimer* pTimer) = 0;

	// May override for mouse/keyboard messages
	virtual void MouseMove(HWND hWnd, int x, int y) = 0;
	virtual void MouseButton(HWND hWnd, eButtonID Button, bool bDown, int x, int y) = 0;

	// Override either one of these, not both
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown, int x, int y) = 0;
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown) = 0;

	// Window resizing callback.  The default implementation does two things.  It 
	// saves the window's width, height and aspect in local member variables for
	// use by derived classes.  By default it also updates the graphics viewport
	// for OpenGL.  If you wish to call the parent class but not update the
	// viewport, pass false in the fourth argument (currently unimplemented, see
	// below).
	//
	// You are guaranteed to get at least one Resize call after Initialize() and
	// before Tick().
	//
	virtual void Resize(HWND hWnd, int w, int h, bool fUpdateViewport) = 0;

	// Methods to get the DLL location
	virtual HMODULE GetModuleHandle() = 0;

	// API version related methods
	virtual unsigned int GetEffectApiMajorVersion() = 0;
	virtual unsigned int GetEffectApiMinorVersion() = 0;

	/*
    virtual bool GetCurrentCaps(D3DCAPS8 *caps) = 0;

	// Load a .vso or .pso file and create a D3D vertex or pixel shader for it
	// Moved back to protected as there are global funcs for this
	// effect_api::LoadAndCreateShader( pDev, ..);
	virtual HRESULT LoadAndCreateShader(const std::string& strFilePath, const DWORD* pDeclaration,
		                                DWORD Usage, EBSHADERTYPE ShaderType, DWORD* pHandle) = 0;

	// This function tries really hard to find the file you have asked for.
	// It looks in the current .dll's directory, it's media directory, 
	// the source process directory (nveffectsbrowser), and it's media directory.
	// ** 
	// There is also global function for this:
	// namespace effect_api::GetFilePath(..);
	//
	static std::string GetFilePath(const std::string& strFileName)
	{
		// This is just a convenient accessor to the real method
		return effect_api::GetFilePath(strFileName);
	}

	// Accessors for width/height/aspect of window to subclasses
	UINT  GetWndWidth() const  { return m_iWidth; };
	UINT  GetWndHeight() const { return m_iHeight; };
	float GetWndAspect() const { return m_fAspect; };
	*/
};

typedef void (*LPGETAPIVERSION)(unsigned int*, unsigned int*);
typedef void (*LPSETSCANOPTS)(bool, const EBString&);
typedef void (*LPGETSCANOPTS)(bool&, EBString&);

#endif // __I_EBEffect_H

