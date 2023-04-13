// this class comes from the NVidia SDK

#ifndef _ScreenCapture_H
#define _ScreenCapture_H

//  ===============================================
//  ScreenCapture class definition.
//  ===============================================

#include <windows.h>
//#include <windef.h>
#include <vfw.h>

//#include <vfw.h>
//#include <windows.h>


#define TEXT_HEIGHT	20
#define AVIIF_KEYFRAME	0x00000010L // this frame is a key frame.
#define BUFSIZE 260

class ScreenCapture {
private:

    int width;
    int height;

	DWORD dwTextFormat;
	char szText[BUFSIZE];

    HRESULT SaveToFile(LPTSTR pszFile, HBITMAP hBMP, HDC hDC);

public:

    ScreenCapture();
    ~ScreenCapture();

    bool Snap(HWND,int width,int height);
};

#endif
