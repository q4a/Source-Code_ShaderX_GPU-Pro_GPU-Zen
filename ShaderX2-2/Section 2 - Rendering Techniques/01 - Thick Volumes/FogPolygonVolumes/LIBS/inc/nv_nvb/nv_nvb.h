/*********************************************************************NVMH4****
Path:  SDK\LIBS\inc\nv_nvb
File:  nv_nvb.h

Copyright NVIDIA Corporation 2002
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
*AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.



Comments:


******************************************************************************/

#ifndef _nv_nvb_h_
#define _nv_nvb_h_

#ifdef WIN32
#pragma warning (disable:4786) 
#endif

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <queue>
#include <algorithm>

#ifndef _nv_mathdecl_h_
#include <nv_math/nv_math.h>
#endif // _nv_mathdecl_h_

#ifndef _nv_core_h_
#include <nv_core/nv_core.h>
#endif // _nv_core_h_

#ifndef _nv_nvbdecl_h_
#include "nv_nvbdecl.h"
#endif // _nv_nvbdecl_h_

#ifdef WIN32

#ifndef NV_NVB_PROJECT // defined if we are building nv_nvb.lib

#ifndef NV_NVB_EXPORTS

#if _MSC_VER >= 1300

#ifdef _DLL
	///////////////////////////////////////////////*
    #pragma message("  nv_nvb_mtdll_vc7.lib        (MULTITHREADED DLL)\n")
    #pragma comment(lib,"nv_nvb_mtdll_vc7.lib")
#else
    #ifdef _MT
		///////////////////////////////////////////////*
	    #pragma message("  nv_nvb_mt_vc7.lib           (MULTITHREADED)\n")
        #pragma comment(lib,"nv_nvb_mt_vc7.lib")
    #else
		///////////////////////////////////////////////*
	    #pragma message("  nv_nvb_vc7.lib              (SINGLE THREADED)\n")
        #pragma comment(lib,"nv_nvb_vc7.lib")
    #endif
#endif
#else
#ifdef _DLL
	///////////////////////////////////////////////*
    #pragma message("  nv_nvb_mtdll.lib            (MULTITHREADED DLL)\n")
    #pragma comment(lib,"nv_nvb_mtdll.lib")
#else
    #ifdef _MT
		///////////////////////////////////////////////*
	    #pragma message("  nv_nvb_mt.lib               (MULTITHREADED)\n")
        #pragma comment(lib,"nv_nvb_mt.lib")
    #else
		///////////////////////////////////////////////*
	    #pragma message("  nv_nvb.lib                  (SINGLE THREADED)\n")
        #pragma comment(lib,"nv_nvb.lib")
    #endif
#endif
#endif
#endif
#endif // NV_NVB_PROJECT
#endif // WIN32

typedef unsigned int    nv_idx;             // index type

#define NV_BAD_IDX      0xFFFFFFFF

#ifndef _nv_refcount_h_
#include "nv_refcount.h"
#endif // _nv_refcount_h_

#ifndef _nv_animation_h_
#include "nv_animation.h"
#endif // _nv_animation_h_

#ifndef _nv_visitor_h_
#include "nv_visitor.h"
#endif // _nv_visitor_h_

#ifndef _nv_node_h_
#include "nv_node.h"
#endif // _nv_node_h_

#ifndef _nv_texture_h_
#include "nv_texture.h"
#endif // _nv_texture_h_

#ifndef _nv_texcoord_set_h_
#include "nv_texcoord_set.h"
#endif // _nv_texcoord_set_h_

#ifndef _nv_material_h_
#include "nv_material.h"
#endif // _nv_material_h_

#ifndef _nv_camera_h_
#include "nv_camera.h"
#endif // _nv_camera_h_

#ifndef _nv_light_h_
#include "nv_light.h"
#endif // _nv_light_h_

#ifndef _nv_mesh_h_
#include "nv_mesh.h"
#endif // _nv_mesh_h_

#ifndef _nv_model_h_
#include "nv_model.h"
#endif // _nv_model_h_

#ifndef _nv_scene_h_
#include "nv_scene.h"
#endif // _nv_scene_h_

#ifndef _nv_drawmodel_visitor_h_
#include "nv_drawmodel_visitor.h"
#endif // _nv_drawmodel_visitor_h_

#ifndef _nv_factory_h_
#include "nv_factory.h"
#endif // _nv_factory_h_

#define     NVB_LHS     0x00000001
#define     NVB_RHS     0x00000002

bool    DECLSPEC_NV_NVB NVBLoad(const char * file, nv_scene * scene, long options = NVB_RHS);
void    DECLSPEC_NV_NVB NVBSetLogCallback(void* cbfn, unsigned long userparam=0);

#endif //_nv_nvb_h_
