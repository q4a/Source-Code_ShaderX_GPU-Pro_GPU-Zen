/****************************************************************************/
/* Copyright (c) 2011, Ola Olsson
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE
 */
/****************************************************************************/
#ifndef _Config_h_
#define _Config_h_

// Note: since each uniform buffer may at most be 64kb, care must be taken that the grid resolution doesnt exceed this
//       e.g. 1920x1200 wont do with 16x16.
#define LIGHT_GRID_TILE_DIM_X 32
#define LIGHT_GRID_TILE_DIM_Y 32

// Max screen size of 1920x1080
#define LIGHT_GRID_MAX_DIM_X ((1920 + LIGHT_GRID_TILE_DIM_X - 1) / LIGHT_GRID_TILE_DIM_X)
#define LIGHT_GRID_MAX_DIM_Y ((1080 + LIGHT_GRID_TILE_DIM_Y - 1) / LIGHT_GRID_TILE_DIM_Y)

// the maximum number if lights supported, this is limited by constant buffer size, commonly
// this is 64kb, but AMD only seem to allow 2048 lights...
#define NUM_POSSIBLE_LIGHTS (1024)

// Must be a power of two, configure to suit hardware/wishes, also limited at runtime 
// by glGet GL_MAX_COLOR_TEXTURE_SAMPLES. The smaller value is used.
// Note that 16x is supported using super sampling on NVidia GPUs. For higher, CSAA should really be used.
#define MAX_ALLOWED_MSAA_SAMPLES 4


#define DEFAULT_SHADING_MODEL_STRING "SHADING_MODEL_DEFAULT"

#ifdef DISABLE_CUDA
	#define ENABLE_CUDA_OPT_TILED_DEFERRED 0
#else // !DISABLE_CUDA
	// If enabled then the machinery to support the CUDA optimized tiled deferred implementation is enabled.
	// Note that this requires a step to do copy all the G-Buffers as CUDA unfortunately cannot access MSAA textures.
	// Especially note that this copy requires a LOT of memory, at high resolutions and MSAA levels!
	#define ENABLE_CUDA_OPT_TILED_DEFERRED 1
#endif // DISABLE_CUDA

#endif  // _Config_h_
