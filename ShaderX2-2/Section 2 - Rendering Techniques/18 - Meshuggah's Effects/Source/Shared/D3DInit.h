///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e Shared::CD3DInit.
 *
 * @file
 *  D3DInit.h
 * @author
 *  Carsten Wenzel
 * @date
 *  21.10.2001
 * @brief
 *  Declaration of \e Shared::CD3DInit.
 */



#ifndef _D3DINIT_H_INCLUDED_
#define _D3DINIT_H_INCLUDED_

#pragma once



namespace Shared
{



    /**
    * Contains methods that simplify D3D device creation.
    * Also functions as wrapper around d3d8.dll. Loads the dll on demand.
    * As D3D8 has only one main entry point namely Direct3DCreate8( ... )
    * -- the rest is accessed via COM -- it is easy to load it
    * dynamically to avoid early program termination if dll is
    * not installed on a system. Implemented as singleton.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  21.10.2001
    * @brief
    *  Contains methods that simplify D3D device creation.
    */
    class CD3DInit
    {
    public:
        /**
        * Function pointer for Direct3DCreate( ... ).
        *
        * @brief
        *  Function pointer for Direct3DCreate( ... ).
        */
        typedef IDirect3D8* (WINAPI *FPDirect3DCreate8) ( UINT SDKVersion );

    public:
        /**
        * Retrieves a suitable 32 bit color buffer format.
        *
        * @param pD3D
        *  Pointer to an \e IDirect3D interface which should be used to retrieve
        *  a suitable 32 bit color buffer format.
        * @param eColBufFormat
        *  The retrieved color buffer format. Only valid if \e Get32bitColorBufferFormat
        *  returns \b true.
        * @return
        *  \b true if successful else \b false.
        * @brief
        *  Retrieves a suitable 32 bit color buffer format.
        */
        bool Get32bitColorBufferFormat( IDirect3D8* pD3D, D3DFORMAT &eColBufFormat );

        /**
        * Retrieves a suitable z buffer format.
        *
        * @param pD3D
        *  Pointer to an \e IDirect3D interface which should be used to retrieve
        *  a suitable z buffer format.
        * @param eColBufFormat
        *  Format of color buffer that should be used in conjunction with the z buffer format.
        * @param eZBufFormat
        *  The z buffer format. Only valid if \e GetZBufferFormat returns \b true.
        * @return
        *  \b true if successful else \b false.
        * @brief
        *  Retrieves a suitable z buffer format.
        */
        bool GetZBufferFormat( IDirect3D8* pD3D, D3DFORMAT eColBufFormat, D3DFORMAT &eZBufFormat );

        /**
        * Creates and initializes a D3D device.
        *
        * @param hWndFocus
        *  Focus of windows for which D3D device should be created.
        * @param sD3DPresParams
        *  Present parameters for D3D device.
        * @param pD3DDev
        *  Returned D3D device.
        * @param bHardwareVertexProcessing
        *  If \b true then method will request a D3D device supporting
        *  hardware vertex processing. In debug builds this flag is ignored
        *  and a D3D device supporting only software vertex processing
        *  is created by default.
        *
        * @return
        *  \b true if successful else \b false.
        *  - Possible causes for a failed method call:
        *    -# No DirectX 8.1 installed
        *    -# Device doesn't support hardware vertex processing and
        *       \e bHardwareVertexProcessing was set to \b true
        *
        * @brief
        *  Creates and initializes a D3D device.
        */
        bool InitializeD3D( HWND hWndFocus, D3DPRESENT_PARAMETERS& sD3DPresParams,
                            IDirect3DDevice8*& pD3DDev, bool bHardwareVertexProcessing = true );

        /**
        * Returns the only instance of CD3DInit.
        *
        * @return
        *  The only instance of CD3DInit.
        * @brief
        *  Returns the only instance of CD3DInit.
        */
        static CD3DInit& GetInstance();

        /**
        * Destructor.
        *
        * @brief
        *  Destructor.
        */
        ~CD3DInit();

        /**
        * Creates a Direct3D object and returns a pointer to its interface.
        *
        * @return
        *  Pointer to D3D interface. \b NULL if creation failed.
        * @brief
        *  Creates a Direct3D object and returns a pointer to its interface.
        */
        IDirect3D8* Direct3DCreate8();

    private:
        /**
        * Private constructor as class is implemented as singleton.
        *
        * @brief
        *  Constructor.
        */
        CD3DInit();

    private:
        /**
        * Handle to d3d8.dll.
        *
        * @brief
        *  Handle to d3d8.dll.
        */
        HMODULE m_hD3D8;

        /**
        * Function pointer to Direct3DCreate8( ... ).
        *
        * @brief
        *  Function pointer to Direct3DCreate8( ... ).
        */
        FPDirect3DCreate8 m_pfCreate;
    };



} // namespace WindowSystem



#endif // _D3DINIT_H_INCLUDED_
