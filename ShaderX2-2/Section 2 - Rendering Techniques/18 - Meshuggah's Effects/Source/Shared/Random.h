///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e Shared::CRandom.
 *
 * @file
 *  Random.h
 * @author
 *  Carsten Wenzel
 * @date
 *  01.12.2001
 * @brief
 *  Declaration of \e Shared::CRandom.
 */



#ifndef _RANDOM_H_INCLUDED_
#define _RANDOM_H_INCLUDED_

#pragma once



namespace Shared
{



    /**
    * A random number class. It implements the <em>mersenne twister</em> algorithm to produce 
    * "high quality" pseudo random numbers.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  01.12.2001
    * @brief
    *  A random number class.
    */
    class CRandom  
    {
    public:
        enum EMersenneTwisterInitialSeed
        {
            /** 
            * Initial seed value.
            */
            INITIAL_SEED = 4357U
        };

    public:
        /**
        * Constructor.
        * 
        * @param ulSeed
        *  Seed value.
        * @brief
        *  Constructor.
        */
        CRandom( unsigned long ulSeed = INITIAL_SEED );

        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        ~CRandom();

        /**
        * Sets seed value for random number generator.
        * 
        * @param ulSeed
        *  New seed value.
        * @brief
        *  Sets seed value for random number generator.
        */
        void SetSeed( unsigned long ulSeed );

        /**
        * Returns a \c integer value from a draw of random number generator.
        * 
        * @return
        *  Random \c integer value.
        * @brief
        *  Returns a \c integer value from a draw of random number generator.
        */
        unsigned long GetInteger();

        /**
        * Returns a \c double value from a draw of random number generator.
        * 
        * @return
        *  Random \c double value in range [0..1).
        * @brief
        *  Returns a \c integer value from a draw of random number generator.
        */
        double Get();

        /**
        * Returns a \c double value from a draw of random number generator.
        * 
        * @return
        *  Random \c double value in range [<em>dLower</em>..<em>dHigher</em>).
        * @brief
        *  Returns a \c double value from a draw of random number generator.
        */
        double GetInRange( double dLower = 0.0, double dUpper = 1.0 );

        /**
        * Returns a gaussian distributed \c double value 
        * from a draw of random number generator.
        * 
        * @return
        *  Random \c double value.
        * @brief
        *  Returns a gaussian distributed \c double value 
        *  from a draw of random number generator.
        */
        double GetGauss( double dMean = 0.0, double dStdDeviation = 1.0 );

    private:
        /** 
        * Constants for <em>mersenne twister</em> algorithms.
        * 
        * @brief
        *  Constants for <em>mersenne twister</em> algorithms.
        */
        enum EMersenneTwisterConstants
        {
            GENERATOR   = 69069U,

            N = 624U,
            M = 397U,

            MATRIX_A   = 0x9908B0DFU,
            UPPER_MASK = 0x80000000U,
            LOWER_MASK = 0x7FFFFFFFU,

            TEMPERING_MASK_B = 0x9D2C5680U,
            TEMPERING_MASK_C = 0xEFC60000U
        };

        /** 
        * Reloads state array (#m_ulState).
        * 
        * @brief
        *  Reloads state array.
        */
        void Reload();

        /** 
        * <tt>uilX >> 11</tt>
        * 
        * @brief
        *  <tt>uilX >> 11</tt>
        */
        unsigned long TemperingShiftU( unsigned long ulX );

        /** 
        * <tt>uilX << 7</tt>
        * 
        * @brief
        *  <tt>uilX << 7</tt>
        */
        unsigned long TemperingShiftS( unsigned long ulX );

        /** 
        * <tt>uilX << 15</tt>
        * 
        * @brief
        *  <tt>uilX << 15</tt>
        */
        unsigned long TemperingShiftT( unsigned long ulX );

        /** 
        * <tt>uilX >> 18</tt>
        * 
        * @brief
        *  <tt>uilX >> 18</tt>
        */
        unsigned long TemperingShiftL( unsigned long ulX );

    private:
        /** 
        * Current index in state array.
        * 
        * @brief
        *  Current index in state array.
        */
        unsigned long m_ulIndex;

        /** 
        * State array.
        * 
        * @brief
        *  State array.
        */
        unsigned long m_ulState[ N ];
    };



#include "Random.inl"



} // namespace Shared



#endif // #ifndef _RANDOM_H_INCLUDED_