/*********************************************************************NVMH4****
Path:  SDK\DEMOS\Direct3D9\inc\shared
File:  NVBEffect9.h

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


#ifndef NVBEFFECT_H
#define NVBEFFECT_H

#include <NVEffectsBrowser/eb_effect9.h>

#include <string>
#include <vector>
#pragma warning(disable: 4786)
#include <cg/cgD3D9.h>
#include <shared/MouseUI9.h>
#include <shared/NVFileDialog.h>
#include <shared/NVBScene9.h>
#include <shared/NV_Error.h>

class NVBEffect : public EBEffect {
public:
	// Application
	NVBEffect();
	~NVBEffect();
	virtual HRESULT Free();
	virtual HRESULT Initialize(IDirect3DDevice9*);
	void Reset();
	virtual HRESULT Start();
	virtual HRESULT Tick(EBTimer*);
	HRESULT DrawPointLight();
	HRESULT DrawDirectionLight();

	// Geometry
	static const std::string m_DefaultSceneFilename;
	std::string m_SceneFilename;
	NVBScene* m_Scene;
	NVBScene* m_Light;
	
	// Textures
	HRESULT LoadTexture(const std::string&, LPDIRECT3DTEXTURE9&, D3DPOOL pool = D3DPOOL_DEFAULT, D3DXIMAGE_INFO* info = 0);
	HRESULT LoadTexture(const std::string&, LPDIRECT3DCUBETEXTURE9&, D3DXIMAGE_INFO* info = 0);
	HRESULT LoadTextures();
	void ReleaseTextures();
	std::string m_DiffuseMapFilename;
	LPDIRECT3DTEXTURE9 m_DiffuseMap;
	std::string m_HeightMapFilename;
	LPDIRECT3DTEXTURE9 m_HeightMap;
	std::string m_NormalMapFilename;
	LPDIRECT3DTEXTURE9 m_NormalMap;
	std::string m_CubeMapFilename;
	LPDIRECT3DCUBETEXTURE9 m_CubeMap;

	// Shaders
	static const std::string m_SimpleTransformCode;
	HRESULT LoadShaders();
	CGcontext m_Context;
	CGprogram m_SimpleTransformVS;
	CGparameter m_SimpleTransformVSWorldViewProj;
	CGparameter m_SimpleTransformVSColor;
	IDirect3DVertexDeclaration9* m_pDeclaration;

	// Transforms
	D3DXMATRIX m_View;
	D3DXMATRIX m_Projection;
	D3DXMATRIX m_World;
	D3DXVECTOR3 m_LightPosition;
	D3DXVECTOR3 m_LightDirection;

	// Errors
	static void CgErrorCallback();
	CGerror CgCheckError();

	// GUI
	//parameters
	bool m_UseDefaultDiffuseMap;
	bool m_UseDefaultNormalMap;
	bool m_Wireframe;
	bool m_DrawLight;
	bool m_DrawAxis;
	bool m_DrawNormals;
	bool m_DrawTangentBasis;
	bool m_PauseScene;
	bool m_PauseCamera;
	bool m_PauseLight;
	float m_Ambient;
	float m_Time;
	unsigned int m_Key;
	float m_SubKey;
	//constant to customize a bit the interface
	static const unsigned int NO_DIFFUSEMAP;
	static const unsigned int NO_HEIGHTMAP;
	static const unsigned int NO_NORMALMAP;
	static const unsigned int NO_CUBEMAP;
	static const unsigned int NO_SCENE;
	static const unsigned int NO_AMBIENT;
	//keyboard interface
#define SHORTCUT_VALUE_TO_INT(key, modifier) (((modifier) << 16) | (key))
#define INT_TO_KEY(value) ((value) & 0xffff)
#define INT_TO_MODIFIER(value) ((value) >> 16)
	class KeyShortcut {
	public:
		KeyShortcut() : m_Key(0), m_Modifier(0) {}
		KeyShortcut(unsigned int value) : m_Key(INT_TO_KEY(value)), m_Modifier(INT_TO_MODIFIER(value)) { }
		operator unsigned int() const { return SHORTCUT_VALUE_TO_INT(m_Key, m_Modifier); }
		static const KeyShortcut m_Invalid;
		unsigned short m_Key;
		unsigned short m_Modifier;
		static KeyShortcut FromVirtualKey(unsigned int);
		std::string ToString();
	};
	virtual void Keyboard(DWORD, UINT, bool);
	virtual void Keyboard(KeyShortcut);
	//menu interface
	class MenuItem {
	public:
		MenuItem(const std::string&, const std::string&, KeyShortcut);
		MenuItem(const std::string&, const std::string&, KeyShortcut, KeyShortcut);
		std::string m_Name;
		std::string m_Description;
		std::vector<KeyShortcut> m_KeyShortcut;
		virtual void Build(NVBEffect&);
		virtual std::string Description();
		virtual bool Update(unsigned int) const;
	protected:
		void BuildName(const std::string&);
	};
	class MenuItemBoolean : public MenuItem {
	public:
		MenuItemBoolean(const std::string&, const std::string&, KeyShortcut, bool*);
		MenuItemBoolean(const std::string&, const std::string&, KeyShortcut, KeyShortcut, bool*);
		virtual void Build(NVBEffect&);
		virtual bool Update(unsigned int) const;
		bool* m_Value;
	};
	class MenuItemEnumerant : public MenuItem {
	public:
		MenuItemEnumerant(const std::string&, const std::string&, KeyShortcut, unsigned int*);
		MenuItemEnumerant(const std::string&, const std::string&, KeyShortcut, KeyShortcut, unsigned int*);
		void Add(unsigned int, const std::string&, const std::string&, KeyShortcut);
		virtual void Build(NVBEffect&);
		virtual std::string Description();
		virtual bool Update(unsigned int) const;
		unsigned int* m_Value;
		std::vector<unsigned int> m_EnumerantValue;
		std::vector<std::string> m_EnumerantLabel;
		std::vector<std::string> m_EnumerantDescription;
		std::vector<KeyShortcut> m_EnumerantKeyShortcut;
	};
	void AddMenuItem(const std::string&, const std::string&, KeyShortcut);
	void AddMenuItem(const std::string&, const std::string&, KeyShortcut, KeyShortcut);
	void AddMenuItem(const std::string&, const std::string&, KeyShortcut, bool*);
	void AddMenuItem(const std::string&, const std::string&, KeyShortcut, KeyShortcut, bool*);
	MenuItemEnumerant* AddMenuItem(const std::string&, const std::string&, KeyShortcut, unsigned int*);
	MenuItemEnumerant* AddMenuItem(const std::string&, const std::string&, KeyShortcut, KeyShortcut, unsigned int*);
	virtual void UpdateProperties();
	virtual void CreateMenu(unsigned int option = 0);
	virtual void PropertyUpdateCallback(const EBProperty*, bool);
	//mouse interface
	MouseUI* NewMouseUI(const std::string&);
	MouseUI* m_UIScene;
	MouseUI* m_UICamera;
	virtual void MouseButton(HWND, eButtonID, bool, int, int);
	virtual void MouseMove(HWND, int, int);
private:
	//keyboard interface
	static const unsigned int m_LoadDiffuseMapShortcut;
	static const unsigned int m_LoadHeightMapShortcut;
	static const unsigned int m_LoadNormalMapShortcut;
	static const unsigned int m_LoadCubeMapShortcut;
	static const unsigned int m_ResetSceneShortcut;
	static const unsigned int m_LoadSceneShortcut;
	static const unsigned int m_IncreaseAmbientShortcut;
	static const unsigned int m_DecreaseAmbientShortcut;
	static const unsigned int m_WireframeShortcut;
	static const unsigned int m_DrawLightShortcut;
	static const unsigned int m_DrawAxisShortcut;
	static const unsigned int m_UseDefaultDiffuseMapShortcut;
	static const unsigned int m_UseDefaultNormalMapShortcut;
	static const unsigned int m_DrawNormalsShortcut;
	static const unsigned int m_DrawTangentBasisShortcut;
	static const unsigned int m_PauseSceneShortcut;
	static const unsigned int m_PauseCameraShortcut;
	static const unsigned int m_PauseLightShortcut;
	static const unsigned int m_PauseShortcut0;
	static const unsigned int m_PauseShortcut1;
	static const unsigned int m_HelpShortcut0;
	static const unsigned int m_HelpShortcut1;
	static const float m_DefaultAmbient;
	static const float m_AmbientStep;
	//menu interface
	std::vector<MenuItem*> m_MenuItemList;
	//mouse interface
protected:
	class NamedMouseUI {
	public:
		NamedMouseUI(const RECT&, const std::string&);
		MouseUI* m_UI;
		std::string m_Name;
	};
	unsigned int m_CurrentMouseUI;
	std::vector<NamedMouseUI> m_MouseUIList;
};

#endif
