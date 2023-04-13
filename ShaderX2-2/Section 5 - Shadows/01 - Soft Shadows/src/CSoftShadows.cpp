///
///		Copyright (c) 2003 F. Brebion (f.brebion@vrcontext.com)
///
///		Licence/disclaimer: This demo is provided 'as-is', without any express or implied
///		warranty. In no event can i be held liable for any damages arising from the use
///		of that code or this demo.
///
///		Permission is granted to anyone to use the code for any purpose, including
///		commercial usage, and to alter it and redistribute it freely, subject to the
///		following restrictions:
///
///		1. Proper credits must be given to the author for his contribution to the code
///		if the code is totally or partially reused. If you implement the described technic
///		yourself, credits are appreciated but not required. I'd also appreciate it
///		if you send me an email saying how you're going to use the code for.
///
///		2. The 3D models and textures in this demo belong to Daniel Cornibert
///		(danielcornibert@hotmail.com) and are included in it as pure curtesy from his
///		part. PERMISSION TO USE THIS ART IS NOT GRANTED by this license.
///

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <d3d9.h>
#include <d3dx9.h> 
#include <assert.h>

#include "CUtils.h"
#include "CScene.h"
#include "CEntity.h"
#include "CASEFile.h"
#include "CObjMd3.h"
#include "CSoftShadows.h"

//#define USER_SETTINGS

CSoftShadows *CSoftShadows::ms_instance = NULL;

float g_shadowScale = 3.9f;
float g_penumbra = 6.4f;
float g_fix1 = 0.750f;
float g_fix2 = 1.750f;
int g_count = 3;
extern float tt;
bool g_hasPS14 = false;
bool g_hasVS11 = false;
float g_lightOffset = -113.0f;

///
///     Creates a soft shadows application
///
CSoftShadows::CSoftShadows()
{
    m_isFinished = false;
    set3f(m_pos, 0);
    D3DXMatrixIdentity(&m_rot);
    m_scene = NULL;
    m_pitch = 0;
    m_yaw = 0;
    g_hasPS14 = false;
    g_hasVS11 = false;
    m_cattaur = NULL;

    m_pos[0] = -798.5f;
    m_pos[1] = -526.2f;
    m_pos[2] = 525.1f;
}

///
///     Destroys the soft shadows application
///
CSoftShadows::~CSoftShadows()
{
    cleanDX();
}

///
///     Is the soft shadows application finished?
///
bool CSoftShadows::isFinished()
{
    return(m_isFinished);
}

///
///     Terminates the soft shadows application
///
void CSoftShadows::finish()
{
    m_isFinished = true;
}

///
///     Sets the soft shadows application instance
///
CSoftShadows *CSoftShadows::setInstance(CSoftShadows *a_instance)
{
    ms_instance = a_instance;
    return(a_instance);
}

///
///     Gets the soft shadows application instance
///
CSoftShadows *CSoftShadows::getInstance()
{
    return(ms_instance);
}

///
///     Window events callback
///
LRESULT CALLBACK CSoftShadows::wndProc(HWND a_hWnd, UINT a_msg, WPARAM a_wParam,
    LPARAM a_lParam)
{
	switch (a_msg)
	{
		case WM_ACTIVATE:
		{
			return 0;
		}

		case WM_SYSCOMMAND:	
		{
			switch (a_wParam)
			{
				case SC_SCREENSAVE:	
				case SC_MONITORPOWER:
				return 0;
			}
			break;	
		}

		case WM_CLOSE:	
		{
			PostQuitMessage(0);	
			return 0;	
		}

		case WM_KEYDOWN:
		{
            switch (a_wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                    break;

            }
			return 0;
		}

        case WM_CHAR:
        {
#ifdef USER_SETTINGS
            switch (a_wParam)
            {
                case 'A':
                case 'a':
                    g_shadowScale += 0.1f;
                    break;
                case 'Q':
                case 'q':
                    g_shadowScale -= 0.1f;
                    if (g_shadowScale < 0.0f)
                        g_shadowScale = 0.0f;
                    break;
                case 'Z':
                case 'z':
                    g_penumbra += 0.1f;
                    break;
                case 'S':
                case 's':
                    g_penumbra -= 0.1f;
                    if (g_penumbra < 0.0f)
                        g_penumbra = 0.0f;
                    break;
                case 'E':
                case 'e':
                    g_count = (g_count + 1) % 4;
                    break;
                case 'D':
                case 'd':
                    g_count--;
                    if (g_count < 0)
                        g_count = 3;
                    break;
                case 't':
                case 'T':
                    tt += 0.01f;
                    break;
                case 'G':
                case 'g':
                    tt -= 0.01f;
                    break;
                case 'Y':
                case 'y':
                    g_fix1 += 0.1f;
                    break;
                case 'H':
                case 'h':
                    g_fix1 -= 0.1f;
                    break;
                case 'U':
                case 'u':
                    g_fix2 += 0.1f;
                    break;
                case 'J':
                case 'j':
                    g_fix2 -= 0.1f;
                    break;
				case 'O':
				case 'o':
					g_lightOffset -= 1.0f;
					break;
				case 'L':
				case 'l':
					g_lightOffset += 1.0f;
					break;
            }
            char str[256];
            sprintf(str, "shadow scale = %f penumbra = %f c= %d fix = %.3f,%.3f offset = %.3f\n", g_shadowScale, g_penumbra,
                g_count, g_fix1, g_fix2, g_lightOffset);
            warning(str);
#endif
            return 0;
        }

		case WM_KEYUP:	
		{
			return 0;
		}

		case WM_SIZE:
		{
			return 0;
		}
	}

	return DefWindowProc(a_hWnd, a_msg, a_wParam, a_lParam);
}

///
///     Creates the window
///
bool CSoftShadows::createWindow()
{
	WNDCLASS l_wc;
	DWORD l_dwExStyle;
	DWORD l_dwStyle;
	RECT l_windowRect;
    bool l_fullScreen = true;
    int l_width = 800;
    int l_height = 600;
    int l_bits = 32;

    l_windowRect.left = 0;
    l_windowRect.top = 0;
    l_windowRect.right = l_width;
    l_windowRect.bottom = l_height;

	l_wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	l_wc.lpfnWndProc = (WNDPROC) wndProc;
	l_wc.cbClsExtra = 0;
	l_wc.cbWndExtra	= 0;
	l_wc.hInstance = GetModuleHandle(NULL);
	l_wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	l_wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	l_wc.hbrBackground = NULL;
	l_wc.lpszMenuName = NULL;
	l_wc.lpszClassName = "SoftShadowsClass";

	if (!RegisterClass(&l_wc))
    {
		MessageBox(NULL, "Impossible to register window class.", "ERROR",
            MB_OK | MB_ICONEXCLAMATION);
		return false;
    }
	
	if (l_fullScreen)
	{
		DEVMODE l_dmScreenSettings;
		memset(&l_dmScreenSettings, 0, sizeof(l_dmScreenSettings));
		l_dmScreenSettings.dmSize = sizeof(l_dmScreenSettings);
		l_dmScreenSettings.dmPelsWidth = l_width;
		l_dmScreenSettings.dmPelsHeight	= l_height;
		l_dmScreenSettings.dmBitsPerPel	= l_bits;
		l_dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&l_dmScreenSettings, CDS_FULLSCREEN) !=
            DISP_CHANGE_SUCCESSFUL)
            /// ignore, using windowed mode
            l_fullScreen = false;
	}

	if (l_fullScreen)
	{
		l_dwExStyle = WS_EX_APPWINDOW;
		l_dwStyle = WS_POPUP;
		ShowCursor(false);
	}
	else
	{
		l_dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		l_dwStyle = WS_OVERLAPPEDWINDOW;
	}

	AdjustWindowRectEx(&l_windowRect, l_dwStyle, false, l_dwExStyle);

	// Create The Window
	if (!(m_hWnd = CreateWindowEx(l_dwExStyle, "SoftShadowsClass",
        "DX9 Soft Shadows demo", l_dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
     	0, 0, l_windowRect.right - l_windowRect.left,
    	l_windowRect.bottom - l_windowRect.top, NULL, NULL,
        GetModuleHandle(NULL), NULL)))
	{
		MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        return false;
	}

   	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);

    return true;
}

///
///     Checks the device capabilities
///
bool CSoftShadows::checkCaps(D3DDEVTYPE a_devType)
{
    assert(m_lpD3D != NULL);

    D3DCAPS9 l_d3dcaps9;
    ZeroMemory(&l_d3dcaps9, sizeof(l_d3dcaps9));

    HRESULT l_hRes = m_lpD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, a_devType, &l_d3dcaps9);
    if (FAILED(l_hRes))
        return false;

    if (l_d3dcaps9.PixelShaderVersion >= D3DPS_VERSION(1, 1))
        warning("Supports ps 1.1\n");
    if (l_d3dcaps9.PixelShaderVersion >= D3DPS_VERSION(1, 2))
        warning("Supports ps 1.2\n");
    if (l_d3dcaps9.PixelShaderVersion >= D3DPS_VERSION(1, 3))
        warning("Supports ps 1.3\n");
    if (l_d3dcaps9.PixelShaderVersion >= D3DPS_VERSION(1, 4))
        warning("Supports ps 1.4\n");
    if (l_d3dcaps9.PixelShaderVersion >= D3DPS_VERSION(2, 0))
        warning("Supports ps 2.0\n");
    if (l_d3dcaps9.PixelShaderVersion >= D3DPS_VERSION(3, 0))
        warning("Supports ps 3.0\n");

    if (l_d3dcaps9.VertexShaderVersion >= D3DVS_VERSION(1, 1))
        warning("Supports vs 1.1\n");
    if (l_d3dcaps9.VertexShaderVersion >= D3DVS_VERSION(2, 0))
        warning("Supports vs 2.0\n");
    if (l_d3dcaps9.VertexShaderVersion >= D3DVS_VERSION(3, 0))
        warning("Supports vs 3.0\n");

    g_hasPS14 = true;
    if (l_d3dcaps9.PixelShaderVersion < D3DPS_VERSION(1, 4))
	{
        g_hasPS14 = false;
		MessageBox(NULL, "This demo requires PS 1.4 (Geforce FX or Radeon 8500+)",
			"Error", MB_OK);
		exit(1);
	}

    g_hasVS11 = true;
    if (l_d3dcaps9.VertexShaderVersion < D3DVS_VERSION(1, 1))
	{
        g_hasVS11 = false;
		MessageBox(NULL, "This demo requires VS 1.1 (Geforce FX or Radeon 8500+)",
			"Error", MB_OK);
		exit(1);
	}

    if (g_hasPS14 && g_hasVS11)
    {
        if (l_d3dcaps9.MaxSimultaneousTextures < 6)
            return false;

        if ((l_d3dcaps9.StencilCaps & 255) != 255)
            /// invalid stencil capabilities
            return false;

        if (l_d3dcaps9.NumSimultaneousRTs < 1)
            /// no rendering targets supported?!
            return false;

        if (!(l_d3dcaps9.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP))
            /// need auto mipmap generation
            return false;

        if (!(l_d3dcaps9.PrimitiveMiscCaps & D3DPMISCCAPS_COLORWRITEENABLE))
            /// no per-component write enable
            return false;
    }
  
    return true;
}

///
///     Initializes direct X
///
bool CSoftShadows::initDX()
{
    assert(m_hWnd != NULL);
    m_lpD3D = Direct3DCreate9(D3D_SDK_VERSION);

    if (!m_lpD3D)
        return false;

    /// check for hardware acceleration..
    D3DDEVTYPE l_devType = D3DDEVTYPE_HAL;
    if (!checkCaps(l_devType))
    {
        l_devType = D3DDEVTYPE_REF;
        
        if (!checkCaps(l_devType))
            /// can't even use software mode ?
            return false;
    }

    D3DDISPLAYMODE l_mode;
    ZeroMemory(&l_mode, sizeof(l_mode));
    HRESULT l_hRes = m_lpD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &l_mode);
    if (FAILED(l_hRes))
        return false;

    D3DPRESENT_PARAMETERS l_params;
    ZeroMemory(&l_params, sizeof(l_params));

    l_params.BackBufferFormat = l_mode.Format;
    l_params.Windowed = true;
    l_params.BackBufferCount = 1;
    l_params.SwapEffect = D3DSWAPEFFECT_FLIP;
    l_params.EnableAutoDepthStencil = TRUE;
    l_params.AutoDepthStencilFormat = D3DFMT_D24S8;
    l_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    l_hRes = m_lpD3D->CreateDevice(D3DADAPTER_DEFAULT, l_devType, m_hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE, &l_params, &m_lpDevice);
    if (FAILED(l_hRes))
        return false;
                                 
    return true;
}

///
///     Cleans direct X
///
void CSoftShadows::cleanDX()
{
    if (m_lpDevice)
    {
        m_lpDevice->Release();
        m_lpDevice = NULL;
    }

    if (m_lpD3D)
    {
        m_lpD3D->Release();
        m_lpD3D = NULL;
    }
}

///
///     Initializes the application
//
bool CSoftShadows::init()
{
    if (!createWindow())
        return false;

    if (!initDX())
        return false;

    ShowCursor(false);

    /// Scene initializations
    c_texMgr::ms_mgr = new c_texMgr();
    m_scene = new c_scene(m_lpDevice);

    c_entity *sceneEntity = new c_entity("scene");
    c_aseFile *sceneFile = new c_aseFile(m_lpDevice, "sceneOK");
    sceneFile->load();
    sceneEntity->giveMeshes(sceneFile);
    delete sceneFile;
    m_scene->addEntity(sceneEntity);

    /// loads the cattaur
    s_material mat;
    mat.m_ambient = 0.2f;
    mat.m_recvShadows = true;
    mat.m_tex = c_texMgr::ms_mgr->getTexture("amuskinfinal.jpg");
    if (mat.m_tex == NULL)
    {
        c_texture *texObj = new c_texture(m_lpDevice, "amuskinfinal.jpg");
        mat.m_tex = texObj;
    }
    const float translation[3] = {0, 40.0f, 0};
    m_cattaur = new c_objMd3(m_lpDevice, "cattaur", "cat.md3", &mat, translation);
    c_entity *cattaurEntity = new c_entity("cattaur");
    cattaurEntity->setMesh(m_cattaur->getMesh());
    m_scene->addEntity(cattaurEntity);

    return true;
}

///
///     Renders the scene
///
void CSoftShadows::render()
{
    static int frames = 0;
    static DWORD myTime = 0;
    static float t = 0;
    HRESULT hRes;

    frames++;
    if (timeGetTime() - myTime > 1000)
    {
        char str[256];
        sprintf(str, "%d fps", frames);
        SetWindowText(m_hWnd, str);
        frames = 0;
        myTime = timeGetTime();
    }

    m_lpDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
                          D3DCOLOR_COLORVALUE(0.0, 0.0, 0.25, 1.0),
                          1.0,
                          0);
    
    m_lpDevice->BeginScene();

    // For the projection matrix, we set up a perspective transform (which
    // transforms geometry from 3D view space to 2D viewport space, with
    // a perspective divide making objects smaller in the distance). To build
    // a perpsective transform, we need the field of view (1/4 pi is common),
    // the aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI/3, 1.3333f, 10.0f, 2000000.0f );
    m_lpDevice->SetTransform(D3DTS_PROJECTION, &matProj );

    D3DXMATRIXA16 matWorld;
    D3DXMatrixIdentity(&matWorld );
    m_lpDevice->SetTransform(D3DTS_WORLD, &matWorld);

    D3DXMATRIXA16 matView;
    D3DXMATRIXA16 matTransl;
    D3DXMatrixTranslation(&matTransl, m_pos[0], m_pos[1], m_pos[2]);
    D3DXMatrixMultiply(&matView, &matTransl, &m_rot);
    m_lpDevice->SetTransform(D3DTS_VIEW, &matView);

    m_scene->prepare(m_pos, &m_rot);

    float normal[3];
    float pang = -m_pitch / 57.2951f - 3.1415927f / 2.0f;
    float yang = -m_yaw / 57.2951f;
    normal[0] = cosf(pang) * cosf(yang) * 90.0f;
    normal[1] = sinf(yang) * 90.0f;
    normal[2] = sinf(pang) * cosf(yang) * 90.0f;
    float plane[4];
    calculatePlane(plane, m_pos, normal);

    if (g_hasPS14 && g_hasVS11)
        m_scene->render(m_pos, plane, &matProj, &matView, &matWorld);
    else
        m_scene->renderNoPS14(m_pos, plane, &matProj, &matView, &matWorld);

    m_lpDevice->EndScene();

    hRes = m_lpDevice->Present(NULL, NULL, NULL, NULL);
}

///
///     Updates the scene
///
void CSoftShadows::update(const float elapsed1)
{
    const float elapsed = elapsed1 * 3.0f;

    if (GetAsyncKeyState(VK_UP))
    {
        float pang = -m_pitch / 57.2951f - 3.1415927f / 2.0f;
        float yang = -m_yaw / 57.2951f;

        float dx = cosf(pang) * cosf(yang) * 90.0f;
        float dy = sinf(yang) * 90.0f;
        float dz = sinf(pang) * cosf(yang) * 90.0f;

        m_pos[0] += dx * elapsed;
        m_pos[1] += dy * elapsed;
        m_pos[2] += dz * elapsed;
    }
    if (GetAsyncKeyState(VK_DOWN))
    {
        float pang = -m_pitch / 57.2951f - 3.1415927f / 2.0f;
        float yang = -m_yaw / 57.2951f;

        float dx = cosf(pang) * cosf(yang) * 90.0f;
        float dy = sinf(yang) * 90.0f;
        float dz = sinf(pang) * cosf(yang) * 90.0f;

        m_pos[0] -= dx * elapsed;
        m_pos[1] -= dy * elapsed;
        m_pos[2] -= dz * elapsed;
    }
    if (GetAsyncKeyState(VK_LEFT))
    {
        float pang = -m_pitch / 57.2951f - 3.1415927f / 2.0f;
        float yang = -m_yaw / 57.2951f;

        float dx = cosf(pang) * cosf(yang) * 60.0f;
        float dy = sinf(yang) * 60.0f;
        float dz = sinf(pang) * cosf(yang) * 60.0f;
                    
        float ddx, ddy, ddz;
        cross(dx, dy, dz, 0, 1, 0, ddx, ddy, ddz);
        normalize(ddx, ddy, ddz);

        m_pos[0] += ddx * elapsed * 90.0f;
        m_pos[1] += ddy * elapsed * 90.0f;
        m_pos[2] += ddz * elapsed * 90.0f;
    }
    if (GetAsyncKeyState(VK_RIGHT))
    {
        float pang = -m_pitch / 57.2951f - 3.1415927f / 2.0f;
        float yang = -m_yaw / 57.2951f;

        float dx = cosf(pang) * cosf(yang) * 60.0f;
        float dy = sinf(yang) * 60.0f;
        float dz = sinf(pang) * cosf(yang) * 60.0f;
                    
        float ddx, ddy, ddz;
        cross(dx, dy, dz, 0, 1, 0, ddx, ddy, ddz);
        normalize(ddx, ddy, ddz);

        m_pos[0] -= ddx * elapsed * 90.0f;
        m_pos[1] -= ddy * elapsed * 90.0f;
        m_pos[2] -= ddz * elapsed * 90.0f;
    }

    POINT p;
    GetCursorPos(&p);
    float x = (p.x - 320)* 3.141592f / (20);
    float y = (p.y - 200)* 3.141592f / (20);
    SetCursorPos(320, 200);

    if (x != 0)
        m_pitch += x;
    if (y != 0)
        m_yaw -= y;

    if (m_yaw > 89)
        m_yaw = 89;
    if (m_yaw < -89)
        m_yaw = -89;

    if (x != 0 || y != 0)
    {
        D3DXMatrixRotationY(&m_rot, -m_pitch / 57.295f);

        D3DXMATRIXA16 oldRot = m_rot;
        D3DXMATRIXA16 rotStep;
        D3DXMatrixRotationX(&rotStep, m_yaw / 57.295f);
        D3DXMatrixMultiply(&m_rot, &oldRot, &rotStep);
    }

    char str[256];
    sprintf(str, "Pos = %.3f %.3f %.3f\n", m_pos[0], m_pos[1], m_pos[2]);
    warning(str);

    /// cattaur update
    static md3_state cattaur_state;
    if (m_cattaur != NULL)
        m_cattaur->update(elapsed * 0.15f, &cattaur_state);

    if (m_pos[0] < -2118)
        m_pos[0] = -2118;
    if (m_pos[0] > 2161)
        m_pos[0] = 2161;

    if (m_pos[1] < -867)
        m_pos[1] = -867;
    if (m_pos[1] > 264)
        m_pos[1] = 264;

    if (m_pos[2] < -1236)
        m_pos[2] = -1236;
    if (m_pos[2] > 1190)
        m_pos[2] = 1190;
}

///
///     WinMain
///
///     Entry point of the program
///
int WINAPI WinMain(
    HINSTANCE       a_hInstance,
	HINSTANCE	    a_prevInstance,
    LPSTR		    a_lpCmdLine,
	int			    a_nCmdShow
    )
{
    MSG	l_msg;
    CSoftShadows *l_mySoftShadows = CSoftShadows::setInstance(new CSoftShadows());

	MessageBox(NULL, "Copyright (c) 2003 F. Brebion (f.brebion@vrcontext.com)\n\nSee readme.txt for license\n\nArt: curtesy of D. Cornibert (danielcornibert@hotmail.com)\n\n- Demo requires vs1.1, ps1.4 and 6 texture units\n- Demo might take a few seconds to load..\n- Demo will run in 800x600x32 fullscreen",
		"ShaderX2: DX9 Soft Shadows demo", MB_OK);

    if (!l_mySoftShadows->init())
    {
        /// can't initialize application
        MessageBox(NULL, "Can't initialize application, invalid device", "Error", MB_OK);
        return 1;
    }

    DWORD l_time = timeGetTime();
    while (!l_mySoftShadows->isFinished())
	{
		while (PeekMessage(&l_msg, NULL, 0, 0, PM_REMOVE))
		{
			if (l_msg.message == WM_QUIT)
                /// exit application:
                l_mySoftShadows->finish();
			else				
			{
                /// update message queue:
				TranslateMessage(&l_msg);
				DispatchMessage(&l_msg);
			}
   		}

        /// update game
        l_mySoftShadows->render();
        DWORD l_newTime = timeGetTime();
        l_mySoftShadows->update((float)(l_newTime - l_time) / 1000.0f);
        l_time = l_newTime;
    }

    delete l_mySoftShadows;

    return 0;
}
