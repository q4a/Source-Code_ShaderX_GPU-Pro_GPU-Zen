/*********************************************************************NVMH4****
Path:  SDK\DEMOS\common\src\NVEffectsBrowser
File:  eb_property.cpp

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

#include <NVEffectsBrowser/ieb_effect.h>

#if STANDALONE

/*******************************************************************************

	Window management

*******************************************************************************/

EBEffect* gEffect; //must be global to be accessible from within WndProc

// Process messages for the main window.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message) {
	case WM_SIZE:
		gEffect->m_WindowHidden = (wParam == SIZE_MAXHIDE) || (wParam == SIZE_MINIMIZED);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		gEffect->Render();
		EndPaint(hWnd, &ps);
		break;
	case WM_KEYDOWN:
		if ((int)wParam == VK_ESCAPE)
			PostQuitMessage(0);
		else
			gEffect->Keyboard((int)wParam, 0, true);
		break;
	case WM_MOUSEMOVE:
		gEffect->MouseMove(hWnd, LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_LBUTTONDOWN:
		gEffect->MouseButton(hWnd, MOUSE_LEFTBUTTON, true, LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_MBUTTONDOWN:
		gEffect->MouseButton(hWnd, MOUSE_MIDDLEBUTTON, true, LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_RBUTTONDOWN:
		gEffect->MouseButton(hWnd, MOUSE_RIGHTBUTTON, true, LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_LBUTTONUP:
		gEffect->MouseButton(hWnd, MOUSE_LEFTBUTTON, false, LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_MBUTTONUP:
		gEffect->MouseButton(hWnd, MOUSE_MIDDLEBUTTON, false, LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_RBUTTONUP:
		gEffect->MouseButton(hWnd, MOUSE_RIGHTBUTTON, false, LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Create a window.
HWND EBEffect::InitInstance(HINSTANCE hInstance, unsigned int width, unsigned int height)
{
	HWND hWnd;
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= 0;
	wcex.hCursor		= LoadCursor(0, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)m_strEffectName.c_str();
	wcex.lpszClassName	= m_strEffectName.c_str();
	wcex.hIconSm		= 0;
	RegisterClassEx(&wcex);
	hWnd = CreateWindow(m_strEffectName.c_str(), m_strEffectName.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, width, height, 0, 0, hInstance, 0);
	if (hWnd) {
		ShowWindow(hWnd, TRUE);
		UpdateWindow(hWnd);
	}
	return hWnd;
}

// D3D initialization
LPDIRECT3DDEVICE8 EBEffect::InitD3D(HWND hWnd)
{
	// Create the D3D object, which is needed to create the D3DDevice.
	LPDIRECT3D8 d3d = Direct3DCreate8(D3D_SDK_VERSION);
	assert(d3d);

	// Get the current desktop display mode
	D3DDISPLAYMODE d3ddm;
	d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

	// Create the Direct3D device.
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = d3ddm.Format;
	d3dpp.hDeviceWindow  = hWnd;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	LPDIRECT3DDEVICE8 device = 0;
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_MIXED_VERTEXPROCESSING, &d3dpp, &device);
	return device;
}

// Render
void EBEffect::Render()
{
	if (m_pD3DDev == 0)
		return;

	// Begin the scene
	m_pD3DDev->BeginScene();

	// Update timer
	m_Timer.Frame();

	// Compute effect
	Tick(&m_Timer);

	// End the scene
	m_pD3DDev->EndScene();

	// Present the backbuffer contents to the display
	m_pD3DDev->Present(0, 0, 0, 0);
}

/*******************************************************************************

	Main function

*******************************************************************************/

int EBEffect::Main(HINSTANCE hInstance, unsigned int width, unsigned int height)
{
	gEffect = this;

	std::string executablePath;
	executablePath.resize(MAX_PATH);
	DWORD dwReturnSize = GetModuleFileName(0, &executablePath[0], MAX_PATH);
	assert(dwReturnSize <= MAX_PATH);
	effect_api::SetModulePath(executablePath);

	// Create window
	HWND hWnd = InitInstance(hInstance, width, height);
	assert(hWnd);

	// Initialize D3D
	LPDIRECT3DDEVICE8 device = InitD3D(hWnd);
	assert(device);

	// Initialize effect
	if (FAILED(Initialize(device)))
		return -1;

	// Start effect
	Start();
	m_Timer.Start(30.0f);

	// Run main loop
	MSG msg;
	do {
		BOOL gotAMsg;
		if (m_WindowHidden)
			gotAMsg = GetMessage(&msg, 0, 0, 0);
		else
			gotAMsg = PeekMessage(&msg, 0, 0, 0, PM_REMOVE);
		if (gotAMsg) {
			TranslateMessage(&msg); 
			DispatchMessage(&msg);
		}
		else if (!m_WindowHidden)
			Render();
	} while (msg.message != WM_QUIT);

	return (int)msg.wParam;
}

#endif

// Property class helpers
void EBTriggerProperty::Trigger(IPropertySet* pObj)
{
	assert(m_PropertyType == EBTYPE_TRIGGER_PROP);
	assert(m_dwPropertyFlags & PROPFLAG_CALLBACK);
	pObj->PropertyWritten(this);
}

void EBProperty::WriteValue(IPropertySet* pObj, const EBValue& Value)
{
	assert(m_PropertyType != EBTYPE_TRIGGER_PROP);
	if (!Value.IsKindOf(m_PropertyType))
	{
		assert(0);
		return;
	}

	Value.Write((BYTE*)pObj + m_dwOffset);
	pObj->PropertyWritten(this);
}

EBValue EBProperty::ReadValue(IPropertySet* pObj) 
{
	assert(m_PropertyType != EBTYPE_TRIGGER_PROP);

	EBValue Value(m_PropertyType);
	Value.Read((BYTE*)pObj + m_dwOffset);
	pObj->PropertyRead(this);
	return Value;
}
