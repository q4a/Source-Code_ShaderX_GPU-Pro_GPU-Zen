//---------------------------------------------------------------------------

#ifndef FLOATING_POINT_TEXTURE_SAMPLE
#define FLOATING_POINT_TEXTURE_SAMPLE
//---------------------------------------------------------------------------

class CMyD3DApplication : public CD3DApplication
{
	BOOL m_bCapture;

	D3DXMATRIXA16 m_matProject;
	D3DXMATRIXA16 m_matView;
	D3DXMATRIXA16 m_matTexture;
	D3DXMATRIXA16 m_matWorld;
	D3DXMATRIXA16 m_matTrackBall;

	CD3DFont* m_pFont;
	CD3DMesh* m_pShinyTeapot;

	ID3DXEffect* m_pEffect;

	IDirect3DCubeTexture9* m_pCubeMapRGBA;
	IDirect3DCubeTexture9* m_pCubeMapRGBE;

protected:
	HRESULT RenderTeapot(CONST D3DXMATRIXA16 *pView, CONST D3DXMATRIXA16 *pProject);

	HRESULT ConfirmDevice(D3DCAPS9*, DWORD, D3DFORMAT, D3DFORMAT);
	HRESULT OneTimeSceneInit();
	HRESULT InitDeviceObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT Render();
	HRESULT FrameMove();
	HRESULT FinalCleanup();

public:
	LRESULT MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	CMyD3DApplication();
};
//---------------------------------------------------------------------------

#endif
//---------------------------------------------------------------------------

