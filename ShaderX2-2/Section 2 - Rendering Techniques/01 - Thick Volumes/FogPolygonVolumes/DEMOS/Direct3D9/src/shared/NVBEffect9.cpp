/*********************************************************************NVMH4****
Path:  SDK\DEMOS\Direct3D9\src\shared
File:  NVBEffect9.cpp

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

#include <shared/NVBEffect9.h>

/*******************************************************************************

	Application management

*******************************************************************************/

NVBEffect::NVBEffect() :
	m_Scene(0),
	m_Light(0),
	m_DiffuseMap(0),
	m_HeightMap(0),
	m_NormalMap(0),
	m_CubeMap(0),
	m_UIScene(0),
	m_UICamera(0),
	m_Wireframe(false),
	m_DrawLight(false),
	m_DrawAxis(false),
	m_UseDefaultDiffuseMap(false),
	m_UseDefaultNormalMap(false),
	m_DrawNormals(false),
	m_DrawTangentBasis(false),
	m_Time(1),
	m_pDeclaration(NULL),
	m_Key(1),
	m_SubKey(0),
	m_CurrentMouseUI(1),
	m_PauseScene(false),
	m_PauseCamera(false),
	m_PauseLight(false),
	m_Ambient(m_DefaultAmbient)
{
}

NVBEffect::~NVBEffect()
{	
	Free();	
}

HRESULT NVBEffect::Free()
{
	ReleaseTextures();
	SAFE_DELETE(m_Scene);
	SAFE_DELETE(m_Light);
	SAFE_RELEASE(m_pDeclaration);
	for (unsigned int i = 0; i < m_MouseUIList.size(); ++i)
		SAFE_DELETE(m_MouseUIList[i].m_UI);
	m_MouseUIList.clear();
	for (unsigned int j = 0; j < m_MenuItemList.size(); ++j)
		SAFE_DELETE(m_MenuItemList[j]);
	m_MenuItemList.clear();
	SAFE_RELEASE(m_pD3DDev);
	return S_OK;
}

/*******************************************************************************

	Main initialization

*******************************************************************************/

HRESULT NVBEffect::Initialize(IDirect3DDevice9* device)
{
	HRESULT hr;

	NVBEffect::Free();

	// Device
	m_pD3DDev = device;
	device->AddRef();

	// Geometry
	// Scene
	m_Scene = new NVBScene;
	if (FAILED(hr = m_Scene->Load(m_SceneFilename, m_pD3DDev, GetFilePath))) {
		m_strLastError = "Failed to load the scene: " + m_Scene->m_ErrorMessage;
		return hr;
	}

	// Textures
	if (FAILED(hr = LoadTextures())) {
		m_strLastError = "Failed to load textures";
		return hr;
	}

	// Cg
	cgD3D9SetDevice(device);
	m_Context = cgCreateContext();
	cgSetErrorCallback(CgErrorCallback);

	// Shaders
	if (FAILED(hr = LoadShaders())) {
		m_strLastError = "Failed to load shaders";
		return hr;
	}

	// GUI
	m_UIScene = NewMouseUI("scene");
	m_UICamera = NewMouseUI("camera");

	// Reset animation, transforms and user interface
	Reset();

	return S_OK;
}

void NVBEffect::Reset()
{
	// Animation
	if (m_Scene->m_NumMeshKeys == 0)
		m_PauseScene = true;
	if (!m_Scene->m_NumCameras || (m_Scene->m_NumCameraKeys == 0))
		m_PauseCamera = true;
	if (m_Scene->m_NumLights && (m_Scene->m_NumLightKeys == 0))
		m_PauseLight = true;

	// Light position and direction
	m_LightPosition.x = 0;
	m_LightPosition.y = 0.6f * m_Scene->m_Radius;
	m_LightPosition.z = - 0.42f * m_Scene->m_Radius;
	m_LightDirection = - m_LightPosition;
	D3DXVec3Normalize(&m_LightDirection, &m_LightDirection);

	// View and projection
	float viewRadius = 1.5f * m_Scene->m_Radius;
	if (m_Scene->m_NumCameras) {
		m_View = m_Scene->m_Cameras[0].m_View;
		m_Projection = m_Scene->m_Cameras[0].m_Projection;
	}
	else {
		D3DXVECTOR3 eyePt(0.0f, 0.0f, - viewRadius);
		D3DXVECTOR3 lookatPt(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMatrixLookAtLH(&m_View, &eyePt, &lookatPt, &up);
		float zFar = 10 * viewRadius;
		float zNear = zFar / 500.0f;
		D3DXMatrixPerspectiveFovLH(&m_Projection, D3DXToRadian(60.0f), 1.0f, zNear, zFar);
	}
	
	// Mouse UI
	float translationalSensibility = 0.1f * m_Scene->m_Radius;
	m_UIScene->SetViewMode(MouseUI::VM_CAMERA_AWARE);
	D3DXMatrixTranslation(&m_World, - m_Scene->m_Center.x, - m_Scene->m_Center.y, - m_Scene->m_Center.z);
	m_UIScene->SetTranslationMatrix(m_World);
	m_UIScene->SetTranslationalSensitivityFactor(translationalSensibility);
	m_UICamera->SetTranslationalSensitivityFactor(translationalSensibility);
	m_UICamera->SetMatrix(m_View);
}

HRESULT NVBEffect::Start()
{
	m_Time = 1;
	m_Key = 1;
	m_SubKey = 0;
	return S_OK;
}

/*******************************************************************************

	Simulation

*******************************************************************************/

HRESULT NVBEffect::Tick(EBTimer* timer)
{
	HRESULT hr = S_OK;
	
	// Update scene
	if (m_PauseScene)
		m_World = m_UIScene->GetRotationMatrix() * m_UIScene->GetTranslationMatrix();

	if (!m_PauseScene || !m_PauseCamera || !m_PauseLight)
		m_Time += 30.0f * timer->GetSecsPerFrame();

	m_Scene->Update(m_Time, &m_World, (m_PauseScene ? 0 : NVBScene::MESH) | (m_PauseCamera ? 0 : NVBScene::CAMERA) | (m_PauseLight ? 0 : NVBScene::LIGHT));

	// Update view matrix
	if (m_PauseCamera || !m_Scene->m_NumCameras)
		m_View = m_UICamera->GetRotationMatrix() * m_UICamera->GetTranslationMatrix();
	else {
		m_View = m_Scene->m_Cameras[0].m_View;
		m_Projection = m_Scene->m_Cameras[0].m_Projection;
		m_UICamera->SetMatrix(m_View);
	}
	m_UIScene->SetControlOrientationMatrix(m_View);

	// Update light position
	if (!m_PauseLight)
		if (m_Scene->m_NumLights && m_Scene->m_NumLightKeys) {
			m_LightPosition = m_Scene->m_Lights[0].Position;
			m_LightDirection = m_Scene->m_Lights[0].Direction;
		}
		else {
			float angle = atan2f(m_LightPosition.x, m_LightPosition.y) + timer->GetSecsPerFrame();
			float lightDistance = - 1.4f * m_LightPosition.z;
			m_LightPosition.x = m_Scene->m_Radius * sinf(angle);
			m_LightPosition.y = m_Scene->m_Radius * cosf(angle);
			m_LightDirection = - m_LightPosition;
			D3DXVec3Normalize(&m_LightDirection, &m_LightDirection);
		}

	return hr;
}

/*******************************************************************************

	Rendering

*******************************************************************************/

HRESULT NVBEffect::DrawPointLight()
{
	if (!m_Light) {
		// Load light model if not done yet
		m_Light = new NVBScene;
		HRESULT hr;
		if (FAILED(hr = m_Light->Load("pointLight.nvb", m_pD3DDev, GetFilePath))) {
			m_strLastError = "Failed to load the point light model";
			return hr;
		}
	}

	// Scale
	float scale = 0.1f * m_Scene->m_Radius / m_Light->m_Radius;
	D3DXMATRIX world;
	D3DXMatrixScaling(&world, scale, scale, scale);

	// Translate to offset from the object
	D3DXMATRIX translation;
	D3DXMatrixTranslation(&translation, m_LightPosition.x, m_LightPosition.y, m_LightPosition.z);
	world *= translation;

	// Set shader parameters
	D3DXMATRIX worldViewProj = world * m_View * m_Projection;
	D3DXMatrixTranspose(&worldViewProj, &worldViewProj);
	cgD3D9SetUniformMatrix(m_SimpleTransformVSWorldViewProj, &worldViewProj);
	D3DXVECTOR3 lightColor(1.0f, 1.0f, 1.0f);
	cgD3D9SetUniform(m_SimpleTransformVSColor, lightColor);

	// Activate shader
	cgD3D9BindProgram(m_SimpleTransformVS);
	m_pD3DDev->SetVertexDeclaration(m_pDeclaration);
	m_pD3DDev->SetPixelShader(0);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	// Draw
	return m_Light->m_Meshes[0].Draw();
}

HRESULT NVBEffect::DrawDirectionLight()
{
	if (!m_Light) {
		// Load light model if not done yet
		m_Light = new NVBScene;
		HRESULT hr;
		if (FAILED(hr = m_Light->Load("directionLight.nvb", m_pD3DDev, GetFilePath))) {
			m_strLastError = "Failed to load the direction light model";
			return hr;
		}
	}

	// Arrow direction in object space
	D3DXVECTOR3 dirArrow = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	// Rotate to make the arrow parallel to the light direction
	D3DXVECTOR3 rotationAxis;
	D3DXVec3Cross(&rotationAxis, &dirArrow, &m_LightDirection);
	D3DXVec3Normalize(&rotationAxis, &rotationAxis);
	D3DXMATRIX world;
	D3DXMatrixRotationAxis(&world, &rotationAxis, acosf(D3DXVec3Dot(&m_LightDirection, &dirArrow)));

	// Scale
	float scale = 0.1f * m_Scene->m_Radius / m_Light->m_Radius;
	D3DXMATRIX scaling;
	D3DXMatrixScaling(&scaling, scale, scale, scale);
	world *= scaling;

	// Translate to offset from the object
	D3DXMATRIX translation = m_UIScene->GetTranslationMatrix();
	D3DXMatrixTranslation(&translation, m_LightPosition.x + translation._41, m_LightPosition.y + translation._42, m_LightPosition.z + translation._43);
	world *= translation;

	// Set shader parameters
	D3DXMATRIX worldViewProj = world * m_View * m_Projection;
	D3DXMatrixTranspose(&worldViewProj, &worldViewProj);
	cgD3D9SetUniformMatrix(m_SimpleTransformVSWorldViewProj, &worldViewProj);
	D3DXVECTOR3 lightColor(1.0f, 1.0f, 1.0f);
	cgD3D9SetUniform(m_SimpleTransformVSColor, lightColor);

	m_pD3DDev->SetVertexDeclaration(m_pDeclaration);

	// Activate shader
	cgD3D9BindProgram(m_SimpleTransformVS);

	m_pD3DDev->SetFVF(D3DFVF_XYZ);
	m_pD3DDev->SetPixelShader(0);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	// Draw
	return m_Light->m_Meshes[0].Draw();
}

/*******************************************************************************

	Shader management

*******************************************************************************/

const std::string NVBEffect::m_SimpleTransformCode = "\
struct a2v {\n\
	float4 Position : POSITION; //in object space\n\
};\n\
struct v2f {\n\
	float4 Position : POSITION; //in projection space\n\
	float4 Color : COLOR0;\n\
};\n\
v2f main(a2v IN, uniform float4x4 WorldViewProj, uniform float4 Color)\n\
{\n\
	v2f OUT;\n\
	OUT.Color = Color;\n\
	OUT.Position = mul(WorldViewProj, IN.Position);\n\
	return OUT;\n\
}\n\
";

HRESULT NVBEffect::LoadShaders()
{
    const char* profileOpts[] = 
    {
        "-profileopts", "dcls", NULL,
    };

	// Load a vertex program into the context
	m_SimpleTransformVS = cgCreateProgram(m_Context, CG_SOURCE, m_SimpleTransformCode.c_str(), CG_PROFILE_VS_1_1, 0, profileOpts);

	// Assign registers to the relevant vertex attributes
    D3DVERTEXELEMENT9 declaration[] =
    {
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 }, 
		{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

        D3DDECL_END()
    };

	m_pD3DDev->CreateVertexDeclaration(declaration, &m_pDeclaration);
	/*
	DWORD declaration[] = {
		D3DVSD_STREAM(0),
		D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
		D3DVSD_SKIP(6),
		D3DVSD_END()
	};
	*/
	if (!cgD3D9ValidateVertexDeclaration(m_SimpleTransformVS, declaration)) {
		m_strLastError = "Vertex format incompatible with Cg program";
		return E_FAIL;
	}

	// Assemble the shader
	if (cgD3D9LoadProgram(m_SimpleTransformVS, false, 0) == E_FAIL) {
		m_strLastError = "Failed to load Cg vertex program";
		return E_FAIL;
	}
	// Creates references to the vertex program input parameters
	m_SimpleTransformVSWorldViewProj = cgGetNamedParameter(m_SimpleTransformVS, "WorldViewProj");
	m_SimpleTransformVSColor = cgGetNamedParameter(m_SimpleTransformVS, "Color");
	return S_OK;
}

/*******************************************************************************

	Texture management

*******************************************************************************/

HRESULT NVBEffect::LoadTexture(const std::string& filename, LPDIRECT3DTEXTURE9& texture, D3DPOOL pool, D3DXIMAGE_INFO* info)
{
	SAFE_RELEASE(texture);
	HRESULT hr;
	if (FAILED(hr = D3DXCreateTextureFromFileEx(m_pD3DDev, GetFilePath(filename).c_str(),
												D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
												0, D3DFMT_A8R8G8B8, pool,
												D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR, 0,
												info, 0, &texture)))
		m_strLastError = EBString("Could not create texture ") + filename;
	return hr;
}

HRESULT NVBEffect::LoadTexture(const std::string& filename, LPDIRECT3DCUBETEXTURE9& texture, D3DXIMAGE_INFO* info)
{
	SAFE_RELEASE(texture);
	HRESULT hr;
	if (FAILED(hr = D3DXCreateCubeTextureFromFileExA(m_pD3DDev, GetFilePath(filename).c_str(), 
													D3DX_DEFAULT, 0, 0,
													D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
													D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR, 0,
													info, 0, &texture)))
		m_strLastError = EBString("Could not create texture ") + filename;
	return hr;
}

void NVBEffect::ReleaseTextures()
{
	SAFE_RELEASE(m_DiffuseMap);
	SAFE_RELEASE(m_HeightMap);
	SAFE_RELEASE(m_NormalMap);
	SAFE_RELEASE(m_CubeMap);
}

HRESULT NVBEffect::LoadTextures()
{
	ReleaseTextures();
	HRESULT hr;
	if (m_DiffuseMapFilename != "")
		if (FAILED(hr = LoadTexture(m_DiffuseMapFilename, m_DiffuseMap)))
			return hr;
	if (m_HeightMapFilename != "")
		if (FAILED(hr = LoadTexture(m_HeightMapFilename, m_HeightMap)))
			return hr;
	if (m_NormalMapFilename != "")
		if (FAILED(hr = LoadTexture(m_NormalMapFilename, m_NormalMap)))
			return hr;
	if (m_CubeMapFilename != "")
		if (FAILED(hr = LoadTexture(m_CubeMapFilename, m_CubeMap)))
			return hr;
	return S_OK;
}

/*******************************************************************************

	Error management

*******************************************************************************/

void NVBEffect::CgErrorCallback()
{
#if 0
	bool thereIsAnError = true;
#else
	CGerror error = cgGetError(); // Recall that cgGetError() removes the error from the stack error, so you won't be able to check it later on
	const char* str = cgD3D9TranslateCGerror(error);
	if (error == cgD3D9Failed) {
		HRESULT hres = cgD3D9GetLastError();
		const char* errStr = cgD3D9TranslateHRESULT(hres);
	}
	else if (error == cgD3D9DebugTrace)
		bool debugTrace = true;
	else
		bool otherError = true;
#endif
}

CGerror NVBEffect::CgCheckError()
{
	CGerror error = cgGetError();
	const char* str = cgD3D9TranslateCGerror(error);
	if (error == cgD3D9Failed) {
		HRESULT hres = cgD3D9GetLastError();
		const char* errStr = cgD3D9TranslateHRESULT(hres);
	}
	else if (error == cgD3D9DebugTrace)
		bool debugTrace = true;
	else
		bool otherError = true;
	return error;
}

/*******************************************************************************

	GUI management

*******************************************************************************/

const unsigned int NVBEffect::m_LoadDiffuseMapShortcut = SHORTCUT_VALUE_TO_INT('A', VK_ESCAPE);
const unsigned int NVBEffect::m_LoadHeightMapShortcut = SHORTCUT_VALUE_TO_INT('B', VK_ESCAPE);
const unsigned int NVBEffect::m_LoadNormalMapShortcut = SHORTCUT_VALUE_TO_INT('C', VK_ESCAPE);
const unsigned int NVBEffect::m_LoadCubeMapShortcut = SHORTCUT_VALUE_TO_INT('D', VK_ESCAPE);
const unsigned int NVBEffect::m_LoadSceneShortcut = SHORTCUT_VALUE_TO_INT('E', VK_ESCAPE);
const unsigned int NVBEffect::m_ResetSceneShortcut = '0';
const unsigned int NVBEffect::m_IncreaseAmbientShortcut = 'A';
const unsigned int NVBEffect::m_DecreaseAmbientShortcut = SHORTCUT_VALUE_TO_INT('A', VK_SHIFT);
const unsigned int NVBEffect::m_WireframeShortcut = 'W';
const unsigned int NVBEffect::m_DrawLightShortcut = 'G';
const unsigned int NVBEffect::m_DrawAxisShortcut = 'X';
const unsigned int NVBEffect::m_UseDefaultDiffuseMapShortcut = SHORTCUT_VALUE_TO_INT('F', VK_ESCAPE);
const unsigned int NVBEffect::m_UseDefaultNormalMapShortcut = SHORTCUT_VALUE_TO_INT('G', VK_ESCAPE);
const unsigned int NVBEffect::m_DrawNormalsShortcut = 'N';
const unsigned int NVBEffect::m_DrawTangentBasisShortcut = 'T';
const unsigned int NVBEffect::m_PauseSceneShortcut = 'S';
const unsigned int NVBEffect::m_PauseCameraShortcut = 'C';
const unsigned int NVBEffect::m_PauseLightShortcut = 'L';
const unsigned int NVBEffect::m_PauseShortcut0 = ' ';
const unsigned int NVBEffect::m_PauseShortcut1 = VK_PAUSE;
const unsigned int NVBEffect::m_HelpShortcut0 = 'H';
const unsigned int NVBEffect::m_HelpShortcut1 = VK_F1;
const float NVBEffect::m_DefaultAmbient = 0.1f;
const float NVBEffect::m_AmbientStep = 0.01f;
const unsigned int NVBEffect::NO_DIFFUSEMAP = 1 << 0;
const unsigned int NVBEffect::NO_HEIGHTMAP = 1 << 1;
const unsigned int NVBEffect::NO_NORMALMAP = 1 << 2;
const unsigned int NVBEffect::NO_CUBEMAP = 1 << 3;
const unsigned int NVBEffect::NO_SCENE = 1 << 4;
const unsigned int NVBEffect::NO_AMBIENT = 1 << 5;

const NVBEffect::KeyShortcut NVBEffect::KeyShortcut::m_Invalid = 0;

//translate a virtual key into a key shortcut
NVBEffect::KeyShortcut NVBEffect::KeyShortcut::FromVirtualKey(unsigned int key)
{
	BYTE keyboardState[256];
	GetKeyboardState(keyboardState);
	unsigned int modifier = 0;
	if (keyboardState[VK_SHIFT] & 0x80)
		modifier = VK_SHIFT;
	else if (keyboardState[VK_CONTROL] & 0x80)
		modifier = VK_CONTROL;
	return KeyShortcut(SHORTCUT_VALUE_TO_INT(key, modifier));
}

//outputs a string describing the key shortcut
std::string NVBEffect::KeyShortcut::ToString()
{
	std::string modifierString;
	switch (m_Modifier) {
	case VK_SHIFT:
		modifierString = "shift ";
		break;
	case VK_CONTROL:
		modifierString = "ctrl ";
		break;
	case VK_ESCAPE:
		return "";
	default:
		modifierString = "";
		break;
	}
	std::string keyString;
	switch (m_Key) {
	case 0:
		keyString = "";
		break;
	case VK_PRIOR:
		keyString = "PageDown";
		break;
	case VK_NEXT:
		keyString = "PageUp";
		break;
	case VK_UP:
		keyString = "Up";
		break;
	case VK_DOWN:
		keyString = "Down";
		break;
	case VK_ADD:
		keyString = "+";
		break;
	case VK_SUBTRACT:
		keyString = "-";
		break;
	case VK_LEFT:
		keyString = "left";
		break;
	case VK_RIGHT:
		keyString = "right";
		break;
	case VK_PAUSE:
		keyString = "pause";
		break;
	case VK_F1:
		keyString = "F1";
		break;
	case ' ':
		keyString = "space";
		break;
	default:
		keyString = std::string(1, tolower(static_cast<char>(m_Key)));
		break;
	}
	return (modifierString + keyString);
}

//a menu item is a title and a key shortcut
NVBEffect::MenuItem::MenuItem(const std::string& name, const std::string& description, KeyShortcut keyShortcut)
{
	m_KeyShortcut.push_back(keyShortcut);
	m_Description = description;
	BuildName(name);
}

NVBEffect::MenuItem::MenuItem(const std::string& name, const std::string& description, KeyShortcut keyShortcut0, KeyShortcut keyShortcut1)
{
	m_KeyShortcut.push_back(keyShortcut0);
	m_KeyShortcut.push_back(keyShortcut1);
	m_Description = description;
	BuildName(name);
}

void NVBEffect::MenuItem::Build(NVBEffect& instancePtr)
{
	instancePtr.AddProperty(new EBTriggerProperty(m_Name.c_str()));
}

std::string NVBEffect::MenuItem::Description()
{
	if (m_Description == "")
		return m_Name + "\n";
	else
		return m_Name + ": " + m_Description + "\n";
}

bool NVBEffect::MenuItem::Update(unsigned int key) const
{
	for (unsigned int i = 0; i < m_KeyShortcut.size(); ++i)
		if (m_KeyShortcut[i] == key)
			return true;
	return false;
}

NVBEffect::MenuItemBoolean::MenuItemBoolean(const std::string& name, const std::string& description, KeyShortcut keyShortcut, bool* value) : MenuItem(name, description, keyShortcut)
{
	m_Value = value;
}

NVBEffect::MenuItemBoolean::MenuItemBoolean(const std::string& name, const std::string& description, KeyShortcut keyShortcut0, KeyShortcut keyShortcut1, bool* value) : MenuItem(name, description, keyShortcut0, keyShortcut1)
{
	m_Value = value;
}

void NVBEffect::MenuItemBoolean::Build(NVBEffect& instancePtr)
{
	instancePtr.AddProperty(new EBProperty(m_Name.c_str(), reinterpret_cast<char*>(m_Value) - reinterpret_cast<char*>(&instancePtr), EBTYPE_BOOL_PROP, PROPFLAG_CALLBACK));
}

bool NVBEffect::MenuItemBoolean::Update(unsigned int key) const
{
	for (unsigned int i = 0; i < m_KeyShortcut.size(); ++i)
		if (m_KeyShortcut[i] == key) {
			*m_Value = !*m_Value;
			return true;
		}
	return false;
}

NVBEffect::MenuItemEnumerant::MenuItemEnumerant(const std::string& name, const std::string& description, KeyShortcut keyShortcut, unsigned int* value) : MenuItem(name, description, keyShortcut)
{
	m_Value = value;
}

NVBEffect::MenuItemEnumerant::MenuItemEnumerant(const std::string& name, const std::string& description, KeyShortcut keyShortcut0, KeyShortcut keyShortcut1, unsigned int* value) : MenuItem(name, description, keyShortcut0, keyShortcut1)
{
	m_Value = value;
}

void NVBEffect::MenuItemEnumerant::Build(NVBEffect& instancePtr)
{
	EBEnumProperty* prop = new EBEnumProperty(m_Name.c_str(), reinterpret_cast<char*>(m_Value) - reinterpret_cast<char*>(&instancePtr), EBTYPE_DWORD_PROP, PROPFLAG_CALLBACK);
	for (unsigned int i = 0; i < m_EnumerantValue.size(); ++i)
		prop->AddEnumerant(new EBEnumValue(prop, m_EnumerantLabel[i].c_str(), (DWORD)m_EnumerantValue[i], EBTYPE_DWORD_PROP));
	instancePtr.AddProperty(prop);
}

std::string NVBEffect::MenuItemEnumerant::Description()
{
	std::string desc;
	if (m_Description == "")
		desc = m_Name + ":";
	else
		desc = m_Name + ": " + m_Description;
	for (unsigned int i = 0; i < m_EnumerantLabel.size(); ++i)
		desc += "\n\t- " + m_EnumerantLabel[i] + ": " + m_EnumerantDescription[i];
	return desc + "\n";
}

void NVBEffect::MenuItemEnumerant::Add(unsigned int value, const std::string& label, const std::string& description, KeyShortcut keyShortcut)
{
	m_EnumerantValue.push_back(value);
	m_EnumerantLabel.push_back(label);
	m_EnumerantDescription.push_back(description);
	m_EnumerantKeyShortcut.push_back(keyShortcut);
}

bool NVBEffect::MenuItemEnumerant::Update(unsigned int key) const
{
	for (unsigned int i = 0; i < m_KeyShortcut.size(); ++i)
		if (m_KeyShortcut[i] == key) {
			for (unsigned int j = 0; j < m_EnumerantValue.size(); ++j)
				if (m_EnumerantValue[j] == *m_Value) {
					if (j == m_EnumerantValue.size() - 1)
						*m_Value = m_EnumerantValue[0];
					else
						*m_Value = m_EnumerantValue[j + 1];
					break;
				}
			return true;
		}
	for (unsigned int j = 0; j < m_EnumerantKeyShortcut.size(); ++j)
		if (m_EnumerantKeyShortcut[j] == key) {
			*m_Value = m_EnumerantValue[j];
			return true;
		}
	return false;
}

void NVBEffect::MenuItem::BuildName(const std::string& name)
{
	m_Name = name;
	for (unsigned int i = 0; i < m_KeyShortcut.size(); ++i) {
		std::string shortcutString = m_KeyShortcut[i].ToString();
		if (shortcutString == "")
			break;
		else
			m_Name += " (" + shortcutString + ")";
	}
}

void NVBEffect::AddMenuItem(const std::string& name, const std::string& description, KeyShortcut keyShortcut)
{
	m_MenuItemList.push_back(new MenuItem(name, description, keyShortcut));
}

void NVBEffect::AddMenuItem(const std::string& name, const std::string& description, KeyShortcut keyShortcut0, KeyShortcut keyShortcut1)
{
	m_MenuItemList.push_back(new MenuItem(name, description, keyShortcut0, keyShortcut1));
}

void NVBEffect::AddMenuItem(const std::string& name, const std::string& description, KeyShortcut keyShortcut, bool* value)
{
	m_MenuItemList.push_back(new MenuItemBoolean(name, description, keyShortcut, value));
}

void NVBEffect::AddMenuItem(const std::string& name, const std::string& description, KeyShortcut keyShortcut0, KeyShortcut keyShortcut1, bool* value)
{
	m_MenuItemList.push_back(new MenuItemBoolean(name, description, keyShortcut0, keyShortcut1, value));
}

NVBEffect::MenuItemEnumerant* NVBEffect::AddMenuItem(const std::string& name, const std::string& description, KeyShortcut keyShortcut, unsigned int* value)
{
	MenuItemEnumerant* item = new MenuItemEnumerant(name, description, keyShortcut, value);
	m_MenuItemList.push_back(item);
	return item;
}

NVBEffect::MenuItemEnumerant* NVBEffect::AddMenuItem(const std::string& name, const std::string& description, KeyShortcut keyShortcut0, KeyShortcut keyShortcut1, unsigned int* value)
{
	MenuItemEnumerant* item = new MenuItemEnumerant(name, description, keyShortcut0, keyShortcut1, value);
	m_MenuItemList.push_back(item);
	return item;
}

// Menu building
void NVBEffect::CreateMenu(unsigned int option)
{
	if (!(option & NO_AMBIENT)) {
		AddMenuItem("Ambient increase", "", m_IncreaseAmbientShortcut);
		AddMenuItem("Ambient decrease", "", m_DecreaseAmbientShortcut);
	}
	NVBEffect::MenuItemEnumerant* item = AddMenuItem("Control Options", "", 0, &m_CurrentMouseUI);
	for (unsigned int i = 0; i < m_MouseUIList.size(); ++i)
		item->Add(i, "Mouse " + m_MouseUIList[i].m_Name, "change position and orientation of the " + m_MouseUIList[i].m_Name + " using the mouse", 0);
	AddMenuItem("Pause scene", "pause/run the scene animation if available", m_PauseSceneShortcut, &m_PauseScene);
	AddMenuItem("Pause camera", "pause/run the camera animation if available", m_PauseCameraShortcut, &m_PauseCamera);
	AddMenuItem("Pause light", "pause/run the light animation if available", m_PauseLightShortcut, &m_PauseLight);
	AddMenuItem("Pause", "pause/run altogether the camera, scene and light animations for the ones available", m_PauseShortcut0, m_PauseShortcut1);
	AddMenuItem("Reset scene position", "cancel the positional and rotational changes made by moving the scene using the mouse", m_ResetSceneShortcut);
	AddMenuItem("Draw normals", "", m_DrawNormalsShortcut, &m_DrawNormals);
	AddMenuItem("Draw tangent basis", "\n\t- red for the vector along the first texture coordinate\n\t- green for the vector along the second texture coordinate\n\t- blue for the normal", m_DrawTangentBasisShortcut, &m_DrawTangentBasis);
	AddMenuItem("Draw world coordinate axis", "", m_DrawAxisShortcut, &m_DrawAxis);
	AddMenuItem("Draw light", "", m_DrawLightShortcut, &m_DrawLight);
	AddMenuItem("Wireframe", "", m_WireframeShortcut, &m_Wireframe);
	AddMenuItem("Use default diffuse map", "instead of the scene diffuse maps", m_UseDefaultDiffuseMapShortcut, &m_UseDefaultDiffuseMap);
	if (m_DiffuseMapFilename != "" && !(option & NO_DIFFUSEMAP))
		AddMenuItem("Load diffuse map...", "load a new default diffuse map", m_LoadDiffuseMapShortcut);
	if (m_HeightMapFilename != "" && !(option & NO_HEIGHTMAP))
		AddMenuItem("Load height map...", "load a new default height map", m_LoadHeightMapShortcut);
	AddMenuItem("Use default normal map", "instead of the scene normal maps", m_UseDefaultNormalMapShortcut, &m_UseDefaultNormalMap);
	if (m_NormalMapFilename != "" && !(option & NO_NORMALMAP))
		AddMenuItem("Load normal map...", "load a new default normal map", m_LoadNormalMapShortcut);
	if (m_CubeMapFilename != "" && !(option & NO_CUBEMAP))
		AddMenuItem("Load cube map...", "load a new default cube map", m_LoadCubeMapShortcut);
	if (!(option & NO_SCENE))
		AddMenuItem("Load scene...", "load a new scene", m_LoadSceneShortcut);
	AddMenuItem("Help...", "", m_HelpShortcut0, m_HelpShortcut1);
}

// Menu callback
void NVBEffect::PropertyUpdateCallback(const EBProperty* property, bool written)
{
	if (!written)
		return;
	EBString name = property->GetPropertyName();
	for (unsigned int i = 0; i < m_MenuItemList.size(); ++i)
		if (name == m_MenuItemList[i]->m_Name) {
			if (property->IsKindOf(EBTYPE_ENUM_PROP)) {
				MenuItemEnumerant* item = reinterpret_cast<MenuItemEnumerant*>(m_MenuItemList[i]);
				Keyboard(item->m_EnumerantKeyShortcut[*item->m_Value]);
			}
			else
				Keyboard(m_MenuItemList[i]->m_KeyShortcut[0]);
			break;
		}
}

// Mouse callbacks
void NVBEffect::MouseButton(HWND, eButtonID button, bool buttonDown, int x, int y)
{
	bool doIt = true;
	if (m_CurrentMouseUI == 0)
		doIt = m_PauseScene;
	else if (m_CurrentMouseUI == 1)
		doIt = m_PauseCamera;
	if (doIt)
		if (button == MOUSE_LEFTBUTTON) {
			if (buttonDown)
				m_MouseUIList[m_CurrentMouseUI].m_UI->OnLButtonDown(x, y);
			else
				m_MouseUIList[m_CurrentMouseUI].m_UI->OnLButtonUp(x, y);
		}
}

void NVBEffect::MouseMove(HWND, int x, int y)
{
	bool doIt = true;
	if (m_CurrentMouseUI == 0)
		doIt = m_PauseScene;
	else if (m_CurrentMouseUI == 1)
		doIt = m_PauseCamera;
	if (doIt)
		m_MouseUIList[m_CurrentMouseUI].m_UI->OnMouseMove(x, y);
}

NVBEffect::NamedMouseUI::NamedMouseUI(const RECT& rect, const std::string& name) : m_Name(name)
{
	m_UI = new MouseUI(rect);
}

MouseUI* NVBEffect::NewMouseUI(const std::string& name)
{
	RECT rect;
	rect.left = rect.top = 0;
	D3DVIEWPORT9 viewport;
	m_pD3DDev->GetViewport(&viewport);
	rect.bottom = viewport.Height;
	rect.right  = viewport.Width;
	NamedMouseUI ui(rect, name);
	m_MouseUIList.push_back(ui);
	return ui.m_UI;
}

// Keyboard callback
void NVBEffect::Keyboard(KeyShortcut key)
{
	switch (key) {
	case m_IncreaseAmbientShortcut:
		m_Ambient += m_AmbientStep;
		if (m_Ambient > 1.0f)
			m_Ambient = 1.0f;
		m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
		break;
	case m_DecreaseAmbientShortcut:
		m_Ambient -= m_AmbientStep;
		if (m_Ambient < 0.0f)
			m_Ambient = 0.0f;
		m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
		break;
	case m_LoadDiffuseMapShortcut:
		{
			NVTextureFileDialog dialog;
			std::string filename;
			LPDIRECT3DTEXTURE9 diffuseMap = 0;
			if (dialog.Open(filename))
				if (FAILED(LoadTexture(filename, diffuseMap)))
					FDebug("Diffuse map %s not loaded\n", filename.c_str());
				else {
					SAFE_RELEASE(m_DiffuseMap);
					m_DiffuseMap = diffuseMap;
					m_DiffuseMapFilename = filename;
					FDebug("Diffuse map %s loaded\n", m_DiffuseMapFilename.c_str());
				}
		}
		break;
	case m_LoadHeightMapShortcut:
		{
			NVTextureFileDialog dialog;
			std::string filename;
			LPDIRECT3DTEXTURE9 heightMap = 0;
			if (dialog.Open(filename))
				if (FAILED(LoadTexture(filename, heightMap)))
					FDebug("Height map %s loaded\n", filename.c_str());
				else {
					SAFE_RELEASE(m_HeightMap);
					m_HeightMap = heightMap;
					m_HeightMapFilename = filename;
					FDebug("Height map %s loaded\n", m_HeightMapFilename.c_str());
				}
		}
		break;
	case m_LoadNormalMapShortcut:
		{
			NVTextureFileDialog dialog;
			std::string filename;
			LPDIRECT3DTEXTURE9 normalMap = 0;
			if (dialog.Open(filename))
				if (FAILED(LoadTexture(filename, normalMap)))
					FDebug("Normal map %s loaded\n", filename.c_str());
				else {
					SAFE_RELEASE(m_NormalMap);
					m_NormalMap = normalMap;
					m_NormalMapFilename = filename;
					FDebug("Normal map %s loaded\n", m_NormalMapFilename.c_str());
				}
		}
		break;
	case m_LoadCubeMapShortcut:
		{
			NVTextureFileDialog dialog;
			std::string filename;
			LPDIRECT3DCUBETEXTURE9 cubeMap = 0;
			if (dialog.Open(filename))
				if (FAILED(LoadTexture(filename, cubeMap)))
					FDebug("Cube map %s loaded\n", filename.c_str());
				else {
					SAFE_RELEASE(m_CubeMap);
					m_CubeMap = cubeMap;
					m_CubeMapFilename = filename;
					FDebug("Cube map %s loaded\n", m_CubeMapFilename.c_str());
				}
		}
		break;
	case m_LoadSceneShortcut:
		{
			NVFileDialog dialog;
			NVBScene* scene = new NVBScene;
			std::string filename;
			if (dialog.Open(filename))
				if (FAILED(scene->Load(filename, m_pD3DDev, GetFilePath))) {
					FDebug("Scene %s not loaded: %s\n", filename.c_str(), scene->m_ErrorMessage.c_str());
					delete scene;
				}
				else {
					delete m_Scene;
					m_Scene = scene;
					m_SceneFilename = filename;
					Reset();
					FDebug("Scene %s loaded\n", m_SceneFilename.c_str());
				}
		}
		break;
	case m_ResetSceneShortcut:
		D3DXMatrixTranslation(&m_World, - m_Scene->m_Center.x, - m_Scene->m_Center.y, - m_Scene->m_Center.z);
		m_UIScene->SetMatrix(m_World);
		break;
	case m_PauseShortcut0:
	case m_PauseShortcut1:
		if ((m_Scene->m_NumMeshKeys && !m_PauseScene) || (m_Scene->m_NumCameraKeys && !m_PauseCamera) || (m_Scene->m_NumLightKeys && !m_PauseLight)) {
			//if one of the component has an animation and is running, pause all components that have an animation
			if (m_Scene->m_NumMeshKeys)
				m_PauseScene = true;
			if (m_Scene->m_NumCameraKeys)
				m_PauseCamera = true;
			if (m_Scene->m_NumLightKeys)
				m_PauseLight = true;
		}
		else {
			//otherwise run all components that have an animation
			if (m_Scene->m_NumMeshKeys)
				m_PauseScene = false;
			if (m_Scene->m_NumCameraKeys)
				m_PauseCamera = false;
			if (m_Scene->m_NumLightKeys)
				m_PauseLight = false;
		}
		m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
		break;
	case m_HelpShortcut0:
	case m_HelpShortcut1:
		{
			std::string message;
			for (unsigned int j = 0; j < m_MenuItemList.size(); ++j)
				message += m_MenuItemList[j]->Description();
			::MessageBoxEx(0, message.c_str(), "Help", MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT));
		}
		break;
	default:
		break;
	}
}

// Keyboard callback
void NVBEffect::Keyboard(DWORD key, UINT, bool buttonDown)
{
	if (buttonDown) {
		for (unsigned int i = 0; i < m_MenuItemList.size(); ++i)
			if (m_MenuItemList[i]->Update(key)) {
				m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
				break;
			}
		Keyboard(KeyShortcut::FromVirtualKey(key));
	}
}

void NVBEffect::UpdateProperties()
{
	EBEffect::UpdateProperties();
	for (unsigned int i = 0; i < m_MenuItemList.size(); ++i)
		m_MenuItemList[i]->Build(*this);
}
