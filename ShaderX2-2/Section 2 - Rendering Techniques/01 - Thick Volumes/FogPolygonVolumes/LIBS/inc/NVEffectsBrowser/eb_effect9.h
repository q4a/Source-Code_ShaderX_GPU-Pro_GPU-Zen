/*********************************************************************NVMH4****
Path:  SDK\DEMOS\common\inc\NVEffectsBrowser
File:  eb_effect9.h

Copyright NVIDIA Corporation 2002
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
*AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.



Comments:
      
        
******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
//
// EBEffect9.h: interface for the EBEffect class
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __EBEFFECT9_H
#define __EBEFFECT9_H

#include "ieb_effect.h"
#include <windows.h>
#include <NVEffectsBrowser/Registry.h>
#include <shared/NV_Common.h>

#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>

#pragma warning(disable: 4786)
#include <string.h>
#include <stdarg.h>

#include <NVEffectsBrowser/eb_string.h>
#include <NVEffectsBrowser/eb_timer.h>
#include <NVEffectsBrowser/eb_property.h>
#include <NVEffectsBrowser/eb_file9.h>

#include <nv_renderdevice9/invrenderdevice_d3d9.h>

////////////////////////////////////////////////////////////////////////////////
// EBEffect public function prototypes and macros
//

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
#define DECLARE_MAIN(effectClass, width, height)		\
  	DECLARE_EFFECT_MAIN()								\
  	DECLARE_EFFECT_COUNT(1)								\
  	DECLARE_EFFECT_CREATE_BEG()							\
  	DECLARE_EFFECT_CREATE(0, effectClass())				\
  	DECLARE_EFFECT_CREATE_END()
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



// Defined below
//
extern "C" __declspec(dllexport) void GetEffectApiVersion(unsigned int*, unsigned int*);


////////////////////////////////////////////////////////////////////////////////
// EBEffect class definition

class EBEffect : public EBPropertySet<IEBEffect>
{
public:
	EBEffect::EBEffect()
	  : m_pD3DDev(NULL),
		m_strEffectName("Change EffectName property"),
		m_strEffectLocation("Change EffectLocation property"),
		m_strEffectVertexShader("Change EffectVertexShader property"),
		m_strEffectPixelShader("Change EffectPixelShader property"),
		m_strEffectVertexProgram("Change EffectVertexProgram property"),
		m_strEffectTextureShader("Change EffectTextureShader property"),
		m_strEffectRegCombState("Change EffectRegisterCombiner property"),
        m_strEffectVertexCg("Change EffectVertexCg property"),
        m_strEffectPixelCg("Change EffectPixelCg property"),
		m_strLastError(""),
		m_nAboutLines(0), 
		m_pAboutLabelEnum(NULL), m_pAboutValueEnum(NULL), m_pAboutLinkEnum(NULL),
		m_idAboutIcon(-1),
		m_dwEffectDirtyFlags(0),
#if STANDALONE
		m_WindowHidden(false),
#endif
#ifdef _DEBUG
        m_bDebugEffect(true),
#else
        m_bDebugEffect(false),
#endif
        m_bEffectEnabled(true)
	{
		GetEffectApiVersion(&m_apiMajorVersion, &m_apiMinorVersion);
		m_hModule = hModule;
	}

	virtual ~EBEffect()
		{}

#if STANDALONE
	// C
	int Main(HINSTANCE, unsigned int, unsigned int);

	// EB
	EBTimer m_Timer;

	// Window
	bool m_WindowHidden;
	HWND InitInstance(HINSTANCE, unsigned int, unsigned int);
	void Render();

	// D3D
	LPDIRECT3DDEVICE8 InitD3D(HWND);
#endif

	virtual HRESULT ConfirmDevice(nv_renderdevice::INVRenderDevice* pRenderDevice)
	{
		HRESULT hr = E_FAIL;

		nv_renderdevice::INVRenderDevice_D3D9* pRenderDeviceD3D9 = NULL;
		if (!pRenderDevice->GetInterface(IID_INVRenderDevice_D3D9, (void**)&pRenderDeviceD3D9))
		{
			m_strLastError = "Failed to get D3D device interface";
			return E_FAIL;
		}
		IDirect3DDevice9* device;
		if (!pRenderDeviceD3D9->GetDevice(&device))
		{
			m_strLastError = "Failed to get D3D device interface";
			return E_FAIL;
		}

		D3DCAPS9 Caps;
		device->GetDeviceCaps(&Caps);
		hr = ConfirmDevice(&Caps, 0, D3DFMT_UNKNOWN);	
		SAFE_RELEASE(device);
		SAFE_RELEASE(pRenderDeviceD3D9);

		return hr;
	}

	virtual HRESULT Initialize(nv_renderdevice::INVRenderDevice* pRenderDevice)
	{
		HRESULT hr;
		nv_renderdevice::INVRenderDevice_D3D9* pRenderDeviceD3D9 = NULL;
		if (!pRenderDevice->GetInterface(IID_INVRenderDevice_D3D9, (void**)&pRenderDeviceD3D9))
		{
			m_strLastError = "Failed to get D3D device interface";
			return E_FAIL;
		}

		IDirect3DDevice9* device;
		if (!pRenderDeviceD3D9->GetDevice(&device))
		{
			m_strLastError = "Failed to get D3D device interface";
			return E_FAIL;
		}

		hr = Initialize(device);	

		SAFE_RELEASE(device);
		SAFE_RELEASE(pRenderDeviceD3D9);

		return hr;
	}

	// Must override for D3D
	virtual HRESULT ConfirmDevice(D3DCAPS9* caps, DWORD behavior, D3DFORMAT)
	{
		return E_FAIL;
	}
	virtual HRESULT Initialize(IDirect3DDevice9* pDevice)
	{
		return E_FAIL;
	}

	// Destroy this effect (deletes it from memory)	
	virtual void DestroyEffect()
		{ delete this; }

	//////////////////////////////////////////////////////////////////////////////
	// These methods must be overridden in each effect
	//////////////////////////////////////////////////////////////////////////////
public:

	// This method informs the browser of which graphics API the effect supports
	//
	// It returns D3D by default so that all of the current D3D
	// effects will continue to function without code changes.
	//
	virtual EBGFXAPI API() const
    { 
        t_listEnum vertEnums = m_pVertexCgEnum->GetEnumerants();
        t_listEnum pixEnums = m_pPixelCgEnum->GetEnumerants();

        if (vertEnums.empty() && pixEnums.empty())
            return GFXAPI_D3D9; 
        else
            return GFXAPI_D3D9CG; 
    };

	//////////////////////////////////////////////////////////////////////////////
	// These methods may be overridden in each effect
	//////////////////////////////////////////////////////////////////////////////
public:

	// May override for mouse/keyboard messages
	virtual void MouseMove(HWND hWnd, int x, int y)
		{ return; }
	virtual void MouseButton(HWND hWnd, eButtonID Button, bool bDown, int x, int y)
		{ return; }

	// Override either one of these, not both
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown, int x, int y)
		{ Keyboard(dwKey, nFlags, bDown); }
	virtual void Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
		{ return; }

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
	virtual void Resize(HWND hWnd, int w, int h, bool fUpdateViewport)
	{
		if ((w <= 0) || (h <= 0))
			return;
		
		m_iWidth  = (UINT) w;
		m_iHeight = (UINT) h;
		m_fAspect = ((float) w ) / ((float) h);
	};

	// Update the properties for this effect (must call this base class version if
	// you inherit from it).  The default properties listed below are the 'interface'
	// to the effects browser.  It's easy to extend the interface to the browser by
	// exporting new properties from the EBEffect and having the browser recognise
	// them - this doesn't break the class interface between the two.
	//
	virtual void UpdateProperties()
	{	
		// Discard existing properties
		EBPropertySet<IEBEffect>::UpdateProperties();
		m_nAboutLines = 0;

		// Add the effect name, location, and effect version (not the API version)
		AddProperty(new EBProperty("EffectName", OBJECT_MEMBER(m_strEffectName), 
			EBTYPE_STRING_PROP, PROPFLAG_PRIVATE));
		AddProperty(new EBProperty("EffectLocation", OBJECT_MEMBER(m_strEffectLocation), 
			EBTYPE_STRING_PROP, PROPFLAG_PRIVATE));
		AddProperty(new EBProperty("EffectVersion", OBJECT_MEMBER(m_strEffectVersion), 
			EBTYPE_STRING_PROP, PROPFLAG_PRIVATE));
		AddProperty(new EBProperty("EffectAboutIcon", OBJECT_MEMBER(m_idAboutIcon), 
			EBTYPE_DWORD_PROP, PROPFLAG_PRIVATE));
		AddProperty(new EBProperty("EffectEnabled", OBJECT_MEMBER(m_bEffectEnabled), 
			EBTYPE_BOOL_PROP, PROPFLAG_PRIVATE));
		AddProperty(new EBProperty("DebugEffect", OBJECT_MEMBER(m_bDebugEffect), 
			EBTYPE_BOOL_PROP, PROPFLAG_PRIVATE));

		// Add the administrative properties
		AddProperty(new EBProperty("LastError", OBJECT_MEMBER(m_strLastError),
			EBTYPE_STRING_PROP, PROPFLAG_PRIVATE));
		AddProperty(new EBProperty("EffectDirtyFlags", OBJECT_MEMBER(m_dwEffectDirtyFlags),
			EBTYPE_DWORD_PROP, PROPFLAG_PRIVATE));

		// Add the additional effect About information (optionally can be set with SetAboutInfo())
		m_pAboutLabelEnum = new EBEnumProperty("EffectAboutLabel",
			/* no default member */ (DWORD) -1, EBTYPE_STRING_PROP, PROPFLAG_PRIVATE);
		AddProperty(m_pAboutLabelEnum);
		m_pAboutValueEnum = new EBEnumProperty("EffectAboutValue",
			/* no default member */ (DWORD) -1, EBTYPE_STRING_PROP, PROPFLAG_PRIVATE);
		AddProperty(m_pAboutValueEnum);
		m_pAboutLinkEnum  = new EBEnumProperty("EffectAboutLink",
			/* no default member */ (DWORD) -1, EBTYPE_STRING_PROP, PROPFLAG_PRIVATE);
		AddProperty(m_pAboutLinkEnum);

		// D3D vertex/pixel shader info
		m_pVertexShaderEnum = new EBEnumProperty("EffectVertexShader", 
			OBJECT_MEMBER(m_strEffectVertexShader),	EBTYPE_STRING_PROP,
			PROPFLAG_PRIVATE | PROPFLAG_CALLBACK);
		AddProperty(m_pVertexShaderEnum);
		m_pPixelShaderEnum = new EBEnumProperty("EffectPixelShader", 
			OBJECT_MEMBER(m_strEffectPixelShader), EBTYPE_STRING_PROP, 
			PROPFLAG_PRIVATE | PROPFLAG_CALLBACK);
		AddProperty(m_pPixelShaderEnum);

		m_pVertexCgEnum = new EBEnumProperty("EffectVertexCg", 
			OBJECT_MEMBER(m_strEffectVertexCg),	EBTYPE_STRING_PROP,
			PROPFLAG_PRIVATE | PROPFLAG_CALLBACK);
		AddProperty(m_pVertexCgEnum);
		m_pPixelCgEnum = new EBEnumProperty("EffectPixelCg", 
			OBJECT_MEMBER(m_strEffectPixelCg), EBTYPE_STRING_PROP, 
			PROPFLAG_PRIVATE | PROPFLAG_CALLBACK);
		AddProperty(m_pPixelCgEnum);

        // OpenGL VP/TS/RC shader info
		m_pVertexProgramEnum = new EBEnumProperty("EffectVertexProgram", 
			OBJECT_MEMBER(m_strEffectVertexProgram), EBTYPE_STRING_PROP,
			PROPFLAG_PRIVATE | PROPFLAG_CALLBACK);
		AddProperty(m_pVertexProgramEnum);
		m_pTextureShaderEnum = new EBEnumProperty("EffectTextureShader", 
			OBJECT_MEMBER(m_strEffectTextureShader), EBTYPE_STRING_PROP,
			PROPFLAG_PRIVATE | PROPFLAG_CALLBACK);
		AddProperty(m_pTextureShaderEnum);
		m_pRegCombStateEnum = new EBEnumProperty("EffectRegisterCombiner", 
			OBJECT_MEMBER(m_strEffectRegCombState), EBTYPE_STRING_PROP,
			PROPFLAG_PRIVATE | PROPFLAG_CALLBACK);
		AddProperty(m_pRegCombStateEnum);
	};


	//////////////////////////////////////////////////////////////////////////////
	// These methods may be used to setup state in the Effect more easily
	//////////////////////////////////////////////////////////////////////////////
protected:

	// Sets up the effect's About dialog information
	//
	// The current effect About dialog contains up to three lines.  Each call
	// to this function defines one of these lines.  NULL may be legally passed
	// as the label and/or link argument.  If a label and value are both
	// specified the line will be displayed as:
	//
	//   Label:    Value
	// 
	// If just a label is provided, it will be displayed centered on the line.
	// Additionally, in either case, if a link is provided, the value text will
	// be linked to the provided URL.
	//
	// For example, an effect might say:
	//
	//  m_strEffectName     = "SpookyEffect";
	//  m_strEffectLocation = "Foobar Inc.\\Spooky Effect";
	//  m_strEffectVersion  = "1.1a5";
	//  
	//  SetAboutInfo(NULL,          _T("My Effect Page"), _T("http://www.doe.com/john/spookyeffect.html"));
	//  SetAboutInfo(_T("Author"),  _T("John Doe"), _T("http://www.doe.com/john"));
	//  SetAboutInfo(_T("Company"), _T("Foobar Inc."));
	//
	// And the about dialog would be displayed as:
	//
	//  +================================+ 
	//  |                                |
	//  |  SpookyEffect (version 1.1a5)  |
	//  |    (Effect API version 2.0)    |
	//  |                                |
	//  |            *******             |
	//  |            *******             |
	//  |            *******             |
	//  |                                |
	//  |        My Effect Page          |
	//  |  Author:      John Doe         |
	//  |  Company:     Foobar Inc.      |
	//  |                                |
	//  |              O K               |
	//  |                                |
	//  +================================+ 
	//
	// NOTE: the dialog size is not dynamic.  Do not make your labels/values 
	// too long!
	//
	virtual HRESULT SetAboutInfo(LPCTSTR lpLabel, LPCTSTR lpValue, LPCTSTR lpLink=NULL)
	{
		// Make sure that the properties are updated first
		if (!m_pAboutLabelEnum)
			return E_FAIL;

		if (!lpLabel) lpLabel = _T("");
		if (!lpLink)  lpLink  = _T("");

		// Make sure that we don't have too many lines...
		if (m_nAboutLines++ >= 3)
			return E_FAIL;

		EBString strLabel = lpLabel;
		EBString strValue = lpValue;
		EBString strLink  = lpLink;

		m_pAboutLabelEnum->AddEnumerant(new EBEnumValue(m_pAboutLabelEnum, "About Label",
														strLabel, EBTYPE_STRING_PROP));
		m_pAboutValueEnum->AddEnumerant(new EBEnumValue(m_pAboutValueEnum, "About Value",
														strValue, EBTYPE_STRING_PROP));
		m_pAboutLinkEnum->AddEnumerant(new EBEnumValue(m_pAboutLinkEnum, "About Link",
														strLink, EBTYPE_STRING_PROP));

		return S_OK;
	}

	// Set the default pane that will first be selected when this effect is displayed.
	// The title should match the title of one of the shaders added in a call to
	// AddShaderCodeFile() or AddShaderCodeBuffer().
	//
	virtual HRESULT SetDefaultCodePane(EBSHADERTYPE ShaderType, const char *strTitle)
	{
		switch (ShaderType) {
		case SHADERTYPE_VERTEXSHADER:   m_strEffectVertexShader  = strTitle;  break;
        case SHADERTYPE_VERTEXCG:       m_strEffectVertexCg  = strTitle;      break;
		case SHADERTYPE_PIXELSHADER:    m_strEffectPixelShader   = strTitle;  break;
        case SHADERTYPE_PIXELCG:        m_strEffectPixelCg  = strTitle;       break;
		case SHADERTYPE_VERTEXPROG:     m_strEffectVertexProgram = strTitle;  break;
		case SHADERTYPE_TEXTURESHADER:  m_strEffectTextureShader = strTitle;  break;
		case SHADERTYPE_REGCOMBSTATE:   m_strEffectRegCombState  = strTitle;  break;
		default:  return E_FAIL;
		}
		return S_OK;
	}

	// Add a new shader to the list for this effect.  Provide the type of shader, a text
	// title for the tab control, and a file name containing the shader code itself.
	//
	virtual HRESULT AddShaderCodeFile(EBSHADERTYPE ShaderType, const char *strTitle, const char *strFile)
	{
		EBEnumProperty *pEnumProp = NULL;
		switch (ShaderType) {
		case SHADERTYPE_VERTEXSHADER:   pEnumProp = m_pVertexShaderEnum;   break;
		case SHADERTYPE_VERTEXCG:       pEnumProp = m_pVertexCgEnum;       break;
		case SHADERTYPE_PIXELSHADER:    pEnumProp = m_pPixelShaderEnum;    break;
		case SHADERTYPE_PIXELCG:        pEnumProp = m_pPixelCgEnum;        break;
		case SHADERTYPE_VERTEXPROG:     pEnumProp = m_pVertexProgramEnum;  break;
		case SHADERTYPE_TEXTURESHADER:  pEnumProp = m_pTextureShaderEnum;  break;
		case SHADERTYPE_REGCOMBSTATE:   pEnumProp = m_pRegCombStateEnum;   break;
		default:  return E_FAIL;
		}
		if (!pEnumProp)
			return E_FAIL;

		EBEnumValue *pValue = new EBEnumValue(pEnumProp, strTitle, GetFilePath(strFile), EBTYPE_STRING_PROP);
		pEnumProp->AddEnumerant(pValue);

		return S_OK;
	}

	// Add a new shader to the list for this effect.  Provide the type of shader, a text
	// title for the tab control, and the text of the shader itself.
	//
	virtual HRESULT AddShaderCodeBuffer(EBSHADERTYPE ShaderType, const char *strTitle, const char *strBuffer)
	{
		EBEnumProperty *pEnumProp = NULL;
		switch (ShaderType) {
		case SHADERTYPE_VERTEXSHADER:   pEnumProp = m_pVertexShaderEnum;   break;
		case SHADERTYPE_VERTEXCG:       pEnumProp = m_pVertexCgEnum;       break;
		case SHADERTYPE_PIXELSHADER:    pEnumProp = m_pPixelShaderEnum;    break;
		case SHADERTYPE_PIXELCG:        pEnumProp = m_pPixelCgEnum;        break;
		case SHADERTYPE_VERTEXPROG:     pEnumProp = m_pVertexProgramEnum;  break;
		case SHADERTYPE_TEXTURESHADER:  pEnumProp = m_pTextureShaderEnum;  break;
		case SHADERTYPE_REGCOMBSTATE:   pEnumProp = m_pRegCombStateEnum;   break;
		default:  return E_FAIL;
		}
		if (!pEnumProp)
			return E_FAIL;

		// Prepend to a '-' to signify that this is not a file name, but the code itself...
		std::string strCode = "-";
		strCode += strBuffer;

		EBEnumValue *pValue = new EBEnumValue(pEnumProp, strTitle, strCode, EBTYPE_STRING_PROP);
		pEnumProp->AddEnumerant(pValue);

		return S_OK;
	}


	// This method can be used to translate a key code into an action
	//
	virtual eEBKeyAction TranslateEffectKey(DWORD dwKeyCode, DWORD nFlags, bool bDown)
	{
		if (!bDown)
			return EB_UNKNOWN;
		
		switch (dwKeyCode) {
		case 'H':
		case VK_F1:			return EB_HELP;
		case 'W':			return EB_WIREFRAME;
		case VK_HOME :
		case VK_NUMPAD7 :
		case '7' :			return EB_RESET;
		case VK_ESCAPE:		return EB_QUIT;
		case VK_F12:
		case VK_TAB:		return EB_HUD;
		case '`':
		case '¬':			return EB_CONSOLE;
		case VK_PAUSE:
		case VK_SPACE:		return EB_PAUSE;
		case VK_ADD:
		case 0xBB:
		case '+':
		case '=':			return EB_ADD;
		case VK_SUBTRACT:
		case 0xBD:
		case '-':
		case '_':			return EB_SUBTRACT;
		case VK_MULTIPLY:	return EB_MULTIPLY;
		case VK_DIVIDE:		return EB_DIVIDE;
		case VK_PRIOR:		return EB_PAGEUP;
		case VK_NEXT:		return EB_PAGEDOWN;
		case 'P':			return EB_POINTS;
		case 'C':			return EB_CUBEMAP;
		case 'T':			return EB_TEXTURES;
		case 'N':			return EB_NORMALMAP;
		case 'M':			return EB_MIPMAP;
		case 'L':			return EB_LIGHTING;
		default:			return EB_UNKNOWN;
		}
	};

	
	//////////////////////////////////////////////////////////////////////////////
	// These methods should not be changed
	// They are public for convenient access.
	//////////////////////////////////////////////////////////////////////////////
public:

	// Methods to get the DLL location
	HMODULE GetModuleHandle() { return m_hModule; }
	static HMODULE hModule;

	// API version related methods
	unsigned int GetEffectApiMajorVersion() { return m_apiMajorVersion; }
	unsigned int GetEffectApiMinorVersion() { return m_apiMinorVersion; }



protected:
/*	// Load a .vso or .pso file and create a D3D vertex or pixel shader for it
	// Moved back to protected as there are global funcs for this
	// effect_api::LoadAndCreateShader( pDev, ..);
	virtual HRESULT LoadAndCreateShader(const std::string& strFilePath, const DWORD* pDeclaration,
		                                DWORD Usage, EBSHADERTYPE ShaderType, DWORD* pHandle)
	{
		HRESULT hr;
		hr = effect_api::LoadAndCreateShader(m_pD3DDev, strFilePath, pDeclaration, Usage, ShaderType, pHandle);
		m_strLastError = effect_api::strLastError;
		return hr;
	};
	*/

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

	UINT   m_iWidth, m_iHeight;
	float  m_fAspect;

	LPDIRECT3DDEVICE9 m_pD3DDev;

	EBString m_strEffectName;			// A string holding the name of the effect
	EBString m_strEffectLocation;		// A string holding the location of the effect in the tree
	EBString m_strEffectLink;			// A string holding the URL link for this effect
	EBString m_strEffectVersion;		// A string holding the version of this effect
										// NOTE: this is not the Effect API version (see above)
	DWORD    m_idAboutIcon;             // The ID of the icon for the about box...

	EBString m_strLastError;			// A string holding the last error the effect had

	// Properties for About dialog
	int m_nAboutLines;
	EBEnumProperty *m_pAboutLabelEnum;
	EBEnumProperty *m_pAboutValueEnum;
	EBEnumProperty *m_pAboutLinkEnum;
	
	// Enumerated properties for pixel shaders
	EBEnumProperty* m_pVertexShaderEnum;
	EBEnumProperty* m_pPixelShaderEnum;
	EBEnumProperty* m_pVertexProgramEnum;
	EBEnumProperty* m_pTextureShaderEnum;
	EBEnumProperty* m_pRegCombStateEnum;
    EBEnumProperty* m_pVertexCgEnum;
    EBEnumProperty* m_pPixelCgEnum;

	EBString m_strEffectVertexShader;		// A string holding the current vertex shader
	EBString m_strEffectPixelShader;		// A string holding the current pixel shader
	EBString m_strEffectVertexProgram;		// A string holding the current vertex program
	EBString m_strEffectTextureShader;		// A string holding the current texture shader
	EBString m_strEffectRegCombState;		// A string holding the current register combiner
    EBString m_strEffectVertexCg;           // A string holding the current Cg vertex shader
    EBString m_strEffectPixelCg;            // A string holding the current Cg pixel shader

    DWORD m_dwEffectDirtyFlags;			// Flags to show dirty state.

	HMODULE      m_hModule;
	unsigned int m_apiMajorVersion;
	unsigned int m_apiMinorVersion;

    bool m_bDebugEffect;
    bool m_bEffectEnabled;
};


////////////////////////////////////////////////////////////////////////////////
// EBEffect private prototypes and macros
//
// These prototypes and macros are used internally or by the browser.  Effect
// developer's shouldn't need to worry about them.


#define __DECLARE_EFFECT_STATICS()											\
bool effect_api::Rescanning;												\
EBString    effect_api::EffectState = "";									\
std::string effect_api::strStartPath = "";									\
HMODULE EBEffect::hModule;													\
std::string effect_api::strLastError = "";

#define __DECLARE_EFFECT_SCANFUNCS()										\
extern "C" {																\
	__declspec(dllexport) void												\
	SetScanOpts(bool rescanning, const EBString &state)						\
	{																		\
		effect_api::Rescanning = rescanning;								\
		effect_api::EffectState = state;									\
	}																		\
	__declspec(dllexport) void												\
	GetScanOpts(bool &rescanning, EBString &state)							\
	{																		\
		rescanning = effect_api::Rescanning;								\
		state = effect_api::EffectState;									\
	}																		\
}

#define __DECLARE_EFFECT_APIVERSION()										\
extern "C" {																\
	__declspec(dllexport) void												\
	GetEffectApiVersion(unsigned int *major, unsigned int *minor)			\
	{																		\
		*major = EBEFFECT_APIVERSION_MAJOR;									\
		*minor = EBEFFECT_APIVERSION_MINOR;									\
	}																		\
}

#define __DECLARE_EFFECT_DLLMAIN()											\
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ulReason, LPVOID lpReserved)	\
{																			\
	switch (ulReason)														\
	{																		\
		case DLL_PROCESS_ATTACH:											\
			{																\
				std::string strProcessPath;									\
				strProcessPath.resize(MAX_PATH);							\
				DWORD dwReturnSize = GetModuleFileName((HINSTANCE) hModule,	\
													   &strProcessPath[0],	\
													   MAX_PATH);			\
				assert(dwReturnSize <= MAX_PATH); /*Should never happen*/	\
				effect_api::SetModulePath(strProcessPath);					\
																			\
				EBEffect::hModule = (HMODULE) hModule;						\
			}																\
		case DLL_THREAD_ATTACH:												\
		case DLL_THREAD_DETACH:												\
		case DLL_PROCESS_DETACH:											\
			break;															\
    }																		\
    return TRUE;															\
}

#endif // __EBEffect_H

