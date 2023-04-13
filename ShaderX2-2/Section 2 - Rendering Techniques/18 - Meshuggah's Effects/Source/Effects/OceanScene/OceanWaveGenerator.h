///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e COceanWaveGenerator.
 *
 * @file
 *  OceanWaveGenerator.h
 * @author
 *  Carsten Wenzel
 * @date
 *  22.12.2001
 * @brief
 *  Declaration of \e COceanWaveGenerator.
 */



#ifndef _OCEANWAVEGENERATOR_H_INCLUDED_
#define _OCEANWAVEGENERATOR_H_INCLUDED_

#pragma once



#include <vector>
#include <complex>



namespace Effects
{



    /**
    * An ocean wave generator class.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  07.03.2002
    * @brief
    *  An ocean wave generator class.
    */
    class COceanWaveGenerator
    {
    public:
        enum EGridSize
        {
            /** 
            * Logarithmic size of generated ocean height map.
            * <tt>log2( #GRIDSIZE )</tt>.
            */
            LOG_GRIDSIZE = 6,

            /** 
            * Size of generated ocean height map.
            */
            GRIDSIZE     = 64
        };

    public:
        /** 
        * Constructor.
        * 
        * @brief
        *  Constructor.
        */
        COceanWaveGenerator();

        /** 
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        ~COceanWaveGenerator();

        /**
        * Sets ocean parameters.
        * 
        * @param ulSeed
        *  Seed value for base wave frequencies and amplitudes.
        * @param fWindDirection
        *  Angle of wind direction (in radians).
        * @param fWindSpeed
        *  Wind speed.
        * @param fWaveHeight
        *  Maximum wave height.
        * @param fDirectionalDependence
        *  Directional dependence value. Must be a integer multiple of two and >= 2!
        * @param fChoppyWavesFactor
        *  Choppy waves factor.
        * @param fSuppressSmallWavesFactor
        *  Suppress small waves factor.
        * @brief
        *  Sets ocean parameters.
        */
        void SetOceanParameters( unsigned long ulSeed, float fWindDirection, float fWindSpeed,
                                 float fWaveHeight, float fDirectionalDependence, 
                                 float fChoppyWavesFactor, float fSuppressSmallWavesFactor );

        /**
        * Updates ocean height map.
        * 
        * @param fTime
        *  Time of ocean animation (in seconds).
        * @brief
        *  Updates ocean height map.
        */
        void UpdateOceanMap( float fTime );

        /** 
        * Gets ocean height at (x, y). x and y get wrapped to \c GRIDSIZE.
        * 
        * @brief
        *  Gets ocean height at (x, y).
        */
        float GetOceanHeight( int x, int y );

        /** 
        * Gets ocean x displacement at (x, y). x and y get wrapped to \c GRIDSIZE.
        * 
        * @brief
        *  Gets ocean x displacement at (x, y).
        */
        float GetOceanDisplaceX( int x, int y );

        /** 
        * Gets ocean y displacement at (x, y). x and y get wrapped to \c GRIDSIZE.
        * 
        * @brief
        *  Gets ocean y displacement at (x, y).
        */
        float GetOceanDisplaceY( int x, int y );

        /** 
        * Gets x component of ocean normal at (x, y). x and y get wrapped to \c GRIDSIZE.
        * 
        * @brief
        *  Gets x component of ocean normal at (x, y).
        */
        float GetOceanNormalX( int x, int y );

        /** 
        * Gets y component of ocean normal at (x, y). x and y get wrapped to \c GRIDSIZE.
        * 
        * @brief
        *  Gets y component of ocean normal at (x, y).
        */
        float GetOceanNormalY( int x, int y );

    public:
        /** 
        * \e pi.
        * 
        * @brief
        *  \e pi.
        */
        static const float sc_fPi;

        /** 
        * \e G (9.81 m/s^2).
        * 
        * @brief
        *  \e G.
        */
        static const float sc_fGravity;

        /** 
        * Default wind direction (in radians).
        * 
        * @brief
        *  Default wind direction (in radians).
        */
        static const float sc_fDefaultWindDirection;

        /** 
        * Default wind speed.
        * 
        * @brief
        *  Default wind speed.
        */
        static const float sc_fDefaultWindSpeed;
        
        /** 
        * Default wave height.
        * 
        * @brief
        *  Default wave height.
        */
        static const float sc_fDefaultWaveHeight;

        /** 
        * Default directional dependence.
        * 
        * @brief
        *  Default directional dependence.
        */
        static const float sc_fDefaultDirectionalDependence;

        /** 
        * Default <em>Choppy waves</em> factor.
        * 
        * @brief
        *  Default <em>Choppy waves</em> factor.
        */
        static const float sc_fDefaultChoppyWaveFactor;
        
        /** 
        * Default <em>Suppress small waves</em> factor.
        * 
        * @brief
        *  Default <em>Suppress small waves</em> factor.
        */
        static const float sc_fDefaultSuppressSmallWavesFactor;
        
        /** 
        * Grid delta of ocean mesh.
        * 
        * @brief
        *  Grid delta of ocean mesh.
        */
        static const float sc_fGridDelta;

    private:
        /** 
        * A type holding a complex number \e a + <em>b</em><b>i</b>.
        * \e a and \e b are of type \c float.
        * 
        * @brief
        *  A type holding a complex number \e a + <em>b</em><b>i</b>.
        */
        typedef std::complex< float > Complex;

    private:
        /** 
        * Gets index for (x, y) in #m_colH0.
        * 
        * @brief
        *  Gets index for (x, y) in #m_colH0.
        */
        int IndexH0( int x, int y );

        /** 
        * Gets fft index for (x, y) in #m_colH, #m_colD or #m_colN.
        * 
        * @brief
        *  Gets fft index for (x, y) in #m_colH, #m_colD or #m_colN.
        */
        int IndexFFT( int x, int y );

        /** 
        * Gets index for (x, y) in #m_colH, #m_colD or #m_colN.
        * 
        * @brief
        *  Gets index for (x, y) in #m_colH, #m_colD or #m_colN.
        */
        int Index( int x, int y );
        
        /** 
        * Helper for GetOceanHeight(), GetOceanDisplaceX(), GetOceanDisplaceY(),
        * GetOceanNormalX(), GetOceanNormalY().
        * 
        * @brief
        *  Helper for GetOceanHeight(), GetOceanDisplaceX(), GetOceanDisplaceY(),
        *  GetOceanNormalX(), GetOceanNormalY().
        */
        int Offset( int x, int y );

        /** 
        * Helper for GetOceanHeight(), GetOceanDisplaceX(), GetOceanDisplaceY(),
        * GetOceanNormalX(), GetOceanNormalY().
        * 
        * @brief
        *  Helper for GetOceanHeight(), GetOceanDisplaceX(), GetOceanDisplaceY(),
        *  GetOceanNormalX(), GetOceanNormalY().
        */
        int OffsetWrap( int x, int y );

        /**
        * Calculates 1D FFT in-place. Helper for FFT2D().
        * 
        * @param iDir
        *  Direction. \b 1 forward, \b -1 inverse.
        * @param real
        *  Array of real components.
        * @param imag
        *  Array of imaginary components.
        * @brief
        *  Calculates 1D FFT in-place.
        */
        void FFT( int iDir, float* real, float* imag );

        /**
        * Calculates 2D FFT in-place.
        * 
        * @param iDir
        *  Direction. \b 1 means forward, \b -1 means inverse.
        * @param cmp
        *  2D array of complex numbers <tt>(GRIDSIZE * GRIDSIZE)</tt>.
        * @brief
        *  Calculates 2D FFT in-place.
        */
        void FFT2D( int iDir, std::vector< Complex >& cmp );

        /** 
        * Gets x component of wave vector K.
        * 
        * @brief
        *  Gets x component of wave vector K.
        */
        float GetKx( int x );

        /** 
        * Gets y component of wave vector K.
        * 
        * @brief
        *  Gets y component of wave vector K.
        */
        float GetKy( int y );

        /** 
        * <tt> x * x </tt>.
        * 
        * @brief
        *  <tt> x * x </tt>.
        */
        float sqrf( float x );

        /** 
        * Gets Phillips spepctrum for wave vector K.
        * 
        * @brief
        *  Gets Phillips spepctrum for wave vector K.
        */
        float PhillipsSpectrum( float fKx, float fKy );

        /** 
        * Gets angular frequency for length of wave vector K.
        * 
        * @brief
        *  Gets angular frequency for length of wave vector K.
        */
        float GetAngularFrequency( float fKLength );

    private:
        /** 
        * Array of base wave frequencies and amplitudes.
        * 
        * @brief
        *  Array of base wave frequencies and amplitudes.
        */
        std::vector< Complex > m_colH0;

        /** 
        * Array of ocean heights.
        * 
        * @brief
        *  Array of ocean heights.
        */
        std::vector< Complex > m_colH;

        /** 
        * Array of ocean displacements.
        * 
        * @brief
        *  Array of ocean displacements.
        */
        std::vector< Complex > m_colD;

        /** 
        * Array of ocean normals.
        * 
        * @brief
        *  Array of ocean normals.
        */
        std::vector< Complex > m_colN;

        /** 
        * Array of angular frequencies.
        * 
        * @brief
        *  Array of angular frequencies.
        */
        std::vector< float > m_colAngularFreq;

        /** 
        * Array of inverse lengths of wave vectors K.
        * 
        * @brief
        *  Array of inverse lengths of wave vectors K.
        */
        std::vector< float > m_colKScale;

        /** 
        * Normalized x component of wind direction.
        * 
        * @brief
        *  Normalized x component of wind direction.
        */
        float m_fWindX;    

        /** 
        * Normalized y component of current wind direction.
        * 
        * @brief
        *  Normalized y component of current wind direction.
        */
        float m_fWindY;

        /** 
        * Current wind speed.
        * 
        * @brief
        *  Current wind speed.
        */
        float m_fWindSpeed;

        /** 
        * Current <em>Largest possible wave</em> value.
        * 
        * @brief
        *  Current <em>Largest possible wave</em> value.
        */
        float m_fLargestPossibleWave;

        /** 
        * Current <em>Suppress small waves</em> value.
        * 
        * @brief
        *  Current <em>Suppress small waves</em> value.
        */
        float m_fSuppressSmallWaves;

        /** 
        * Current wave height.
        * 
        * @brief
        *  Current wave height.
        */
        float m_fWaveHeight;

        /** 
        * Current directional dependence.
        * 
        * @brief
        *  Current directional dependence.
        */
        float m_fDirectionalDependence;

        /** 
        * Current <em>Choppy waves</em> factor.
        * 
        * @brief
        *  Current <em>Choppy waves</em> factor.
        */
        float m_fChoppyWaveFactor;

        /** 
        * Current <em>Suppress small waves</em> factor.
        * 
        * @brief
        *  Current <em>Suppress small waves</em> factor.
        */
        float m_fSuppressSmallWavesFactor;
    };



#include "OceanWaveGenerator.inl"



} // namespace Effects



#endif // #ifndef _OCEANWAVEGENERATOR_H_INCLUDED_