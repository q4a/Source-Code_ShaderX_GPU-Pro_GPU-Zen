///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Some enumerations for MD5 to work.
 * 
 * @file
 *  MD5Constants.h
 * @author
 *  Carsten Wenzel
 * @date
 *  31.10.2001
 * @brief
 *  Some enumerations for MD5 to work.
 */



#ifndef _MD5CONSTANTS_H_INCLUDED_
#define _MD5CONSTANTS_H_INCLUDED_

#pragma once



namespace ScrambledArchive
{

        

    enum EMD5InitConstants
    {
        MD5_INIT_STATE_0 = 0x67452301,
        MD5_INIT_STATE_1 = 0xefcdab89,
        MD5_INIT_STATE_2 = 0x98badcfe,
        MD5_INIT_STATE_3 = 0x10325476
    };



    enum EMD5TransformationConstants
    {
        MD5_S11 =  7,
        MD5_S12 = 12,
        MD5_S13 = 17,
        MD5_S14 = 22,
        MD5_S21 =  5,
        MD5_S22 =  9,
        MD5_S23 = 14,
        MD5_S24 = 20,
        MD5_S31 =  4,
        MD5_S32 = 11,
        MD5_S33 = 16,
        MD5_S34 = 23,
        MD5_S41 =  6,
        MD5_S42 = 10,
        MD5_S43 = 15,
        MD5_S44 = 21
    };



    enum EMD5TransformationConstantsRound1
    {
        MD5_T01 = 0xd76aa478,
        MD5_T02 = 0xe8c7b756,
        MD5_T03 = 0x242070db,
        MD5_T04 = 0xc1bdceee,
        MD5_T05 = 0xf57c0faf,
        MD5_T06 = 0x4787c62a,
        MD5_T07 = 0xa8304613,
        MD5_T08 = 0xfd469501,
        MD5_T09 = 0x698098d8,
        MD5_T10 = 0x8b44f7af,
        MD5_T11 = 0xffff5bb1,
        MD5_T12 = 0x895cd7be,
        MD5_T13 = 0x6b901122,
        MD5_T14 = 0xfd987193,
        MD5_T15 = 0xa679438e,
        MD5_T16 = 0x49b40821
    };



    enum EMD5TransformationConstantsRound2
    {
        MD5_T17 = 0xf61e2562,
        MD5_T18 = 0xc040b340,
        MD5_T19 = 0x265e5a51,
        MD5_T20 = 0xe9b6c7aa,
        MD5_T21 = 0xd62f105d,
        MD5_T22 = 0x02441453,
        MD5_T23 = 0xd8a1e681,
        MD5_T24 = 0xe7d3fbc8,
        MD5_T25 = 0x21e1cde6,
        MD5_T26 = 0xc33707d6,
        MD5_T27 = 0xf4d50d87,
        MD5_T28 = 0x455a14ed,
        MD5_T29 = 0xa9e3e905,
        MD5_T30 = 0xfcefa3f8,
        MD5_T31 = 0x676f02d9,
        MD5_T32 = 0x8d2a4c8a
    };



    enum EMD5TransformationConstantsRound3
    {
        MD5_T33 = 0xfffa3942,
        MD5_T34 = 0x8771f681,
        MD5_T35 = 0x6d9d6122,
        MD5_T36 = 0xfde5380c,
        MD5_T37 = 0xa4beea44,
        MD5_T38 = 0x4bdecfa9,
        MD5_T39 = 0xf6bb4b60,
        MD5_T40 = 0xbebfbc70,
        MD5_T41 = 0x289b7ec6,
        MD5_T42 = 0xeaa127fa,
        MD5_T43 = 0xd4ef3085,
        MD5_T44 = 0x04881d05,
        MD5_T45 = 0xd9d4d039,
        MD5_T46 = 0xe6db99e5,
        MD5_T47 = 0x1fa27cf8,
        MD5_T48 = 0xc4ac5665
    };



    enum EMD5TransformationConstantsRound4
    {
        MD5_T49 = 0xf4292244,
        MD5_T50 = 0x432aff97,
        MD5_T51 = 0xab9423a7,
        MD5_T52 = 0xfc93a039,
        MD5_T53 = 0x655b59c3,
        MD5_T54 = 0x8f0ccc92,
        MD5_T55 = 0xffeff47d,
        MD5_T56 = 0x85845dd1,
        MD5_T57 = 0x6fa87e4f,
        MD5_T58 = 0xfe2ce6e0,
        MD5_T59 = 0xa3014314,
        MD5_T60 = 0x4e0811a1,
        MD5_T61 = 0xf7537e82,
        MD5_T62 = 0xbd3af235,
        MD5_T63 = 0x2ad7d2bb,
        MD5_T64 = 0xeb86d391,
    };



} // namespace ScrambledArchive



#endif // #ifndef _MD5CONSTANTS_H_INCLUDED_
