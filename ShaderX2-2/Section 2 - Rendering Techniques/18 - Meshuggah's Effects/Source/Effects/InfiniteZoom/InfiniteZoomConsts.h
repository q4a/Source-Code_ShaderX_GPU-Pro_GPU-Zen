///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Static const variables for \e Effects::CInfiniteZoom. For use in InfiniteZoom.cpp only!
 *
 * @file
 *  InfiniteZoomConsts.h
 * @author
 *  Carsten Wenzel
 * @date
 *  27.11.2001
 * @brief
 *  Static const variables for \e Effects::CInfiniteZoom. For use in InfiniteZoom.cpp only!
 */



#ifndef _INFINITEZOOMCONSTS_H_INCLUDED_
#define _INFINITEZOOMCONSTS_H_INCLUDED_

#pragma once



const char c_pcEffectName[]     = "Infinite Zoom";
const char c_pcSpotTexture[]    = "InfiniteZoom\\Spot.jpg";
const char c_pcCreditsTexture[] = "InfiniteZoom\\Credits.jpg";
const char c_pcBassSpectrum[]   = "InfiniteZoom\\BassSpectrum.raw";

const char* c_pcTextureNames[] = 
{
    "InfiniteZoom\\InfiniteZoom00.dds",
    "InfiniteZoom\\InfiniteZoom01.dds",
    "InfiniteZoom\\InfiniteZoom02.dds",
    "InfiniteZoom\\InfiniteZoom03.dds",
    "InfiniteZoom\\InfiniteZoom04.dds",
    "InfiniteZoom\\InfiniteZoom05.dds",
    "InfiniteZoom\\InfiniteZoom06.dds",
    "InfiniteZoom\\InfiniteZoom07.dds",
    "InfiniteZoom\\InfiniteZoom08.dds",
    "InfiniteZoom\\InfiniteZoom09.dds",
};

const float c_pfZoomKeys[ Effects::CInfiniteZoom::NUM_ZOOM_KEYS + 2 ][ 3 ] = 
{
    // repeat first key for catmull rom spline interpolation
    {
        -7.1496582031250000e+004f,
        -2.8656005859375000e+004f,
         2.3437500000000000e+003f
    },

    // first key
    {
        -7.1496582031250000e+004f,
        -2.8656005859375000e+004f,
         2.3437500000000000e+003f
    },

    {
        -7.1102905273437500e+004f,
        -2.8335571289062500e+004f,
         1.1718750000000000e+003f
    },

    {
        -7.1171569824218750e+004f,
        -2.7969360351562500e+004f,
         5.8593750000000000e+002f
    },

    {
        -7.1306610107421875e+004f,
        -2.7832031250000000e+004f,
         2.9296875000000000e+002f
    },

    {
        -7.1213912963867188e+004f,
        -2.7908706665039063e+004f,
         1.4648437500000000e+002f
    },

    {
        -7.1166992187500000e+004f,
        -2.7975082397460938e+004f,
         7.3242187500000000e+001f
    },

    {
        -7.1167278289794922e+004f,
        -2.7975940704345703e+004f,
         3.6621093750000000e+001f
    },

    {
        -7.1167135238647461e+004f,
        -2.7976512908935547e+004f,
         1.8310546875000000e+001f
    },

    {
        -7.1166992187500000e+004f,
        -2.7976584434509277e+004f,
         9.1552734375000000e+000f
    },

    // last key 
    {
        -7.1163022518157959e+004f,
        -2.7974832057952881e+004f,
         4.5776367187500000e+000f
    },

    // repeat last key for catmull rom spline interpolation
    {
        -7.1163022518157959e+004f,
        -2.7974832057952881e+004f,
         4.5776367187500000e+000f
    }
};



#endif // #ifndef _INFINITEZOOMCONSTS_H_INCLUDED_