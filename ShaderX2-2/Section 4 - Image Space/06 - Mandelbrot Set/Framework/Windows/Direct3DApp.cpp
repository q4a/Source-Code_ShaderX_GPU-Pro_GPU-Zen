                  /*--.          .-"-. 
                 /   ._.        / ´ ` \
                 \_  (__\       \_°v°_/
                 //   \\        //   \\
                ((     ))      ((     ))
 ¤¤¤¤¤¤¤¤¤¤¤¤¤¤--""---""--¤¤¤¤--""---""--¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
 ¤                 |||            |||                                               ¤
 ¤  Author's note:  |              |                                                ¤
 ¤                                                                                  ¤
 ¤  This file is part of work done by Emil Persson, aka Humus.                      ¤
 ¤  You're free to use the code in any way you like, modified, unmodified           ¤
 ¤  or cut'n'pasted into your own work. But for the good of the whole               ¤
 ¤  programming community I will ask you to as long as possible and to the          ¤
 ¤  extent you find suitable to follow these points:                                ¤
 ¤   * If you use this file and it's contents unmodified, please leave this note.   ¤
 ¤   * If you use a major part of the file in your app, please credit the author(s) ¤
 ¤     in one way or the other as you find suitable, for instance a small notice    ¤
 ¤     in the readme file may do, or perhaps a link to the authors site.            ¤
 ¤   * Share your work and ideas too as much as you can.                            ¤
 ¤    _______                                                                       ¤
 ¤  /` _____ `\;,    Humus                                                          ¤
 ¤ (__(^===^)__)';,  emiper-8@student.luth.se                         ___           ¤
 ¤   /  :::  \   ,;  ICQ #47010716                                  /^   ^\         ¤
 ¤  |   :::   | ,;'                                                ( Ö   Ö )        ¤
 ¤¤¤'._______.'`¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤--°oOo--(_)--oOo°--¤¤*/

#include "Direct3DApp.h"

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")


String Direct3DApp::getErrorString(){
	String error;
	return error;
}

bool Direct3DApp::processCommand(const char *cmd, String &response){
	D3DADAPTER_IDENTIFIER9 id;
	char str[256];

	Tokenizer tok;
	tok.setString(cmd);
	tok.next(str);

	if (stricmp(str, "vendor") == 0){
		d3d->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &id);
		response = id.DeviceName;
	} else if (stricmp(str, "renderer") == 0){
		d3d->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &id);
		response = id.Description;
	} else if (stricmp(str, "version") == 0){
		d3d->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &id);
		response.sprintf("%d.%d.%d.%d", HIWORD(id.DriverVersion.HighPart), LOWORD(id.DriverVersion.HighPart), HIWORD(id.DriverVersion.LowPart),  LOWORD(id.DriverVersion.LowPart));
	} else {
		return RenderApp::processCommand(cmd, response);
	}

	return true;
}


bool Direct3DApp::initAPI(HWND hwnd, int fsaaLevel, bool fullscreen, unsigned int refreshRate){
	renderer = NULL;

	if ((d3d = Direct3DCreate9(D3D_SDK_VERSION)) == NULL){
		MessageBox(hwnd, "Couldn't initialize Direct3D\nMake sure you have DirectX 9.0 or later installed.", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	d3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

	if (!checkBaseCapabilities()){
		return false;
	}

	PixelFormat pf;
	memset(&pf, 0, sizeof(pf));
	selectPixelFormat(pf);

    memset(&d3dpp, 0, sizeof(d3dpp));

	//D3DDISPLAYMODE displayMode;
	//d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode);

	if (pf.alphaBits > 0){
		d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	} else {
		d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	}

	RECT r;
	GetClientRect(hwnd, &r);
	d3dpp.BackBufferWidth  = r.right;
	d3dpp.BackBufferHeight = r.bottom;

	if (fullscreen){
		d3dpp.Windowed = FALSE;
		d3dpp.FullScreen_RefreshRateInHz = refreshRate;
	} else {
		d3dpp.Windowed = TRUE;
	}
	d3dpp.BackBufferCount        = 1;
	d3dpp.MultiSampleType        = (D3DMULTISAMPLE_TYPE) fsaaLevel;
	d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;

	d3dpp.EnableAutoDepthStencil = pf.depthBits > 0;
	d3dpp.AutoDepthStencilFormat = (pf.depthBits > 16)? ((pf.stencilBits > 0)? D3DFMT_D24S8 : D3DFMT_D24X8) : D3DFMT_D16;

    if (FAILED(d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, /*D3DCREATE_PUREDEVICE | */ D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &dev))){
		MessageBox(hwnd, "Couldn't create Direct3D device interface.", "Error", MB_OK | MB_ICONERROR);
        return false;
    }


/*
	if (fullScreen){
		d3dpp.FullScreen_RefreshRateInHz = Refresh;
		if (vSync){
			d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
			d3dpp.BackBufferCount = 2;
		} else {
			d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
			d3dpp.BackBufferCount = 1;
		}
	} else {

	}
*/
	// Disable some odd defaults
	dev->SetRenderState(D3DRS_LIGHTING, FALSE);
	dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);


	renderer = new Direct3DRenderer(caps, dev);

	return true;
}

bool Direct3DApp::load(){
	return true;
}

bool Direct3DApp::unload(){
	return true;
}

void Direct3DApp::beginFrame(){
    dev->BeginScene();
}

bool Direct3DApp::drawFrame(){
	return false;
}

void Direct3DApp::endFrame(){
    dev->EndScene();
    dev->Present(NULL, NULL, NULL, NULL);
}

bool Direct3DApp::closeAPI(HWND hwnd){
	if (renderer != NULL){
		renderer->clear();
		delete renderer;
	}

    if (dev != NULL){
		dev->Release();
		dev = NULL;
	}
    if (d3d != NULL) d3d->Release();

	return true;
}

void Direct3DApp::setViewport(int cx, int cy){
	D3DVIEWPORT9 vp;
	vp.X      = 0;
	vp.Y      = 0;
	vp.Width  = cx;
	vp.Height = cy;
	vp.MinZ   = 0.0f;
	vp.MaxZ   = 1.0f;
	if (dev != NULL){
		//d3dpp.BackBufferWidth  = cx;
		//d3dpp.BackBufferHeight = cy;
		//dev->Reset(&d3dpp);
		dev->SetViewport(&vp);
	}
}


#include <shlobj.h>

void Direct3DApp::screenShot(){
	char path[MAX_PATH];
	SHGetSpecialFolderPath(NULL, path, CSIDL_DESKTOPDIRECTORY, FALSE);

	FILE *file;
	int pos = strlen(path);
#ifndef NO_PNG
	strcpy(path + pos, "\\ScreenShot00.png");
#else
	strcpy(path + pos, "\\ScreenShot00.tga");
#endif // NO_PNG
	pos += 11;
	
	int i = 0;
	do {
		path[pos]     = '0' + (i / 10);
		path[pos + 1] = '0' + (i % 10);
		
		if (file = fopen(path, "r")){
			fclose(file);
		} else {
			CRect rect;
			m_pMainWnd->GetClientRect(rect);
			int w = rect.right - rect.left;
			int h = rect.bottom - rect.top;

			m_pMainWnd->ClientToScreen(rect);		
			
			int mw = GetSystemMetrics(SM_CXSCREEN);
			int mh = GetSystemMetrics(SM_CYSCREEN);

			unsigned char *pixels  = new unsigned char[w * h * 4];

			LPDIRECT3DSURFACE9 surface;

			dev->CreateOffscreenPlainSurface(mw, mh, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &surface, NULL);
			dev->GetFrontBufferData(0, surface);

			D3DLOCKED_RECT lockedRect;
			memset(&lockedRect, 0, sizeof(lockedRect));
			if (surface->LockRect(&lockedRect, NULL, D3DLOCK_READONLY) != D3D_OK){
				surface->Release();
				return;				
			}

			unsigned char *src, *dest = pixels;
			for (int y = 0; y < h; y++){
				src = ((unsigned char *) lockedRect.pBits) + ((rect.top + y) * mw + rect.left) * 4;
				for (int x = 0; x < w; x++){
					dest[0] = src[0];
					dest[1] = src[1];
					dest[2] = src[2];
					dest += 3;
					src += 4;
				}
			}
			surface->UnlockRect();
			surface->Release();

			Image img;
			img.loadFromMemory(pixels, w, h, FORMAT_RGB8, true);
			img.saveImage(path, false);

			break;
		}
		i++;
	} while (i < 100);
}

void Direct3DApp::drawText(char *str, float x, float y, float charWidth, float charHeight){
	D3DXMATRIX mat;
	
	D3DXMatrixIdentity(&mat);
	dev->SetTransform(D3DTS_VIEW, &mat);

	D3DXMatrixOrthoOffCenterLH(&mat, 0, 1, 1, 0, -1, 100);
	dev->SetTransform(D3DTS_PROJECTION, &mat);

	dev->SetFVF(D3DFVF_XYZ | (1 << D3DFVF_TEXCOUNT_SHIFT) | D3DFVF_TEXCOORDSIZE2(0));

	struct {
		float x,y,z;
		float u, v;
	} quadVertices[4];


	quadVertices[0].z = quadVertices[1].z = quadVertices[2].z = quadVertices[3].z = 0;


    float tx,ty, startx = x;

    while (*str){
        if (*str == '\n'){
            y += charHeight;
            x = startx;
        } else {
            tx = float(((unsigned int) *str) & 15) / 16;
            ty = float(((unsigned int) *str) >> 4) / 16;
            
			quadVertices[0].x = x;
			quadVertices[0].y = y;
			quadVertices[0].u = tx;
			quadVertices[0].v = ty;

			quadVertices[1].x = x + charWidth;
			quadVertices[1].y = y;
			quadVertices[1].u = tx + 1.0f / 16;
			quadVertices[1].v = ty;
			
			quadVertices[2].x = x + charWidth;
			quadVertices[2].y = y + charHeight;
			quadVertices[2].u = tx + 1.0f / 16;
			quadVertices[2].v = ty + 1.0f / 16;
			
			quadVertices[3].x = x;
			quadVertices[3].y = y + charHeight;
			quadVertices[3].u = tx;
			quadVertices[3].v = ty + 1.0f / 16;

			dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, quadVertices, 5 * sizeof(float));

            x += charWidth;
        }
        str++;
    }
}
