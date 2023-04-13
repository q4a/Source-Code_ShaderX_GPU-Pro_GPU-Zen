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

#ifndef __SOFT_SHADOWS_H
#define __SOFT_SHADOWS_H

struct SVertex
{
    FLOAT x, y, z;
    DWORD diffuse_color;
    FLOAT s, t, r;
};

class CSoftShadows
{
    protected:

    static CSoftShadows     *ms_instance;           // program instance
    bool                    m_isFinished;           // program should exit?
    HWND                    m_hWnd;                 // window handle
    IDirect3D9              *m_lpD3D;               // D3D object
    IDirect3DDevice9        *m_lpDevice;            // D3D device
    float                   m_pos[3];               // viewer position
    float                   m_pitch;                // viewer pitch
    float                   m_yaw;                  // viewer yaw
    D3DXMATRIXA16           m_rot;                  // rotation matrix
    c_scene                 *m_scene;               // scene to render
    c_objMd3                *m_cattaur;             // cattaur

    ///
    ///     Creates the window
    ///
    bool createWindow();

    ///
    ///     Checks the device capabilities
    ///
    bool checkCaps(D3DDEVTYPE a_devType);

    ///
    ///     Initializes direct X
    ///
    bool initDX();

    ///
    ///     Cleans direct X
    ///
    void cleanDX();

    ///
    ///     Window events callback
    ///
    static LRESULT CALLBACK wndProc(HWND a_hWnd, UINT a_msg, WPARAM a_wParam,
        LPARAM a_lParam);

    public:

    ///
    ///     Creates a soft shadows application
    ///
    CSoftShadows();

    ///
    ///     Destroys the soft shadows application
    ///
    ~CSoftShadows();

    ///
    ///     Is the soft shadows application finished?
    ///
    bool isFinished();

    ///
    ///     Terminates the soft shadows application
    ///
    void finish();

    ///
    ///     Initializes the application
    ///
    bool init();

    ///
    ///     Renders the scene
    ///
    void render();

    ///
    ///     Updates the scene
    ///
    void update(const float elapsed);

    ///
    ///     Sets the soft shadows application instance
    ///
    static CSoftShadows *setInstance(CSoftShadows *a_instance);

    ///
    ///     Gets the soft shadows application instance
    ///
    static CSoftShadows *getInstance();
};

#endif
