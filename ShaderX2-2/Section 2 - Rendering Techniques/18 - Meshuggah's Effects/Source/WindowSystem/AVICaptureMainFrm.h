///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e WindowSystem::CAVICaptureMainFrame.
 * 
 * @file
 *  DemoMainFrm.h
 * @author
 *  Carsten Wenzel
 * @date
 *  21.12.2001
 * @brief
 *  Declaration of \e WindowSystem::CAVICaptureMainFrame.
 */



#ifndef _AVICAPTUREMAINFRAME_H_INCLUDED_
#define _AVICAPTUREMAINFRAME_H_INCLUDED_

#pragma once



#include "RenderWnd.h"



namespace EffectSystem
{
    class CEffectSequencer;
}

namespace Shared
{
    class CAVICompressor;
}



namespace WindowSystem
{



    /**
    * The mainframe window class of the demo's AVI capture.
    * 
    * @author
    *  Carsten Wenzel
    * @date
    *  11.10.2001
    * @brief
    *  The mainframe window class of the demo's AVI capture.
    */
    class CAVICaptureMainFrame : public CFrameWnd
                               , public IRenderWnd
    {
    public:
        /** 
        * Constructor.
        * 
        * @brief
        *  Constructor.
        */
        CAVICaptureMainFrame();

        /** 
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        virtual ~CAVICaptureMainFrame();
        
        /** 
        * Handles \c WM_DESTROY messages. Releases #m_pkEffectSequencer, #m_pkAVICompressor
        * and the D3D device interface.
        * 
        * @brief
        *  Handles \c WM_DESTROY messages.
        */
        afx_msg void OnDestroy();

        virtual bool RenderFrame();

    #ifdef _DEBUG
        /** 
        * MFC debug hook. Only available in debug builds.
        * 
        * @brief
        *  MFC debug hook. Only available in debug builds.
        */
        virtual void AssertValid() const;

        /** 
        * MFC debug hook. Only available in debug builds.
        * 
        * @brief
        *  MFC debug hook. Only available in debug builds.
        */
        virtual void Dump( CDumpContext& dc ) const;
    #endif

    protected:
        /** 
        * Handles \c WM_CREATE messages. Creates the window, initializes the D3D device #m_pD3DDev 
        * using settings from CAVICaptureMainFrame::SetupPresentParamsForCapture() (copied into 
        * #m_sD3DPresParams), creates the AVI compressor object #m_pkAVICompressor and the effect 
        * sequencer object #m_pkEffectSequencer.
        *
        * @param lpCreateStruct
        *  Window creation parameters.
        * @return
        *  \b 0 if successful. \b -1 if creation failed.
        * @brief
        *  Handles \c WM_CREATE messages.
        */
        afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
        
        DECLARE_MESSAGE_MAP()

    private:
        /**
        * Fills a D3D present parameter structure used to create D3D device
        * for capturing an AVI of the demo.
        * 
        * @return
        *  \b true if successful else \b false.
        *  - Possible reasons:
        *   -# Failed to initialize DirectX 8.1
        *   -# No suitable color/depth buffer combination was found
        *
        * @brief
        *  Fills a D3D present parameter structure.
        */
        bool SetupPresentParamsForCapture();

    private:
        /** 
        * Effect sequencer.
        * 
        * @brief
        *  Effect sequencer.
        */
        EffectSystem::CEffectSequencer* m_pkEffectSequencer;

        /** 
        * A pointer to a D3D device interface.
        * 
        * @brief
        *  A pointer to a D3D device interface.
        */
        IDirect3DDevice8* m_pD3DDev;

        /** 
        * AVI compressor.
        * 
        * @brief
        *  AVI compressor.
        */
        Shared::CAVICompressor* m_pkAVICompressor;

        /** 
        * Time for the next frame to be captured.
        * 
        * @brief
        *  Time for the next frame to be captured.
        */
        float m_fTime;

        /** 
        * D3D present parameters.
        * 
        * @brief
        *  D3D present parameters.
        */
        D3DPRESENT_PARAMETERS m_sD3DPresParams;
    };



} // namespace WindowSystem



#endif // #ifndef _AVICAPTUREMAINFRAME_H_INCLUDED_