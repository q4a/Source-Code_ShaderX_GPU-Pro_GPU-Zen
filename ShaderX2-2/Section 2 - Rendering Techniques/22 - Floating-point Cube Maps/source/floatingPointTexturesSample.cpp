//-----------------------------------------------------------------------------
//
//   This program is based on classic Env Mapping sample found in DX9 SDK.
//...Arkadiusz Waliszewski - arsil@acm.org
//
//-----------------------------------------------------------------------------

#define STRICT
#include <Windows.h>
#include <commctrl.h>
#include <tchar.h>
#include <math.h>
#include <stdio.h>
#include <D3DX9.h>
#include "DXUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DFile.h"
#include "D3DFont.h"
#include "D3DUtil.h"

#include "floatingPointTexturesSample.h"
//-----------------------------------------------------------------------------

struct ENVMAPPEDVERTEX
{
	D3DXVECTOR3 p; // Position
	D3DXVECTOR3 n; // Normal

	static const DWORD FVF;
};
const DWORD ENVMAPPEDVERTEX::FVF = D3DFVF_XYZ | D3DFVF_NORMAL;
//-----------------------------------------------------------------------------

#define CUBEMAP_RESOLUTION 64
//-----------------------------------------------------------------------------

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	CMyD3DApplication d3dApp;

	InitCommonControls();
	if(FAILED(d3dApp.Create(hInst)))
		return 0;

	return d3dApp.Run();
}
//-----------------------------------------------------------------------------

CMyD3DApplication::CMyD3DApplication():
	m_bCapture(FALSE),
	m_pFont(NULL),
	m_pShinyTeapot(NULL),
	m_pEffect(NULL),
	m_pCubeMapRGBA(NULL),
	m_pCubeMapRGBE(NULL)
{
	m_strWindowTitle    = _T("Floating point cube maps + filtering");
	m_d3dEnumeration.AppUsesDepthBuffer = TRUE;
}
//-----------------------------------------------------------------------------

HRESULT CMyD3DApplication::OneTimeSceneInit()
{
	D3DXMatrixIdentity(&m_matWorld);

	m_pFont        = new CD3DFont(_T("Arial"), 10, D3DFONT_BOLD);
	m_pShinyTeapot = new CD3DMesh();

	if(!m_pFont || !m_pShinyTeapot)
		return E_OUTOFMEMORY;

	D3DXMatrixIdentity(&m_matTrackBall);
	D3DXMatrixTranslation(&m_matView, 0.0f, 0.0f, 3.1f);

	D3DXMatrixIdentity(&m_matTexture);

	return S_OK;
}
//-----------------------------------------------------------------------------

HRESULT CMyD3DApplication::FrameMove()
{
	if(m_bCapture)
	{
		D3DXMATRIXA16 matCursor;
		D3DXQUATERNION qCursor = D3DUtil_GetRotationFromCursor(m_hWnd);
		D3DXMatrixRotationQuaternion(&matCursor, &qCursor);
		D3DXMatrixMultiply(&m_matView, &m_matTrackBall, &matCursor);

		D3DXMatrixInverse(&m_matTexture, NULL, &m_matView);

		D3DXMATRIXA16 matTrans;
		D3DXMatrixTranslation(&matTrans, 0.0f, 0.0f, 3.1f);
		D3DXMatrixMultiply(&m_matView, &m_matView, &matTrans);
	}

	return S_OK;
}
//-----------------------------------------------------------------------------

HRESULT CMyD3DApplication::RenderTeapot(CONST D3DXMATRIXA16 *pView, 
	CONST D3DXMATRIXA16 *pProject)
{
	m_pEffect->SetMatrix("matTexture", (D3DXMATRIXA16*)&m_matTexture);
	m_pEffect->SetMatrix("matWorld", &m_matWorld);
	m_pEffect->SetMatrix("matView", (D3DXMATRIXA16*) pView);
	m_pEffect->SetMatrix("matProject", (D3DXMATRIXA16*) pProject);

	// Draw teapot
	LPDIRECT3DVERTEXBUFFER9 pVB;
	LPDIRECT3DINDEXBUFFER9 pIB;

	m_pShinyTeapot->m_pLocalMesh->GetVertexBuffer(&pVB);
	m_pShinyTeapot->m_pLocalMesh->GetIndexBuffer(&pIB);

	m_pd3dDevice->SetStreamSource(0, pVB, 0, sizeof(ENVMAPPEDVERTEX));
	m_pd3dDevice->SetFVF(ENVMAPPEDVERTEX::FVF);
	m_pd3dDevice->SetIndices(pIB);

	UINT uPasses;
	m_pEffect->Begin(&uPasses, 0);

	for(unsigned int iPass = 0; iPass < uPasses; iPass++)
	{
		m_pEffect->Pass(iPass);

		m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0,
			0, m_pShinyTeapot->m_pLocalMesh->GetNumVertices(),
			0, m_pShinyTeapot->m_pLocalMesh->GetNumFaces());
	}

	m_pEffect->End();

	SAFE_RELEASE(pVB);
	SAFE_RELEASE(pIB);

	return S_OK;
}
//-----------------------------------------------------------------------------

HRESULT CMyD3DApplication::Render()
{
	// Begin the scene
	if (SUCCEEDED(m_pd3dDevice->BeginScene()))
	{
		m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
			D3DCOLOR_COLORVALUE(0.1, 0.1, 0.1, 0.0), 1, 0);

		// Projection matrix
		D3DXMATRIXA16 tmpMatrix;
		D3DXMATRIXA16 tmpProjectMatrix;

		FLOAT fAspect = (FLOAT)m_d3dsdBackBuffer.Width
			/ (FLOAT)m_d3dsdBackBuffer.Height;

		D3DXMatrixPerspectiveFovLH(&tmpProjectMatrix,
			D3DX_PI * 0.4f, fAspect, 0.5f, 100.0f);

		D3DXMatrixTranslation(&tmpMatrix, -0.57f, 0.0f, 0.0f);
		D3DXMatrixMultiply(&m_matProject, &tmpProjectMatrix, &tmpMatrix);

		// shader filtering + RGBE decompression
		m_pEffect->SetTechnique("Cube");
		m_pEffect->SetTexture("texCubeMap", m_pCubeMapRGBE);
		RenderTeapot(&m_matView, &m_matProject);

		D3DXMatrixTranslation(&tmpMatrix,  0.57f, 0.0f, 0.0f);
		D3DXMatrixMultiply(&m_matProject, &tmpProjectMatrix, &tmpMatrix);

		// standard bilinear filtering
		m_pEffect->SetTechnique("CubeStandardBilinearFiltering"); 
		m_pEffect->SetTexture("texCubeMap", m_pCubeMapRGBA);
		RenderTeapot(&m_matView, &m_matProject);

		// Output statistics
		m_pFont->DrawText(2,  0, D3DCOLOR_ARGB(255,255,255,0), m_strFrameStats);
		m_pFont->DrawText(2, 17, D3DCOLOR_ARGB(255,255,255,0), m_strDeviceStats);

		m_pFont->DrawText(2, 40, D3DCOLOR_ARGB(255,190,190,190), "Pixel Shader filtering");
		m_pFont->DrawText(2, 57, D3DCOLOR_ARGB(255,190,190,190), "& RGBE decompression");
		m_pFont->DrawText(((float)m_d3dsdBackBuffer.Width / 2), 40, 
			D3DCOLOR_ARGB(255,190,190,190), "Hardware 32-bit RGBA filtering");
		// End the scene.
		m_pd3dDevice->EndScene();
	}

	return S_OK;
}
//-----------------------------------------------------------------------------

HRESULT CMyD3DApplication::InitDeviceObjects()
{
	 // Load the file objects
	 if(FAILED(m_pShinyTeapot->Create(m_pd3dDevice, _T("teapot.x"))))
		  return D3DAPPERR_MEDIANOTFOUND;

	 // Set mesh properties
	 m_pShinyTeapot->SetFVF(m_pd3dDevice, ENVMAPPEDVERTEX::FVF);

	 // Restore the device-dependent objects
	 m_pFont->InitDeviceObjects(m_pd3dDevice);

	 // Create Effect object
	LPD3DXBUFFER pCompilationErrors;

	if(FAILED(D3DXCreateEffectFromFile(m_pd3dDevice, "effectFile.fx", NULL, 
		NULL, 0, NULL, &m_pEffect, &pCompilationErrors)))
	{
		pCompilationErrors->AddRef();

		char* pError = reinterpret_cast<char*>(
			pCompilationErrors->GetBufferPointer());
		MessageBox(0, pError, "ERROR", MB_OK);

		pCompilationErrors->Release();
		return E_FAIL;
	}
	return S_OK;
}
//-----------------------------------------------------------------------------

HRESULT loadCubeMapFace(IDirect3DCubeTexture9* pCubeMap, D3DCUBEMAP_FACES face, 
	const char* szFileName)
{
	IDirect3DSurface9* pSurface;
	D3DXIMAGE_INFO imageInfo;
	if (pCubeMap->GetCubeMapSurface(face, 0, &pSurface) == D3D_OK)
	{
		if (FAILED(D3DXLoadSurfaceFromFile(pSurface, NULL, NULL, szFileName, NULL, 
			D3DX_FILTER_POINT, 0, &imageInfo)))
		{
			MessageBox(0, szFileName, "Error loading texture", MB_OK);
			return E_FAIL;
		}

		pSurface->Release();
		return D3D_OK;
	}
	return E_FAIL;
}
//-----------------------------------------------------------------------------

HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
	// InitDeviceObjects for file objects (build textures and vertex buffers)
	m_pShinyTeapot->RestoreDeviceObjects(m_pd3dDevice);
	m_pFont->RestoreDeviceObjects();
	m_pEffect->OnResetDevice();

	// Create the cubemap, with a format that matches the backbuffer
	if(! (m_d3dCaps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP))
	{
		MessageBox(0, "No cube-maps!", "ERROR", MB_OK);
		return E_FAIL;
	}

	if(FAILED(D3DXCreateCubeTexture(m_pd3dDevice, CUBEMAP_RESOLUTION, 1,
		0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pCubeMapRGBA)))
	{
		MessageBox(0, "CreateCubeTexture", "ERROR", MB_OK);
		return E_FAIL;
	}

	if(FAILED(D3DXCreateCubeTexture(m_pd3dDevice, CUBEMAP_RESOLUTION, 1,
		0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pCubeMapRGBE)))
	{
		MessageBox(0, "CreateCubeTexture", "ERROR", MB_OK);
		return E_FAIL;
	}

	loadCubeMapFace(m_pCubeMapRGBA, D3DCUBEMAP_FACE_POSITIVE_X, "lobbyxpos.TGA");
	loadCubeMapFace(m_pCubeMapRGBA, D3DCUBEMAP_FACE_NEGATIVE_X, "lobbyxneg.TGA");
	loadCubeMapFace(m_pCubeMapRGBA, D3DCUBEMAP_FACE_POSITIVE_Y, "lobbyypos.TGA");
	loadCubeMapFace(m_pCubeMapRGBA, D3DCUBEMAP_FACE_NEGATIVE_Y, "lobbyyneg.TGA");
	loadCubeMapFace(m_pCubeMapRGBA, D3DCUBEMAP_FACE_POSITIVE_Z, "lobbyzpos.TGA");
	loadCubeMapFace(m_pCubeMapRGBA, D3DCUBEMAP_FACE_NEGATIVE_Z, "lobbyzneg.TGA");

	loadCubeMapFace(m_pCubeMapRGBE, D3DCUBEMAP_FACE_POSITIVE_X, "lobbyxposRGBE.TGA");
	loadCubeMapFace(m_pCubeMapRGBE, D3DCUBEMAP_FACE_NEGATIVE_X, "lobbyxnegRGBE.TGA");
	loadCubeMapFace(m_pCubeMapRGBE, D3DCUBEMAP_FACE_POSITIVE_Y, "lobbyyposRGBE.TGA");
	loadCubeMapFace(m_pCubeMapRGBE, D3DCUBEMAP_FACE_NEGATIVE_Y, "lobbyynegRGBE.TGA");
	loadCubeMapFace(m_pCubeMapRGBE, D3DCUBEMAP_FACE_POSITIVE_Z, "lobbyzposRGBE.TGA");
	loadCubeMapFace(m_pCubeMapRGBE, D3DCUBEMAP_FACE_NEGATIVE_Z, "lobbyznegRGBE.TGA");

	return S_OK;
}
//-----------------------------------------------------------------------------

HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
	m_pShinyTeapot->InvalidateDeviceObjects();
	m_pFont->InvalidateDeviceObjects();

	m_pEffect->OnLostDevice();

	SAFE_RELEASE(m_pCubeMapRGBA);
	SAFE_RELEASE(m_pCubeMapRGBE);

	return S_OK;
}
//-----------------------------------------------------------------------------

HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
	m_pFont->DeleteDeviceObjects();
	m_pShinyTeapot->Destroy();

	SAFE_RELEASE(m_pEffect);

	return S_OK;
}
//-----------------------------------------------------------------------------

HRESULT CMyD3DApplication::FinalCleanup()
{
	SAFE_DELETE(m_pFont);
	SAFE_DELETE(m_pShinyTeapot);

	return S_OK;
}
//-----------------------------------------------------------------------------

HRESULT CMyD3DApplication::ConfirmDevice(D3DCAPS9* pCaps, DWORD dwBehavior,
	D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat)
{
	if(dwBehavior & D3DCREATE_PUREDEVICE)
		return E_FAIL;

	if(!(pCaps->TextureCaps & D3DPTEXTURECAPS_CUBEMAP)
		&& !(dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING)
		&& !(pCaps->VertexShaderVersion >= D3DVS_VERSION(1, 0))
		&& !(pCaps->PixelShaderVersion >= D3DPS_VERSION(2, 0)))
	{
		return E_FAIL;
	}

	return S_OK;
}
//-----------------------------------------------------------------------------

LRESULT CMyD3DApplication::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam,
	LPARAM lParam)
{
	if(WM_LBUTTONDOWN == uMsg)
	{
		D3DXMATRIXA16 matCursor;
		D3DXQUATERNION qCursor = D3DUtil_GetRotationFromCursor(m_hWnd);
		D3DXMatrixRotationQuaternion(&matCursor, &qCursor);
		D3DXMatrixTranspose(&matCursor, &matCursor);
		D3DXMatrixMultiply(&m_matTrackBall, &m_matTrackBall, &matCursor);

		SetCapture(m_hWnd);
		m_bCapture = TRUE;
		return 0;
	}

	if(WM_LBUTTONUP == uMsg)
	{
		D3DXMATRIXA16 matCursor;
		D3DXQUATERNION qCursor = D3DUtil_GetRotationFromCursor(m_hWnd);
		D3DXMatrixRotationQuaternion(&matCursor, &qCursor);
		D3DXMatrixMultiply(&m_matTrackBall, &m_matTrackBall, &matCursor);

		ReleaseCapture();
		m_bCapture = FALSE;
		return 0;
	}

	// Pass remaining messages to default handler
	return CD3DApplication::MsgProc(hWnd, uMsg, wParam, lParam);
}
//-----------------------------------------------------------------------------
