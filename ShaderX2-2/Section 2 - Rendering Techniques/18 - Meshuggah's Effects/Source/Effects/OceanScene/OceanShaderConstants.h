///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Definition of constants for \e Ocean shader.
 *
 * @file
 *  OceanShaderConstants.h
 * @author
 *  Carsten Wenzel
 * @date
 *  07.03.2002
 * @brief
 *  Definition of constants for \e Ocean shader.
 */



/** 
 * Scale factor for bump map uv.
 */
#define CV_BUMP_UV_SCALE            10

/** 
 * Scale factor for ocean mesh.
 */
#define CV_MESH_XYZ_SCALE           11 

/** 
 * Offset for tiling the ocean mesh.
 */
#define CV_MESH_XYZ_OFFSET          12 

/** 
 * Curvature factor.
 */
#define CV_CURVATURE                13

/** 
 * Power factor for fresnel term approximation.
 */
#define CV_FRESNELAPPROX_POWFACTOR  14

/** 
 * Dynamic range factor for enironment map.
 */
#define CV_DYNAMIC_RANGE            15



/** 
 * \e Shallow water color.
 */
#define CP_OCEAN_COLOR_LIGHT        0

/** 
 * \e Deep water color.
 */
#define CP_OCEAN_COLOR_DARK         1
