#include "ScreenCapture.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <windowsx.h>

HANDLE  MakeDib( HBITMAP hbitmap, UINT bits );
HBITMAP LoadBMPFromFB(HWND win, int w, int h );

/*
    ===============================================
        Constructors, Destructor
    ===============================================
*/


ScreenCapture::ScreenCapture()
{
    width  = -1;
    height = -1;
}


ScreenCapture::~ScreenCapture()
{

}

bool ScreenCapture::Snap(HWND win,int width,int height)
{
    // Get an image and stuff it into a bitmap.
    HBITMAP bmp;
    bmp = LoadBMPFromFB(win, width, height );
	SaveToFile("screencap.bmp",bmp,GetDC(win));
	return true;
}

static HBITMAP LoadBMPFromFB(HWND win, int w, int h )
{
    // Create a normal DC and a memory DC for the entire screen. The 
    // normal DC provides a "snapshot" of the screen contents. The 
    // memory DC keeps a copy of this "snapshot" in the associated 
    // bitmap. 
 
    HDC hdcScreen = GetDC(win);
    HDC hdcCompatible = CreateCompatibleDC(hdcScreen); 
 
    // Create a compatible bitmap for hdcScreen. 

    HBITMAP hbmScreen = CreateCompatibleBitmap(hdcScreen, 
                           //  GetDeviceCaps(hdcScreen, HORZRES), 
                           //  GetDeviceCaps(hdcScreen, VERTRES)); 
                             w, 
                             h ); 

    if (hbmScreen == 0)
        {
        fprintf( stderr, "hbmScreen == NULL\nExiting.\n" );
        exit( -1 );
        //errhandler("hbmScreen", hwnd); 
        }
 
    // Select the bitmaps into the compatible DC. 
 
    if (!SelectObject(hdcCompatible, hbmScreen)) 
        {
        fprintf( stderr, "Couldn't SelectObject()\nExiting.\n" );
        exit( -1 );
        //errhandler("Compatible Bitmap Selection", hwnd); 
        }
 
    // Hide the application window. 
 
     //       ShowWindow(hwnd, SW_HIDE); 
 
    //Copy color data for the entire display into a 
    //bitmap that is selected into a compatible DC. 
 
    if (!BitBlt(hdcCompatible, 
                 0,0, 
                 w, h, 
                 hdcScreen, 
                // 512,512,
                 0, 0,
                 SRCCOPY)) 
        {
        fprintf( stderr, "Screen to Compat Blt Failed\nExiting.\n" );
        exit( -1 );
        //errhandler("Screen to Compat Blt Failed", hwnd); 
        }
 
   // Redraw the application window.  
   //ShowWindow(hwnd, SW_SHOW); 

    DeleteDC( hdcCompatible );

    return( hbmScreen );
}

HRESULT ScreenCapture::SaveToFile(LPTSTR pszFile, HBITMAP hBMP, HDC hDC) 
{ 
    HANDLE hf;
    BITMAPFILEHEADER hdr;
    PBITMAPINFOHEADER pbih;
    LPBYTE lpBits;
    DWORD dwTotal;
    DWORD cb;
    BYTE *hp;
    DWORD dwTmp;
    BITMAP bmp;
    PBITMAPINFO pbi;
	WORD    cClrBits;

	if (!GetObject(hBMP, sizeof(BITMAP), (LPSTR)&bmp)) 
        return 0;

    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
    if (cClrBits == 1) 
        cClrBits = 1; 
    else if (cClrBits <= 4) 
        cClrBits = 4; 
    else if (cClrBits <= 8) 
        cClrBits = 8; 
    else if (cClrBits <= 16) 
        cClrBits = 16; 
    else if (cClrBits <= 24) 
        cClrBits = 24; 
    else cClrBits = 32; 
	
	if (cClrBits != 24)
	{
		pbi = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER) +
			sizeof(RGBQUAD) * (1<< cClrBits)); 
	}
	else 
		pbi = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER)); 
	
    pbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    pbi->bmiHeader.biWidth = bmp.bmWidth; 
    pbi->bmiHeader.biHeight = bmp.bmHeight; 
    pbi->bmiHeader.biPlanes = bmp.bmPlanes; 
    pbi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
    if (cClrBits < 24) 
        pbi->bmiHeader.biClrUsed = (1<<cClrBits); 
	
    pbi->bmiHeader.biCompression = BI_RGB; 
    pbi->bmiHeader.biSizeImage = ((pbi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
		* pbi->bmiHeader.biHeight; 
	pbi->bmiHeader.biClrImportant = 0; 

    pbih = (PBITMAPINFOHEADER) pbi; 
    lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);
	
    if (!lpBits) 
		return E_FAIL;
	
    if (!GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi, 
        DIB_RGB_COLORS)) 
    {
		return E_FAIL;
    }
	
    hf = CreateFile(pszFile, 
		GENERIC_READ | GENERIC_WRITE, 
		(DWORD) 0, 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		(HANDLE) NULL); 
    if (hf == INVALID_HANDLE_VALUE) 
		return E_FAIL;
    hdr.bfType = 0x4d42;
    hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
		pbih->biSize + pbih->biClrUsed 
		* sizeof(RGBQUAD) + pbih->biSizeImage); 
    hdr.bfReserved1 = 0; 
    hdr.bfReserved2 = 0; 
	
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
		pbih->biSize + pbih->biClrUsed 
		* sizeof (RGBQUAD);
	
    if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), 
        (LPDWORD) &dwTmp,  NULL)) 
    {
		return E_FAIL;
    }
	
    if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) 
		+ pbih->biClrUsed * sizeof (RGBQUAD), 
		(LPDWORD) &dwTmp, ( NULL)) )
		return E_FAIL;
	
    dwTotal = cb = pbih->biSizeImage; 
    hp = lpBits; 
    if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)) 
		return E_FAIL;
	
	CloseHandle(hf);
    GlobalFree((HGLOBAL)lpBits);
	LocalFree(pbi);

	return S_OK;
}




