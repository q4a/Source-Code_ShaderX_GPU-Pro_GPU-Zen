///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Definition of constants for <em>Soft objects</em> and \e Environment shader.
 *
 * @file
 *  SoftObjectsShaderConstants.h
 * @author
 *  Carsten Wenzel
 * @date
 *  13.03.2002
 * @brief
 *  Definition of constants for <em>Soft objects</em> and \e Environment shader.
 */



/** 
 * First row of <em>World x View x Projection</em> matrix.
 */
#define CV_WORLDVIEWPROJ_0  0

/** 
 * Second row of <em>World x View x Projection</em> matrix.
 */
#define CV_WORLDVIEWPROJ_1  1

/** 
 * Third row of <em>World x View x Projection</em> matrix.
 */
#define CV_WORLDVIEWPROJ_2  2

/** 
 * Fourth row of <em>World x View x Projection</em> matrix.
 */
#define CV_WORLDVIEWPROJ_3  3

/** 
 * Viewer position.
 */
#define CV_VIEWERPOS        12

/** 
 * Pseudo refraction index.
 */
#define CV_REFRACT          13

/** 
 * Attenuation factor for approximated fresnel term.
 */
#define CV_FRENSEL_FACTOR   14

/** 
 * Multi purpose constants (0.0, 0.5, 1.0, 2.0).
 */
#define CV_CONSTANTS        20
