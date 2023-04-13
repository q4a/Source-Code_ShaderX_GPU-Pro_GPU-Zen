///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Definition of constants for <em>Render sun</em> shader.
 *
 * @file
 *  RenderSunShaderConstants.h
 * @author
 *  Carsten Wenzel
 * @date
 *  04.03.2002
 * @brief
 *  Definition of constants for <em>Render sun</em> shader.
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
 * Multi purpose constants (0.0, 0.5, 1.0, 2.0).
 */
#define CV_CONSTANTS        20

/** 
 * Scale factors for texture uv of sphere mesh.
 */
#define CV_TEXSCALE         21

/** 
 * Texture offset to generate first texture set.
 */
#define CV_TEXOFFSET_0      30

/** 
 * Texture offset to generate second texture set.
 */
#define CV_TEXOFFSET_1      31

/** 
 * Texture offset to generate thrid texture set.
 */
#define CV_TEXOFFSET_2      32

/** 
 * Texture offset to generate fourth texture set.
 */
#define CV_TEXOFFSET_3      33



/** 
 * Blend value for sun surface.
 */
#define CP_SURFACE_BLEND_VALUE 0
