///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e Effects::COceanSceneDialog.
 *
 * @file
 *  OceanScene.h
 * @author
 *  Carsten Wenzel
 * @date
 *  07.03.2002
 * @brief
 *  Declaration of \e Effects::COceanSceneDialog.
 */



#ifndef _OCEANSCENEDIALOG_H_INCLUDED_
#define _OCEANSCENEDIALOG_H_INCLUDED_

#pragma once



#include "Timer.h"
#include "Random.h"



namespace Effects
{



    class COceanWaveGenerator;



    /**
    * Dialog class for \e Hair effect settings.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  07.03.2002
    * @brief
    *  Dialog class for \e Hair effect settings.
    */
    class COceanSceneDialog : public CDialog
    {
    public:
        /**
        * Constructor.
        * 
        * @param pParent
        *  Pointer to parent window.
        * @brief
        *  Constructor.
        */
        COceanSceneDialog( CWnd* pParent = 0 );   

        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        virtual ~COceanSceneDialog();       

        /**
        * Returns the current time of the effect.
        * 
        * @return
        *  The current time of the effect.
        * @brief
        *  Returns the current time of the effect.
        */
        float GetTime();       

        /**
        * Updates ocean settings.
        * 
        * @param kOceanWaveGenerator
        *  Reference to an ocean heigth map generator object receiving the new ocean settings.
        * @brief
        *  Updates ocean settings.
        */
        void UpdateOceanSettings( COceanWaveGenerator& kOceanWaveGenerator );

        /**
        * Returns current value for dynamic range of environment map.
        * 
        * @return
        *  The current value for dynamic range of environment map.
        * @brief
        *  Returns current value for dynamic range of environment map.
        */
        float GetDynamicRangeOfEnvMap();

    protected:
        /**
        * Validates all user input in effect dialog.
        * 
        * @brief
        *  Validates all user input in effect dialog.
        */
        virtual void DoDataExchange( CDataExchange* pDX );

        /**
        * Initializes effect dialog.
        * 
        * @brief
        *  Initializes effect dialog.
        */
        virtual BOOL OnInitDialog();

        /**
        * Windows message handler. Pauses #m_kTimer while dialog 
        * is being dragged around.
        * 
        * @brief
        *  Windows message handler.
        */
        virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );

        /**
        * Handler for OK button. Empty function to prevent the dialog from being closed
        * when user accidentally presses \e Return.
        * 
        * @brief
        *  Handler for default button.
        */
        virtual void OnOK();

        /**
        * Message handler for <em>Wind direction</em> slider bar.
        * 
        * @brief
        *  Message handler for <em>Wind direction</em> slider bar.
        */
        afx_msg void OnReleasedCaptureWindDirection( NMHDR* pNMHDR, LRESULT* pResult );

        /**
        * Message handler for <em>Wind speed</em> slider bar.
        * 
        * @brief
        *  Message handler for <em>Wind speed</em> slider bar.
        */
        afx_msg void OnReleasedCaptureWindSpeed( NMHDR* pNMHDR, LRESULT* pResult );

        /**
        * Message handler for <em>Wave height</em> slider bar.
        * 
        * @brief
        *  Message handler for <em>Wave height</em> slider bar.
        */
        afx_msg void OnReleasedCaptureWaveHeight( NMHDR* pNMHDR, LRESULT* pResult );

        /**
        * Message handler for <em>Directional dependence</em> slider bar.
        * 
        * @brief
        *  Message handler for <em>Directional dependence</em> slider bar.
        */
        afx_msg void OnReleasedCaptureDirectionalDependence( NMHDR* pNMHDR, LRESULT* pResult );

        /**
        * Message handler for <em>Choppy waves</em> slider bar.
        * 
        * @brief
        *  Message handler for <em>Choppy waves</em> slider bar.
        */
        afx_msg void OnReleasedCaptureChoppyWavesFactor( NMHDR* pNMHDR, LRESULT* pResult );

        /**
        * Message handler for <em>Suppress small waves</em> slider bar.
        * 
        * @brief
        *  Message handler for <em>Suppress small waves</em> slider bar.
        */
        afx_msg void OnReleasedCaptureSuppressSmallWavesFactor( NMHDR* pNMHDR, LRESULT* pResult );

        /**
        * Message handler for <em>Dynamic range of environment map</em> slider bar.
        * 
        * @brief
        *  Message handler for <em>Dynamic range of environment map</em> slider bar.
        */
        afx_msg void OnReleasedCaptureDynamicRangeOfEnvMap( NMHDR* pNMHDR, LRESULT* pResult );

        /**
        * Message handler for <em>Create new height data</em> button.
        * 
        * @brief
        *  Message handler for <em>Create new height data</em> button.
        */        
        afx_msg void OnNewHeightData();

        DECLARE_MESSAGE_MAP()        

    private:
        /**
        * Updates <em>Wind direction</em> label.
        * New value is based on current position of the corresponding slider bar.
        * 
        * @brief
        *  Updates <em>Wind direction</em> label.
        */
        void UpdateWindDirectionEdit();

        /**
        * Updates <em>Wind speed</em> label.
        * New value is based on current position of the corresponding slider bar.
        * 
        * @brief
        *  Updates <em>Wind speed</em> label.
        */
        void UpdateWindSpeedEdit();

        /**
        * Updates <em>Wave height</em> label.
        * New value is based on current position of the corresponding slider bar.
        * 
        * @brief
        *  Updates <em>Wave height</em> label.
        */
        void UpdateWaveHeightEdit();

        /**
        * Updates <em>Directional dependence</em> label.
        * New value is based on current position of the corresponding slider bar.
        * 
        * @brief
        *  Updates <em>Directional dependence</em> label.
        */
        void UpdateDirectionalDependenceEdit();

        /**
        * Updates <em>Choppy waves</em> label.
        * New value is based on current position of the corresponding slider bar.
        * 
        * @brief
        *  Updates <em>Choppy waves</em> label.
        */
        void UpdateChoppyWavesFactorEdit();

        /**
        * Updates <em>Suppress small waves</em> label.
        * New value is based on current position of the corresponding slider bar.
        * 
        * @brief
        *  Updates <em>Suppress small waves</em> label.
        */
        void UpdateSuppressSmallWavesFactorEdit();

        /**
        * Updates <em>Dynamic range of environment map</em> label.
        * New value is based on current position of the corresponding slider bar.
        * 
        * @brief
        *  Updates <em>Dynamic range of environment map</em> label.
        */
        void UpdateDynamicRangeOfEnvMapEdit();

    private:
        /** 
        * Current <em>Wind direction</em> value mapped from slider bar.
        * 
        * @brief
        *  Current <em>Wind direction</em> value mapped from slider bar.
        */
        int m_iWindDir;

        /** 
        * Current <em>Wind speed</em> value mapped from slider bar.
        * 
        * @brief
        *  Current <em>Wind speed</em> value mapped from slider bar.
        */
        int m_iWindSpeed;

        /** 
        * Current <em>Wave height</em> value mapped from slider bar.
        * 
        * @brief
        *  Current <em>Wave height</em> value mapped from slider bar.
        */
        int m_iWaveHeight;

        /** 
        * Current <em>Directional dependence</em> value mapped from slider bar.
        * 
        * @brief
        *  Current <em>Directional dependence</em> value mapped from slider bar.
        */
        int m_iDirectionalDependence;

        /** 
        * Current <em>Choppy waves</em> value mapped from slider bar.
        * 
        * @brief
        *  Current <em>Choppy waves</em> value mapped from slider bar.
        */
        int m_iChoppyWavesFactor;

        /** 
        * Current <em>Suppress small waves</em> value mapped from slider bar.
        * 
        * @brief
        *  Current <em>Suppress small waves</em> value mapped from slider bar.
        */
        int m_iSuppressSmallWavesFactor;

        /** 
        * Current <em>Dynamic range of environment map</em> value mapped from slider bar.
        * 
        * @brief
        *  Current <em>Dynamic range of environment map</em> value mapped from slider bar.
        */
        int m_iDynamicRangeOfEnvMap;

        /** 
        * Current seed value for ocean wave generator.
        * 
        * @brief
        *  Current seed value for ocean wave generator.
        */
        unsigned long m_ulSeed;

        /** 
        * A flag indicating that ocean settings have changed.
        * 
        * @brief
        *  A flag indicating that ocean settings have changed.
        */
        bool m_bSettingsChanged;

        /** 
        * Timer for effect in browser mode.
        * 
        * @brief
        *  Timer for effect in browser mode.
        */
        Shared::CTimer m_kTimer;

        /** 
        * Random number generator.
        * 
        * @brief
        *  Random number generator.
        */
        Shared::CRandom m_kRnd;
    };



} // namespace Effects



#endif // #ifndef _OCEANSCENEDIALOG_H_INCLUDED_