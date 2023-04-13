                  /*--.          .-"-.
                 /   ._.        / ´ ` \
                 \_  (__\       \_°v°_/
                 //   \\        //   \\
                ((     ))      ((     ))
 ¤¤¤¤¤¤¤¤¤¤¤¤¤¤--""---""--¤¤¤¤--""---""--¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
 ¤                 |||            |||                                               ¤
 ¤  Author's note:  |              |                                                ¤
 ¤                                                                                  ¤
 ¤  This file is part of work done by Emil Persson, aka Humus.                      ¤
 ¤  You're free to use the code in any way you like, modified, unmodified           ¤
 ¤  or cut'n'pasted into your own work. But for the good of the whole               ¤
 ¤  programming community I will ask you to as long as possible and to the          ¤
 ¤  extent you find suitable to follow these points:                                ¤
 ¤   * If you use this file and it's contents unmodified, please leave this note.   ¤
 ¤   * If you use a major part of the file in your app, please credit the author(s) ¤
 ¤     in one way or the other as you find suitable, for instance a small notice    ¤
 ¤     in the readme file may do, or perhaps a link to the authors site.            ¤
 ¤   * Share your work and ideas too as much as you can.                            ¤
 ¤    _______                                                                       ¤
 ¤  /` _____ `\;,    Humus                                                          ¤
 ¤ (__(^===^)__)';,  emiper-8@student.luth.se                         ___           ¤
 ¤   /  :::  \   ,;  ICQ #47010716                                  /^   ^\         ¤
 ¤  |   :::   | ,;'                                                ( Ö   Ö )        ¤
 ¤¤¤'._______.'`¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤--°oOo--(_)--oOo°--¤¤*/

#ifndef _GLEXTENSIONS_H_
#define _GLEXTENSIONS_H_

#include "../Platform.h"

#include <string.h>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/gl.h>
#define wglxGetProcAddress wglGetProcAddress
#endif

#ifdef LINUX
 // So that it doesn't include glext.h, it would clash with this file

#define __glext_h_
#include <GL/glx.h>

#undef GL_ARB_multitexture

//#define wglxGetProcAddress(a) glXGetProcAddressARB((const GLubyte *) (a))
#define wglxGetProcAddress(a) glXGetProcAddress((const GLubyte *) (a))

#endif

#ifndef APIENTRY
#define APIENTRY
#endif

typedef int GLintptrARB;
typedef int GLsizeiptrARB;


extern bool GL_ARB_depth_texture;
extern bool GL_ARB_fragment_program;
extern bool GL_ARB_multisample;
extern bool GL_ARB_multitexture;
extern bool GL_ARB_shadow;
extern bool GL_ARB_shadow_ambient;
extern bool GL_ARB_texture_compression;
extern bool GL_ARB_texture_cube_map;
extern bool GL_ARB_texture_env_add;
extern bool GL_ARB_texture_env_combine;
extern bool GL_ARB_texture_env_crossbar;
extern bool GL_ARB_texture_env_dot3;
extern bool GL_ARB_transpose_matrix;
extern bool GL_ARB_vertex_blend;
extern bool GL_ARB_vertex_buffer_object;
extern bool GL_ARB_vertex_program;

extern bool GL_ATI_element_array;
extern bool GL_ATI_envmap_bumpmap;
extern bool GL_ATI_fragment_shader;
extern bool GL_ATI_map_object_buffer;
extern bool GL_ATI_pn_triangles;
extern bool GL_ATI_texture_float;
extern bool GL_ATI_texture_mirror_once;
extern bool GL_ATI_vertex_array_object;
extern bool GL_ATI_vertex_attrib_array_object;
extern bool GL_ATI_vertex_streams;
extern bool GL_ATIX_texture_env_combine3;
extern bool GL_ATIX_texture_env_route;

extern bool GL_EXT_blend_color;
extern bool GL_EXT_blend_func_separate;
extern bool GL_EXT_blend_minmax;
extern bool GL_EXT_blend_subtract;
extern bool GL_EXT_compiled_vertex_array;
extern bool GL_EXT_draw_range_elements;
extern bool GL_EXT_fog_coord;
extern bool GL_EXT_packed_pixels;
extern bool GL_EXT_polygon_offset;
extern bool GL_EXT_stencil_wrap;
extern bool GL_EXT_texture3D;
extern bool GL_EXT_texture_compression_s3tc;
extern bool GL_EXT_texture_edge_clamp;
extern bool GL_EXT_texture_env_dot3;
extern bool GL_EXT_texture_filter_anisotropic;
extern bool GL_EXT_texture_lod_bias;
extern bool GL_EXT_texture_rectangle;
extern bool GL_EXT_vertex_shader;

extern bool GL_GL2_fragment_shader;
extern bool GL_GL2_shader_objects;
extern bool GL_GL2_vertex_shader;

extern bool GL_HP_occlusion_test;

extern bool GL_NV_blend_square;
extern bool GL_NV_occlusion_query;
extern bool GL_NV_register_combiners;
extern bool GL_NV_texture_env_combine4;
extern bool GL_NV_vertex_array_range;

extern bool GL_SGIS_generate_mipmap;

#if defined(WIN32)

extern bool WGL_ARB_extensions_string;
extern bool WGL_ARB_make_current_read;
extern bool WGL_ARB_multisample;
extern bool WGL_ARB_pbuffer;
extern bool WGL_ARB_pixel_format;
extern bool WGL_ARB_render_texture;

extern bool WGL_ATI_pixel_format_float;

extern bool WGL_EXT_swap_control;

#elif defined(LINUX)

//extern bool GLX_EXT_visual_info;

#endif


// GL_ARB_depth_texture
#define GL_DEPTH_COMPONENT16_ARB        0x81A5
#define GL_DEPTH_COMPONENT24_ARB        0x81A6
#define GL_DEPTH_COMPONENT32_ARB        0x81A7
#define GL_TEXTURE_DEPTH_SIZE_ARB       0x884A
#define GL_DEPTH_TEXTURE_MODE_ARB       0x884B


// GL_ARB_fragment_program
#define GL_FRAGMENT_PROGRAM_ARB                                 0x8804
#define GL_PROGRAM_ALU_INSTRUCTIONS_ARB                         0x8805
#define GL_PROGRAM_TEX_INSTRUCTIONS_ARB                         0x8806
#define GL_PROGRAM_TEX_INDIRECTIONS_ARB                         0x8807
#define GL_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB                  0x8808
#define GL_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB                  0x8809
#define GL_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB                  0x880A
#define GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB                     0x880B
#define GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB                     0x880C
#define GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB                     0x880D
#define GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB              0x880E
#define GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB              0x880F
#define GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB              0x8810
#define GL_MAX_TEXTURE_COORDS_ARB                               0x8871
#define GL_MAX_TEXTURE_IMAGE_UNITS_ARB                          0x8872


// GL_ARB_multisample
#define GL_MULTISAMPLE_ARB              0x809D
#define GL_SAMPLE_ALPHA_TO_COVERAGE_ARB 0x809E
#define GL_SAMPLE_ALPHA_TO_ONE_ARB      0x809F
#define GL_SAMPLE_COVERAGE_ARB          0x80A0
#define GL_SAMPLE_BUFFERS_ARB           0x80A8
#define GL_SAMPLES_ARB                  0x80A9
#define GL_SAMPLE_COVERAGE_VALUE_ARB    0x80AA
#define GL_SAMPLE_COVERAGE_INVERT_ARB   0x80AB
#define GL_MULTISAMPLE_BIT_ARB          0x20000000

typedef void (APIENTRY * PFNGLSAMPLECOVERAGEARBPROC)(GLclampf value, GLboolean invert);
extern PFNGLSAMPLECOVERAGEARBPROC glSampleCoverageARB;


// GL_ARB_multitexture
#define GL_ACTIVE_TEXTURE_ARB               0x84E0
#define GL_CLIENT_ACTIVE_TEXTURE_ARB        0x84E1
#define GL_MAX_ACTIVE_TEXTURES_ARB          0x84E2
#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1
#define GL_TEXTURE2_ARB                     0x84C2
#define GL_TEXTURE3_ARB                     0x84C3
#define GL_TEXTURE4_ARB                     0x84C4
#define GL_TEXTURE5_ARB                     0x84C5
#define GL_TEXTURE6_ARB                     0x84C6
#define GL_TEXTURE7_ARB                     0x84C7
#define GL_TEXTURE8_ARB                     0x84C8
#define GL_TEXTURE9_ARB                     0x84C9
#define GL_TEXTURE10_ARB                    0x84CA
#define GL_TEXTURE11_ARB                    0x84CB
#define GL_TEXTURE12_ARB                    0x84CC
#define GL_TEXTURE13_ARB                    0x84CD
#define GL_TEXTURE14_ARB                    0x84CE
#define GL_TEXTURE15_ARB                    0x84CF
#define GL_TEXTURE16_ARB                    0x84D0
#define GL_TEXTURE17_ARB                    0x84D1
#define GL_TEXTURE18_ARB                    0x84D2
#define GL_TEXTURE19_ARB                    0x84D3
#define GL_TEXTURE20_ARB                    0x84D4
#define GL_TEXTURE21_ARB                    0x84D5
#define GL_TEXTURE22_ARB                    0x84D6
#define GL_TEXTURE23_ARB                    0x84D7
#define GL_TEXTURE24_ARB                    0x84D8
#define GL_TEXTURE25_ARB                    0x84D9
#define GL_TEXTURE26_ARB                    0x84DA
#define GL_TEXTURE27_ARB                    0x84DB
#define GL_TEXTURE28_ARB                    0x84DC
#define GL_TEXTURE29_ARB                    0x84DD
#define GL_TEXTURE30_ARB                    0x84DE
#define GL_TEXTURE31_ARB                    0x84DF

typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC) (GLenum texture);
typedef void (APIENTRY * PFNGLCLIENTACTIVETEXTUREARBPROC) (GLenum texture);
//typedef void (APIENTRY * PFNGLMULTITEXCOORD1DARBPROC) (GLenum texture, GLdouble s);
//typedef void (APIENTRY * PFNGLMULTITEXCOORD1DVARBPROC) (GLenum texture, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1FARBPROC) (GLenum texture, GLfloat s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1FVARBPROC) (GLenum texture, const GLfloat *v);
//typedef void (APIENTRY * PFNGLMULTITEXCOORD1IARBPROC) (GLenum texture, GLint s);
//typedef void (APIENTRY * PFNGLMULTITEXCOORD1IVARBPROC) (GLenum texture, const GLint *v);
//typedef void (APIENTRY * PFNGLMULTITEXCOORD1SARBPROC) (GLenum texture, GLshort s);
//typedef void (APIENTRY * PFNGLMULTITEXCOORD1SVARBPROC) (GLenum texture, const GLshort *v);

//typedef void (APIENTRY * PFNGLMULTITEXCOORD2DARBPROC) (GLenum texture, GLdouble s, GLdouble t);
//typedef void (APIENTRY * PFNGLMULTITEXCOORD2DVARBPROC) (GLenum texture, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FARBPROC) (GLenum texture, GLfloat s, GLfloat t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FVARBPROC) (GLenum texture, const GLfloat *v);
//typedef void (APIENTRY * PFNGLMULTITEXCOORD2IARBPROC) (GLenum texture, GLint s, GLint t);
//typedef void (APIENTRY * PFNGLMULTITEXCOORD2IVARBPROC) (GLenum texture, const GLint *v);
//typedef void (APIENTRY * PFNGLMULTITEXCOORD2SARBPROC) (GLenum texture, GLshort s, GLshort t);
//typedef void (APIENTRY * PFNGLMULTITEXCOORD2SVARBPROC) (GLenum texture, const GLshort *v);

//typedef void (APIENTRY * PFNGLMULTITEXCOORD3DARBPROC) (GLenum texture, GLdouble s, GLdouble t, GLdouble r);
//typedef void (APIENTRY * PFNGLMULTITEXCOORD3DVARBPROC) (GLenum texture, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FARBPROC) (GLenum texture, GLfloat s, GLfloat t, GLfloat r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FVARBPROC) (GLenum texture, const GLfloat *v);
//typedef void (APIENTRY * PFNGLMULTITEXCOORD3IARBPROC) (GLenum texture, GLint s, GLint t, GLint r);
//typedef void (APIENTRY * PFNGLMULTITEXCOORD3IVARBPROC) (GLenum texture, const GLint *v);
//typedef void (APIENTRY * PFNGLMULTITEXCOORD3SARBPROC) (GLenum texture, GLshort s, GLshort t, GLshort r);
//typedef void (APIENTRY * PFNGLMULTITEXCOORD3SVARBPROC) (GLenum texture, const GLshort *v);


//typedef void (APIENTRY * PFNGLMULTITEXCOORD4DARBPROC) (GLenum texture, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
//typedef void (APIENTRY * PFNGLMULTITEXCOORD4DVARBPROC) (GLenum texture, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FARBPROC) (GLenum texture, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FVARBPROC) (GLenum texture, const GLfloat *v);
//typedef void (APIENTRY * PFNGLMULTITEXCOORD4IARBPROC) (GLenum texture, GLint s, GLint t, GLint r, GLint q);
//typedef void (APIENTRY * PFNGLMULTITEXCOORD4IVARBPROC) (GLenum texture, const GLint *v);
//typedef void (APIENTRY * PFNGLMULTITEXCOORD4SARBPROC) (GLenum texture, GLshort s, GLshort t, GLshort r, GLshort q);
//typedef void (APIENTRY * PFNGLMULTITEXCOORD4SVARBPROC) (GLenum texture, const GLshort *v);

extern PFNGLACTIVETEXTUREARBPROC       glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;

extern PFNGLMULTITEXCOORD1FARBPROC  glMultiTexCoord1fARB;
extern PFNGLMULTITEXCOORD1FVARBPROC glMultiTexCoord1fvARB;

extern PFNGLMULTITEXCOORD2FARBPROC  glMultiTexCoord2fARB;
extern PFNGLMULTITEXCOORD2FVARBPROC glMultiTexCoord2fvARB;

extern PFNGLMULTITEXCOORD3FARBPROC  glMultiTexCoord3fARB;
extern PFNGLMULTITEXCOORD3FVARBPROC glMultiTexCoord3fvARB;

extern PFNGLMULTITEXCOORD4FARBPROC  glMultiTexCoord4fARB;
extern PFNGLMULTITEXCOORD4FVARBPROC glMultiTexCoord4fvARB;



// GL_ARB_texture_compression
#define GL_COMPRESSED_ALPHA_ARB					0x84E9
#define GL_COMPRESSED_LUMINANCE_ARB				0x84EA
#define GL_COMPRESSED_LUMINANCE_ALPHA_ARB		0x84EB
#define GL_COMPRESSED_INTENSITY_ARB				0x84EC
#define GL_COMPRESSED_RGB_ARB					0x84ED
#define GL_COMPRESSED_RGBA_ARB					0x84EE
#define GL_TEXTURE_COMPRESSION_HINT_ARB			0x84EF
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB	0x86A0
#define GL_TEXTURE_COMPRESSED_ARB				0x86A1
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB	0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS_ARB		0x86A3


typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE1DARBPROC)(GLenum target, GLint level, GLenum internalFormat, GLsizei width,
				GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)(GLenum target, GLint level, GLenum internalFormat, GLsizei width,
				GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE3DARBPROC)(GLenum target, GLint level, GLenum internalFormat, GLsizei width,
				GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC)(GLenum target, GLint level, GLint xoffset, GLsizei width,
				GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset,
				GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset,
				GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * PFNGLGETCOMPRESSEDTEXIMAGEARBPROC)(GLenum target, GLint lod, GLvoid *img);


extern PFNGLCOMPRESSEDTEXIMAGE1DARBPROC    glCompressedTexImage1DARB;
extern PFNGLCOMPRESSEDTEXIMAGE2DARBPROC    glCompressedTexImage2DARB;
extern PFNGLCOMPRESSEDTEXIMAGE3DARBPROC    glCompressedTexImage3DARB;
extern PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC glCompressedTexSubImage1DARB;
extern PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC glCompressedTexSubImage2DARB;
extern PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC glCompressedTexSubImage3DARB;
extern PFNGLGETCOMPRESSEDTEXIMAGEARBPROC   glGetCompressedTexImageARB;


// GL_ARB_shadow
#define GL_TEXTURE_COMPARE_MODE_ARB     0x884C
#define GL_TEXTURE_COMPARE_FUNC_ARB     0X884D
#define GL_COMPARE_R_TO_TEXTURE_ARB     0x884E


// GL_ARB_shadow_ambient
#define GL_TEXTURE_COMPARE_FAIL_VALUE_ARB 0x80BF


// GL_ARB_texture_cube_map
#define GL_NORMAL_MAP_ARB					0x8511
#define GL_REFLECTION_MAP_ARB				0x8512
#define GL_TEXTURE_CUBE_MAP_ARB				0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP_ARB		0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB	0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB	0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB	0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB  0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB  0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB  0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP_ARB		0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB	0x851C


// GL_ARB_texture_env_combine
#define GL_COMBINE_ARB						0x8570
#define GL_COMBINE_RGB_ARB					0x8571
#define GL_COMBINE_ALPHA_ARB				0x8572
#define GL_SOURCE0_RGB_ARB					0x8580
#define GL_SOURCE1_RGB_ARB					0x8581
#define GL_SOURCE2_RGB_ARB					0x8582
#define GL_SOURCE0_ALPHA_ARB				0x8588
#define GL_SOURCE1_ALPHA_ARB				0x8589
#define GL_SOURCE2_ALPHA_ARB				0x858A
#define GL_OPERAND0_RGB_ARB					0x8590
#define GL_OPERAND1_RGB_ARB					0x8591
#define GL_OPERAND2_RGB_ARB					0x8592
#define GL_OPERAND0_ALPHA_ARB				0x8598
#define GL_OPERAND1_ALPHA_ARB				0x8599
#define GL_OPERAND2_ALPHA_ARB				0x859A
#define GL_RGB_SCALE_ARB					0x8573
#define GL_ADD_SIGNED_ARB					0x8574
#define GL_INTERPOLATE_ARB					0x8575
#define GL_CONSTANT_ARB						0x8576
#define GL_PRIMARY_COLOR_ARB				0x8577
#define GL_PREVIOUS_ARB						0x8578
#define GL_SUBTRACT_ARB						0x84E7


// GL_ARB_texture_env_dot3
#define GL_DOT3_RGB_ARB						0x86AE
#define GL_DOT3_RGBA_ARB					0x86AF


// GL_ARB_transpose_matrix
#define GL_TRANSPOSE_MODELVIEW_MATRIX_ARB	0x84E3
#define GL_TRANSPOSE_PROJECTION_MATRIX_ARB  0x84E4
#define GL_TRANSPOSE_TEXTURE_MATRIX_ARB		0x84E5

typedef void (APIENTRY * PFNGLLOADTRANSPOSEMATRIXFARBPROC)(const GLfloat m[16]);
typedef void (APIENTRY * PFNGLLOADTRANSPOSEMATRIXDARBPROC)(const GLdouble m[16]);
typedef void (APIENTRY * PFNGLMULTTRANSPOSEMATRIXFARBPROC)(const GLfloat m[16]);
typedef void (APIENTRY * PFNGLMULTTRANSPOSEMATRIXDARBPROC)(const GLdouble m[16]);

extern PFNGLLOADTRANSPOSEMATRIXFARBPROC glLoadTransposeMatrixfARB;
extern PFNGLLOADTRANSPOSEMATRIXDARBPROC glLoadTransposeMatrixdARB;
extern PFNGLMULTTRANSPOSEMATRIXFARBPROC glMultTransposeMatrixfARB;
extern PFNGLMULTTRANSPOSEMATRIXDARBPROC glMultTransposeMatrixdARB;


// GL_ARB_vertex_blend
#define GL_MAX_VERTEX_UNITS_ARB				0x86A4
#define GL_ACTIVE_VERTEX_UNITS_ARB			0x86A5
#define GL_WEIGHT_SUM_UNITY_ARB				0x86A6		
#define GL_VERTEX_BLEND_ARB					0x86A7
#define GL_CURRENT_WEIGHT_ARB				0x86A8
#define GL_WEIGHT_ARRAY_TYPE_ARB			0x86A9
#define GL_WEIGHT_ARRAY_STRIDE_ARB			0x86AA
#define GL_WEIGHT_ARRAY_SIZE_ARB			0x86AB
#define GL_WEIGHT_ARRAY_POINTER_ARB			0x86AC
#define GL_WEIGHT_ARRAY_ARB					0x86AD
#define GL_MODELVIEW0_ARB					0x1700
#define GL_MODELVIEW1_ARB					0x850a
#define GL_MODELVIEW2_ARB					0x8722
#define GL_MODELVIEW3_ARB					0x8723
#define GL_MODELVIEW4_ARB					0x8724
#define GL_MODELVIEW5_ARB					0x8725
#define GL_MODELVIEW6_ARB					0x8726
#define GL_MODELVIEW7_ARB					0x8727
#define GL_MODELVIEW8_ARB					0x8728
#define GL_MODELVIEW9_ARB					0x8729
#define GL_MODELVIEW10_ARB					0x872A
#define GL_MODELVIEW11_ARB					0x872B
#define GL_MODELVIEW12_ARB					0x872C
#define GL_MODELVIEW13_ARB					0x872D
#define GL_MODELVIEW14_ARB					0x872E
#define GL_MODELVIEW15_ARB					0x872F
#define GL_MODELVIEW16_ARB					0x8730
#define GL_MODELVIEW17_ARB					0x8731
#define GL_MODELVIEW18_ARB					0x8732
#define GL_MODELVIEW19_ARB					0x8733
#define GL_MODELVIEW20_ARB					0x8734
#define GL_MODELVIEW21_ARB					0x8735
#define GL_MODELVIEW22_ARB					0x8736
#define GL_MODELVIEW23_ARB					0x8737
#define GL_MODELVIEW24_ARB					0x8738
#define GL_MODELVIEW25_ARB					0x8739
#define GL_MODELVIEW26_ARB					0x873A
#define GL_MODELVIEW27_ARB					0x873B
#define GL_MODELVIEW28_ARB					0x873C
#define GL_MODELVIEW29_ARB					0x873D
#define GL_MODELVIEW30_ARB					0x873E
#define GL_MODELVIEW31_ARB					0x873F

typedef void (APIENTRY *PFNGLWEIGHTBVARBPROC)(GLint size, GLbyte *weights);
typedef void (APIENTRY *PFNGLWEIGHTSVARBPROC)(GLint size, GLshort *weights);
typedef void (APIENTRY *PFNGLWEIGHTIVARBPROC)(GLint size, GLint *weights);
typedef void (APIENTRY *PFNGLWEIGHTFVARBPROC)(GLint size, GLfloat *weights);
typedef void (APIENTRY *PFNGLWEIGHTDVARBPROC)(GLint size, GLdouble *weights);
typedef void (APIENTRY *PFNGLWEIGHTUBVARBPROC)(GLint size, GLubyte *weights);
typedef void (APIENTRY *PFNGLWEIGHTUSVARBPROC)(GLint size, GLushort *weights);
typedef void (APIENTRY *PFNGLWEIGHTUIVARBPROC)(GLint size, GLuint *weights);

typedef void (APIENTRY *PFNGLWEIGHTPOINTERARBPROC)(GLint size, GLenum type, GLsizei stride, GLvoid *pointer);
typedef void (APIENTRY *PFNGLVERTEXBLENDARBPROC)(GLint count);

extern PFNGLWEIGHTBVARBPROC  glWeightbvARB;
extern PFNGLWEIGHTSVARBPROC  glWeightsvARB;
extern PFNGLWEIGHTIVARBPROC  glWeightivARB;
extern PFNGLWEIGHTFVARBPROC  glWeightfvARB;
extern PFNGLWEIGHTDVARBPROC  glWeightdvARB;
extern PFNGLWEIGHTUBVARBPROC glWeightubvARB;
extern PFNGLWEIGHTUSVARBPROC glWeightusvARB;
extern PFNGLWEIGHTUIVARBPROC glWeightuivARB;

extern PFNGLWEIGHTPOINTERARBPROC glWeightPointerARB;
extern PFNGLVERTEXBLENDARBPROC   glVertexBlendARB;


// GL_ARB_vertex_buffer_object
#define BUFFER_OFFSET(i) ((char *) NULL + (i))

#define GL_ARRAY_BUFFER_ARB                             0x8892
#define GL_ELEMENT_ARRAY_BUFFER_ARB                     0x8893
#define GL_ELEMENT_ARRAY_BUFFER_ARB                     0x8893
#define GL_ARRAY_BUFFER_BINDING_ARB                     0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB             0x8895
#define GL_VERTEX_ARRAY_BUFFER_BINDING_ARB              0x8896
#define GL_NORMAL_ARRAY_BUFFER_BINDING_ARB              0x8897
#define GL_COLOR_ARRAY_BUFFER_BINDING_ARB               0x8898
#define GL_INDEX_ARRAY_BUFFER_BINDING_ARB               0x8899
#define GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING_ARB       0x889A
#define GL_EDGE_FLAG_ARRAY_BUFFER_BINDING_ARB           0x889B
#define GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING_ARB     0x889C
#define GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING_ARB      0x889D
#define GL_WEIGHT_ARRAY_BUFFER_BINDING_ARB              0x889E
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB       0x889F
#define GL_STREAM_DRAW_ARB                              0x88E0
#define GL_STREAM_READ_ARB                              0x88E1
#define GL_STREAM_COPY_ARB                              0x88E2
#define GL_STATIC_DRAW_ARB                              0x88E4
#define GL_STATIC_READ_ARB                              0x88E5
#define GL_STATIC_COPY_ARB                              0x88E6
#define GL_DYNAMIC_DRAW_ARB                             0x88E8
#define GL_DYNAMIC_READ_ARB                             0x88E9
#define GL_DYNAMIC_COPY_ARB                             0x88EA
#define GL_READ_ONLY_ARB                                0x88B8
#define GL_WRITE_ONLY_ARB                               0x88B9
#define GL_READ_WRITE_ARB                               0x88BA
#define GL_BUFFER_SIZE_ARB                              0x8764
#define GL_BUFFER_USAGE_ARB                             0x8765
#define GL_BUFFER_ACCESS_ARB                            0x88BB
#define GL_BUFFER_MAPPED_ARB                            0x88BC
#define GL_BUFFER_MAP_POINTER_ARB                       0x88BD

typedef void (APIENTRY * PFNGLBINDBUFFERARBPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRY * PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRY * PFNGLGENBUFFERSARBPROC) (GLsizei n, GLuint *buffers);
typedef GLboolean (APIENTRY * PFNGLISBUFFERARBPROC) (GLuint buffer);

typedef void (APIENTRY * PFNGLBUFFERDATAARBPROC) (GLenum target, GLsizeiptrARB size, const void *data, GLenum usage);
typedef void (APIENTRY * PFNGLBUFFERSUBDATAARBPROC) (GLenum target, GLintptrARB offset, GLsizeiptrARB size, const void *data);
typedef void (APIENTRY * PFNGLGETBUFFERSUBDATAARBPROC) (GLenum target, GLintptrARB offset, GLsizeiptrARB size, void *data);

typedef void *(APIENTRY * PFNGLMAPBUFFERARBPROC) (GLenum target, GLenum access);
typedef GLboolean (APIENTRY * PFNGLUNMAPBUFFERARBPROC) (GLenum target);

typedef void (APIENTRY * PFNGLGETBUFFERPARAMETERIVARBPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETBUFFERPOINTERVARBPROC) (GLenum target, GLenum pname, void **params);

extern PFNGLBINDBUFFERARBPROC glBindBufferARB;
extern PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;
extern PFNGLGENBUFFERSARBPROC glGenBuffersARB;
extern PFNGLISBUFFERARBPROC glIsBufferARB;

extern PFNGLBUFFERDATAARBPROC glBufferDataARB;
extern PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB;
extern PFNGLGETBUFFERSUBDATAARBPROC glGetBufferSubDataARB;

extern PFNGLMAPBUFFERARBPROC glMapBufferARB;
extern PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB;

extern PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB;
extern PFNGLGETBUFFERPOINTERVARBPROC glGetBufferPointervARB;


// GL_ARB_vertex_program
typedef void (APIENTRY * PFNGLVERTEXATTRIB1SARBPROC) (GLuint index, GLshort x);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1FARBPROC) (GLuint index, GLfloat x);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1DARBPROC) (GLuint index, GLdouble x);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2SARBPROC) (GLuint index, GLshort x, GLshort y);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2FARBPROC) (GLuint index, GLfloat x, GLfloat y);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2DARBPROC) (GLuint index, GLdouble x, GLdouble y);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3SARBPROC) (GLuint index, GLshort x, GLshort y, GLshort z);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3FARBPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3DARBPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4SARBPROC) (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4FARBPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4DARBPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4NUBARBPROC) (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1SVARBPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1FVARBPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1DVARBPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2SVARBPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2FVARBPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2DVARBPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3SVARBPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3FVARBPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3DVARBPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4BVARBPROC) (GLuint index, const GLbyte *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4SVARBPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4IVARBPROC) (GLuint index, const GLint *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4UBVARBPROC) (GLuint index, const GLubyte *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4USVARBPROC) (GLuint index, const GLushort *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4UIVARBPROC) (GLuint index, const GLuint *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4FVARBPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4DVARBPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4NBVARBPROC) (GLuint index, const GLbyte *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4NSVARBPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4NIVARBPROC) (GLuint index, const GLint *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4NUBVARBPROC) (GLuint index, const GLubyte *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4NUSVARBPROC) (GLuint index, const GLushort *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4NUIVARBPROC) (GLuint index, const GLuint *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIBPOINTERARBPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLENABLEVERTEXATTRIBARRAYARBPROC) (GLuint index);
typedef void (APIENTRY * PFNGLDISABLEVERTEXATTRIBARRAYARBPROC) (GLuint index);
typedef void (APIENTRY * PFNGLPROGRAMSTRINGARBPROC) (GLenum target, GLenum format, GLsizei len, const GLvoid *string); 
typedef void (APIENTRY * PFNGLBINDPROGRAMARBPROC) (GLenum target, GLuint program);
typedef void (APIENTRY * PFNGLDELETEPROGRAMSARBPROC) (GLsizei n, const GLuint *programs);
typedef void (APIENTRY * PFNGLGENPROGRAMSARBPROC) (GLsizei n, GLuint *programs);
typedef void (APIENTRY * PFNGLPROGRAMENVPARAMETER4DARBPROC) (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRY * PFNGLPROGRAMENVPARAMETER4DVARBPROC) (GLenum target, GLuint index, const GLdouble *params);
typedef void (APIENTRY * PFNGLPROGRAMENVPARAMETER4FARBPROC) (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRY * PFNGLPROGRAMENVPARAMETER4FVARBPROC) (GLenum target, GLuint index, const GLfloat *params);
typedef void (APIENTRY * PFNGLPROGRAMLOCALPARAMETER4DARBPROC) (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRY * PFNGLPROGRAMLOCALPARAMETER4DVARBPROC) (GLenum target, GLuint index, const GLdouble *params);
typedef void (APIENTRY * PFNGLPROGRAMLOCALPARAMETER4FARBPROC) (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRY * PFNGLPROGRAMLOCALPARAMETER4FVARBPROC) (GLenum target, GLuint index, const GLfloat *params);
typedef void (APIENTRY * PFNGLGETPROGRAMENVPARAMETERDVARBPROC) (GLenum target, GLuint index, GLdouble *params);
typedef void (APIENTRY * PFNGLGETPROGRAMENVPARAMETERFVARBPROC) (GLenum target, GLuint index, GLfloat *params);
typedef void (APIENTRY * PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC) (GLenum target, GLuint index, GLdouble *params);
typedef void (APIENTRY * PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC) (GLenum target, GLuint index, GLfloat *params);
typedef void (APIENTRY * PFNGLGETPROGRAMIVARBPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETPROGRAMSTRINGARBPROC) (GLenum target, GLenum pname, GLvoid *string);
typedef void (APIENTRY * PFNGLGETVERTEXATTRIBDVARBPROC) (GLuint index, GLenum pname, GLdouble *params);
typedef void (APIENTRY * PFNGLGETVERTEXATTRIBFVARBPROC) (GLuint index, GLenum pname, GLfloat *params);
typedef void (APIENTRY * PFNGLGETVERTEXATTRIBIVARBPROC) (GLuint index, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETVERTEXATTRIBPOINTERVARBPROC) (GLuint index, GLenum pname, GLvoid **pointer);
typedef GLboolean (APIENTRY * PFNGLISPROGRAMARBPROC) (GLuint program);

extern PFNGLVERTEXATTRIB1SARBPROC glVertexAttrib1sARB;
extern PFNGLVERTEXATTRIB1FARBPROC glVertexAttrib1fARB;
extern PFNGLVERTEXATTRIB1DARBPROC glVertexAttrib1dARB;
extern PFNGLVERTEXATTRIB2SARBPROC glVertexAttrib2sARB;
extern PFNGLVERTEXATTRIB2FARBPROC glVertexAttrib2fARB;
extern PFNGLVERTEXATTRIB2DARBPROC glVertexAttrib2dARB;
extern PFNGLVERTEXATTRIB3SARBPROC glVertexAttrib3sARB;
extern PFNGLVERTEXATTRIB3FARBPROC glVertexAttrib3fARB;
extern PFNGLVERTEXATTRIB3DARBPROC glVertexAttrib3dARB;
extern PFNGLVERTEXATTRIB4SARBPROC glVertexAttrib4sARB;
extern PFNGLVERTEXATTRIB4FARBPROC glVertexAttrib4fARB;
extern PFNGLVERTEXATTRIB4DARBPROC glVertexAttrib4dARB;
extern PFNGLVERTEXATTRIB4NUBARBPROC glVertexAttrib4NubARB;
extern PFNGLVERTEXATTRIB1SVARBPROC glVertexAttrib1svARB;
extern PFNGLVERTEXATTRIB1FVARBPROC glVertexAttrib1fvARB;
extern PFNGLVERTEXATTRIB1DVARBPROC glVertexAttrib1dvARB;
extern PFNGLVERTEXATTRIB2SVARBPROC glVertexAttrib2svARB;
extern PFNGLVERTEXATTRIB2FVARBPROC glVertexAttrib2fvARB;
extern PFNGLVERTEXATTRIB2DVARBPROC glVertexAttrib2dvARB;
extern PFNGLVERTEXATTRIB3SVARBPROC glVertexAttrib3svARB;
extern PFNGLVERTEXATTRIB3FVARBPROC glVertexAttrib3fvARB;
extern PFNGLVERTEXATTRIB3DVARBPROC glVertexAttrib3dvARB;
extern PFNGLVERTEXATTRIB4BVARBPROC glVertexAttrib4bvARB;
extern PFNGLVERTEXATTRIB4SVARBPROC glVertexAttrib4svARB;
extern PFNGLVERTEXATTRIB4IVARBPROC glVertexAttrib4ivARB;
extern PFNGLVERTEXATTRIB4UBVARBPROC glVertexAttrib4ubvARB;
extern PFNGLVERTEXATTRIB4USVARBPROC glVertexAttrib4usvARB;
extern PFNGLVERTEXATTRIB4UIVARBPROC glVertexAttrib4uivARB;
extern PFNGLVERTEXATTRIB4FVARBPROC glVertexAttrib4fvARB;
extern PFNGLVERTEXATTRIB4DVARBPROC glVertexAttrib4dvARB;
extern PFNGLVERTEXATTRIB4NBVARBPROC glVertexAttrib4NbvARB;
extern PFNGLVERTEXATTRIB4NSVARBPROC glVertexAttrib4NsvARB;
extern PFNGLVERTEXATTRIB4NIVARBPROC glVertexAttrib4NivARB;
extern PFNGLVERTEXATTRIB4NUBVARBPROC glVertexAttrib4NubvARB;
extern PFNGLVERTEXATTRIB4NUSVARBPROC glVertexAttrib4NusvARB;
extern PFNGLVERTEXATTRIB4NUIVARBPROC glVertexAttrib4NuivARB;
extern PFNGLVERTEXATTRIBPOINTERARBPROC glVertexAttribPointerARB;
extern PFNGLENABLEVERTEXATTRIBARRAYARBPROC glEnableVertexAttribArrayARB;
extern PFNGLDISABLEVERTEXATTRIBARRAYARBPROC glDisableVertexAttribArrayARB;
extern PFNGLPROGRAMSTRINGARBPROC glProgramStringARB;
extern PFNGLBINDPROGRAMARBPROC glBindProgramARB;
extern PFNGLDELETEPROGRAMSARBPROC glDeleteProgramsARB;
extern PFNGLGENPROGRAMSARBPROC glGenProgramsARB;
extern PFNGLPROGRAMENVPARAMETER4DARBPROC glProgramEnvParameter4dARB;
extern PFNGLPROGRAMENVPARAMETER4DVARBPROC glProgramEnvParameter4dvARB;
extern PFNGLPROGRAMENVPARAMETER4FARBPROC glProgramEnvParameter4fARB;
extern PFNGLPROGRAMENVPARAMETER4FVARBPROC glProgramEnvParameter4fvARB;
extern PFNGLPROGRAMLOCALPARAMETER4DARBPROC glProgramLocalParameter4dARB;
extern PFNGLPROGRAMLOCALPARAMETER4DVARBPROC glProgramLocalParameter4dvARB;
extern PFNGLPROGRAMLOCALPARAMETER4FARBPROC glProgramLocalParameter4fARB;
extern PFNGLPROGRAMLOCALPARAMETER4FVARBPROC glProgramLocalParameter4fvARB;
extern PFNGLGETPROGRAMENVPARAMETERDVARBPROC glGetProgramEnvParameterdvARB;
extern PFNGLGETPROGRAMENVPARAMETERFVARBPROC glGetProgramEnvParameterfvARB;
extern PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC glGetProgramLocalParameterdvARB;
extern PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC glGetProgramLocalParameterfvARB;
extern PFNGLGETPROGRAMIVARBPROC glGetProgramivARB;
extern PFNGLGETPROGRAMSTRINGARBPROC glGetProgramStringARB;
extern PFNGLGETVERTEXATTRIBDVARBPROC glGetVertexAttribdvARB;
extern PFNGLGETVERTEXATTRIBFVARBPROC glGetVertexAttribfvARB;
extern PFNGLGETVERTEXATTRIBIVARBPROC glGetVertexAttribivARB;
extern PFNGLGETVERTEXATTRIBPOINTERVARBPROC glGetVertexAttribPointervARB;
extern PFNGLISPROGRAMARBPROC glIsProgramARB;

#define GL_VERTEX_PROGRAM_ARB                                   0x8620
#define GL_VERTEX_PROGRAM_POINT_SIZE_ARB                        0x8642
#define GL_VERTEX_PROGRAM_TWO_SIDE_ARB                          0x8643
#define GL_COLOR_SUM_ARB                                        0x8458
#define GL_PROGRAM_FORMAT_ASCII_ARB                             0x8875
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB                      0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE_ARB                         0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE_ARB                       0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE_ARB                         0x8625
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED_ARB                   0x886A
#define GL_CURRENT_VERTEX_ATTRIB_ARB                            0x8626
#define GL_VERTEX_ATTRIB_ARRAY_POINTER_ARB                      0x8645
#define GL_PROGRAM_LENGTH_ARB                                   0x8627
#define GL_PROGRAM_FORMAT_ARB                                   0x8876
#define GL_PROGRAM_BINDING_ARB                                  0x8677
#define GL_PROGRAM_INSTRUCTIONS_ARB                             0x88A0
#define GL_MAX_PROGRAM_INSTRUCTIONS_ARB                         0x88A1
#define GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB                      0x88A2
#define GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB                  0x88A3
#define GL_PROGRAM_TEMPORARIES_ARB                              0x88A4
#define GL_MAX_PROGRAM_TEMPORARIES_ARB                          0x88A5
#define GL_PROGRAM_NATIVE_TEMPORARIES_ARB                       0x88A6
#define GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB                   0x88A7
#define GL_PROGRAM_PARAMETERS_ARB                               0x88A8
#define GL_MAX_PROGRAM_PARAMETERS_ARB                           0x88A9
#define GL_PROGRAM_NATIVE_PARAMETERS_ARB                        0x88AA
#define GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB                    0x88AB
#define GL_PROGRAM_ATTRIBS_ARB                                  0x88AC
#define GL_MAX_PROGRAM_ATTRIBS_ARB                              0x88AD
#define GL_PROGRAM_NATIVE_ATTRIBS_ARB                           0x88AE
#define GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB                       0x88AF
#define GL_PROGRAM_ADDRESS_REGISTERS_ARB                        0x88B0
#define GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB                    0x88B1
#define GL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB                 0x88B2
#define GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB             0x88B3
#define GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB                     0x88B4
#define GL_MAX_PROGRAM_ENV_PARAMETERS_ARB                       0x88B5
#define GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB                      0x88B6
#define GL_PROGRAM_STRING_ARB                                   0x8628
#define GL_PROGRAM_ERROR_POSITION_ARB                           0x864B
#define GL_CURRENT_MATRIX_ARB                                   0x8641
#define GL_TRANSPOSE_CURRENT_MATRIX_ARB                         0x88B7
#define GL_CURRENT_MATRIX_STACK_DEPTH_ARB                       0x8640
#define GL_MAX_VERTEX_ATTRIBS_ARB                               0x8869
#define GL_MAX_PROGRAM_MATRICES_ARB                             0x862F
#define GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB                   0x862E
#define GL_PROGRAM_ERROR_STRING_ARB                             0x8874
#define GL_MATRIX0_ARB                                          0x88C0
#define GL_MATRIX1_ARB                                          0x88C1
#define GL_MATRIX2_ARB                                          0x88C2
#define GL_MATRIX3_ARB                                          0x88C3
#define GL_MATRIX4_ARB                                          0x88C4
#define GL_MATRIX5_ARB                                          0x88C5
#define GL_MATRIX6_ARB                                          0x88C6
#define GL_MATRIX7_ARB                                          0x88C7
#define GL_MATRIX8_ARB                                          0x88C8
#define GL_MATRIX9_ARB                                          0x88C9
#define GL_MATRIX10_ARB                                         0x88CA
#define GL_MATRIX11_ARB                                         0x88CB
#define GL_MATRIX12_ARB                                         0x88CC
#define GL_MATRIX13_ARB                                         0x88CD
#define GL_MATRIX14_ARB                                         0x88CE
#define GL_MATRIX15_ARB                                         0x88CF
#define GL_MATRIX16_ARB                                         0x88D0
#define GL_MATRIX17_ARB                                         0x88D1
#define GL_MATRIX18_ARB                                         0x88D2
#define GL_MATRIX19_ARB                                         0x88D3
#define GL_MATRIX20_ARB                                         0x88D4
#define GL_MATRIX21_ARB                                         0x88D5
#define GL_MATRIX22_ARB                                         0x88D6
#define GL_MATRIX23_ARB                                         0x88D7
#define GL_MATRIX24_ARB                                         0x88D8
#define GL_MATRIX25_ARB                                         0x88D9
#define GL_MATRIX26_ARB                                         0x88DA
#define GL_MATRIX27_ARB                                         0x88DB
#define GL_MATRIX28_ARB                                         0x88DC
#define GL_MATRIX29_ARB                                         0x88DD
#define GL_MATRIX30_ARB                                         0x88DE
#define GL_MATRIX31_ARB                                         0x88DF







// GL_ATI_element_array
#define GL_ELEMENT_ARRAY_ATI			0x8768
#define GL_ELEMENT_ARRAY_TYPE_ATI		0x8769
#define GL_ELEMENT_ARRAY_POINTER_ATI	0x876A

typedef void (APIENTRY * PFNGLELEMENTPOINTERATIPROC)(GLenum type, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLDRAWELEMENTARRAYATIPROC)(GLenum mode, GLsizei count);
typedef void (APIENTRY * PFNGLDRAWRANGEELEMENTARRAYATIPROC)(GLenum mode, GLuint start, GLuint end, GLsizei count);

extern PFNGLELEMENTPOINTERATIPROC        glElementPointerATI;
extern PFNGLDRAWELEMENTARRAYATIPROC      glDrawElementArrayATI;
extern PFNGLDRAWRANGEELEMENTARRAYATIPROC glDrawRangeElementArrayATI;


// GL_ATI_envmap_bumpmap
#define GL_BUMP_ROT_MATRIX_ATI					   0x8775
#define GL_BUMP_ROT_MATRIX_SIZE_ATI				   0x8776
#define GL_BUMP_NUM_TEX_UNITS_ATI				   0x8777
#define GL_BUMP_TEX_UNITS_ATI					   0x8778
#define GL_DUDV_ATI								   0x8779
#define GL_DU8DV8_ATI							   0x877A
#define GL_BUMP_ENVMAP_ATI						   0x877B
#define GL_BUMP_TARGET_ATI						   0x877C

typedef void (APIENTRY * PFNGLTEXBUMPPARAMETERIVATIPROC) (GLenum pname, GLint *param);
typedef void (APIENTRY * PFNGLTEXBUMPPARAMETERFVATIPROC) (GLenum pname, GLfloat *param);
typedef void (APIENTRY * PFNGLGETTEXBUMPPARAMETERIVATIPROC) (GLenum pname, GLint *param);
typedef void (APIENTRY * PFNGLGETTEXBUMPPARAMETERFVATIPROC) (GLenum pname, GLfloat *param);

extern PFNGLTEXBUMPPARAMETERIVATIPROC    glTexBumpParameterivATI;
extern PFNGLTEXBUMPPARAMETERFVATIPROC    glTexBumpParameterfvATI;
extern PFNGLGETTEXBUMPPARAMETERIVATIPROC glGetTexBumpParameterivATI;
extern PFNGLGETTEXBUMPPARAMETERFVATIPROC glGetTexBumpParameterfvATI;


// GL_ATI_fragment_shader
#define GL_FRAGMENT_SHADER_ATI						0x8920
#define GL_REG_0_ATI								0x8921
#define GL_REG_1_ATI								0x8922
#define GL_REG_2_ATI								0x8923
#define GL_REG_3_ATI								0x8924
#define GL_REG_4_ATI								0x8925
#define GL_REG_5_ATI								0x8926
#define GL_REG_6_ATI								0x8927
#define GL_REG_7_ATI								0x8928
#define GL_REG_8_ATI								0x8929
#define GL_REG_9_ATI								0x892A
#define GL_REG_10_ATI								0x892B
#define GL_REG_11_ATI								0x892C
#define GL_REG_12_ATI								0x892D
#define GL_REG_13_ATI								0x892E
#define GL_REG_14_ATI								0x892F
#define GL_REG_15_ATI								0x8930
#define GL_REG_16_ATI								0x8931
#define GL_REG_17_ATI								0x8932
#define GL_REG_18_ATI								0x8933
#define GL_REG_19_ATI								0x8934
#define GL_REG_20_ATI								0x8935
#define GL_REG_21_ATI								0x8936
#define GL_REG_22_ATI								0x8937
#define GL_REG_23_ATI								0x8938
#define GL_REG_24_ATI								0x8939
#define GL_REG_25_ATI								0x893A
#define GL_REG_26_ATI								0x893B
#define GL_REG_27_ATI								0x893C
#define GL_REG_28_ATI								0x893D
#define GL_REG_29_ATI								0x893E
#define GL_REG_30_ATI								0x893F
#define GL_REG_31_ATI								0x8940
#define GL_CON_0_ATI								0x8941
#define GL_CON_1_ATI								0x8942
#define GL_CON_2_ATI								0x8943
#define GL_CON_3_ATI								0x8944
#define GL_CON_4_ATI								0x8945
#define GL_CON_5_ATI								0x8946
#define GL_CON_6_ATI								0x8947
#define GL_CON_7_ATI								0x8948
#define GL_CON_8_ATI								0x8949
#define GL_CON_9_ATI								0x894A
#define GL_CON_10_ATI								0x894B
#define GL_CON_11_ATI								0x894C
#define GL_CON_12_ATI								0x894D
#define GL_CON_13_ATI								0x894E
#define GL_CON_14_ATI								0x894F
#define GL_CON_15_ATI								0x8950
#define GL_CON_16_ATI								0x8951
#define GL_CON_17_ATI								0x8952
#define GL_CON_18_ATI								0x8953
#define GL_CON_19_ATI								0x8954
#define GL_CON_20_ATI								0x8955
#define GL_CON_21_ATI								0x8956
#define GL_CON_22_ATI								0x8957
#define GL_CON_23_ATI								0x8958
#define GL_CON_24_ATI								0x8959
#define GL_CON_25_ATI								0x895A
#define GL_CON_26_ATI								0x895B
#define GL_CON_27_ATI								0x895C
#define GL_CON_28_ATI								0x895D
#define GL_CON_29_ATI								0x895E
#define GL_CON_30_ATI								0x895F
#define GL_CON_31_ATI								0x8960
#define GL_MOV_ATI									0x8961
#define GL_ADD_ATI									0x8963
#define GL_MUL_ATI									0x8964
#define GL_SUB_ATI									0x8965
#define GL_DOT3_ATI									0x8966
#define GL_DOT4_ATI									0x8967
#define GL_MAD_ATI									0x8968
#define GL_LERP_ATI									0x8969
#define GL_CND_ATI									0x896A
#define GL_CND0_ATI									0x896B
#define GL_DOT2_ADD_ATI								0x896C
#define GL_SECONDARY_INTERPOLATOR_ATI				0x896D
#define GL_NUM_FRAGMENT_REGISTERS_ATI				0x896E
#define GL_NUM_FRAGMENT_CONSTANTS_ATI				0x896F
#define GL_NUM_PASSES_ATI							0x8970
#define GL_NUM_INSTRUCTIONS_PER_PASS_ATI			0x8971
#define GL_NUM_INSTRUCTIONS_TOTAL_ATI				0x8972
#define GL_NUM_INPUT_INTERPOLATOR_COMPONENTS_ATI	0x8973
#define GL_NUM_LOOPBACK_COMPONENTS_ATI				0x8974
#define GL_COLOR_ALPHA_PAIRING_ATI					0x8975
#define GL_SWIZZLE_STR_ATI							0x8976
#define GL_SWIZZLE_STQ_ATI							0x8977
#define GL_SWIZZLE_STR_DR_ATI						0x8978
#define GL_SWIZZLE_STQ_DQ_ATI						0x8979
#define GL_SWIZZLE_STRQ_ATI							0x897A
#define GL_SWIZZLE_STRQ_DQ_ATI						0x897B
#define GL_RED_BIT_ATI								0x00000001
#define GL_GREEN_BIT_ATI							0x00000002
#define GL_BLUE_BIT_ATI								0x00000004
#define GL_2X_BIT_ATI								0x00000001
#define GL_4X_BIT_ATI								0x00000002
#define GL_8X_BIT_ATI								0x00000004
#define GL_HALF_BIT_ATI								0x00000008
#define GL_QUARTER_BIT_ATI							0x00000010
#define GL_EIGHTH_BIT_ATI							0x00000020
#define GL_SATURATE_BIT_ATI							0x00000040
#define GL_COMP_BIT_ATI								0x00000002
#define GL_NEGATE_BIT_ATI							0x00000004
#define GL_BIAS_BIT_ATI								0x00000008


typedef GLuint (APIENTRY *PFNGLGENFRAGMENTSHADERSATIPROC)(GLuint range);
typedef GLvoid (APIENTRY *PFNGLBINDFRAGMENTSHADERATIPROC)(GLuint id);
typedef GLvoid (APIENTRY *PFNGLDELETEFRAGMENTSHADERATIPROC)(GLuint id);
typedef GLvoid (APIENTRY *PFNGLBEGINFRAGMENTSHADERATIPROC)(GLvoid);
typedef GLvoid (APIENTRY *PFNGLENDFRAGMENTSHADERATIPROC)(GLvoid);
typedef GLvoid (APIENTRY *PFNGLPASSTEXCOORDATIPROC)(GLuint dst, GLuint coord, GLenum swizzle);
typedef GLvoid (APIENTRY *PFNGLSAMPLEMAPATIPROC)(GLuint dst, GLuint interp, GLenum swizzle);

typedef GLvoid (APIENTRY *PFNGLCOLORFRAGMENTOP1ATIPROC)(GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod);
typedef GLvoid (APIENTRY *PFNGLCOLORFRAGMENTOP2ATIPROC)(GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod,
														GLuint arg2, GLuint arg2Rep, GLuint arg2Mod);
typedef GLvoid (APIENTRY *PFNGLCOLORFRAGMENTOP3ATIPROC)(GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep,
									   GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod, GLuint arg3, GLuint arg3Rep, GLuint arg3Mod);

typedef GLvoid (APIENTRY *PFNGLALPHAFRAGMENTOP1ATIPROC)(GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod);
typedef GLvoid (APIENTRY *PFNGLALPHAFRAGMENTOP2ATIPROC)(GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2,
														GLuint arg2Rep, GLuint arg2Mod);
typedef GLvoid (APIENTRY *PFNGLALPHAFRAGMENTOP3ATIPROC)(GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod,
														GLuint arg2, GLuint arg2Rep, GLuint arg2Mod, GLuint arg3, GLuint arg3Rep, GLuint arg3Mod);
typedef GLvoid (APIENTRY *PFNGLSETFRAGMENTSHADERCONSTANTATIPROC)(GLuint dst, const GLfloat *value);

extern PFNGLGENFRAGMENTSHADERSATIPROC   glGenFragmentShadersATI;
extern PFNGLBINDFRAGMENTSHADERATIPROC   glBindFragmentShaderATI;
extern PFNGLDELETEFRAGMENTSHADERATIPROC glDeleteFragmentShaderATI;
extern PFNGLBEGINFRAGMENTSHADERATIPROC  glBeginFragmentShaderATI;
extern PFNGLENDFRAGMENTSHADERATIPROC    glEndFragmentShaderATI;
extern PFNGLPASSTEXCOORDATIPROC         glPassTexCoordATI;
extern PFNGLSAMPLEMAPATIPROC            glSampleMapATI;

extern PFNGLCOLORFRAGMENTOP1ATIPROC glColorFragmentOp1ATI;
extern PFNGLCOLORFRAGMENTOP2ATIPROC glColorFragmentOp2ATI;
extern PFNGLCOLORFRAGMENTOP3ATIPROC glColorFragmentOp3ATI;

extern PFNGLALPHAFRAGMENTOP1ATIPROC glAlphaFragmentOp1ATI;
extern PFNGLALPHAFRAGMENTOP2ATIPROC glAlphaFragmentOp2ATI;
extern PFNGLALPHAFRAGMENTOP3ATIPROC glAlphaFragmentOp3ATI;

extern PFNGLSETFRAGMENTSHADERCONSTANTATIPROC glSetFragmentShaderConstantATI;


// GL_ATI_map_object_buffer
typedef void *(APIENTRY * PFNGLMAPOBJECTBUFFERATIPROC)(GLuint buffer);
typedef void (APIENTRY * PFNGLUNMAPOBJECTBUFFERATIPROC)(GLuint buffer);

extern PFNGLMAPOBJECTBUFFERATIPROC   glMapObjectBufferATI;
extern PFNGLUNMAPOBJECTBUFFERATIPROC glUnmapObjectBufferATI;


// GL_ATI_pn_triangles
#define GL_PN_TRIANGLES_ATI							0x87F0
#define GL_MAX_PN_TRIANGLES_TESSELATION_LEVEL_ATI	0x87F1
#define GL_PN_TRIANGLES_POINT_MODE_ATI				0x87F2
#define GL_PN_TRIANGLES_NORMAL_MODE_ATI				0x87F3
#define GL_PN_TRIANGLES_TESSELATION_LEVEL_ATI		0x87F4
#define GL_PN_TRIANGLES_POINT_MODE_LINEAR_ATI		0x87F5
#define GL_PN_TRIANGLES_POINT_MODE_CUBIC_ATI		0x87F6
#define GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATI		0x87F7
#define GL_PN_TRIANGLES_NORMAL_MODE_QUADRATIC_ATI   0x87F8

typedef void (APIENTRY *PFNGLPNTRIANGLESIATIPROC)(GLenum pname, GLint param);
typedef void (APIENTRY *PFNGLPNTRIANGLESFATIPROC)(GLenum pname, GLfloat param);

extern PFNGLPNTRIANGLESIATIPROC glPNTrianglesiATI;
extern PFNGLPNTRIANGLESFATIPROC glPNTrianglesfATI;


// GL_ATI_texture_float
#define GL_RGBA_FLOAT32_ATI                 0x8814
#define GL_RGB_FLOAT32_ATI                  0x8815
#define GL_ALPHA_FLOAT32_ATI                0x8816
#define GL_INTENSITY_FLOAT32_ATI            0x8817
#define GL_LUMINANCE_FLOAT32_ATI            0x8818
#define GL_LUMINANCE_ALPHA_FLOAT32_ATI      0x8819
#define GL_RGBA_FLOAT16_ATI                 0x881A
#define GL_RGB_FLOAT16_ATI                  0x881B
#define GL_ALPHA_FLOAT16_ATI                0x881C
#define GL_INTENSITY_FLOAT16_ATI            0x881D
#define GL_LUMINANCE_FLOAT16_ATI            0x881E
#define GL_LUMINANCE_ALPHA_FLOAT16_ATI      0x881F


// GL_ATI_texture_mirror_once 
#define GL_MIRROR_CLAMP_ATI					0x8742
#define GL_MIRROR_CLAMP_TO_EDGE_ATI			0x8743


// GL_ATI_vertex_array_object
#define GL_STATIC_ATI					0x8760
#define GL_DYNAMIC_ATI					0x8761
#define GL_PRESERVE_ATI					0x8762
#define GL_DISCARD_ATI					0x8763

#define GL_OBJECT_BUFFER_SIZE_ATI		0x8764
#define GL_OBJECT_BUFFER_USAGE_ATI		0x8765
#define GL_ARRAY_OBJECT_BUFFER_ATI		0x8766
#define GL_ARRAY_OBJECT_OFFSET_ATI		0x8767

typedef GLuint (APIENTRY * PFNGLNEWOBJECTBUFFERATIPROC) (GLsizei size, const GLvoid *pointer, GLenum usage);
typedef GLboolean (APIENTRY * PFNGLISOBJECTBUFFERATIPROC) (GLuint buffer);
typedef void (APIENTRY * PFNGLUPDATEOBJECTBUFFERATIPROC) (GLuint buffer, GLuint offset, GLsizei size, const GLvoid *pointer, GLenum preserve);
typedef void (APIENTRY * PFNGLGETOBJECTBUFFERFVATIPROC) (GLuint buffer, GLenum pname, GLfloat *params);
typedef void (APIENTRY * PFNGLGETOBJECTBUFFERIVATIPROC) (GLuint buffer, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLFREEOBJECTBUFFERATIPROC) (GLuint buffer);

typedef void (APIENTRY * PFNGLARRAYOBJECTATIPROC) (GLenum array, GLint size, GLenum type, GLsizei stride, GLuint buffer, GLuint offset);
typedef void (APIENTRY * PFNGLGETARRAYOBJECTFVATIPROC) (GLenum array, GLenum pname, GLfloat *params);
typedef void (APIENTRY * PFNGLGETARRAYOBJECTIVATIPROC) (GLenum array, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLVARIANTARRAYOBJECTATIPROC) (GLuint id, GLenum type, GLsizei stride, GLuint buffer, GLuint offset);

typedef void (APIENTRY * PFNGLGETVARIANTARRAYOBJECTFVATIPROC) (GLuint id, GLenum pname, GLfloat *params);
typedef void (APIENTRY * PFNGLGETVARIANTARRAYOBJECTIVATIPROC) (GLuint id, GLenum pname, GLint *params);

extern PFNGLNEWOBJECTBUFFERATIPROC    glNewObjectBufferATI;
extern PFNGLISOBJECTBUFFERATIPROC     glIsObjectBufferATI;
extern PFNGLUPDATEOBJECTBUFFERATIPROC glUpdateObjectBufferATI;
extern PFNGLGETOBJECTBUFFERFVATIPROC  glGetObjectBufferfvATI;
extern PFNGLGETOBJECTBUFFERIVATIPROC  glGetObjectBufferivATI;
extern PFNGLFREEOBJECTBUFFERATIPROC   glDeleteObjectBufferATI;

extern PFNGLARRAYOBJECTATIPROC        glArrayObjectATI;
extern PFNGLGETARRAYOBJECTFVATIPROC   glGetArrayObjectfvATI;
extern PFNGLGETARRAYOBJECTIVATIPROC   glGetArrayObjectivATI;
extern PFNGLVARIANTARRAYOBJECTATIPROC glVariantArrayObjectATI;

extern PFNGLGETVARIANTARRAYOBJECTFVATIPROC glGetVariantArrayObjectfvATI;
extern PFNGLGETVARIANTARRAYOBJECTIVATIPROC glGetVariantArrayObjectivATI;


// GL_ATI_vertex_attrib_array_object

/* TODO: Fix this */
typedef void (APIENTRY * PFNGLVERTEXATTRIBARRAYOBJECTATIPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint buffer, GLuint offset);

//glGetVertexAttribArrayObjectivATI
//glGetVertexAttribArrayObjectfvATI
//glVertexAttribArrayObjectATI

extern PFNGLVERTEXATTRIBARRAYOBJECTATIPROC glVertexAttribArrayObjectATI;


// GL_ATI_vertex_streams
#define GL_MAX_VERTEX_STREAMS_ATI				0x876B
#define GL_VERTEX_SOURCE_ATI					0x876C
#define GL_VERTEX_STREAM0_ATI					0x876D
#define GL_VERTEX_STREAM1_ATI					0x876E
#define GL_VERTEX_STREAM2_ATI					0x876F
#define GL_VERTEX_STREAM3_ATI					0x8770
#define GL_VERTEX_STREAM4_ATI					0x8771
#define GL_VERTEX_STREAM5_ATI					0x8772
#define GL_VERTEX_STREAM6_ATI					0x8773
#define GL_VERTEX_STREAM7_ATI					0x8774

typedef void (APIENTRY * PFNGLCLIENTACTIVEVERTEXSTREAMATIPROC) (GLenum stream);
typedef void (APIENTRY * PFNGLVERTEXBLENDENVIATIPROC) (GLenum pname, GLint param);
typedef void (APIENTRY * PFNGLVERTEXBLENDENVFATIPROC) (GLenum pname, GLfloat param);

typedef void (APIENTRY * PFNGLVERTEXSTREAM2SATIPROC) (GLenum stream, GLshort x, GLshort y);
typedef void (APIENTRY * PFNGLVERTEXSTREAM2SVATIPROC) (GLenum stream, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM2IATIPROC) (GLenum stream, GLint x, GLint y);
typedef void (APIENTRY * PFNGLVERTEXSTREAM2IVATIPROC) (GLenum stream, const GLint *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM2FATIPROC) (GLenum stream, GLfloat x, GLfloat y);
typedef void (APIENTRY * PFNGLVERTEXSTREAM2FVATIPROC) (GLenum stream, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM2DATIPROC) (GLenum stream, GLdouble x, GLdouble y);
typedef void (APIENTRY * PFNGLVERTEXSTREAM2DVATIPROC) (GLenum stream, const GLdouble *v);

typedef void (APIENTRY * PFNGLVERTEXSTREAM3SATIPROC) (GLenum stream, GLshort x, GLshort y, GLshort z);
typedef void (APIENTRY * PFNGLVERTEXSTREAM3SVATIPROC) (GLenum stream, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM3IATIPROC) (GLenum stream, GLint x, GLint y, GLint z);
typedef void (APIENTRY * PFNGLVERTEXSTREAM3IVATIPROC) (GLenum stream, const GLint *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM3FATIPROC) (GLenum stream, GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRY * PFNGLVERTEXSTREAM3FVATIPROC) (GLenum stream, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM3DATIPROC) (GLenum stream, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRY * PFNGLVERTEXSTREAM3DVATIPROC) (GLenum stream, const GLdouble *v);

typedef void (APIENTRY * PFNGLVERTEXSTREAM4SATIPROC) (GLenum stream, GLshort x, GLshort y, GLshort z, GLshort w);
typedef void (APIENTRY * PFNGLVERTEXSTREAM4SVATIPROC) (GLenum stream, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM4IATIPROC) (GLenum stream, GLint x, GLint y, GLint z, GLint w);
typedef void (APIENTRY * PFNGLVERTEXSTREAM4IVATIPROC) (GLenum stream, const GLint *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM4FATIPROC) (GLenum stream, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRY * PFNGLVERTEXSTREAM4FVATIPROC) (GLenum stream, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM4DATIPROC) (GLenum stream, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRY * PFNGLVERTEXSTREAM4DVATIPROC) (GLenum stream, const GLdouble *v);

typedef void (APIENTRY * PFNGLNORMALSTREAM3BATIPROC) (GLenum stream, GLbyte x, GLbyte y, GLbyte z);
typedef void (APIENTRY * PFNGLNORMALSTREAM3BVATIPROC) (GLenum stream, const GLbyte *v);
typedef void (APIENTRY * PFNGLNORMALSTREAM3SATIPROC) (GLenum stream, GLshort x, GLshort y, GLshort z);
typedef void (APIENTRY * PFNGLNORMALSTREAM3SVATIPROC) (GLenum stream, const GLshort *v);
typedef void (APIENTRY * PFNGLNORMALSTREAM3IATIPROC) (GLenum stream, GLint x, GLint y, GLint z);
typedef void (APIENTRY * PFNGLNORMALSTREAM3IVATIPROC) (GLenum stream, const GLint *v);
typedef void (APIENTRY * PFNGLNORMALSTREAM3FATIPROC) (GLenum stream, GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRY * PFNGLNORMALSTREAM3FVATIPROC) (GLenum stream, const GLfloat *v);
typedef void (APIENTRY * PFNGLNORMALSTREAM3DATIPROC) (GLenum stream, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRY * PFNGLNORMALSTREAM3DVATIPROC) (GLenum stream, const GLdouble *v);

extern PFNGLCLIENTACTIVEVERTEXSTREAMATIPROC glClientActiveVertexStreamATI;
extern PFNGLVERTEXBLENDENVIATIPROC glVertexBlendEnviATI;
extern PFNGLVERTEXBLENDENVFATIPROC glVertexBlendEnvfATI;

extern PFNGLVERTEXSTREAM2SATIPROC  glVertexStream2sATI;
extern PFNGLVERTEXSTREAM2SVATIPROC glVertexStream2svATI;
extern PFNGLVERTEXSTREAM2IATIPROC  glVertexStream2iATI;
extern PFNGLVERTEXSTREAM2IVATIPROC glVertexStream2ivATI;
extern PFNGLVERTEXSTREAM2FATIPROC  glVertexStream2fATI;
extern PFNGLVERTEXSTREAM2FVATIPROC glVertexStream2fvATI;
extern PFNGLVERTEXSTREAM2DATIPROC  glVertexStream2dATI;
extern PFNGLVERTEXSTREAM2DVATIPROC glVertexStream2dvATI;

extern PFNGLVERTEXSTREAM3SATIPROC  glVertexStream3sATI;
extern PFNGLVERTEXSTREAM3SVATIPROC glVertexStream3svATI;
extern PFNGLVERTEXSTREAM3IATIPROC  glVertexStream3iATI;
extern PFNGLVERTEXSTREAM3IVATIPROC glVertexStream3ivATI;
extern PFNGLVERTEXSTREAM3FATIPROC  glVertexStream3fATI;
extern PFNGLVERTEXSTREAM3FVATIPROC glVertexStream3fvATI;
extern PFNGLVERTEXSTREAM3DATIPROC  glVertexStream3dATI;
extern PFNGLVERTEXSTREAM3DVATIPROC glVertexStream3dvATI;

extern PFNGLVERTEXSTREAM4SATIPROC  glVertexStream4sATI;
extern PFNGLVERTEXSTREAM4SVATIPROC glVertexStream4svATI;
extern PFNGLVERTEXSTREAM4IATIPROC  glVertexStream4iATI;
extern PFNGLVERTEXSTREAM4IVATIPROC glVertexStream4ivATI;
extern PFNGLVERTEXSTREAM4FATIPROC  glVertexStream4fATI;
extern PFNGLVERTEXSTREAM4FVATIPROC glVertexStream4fvATI;
extern PFNGLVERTEXSTREAM4DATIPROC  glVertexStream4dATI;
extern PFNGLVERTEXSTREAM4DVATIPROC glVertexStream4dvATI;

extern PFNGLNORMALSTREAM3BATIPROC  glNormalStream3bATI;
extern PFNGLNORMALSTREAM3BVATIPROC glNormalStream3bvATI;
extern PFNGLNORMALSTREAM3SATIPROC  glNormalStream3sATI;
extern PFNGLNORMALSTREAM3SVATIPROC glNormalStream3svATI;
extern PFNGLNORMALSTREAM3IATIPROC  glNormalStream3iATI;
extern PFNGLNORMALSTREAM3IVATIPROC glNormalStream3ivATI;
extern PFNGLNORMALSTREAM3FATIPROC  glNormalStream3fATI;
extern PFNGLNORMALSTREAM3FVATIPROC glNormalStream3fvATI;
extern PFNGLNORMALSTREAM3DATIPROC  glNormalStream3dATI;
extern PFNGLNORMALSTREAM3DVATIPROC glNormalStream3dvATI;


// GL_ATIX_texture_env_combine3
#define GL_MODULATE_ADD_ATIX				0x8744
#define GL_MODULATE_SIGNED_ADD_ATIX			0x8745
#define GL_MODULATE_SUBTRACT_ATIX			0x8746


// GL_ATIX_texture_env_route
#define GL_SECONDARY_COLOR_ATIX				0x8747
#define GL_TEXTURE_OUTPUT_RGB_ATIX			0x8748
#define GL_TEXTURE_OUTPUT_ALPHA_ATIX		0x8749













// GL_EXT_blend_color
#define GL_CONSTANT_COLOR_EXT               0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR_EXT     0x8002
#define GL_CONSTANT_ALPHA_EXT               0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA_EXT     0x8004
#define GL_BLEND_COLOR_EXT                  0x8005

typedef void (APIENTRY * PFNGLBLENDCOLOREXTPROC) (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);

extern PFNGLBLENDCOLOREXTPROC glBlendColorEXT;


// GL_EXT_blend_func_separate
#define GL_BLEND_DST_RGB_EXT				0x80C8
#define GL_BLEND_SRC_RGB_EXT				0x80C9
#define GL_BLEND_DST_ALPHA_EXT				0x80CA
#define GL_BLEND_SRC_ALPHA_EXT				0x80CB

typedef void (APIENTRY * PFNGLBLENDFUNCSEPARATEEXTPROC) (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);

extern PFNGLBLENDFUNCSEPARATEEXTPROC glBlendFuncSeparateEXT;


// GL_EXT_blend_minmax
#define GL_FUNC_ADD_EXT						0x8006
#define GL_MIN_EXT							0x8007
#define GL_MAX_EXT							0x8008
#define GL_BLEND_EQUATION_EXT				0x8009

typedef void (APIENTRY * PFNGLBLENDEQUATIONEXTPROC) (GLenum mode);

extern PFNGLBLENDEQUATIONEXTPROC glBlendEquationEXT;


// GL_EXT_blend_subtract
#define GL_FUNC_SUBTRACT_EXT					0x800A
#define GL_FUNC_REVERSE_SUBTRACT_EXT			0x800B


// GL_EXT_compiled_vertex_array
#define GL_ARRAY_ELEMENT_LOCK_FIRST_EXT     0x81A8
#define GL_ARRAY_ELEMENT_LOCK_COUNT_EXT     0x81A9

typedef void (APIENTRY * PFNGLLOCKARRAYSEXTPROC) (GLint first, GLsizei count);
typedef void (APIENTRY * PFNGLUNLOCKARRAYSEXTPROC) (void);

extern PFNGLLOCKARRAYSEXTPROC   glLockArraysEXT;
extern PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT;


// GL_EXT_draw_range_elements
#define GL_MAX_ELEMENTS_VERTICES_EXT         0x80E8
#define GL_MAX_ELEMENTS_INDICES_EXT          0x80E9

typedef void (APIENTRY * PFNGLDRAWRANGEELEMENTSEXTPROC) (GLenum  mode, GLuint start,
                                                         GLuint end, GLsizei count,
                                                         GLenum type, const GLvoid *indices);

extern PFNGLDRAWRANGEELEMENTSEXTPROC glDrawRangeElementsEXT;



// GL_EXT_fog_coord
#define GL_FOG_COORDINATE_SOURCE_EXT         0x8450
#define GL_FOG_COORDINATE_EXT                0x8451
#define GL_FRAGMENT_DEPTH_EXT                0x8452
#define GL_CURRENT_FOG_COORDINATE_EXT        0x8453
#define GL_FOG_COORDINATE_ARRAY_TYPE_EXT     0x8454
#define GL_FOG_COORDINATE_ARRAY_STRIDE_EXT   0x8455
#define GL_FOG_COORDINATE_ARRAY_POINTER_EXT  0x8456
#define GL_FOG_COORDINATE_ARRAY_EXT          0x8457

typedef void (APIENTRY * PFNGLFOGCOORDFEXTPROC) (GLfloat f);
typedef void (APIENTRY * PFNGLFOGCOORDDEXTPROC) (GLdouble f);
typedef void (APIENTRY * PFNGLFOGCOORDFVEXTPROC) (const GLfloat *v);
typedef void (APIENTRY * PFNGLFOGCOORDDVEXTPROC) (const GLdouble *v);
typedef void (APIENTRY * PFNGLFOGCOORDPOINTEREXTPROC) (GLenum type, GLsizei stride, const GLvoid *pointer);

extern PFNGLFOGCOORDFEXTPROC  glFogCoordfEXT;
extern PFNGLFOGCOORDDEXTPROC  glFogCoorddEXT;
extern PFNGLFOGCOORDFVEXTPROC glFogCoordfvEXT;
extern PFNGLFOGCOORDDVEXTPROC glFogCoorddvEXT;
extern PFNGLFOGCOORDPOINTEREXTPROC glFogCoordPointerEXT;


// GL_EXT_packed_pixels
#define GL_UNSIGNED_BYTE_3_3_2_EXT          0x8032
#define GL_UNSIGNED_SHORT_4_4_4_4_EXT       0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1_EXT       0x8034
#define GL_UNSIGNED_INT_8_8_8_8_EXT         0x8035
#define GL_UNSIGNED_INT_10_10_10_2_EXT      0x8036


// GL_EXT_polygon_offset
#define GL_POLYGON_OFFSET_EXT               0x8037
#define GL_POLYGON_OFFSET_FACTOR_EXT        0x8038
#define GL_POLYGON_OFFSET_BIAS_EXT          0x8039

typedef void (APIENTRY * PFNGLPOLYGONOFFSETEXTPROC) (GLfloat factor, GLfloat bias);

extern PFNGLPOLYGONOFFSETEXTPROC glPolygonOffsetEXT;


// GL_EXT_stencil_wrap
#define GL_INCR_WRAP_EXT					0x8507
#define GL_DECR_WRAP_EXT					0x8508


// GL_EXT_texture3D
#define GL_TEXTURE_BINDING_3D_EXT		    0x806A
#define GL_PACK_SKIP_IMAGES_EXT             0x806B
#define GL_PACK_IMAGE_HEIGHT_EXT            0x806C
#define GL_UNPACK_SKIP_IMAGES_EXT           0x806D
#define GL_UNPACK_IMAGE_HEIGHT_EXT          0x806E
#define GL_TEXTURE_3D_EXT                   0x806F
#define GL_PROXY_TEXTURE_3D_EXT             0x8070
#define GL_TEXTURE_DEPTH_EXT                0x8071
#define GL_TEXTURE_WRAP_R_EXT               0x8072
#define GL_MAX_3D_TEXTURE_SIZE_EXT          0x8073

#ifndef GL_TEXTURE_WRAP_R
#define GL_TEXTURE_WRAP_R GL_TEXTURE_WRAP_R_EXT
#endif

typedef void (APIENTRY * PFNGLTEXIMAGE3DEXTPROC)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth,
												 GLint border, GLenum format, GLenum type,  const GLvoid* pixels);

extern PFNGLTEXIMAGE3DEXTPROC glTexImage3DEXT;


//GL_EXT_texture_compression_s3tc
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT      0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT     0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT     0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT     0x83F3


// GL_EXT_texture_edge_clamp
#define GL_CLAMP_TO_EDGE_EXT                0x812F


//GL_EXT_texture_env_dot3
#define GL_DOT3_RGB_EXT                     0x8740
#define GL_DOT3_RGBA_EXT                    0x8741


// GL_EXT_texture_filter_anisotropic
#define GL_TEXTURE_MAX_ANISOTROPY_EXT		0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT	0x84FF


// GL_EXT_texture_lod_bias
#define GL_TEXTURE_FILTER_CONTROL_EXT		0x8500
#define GL_TEXTURE_LOD_BIAS_EXT				0x8501
#define GL_MAX_TEXTURE_LOD_BIAS_EXT			0x84FD


// GL_EXT_texture_rectangle
#define GL_TEXTURE_RECTANGLE_EXT            0x85B3
#define GL_TEXTURE_BINDING_RECTANGLE_EXT    0x85B4
#define GL_PROXY_TEXTURE_RECTANGLE_EXT      0x85B5
#define GL_MAX_RECTANGLE_TEXTURE_SIZE_EXT   0x85B6


// GL_EXT_vertex_shader
#define GL_VERTEX_SHADER_EXT								0x8780
#define GL_VERTEX_SHADER_BINDING_EXT						0x8781
#define GL_OP_INDEX_EXT										0x8782
#define GL_OP_NEGATE_EXT									0x8783
#define GL_OP_DOT3_EXT										0x8784
#define GL_OP_DOT4_EXT										0x8785
#define GL_OP_MUL_EXT										0x8786
#define GL_OP_ADD_EXT										0x8787
#define GL_OP_MADD_EXT										0x8788
#define GL_OP_FRAC_EXT										0x8789
#define GL_OP_MAX_EXT										0x878A
#define GL_OP_MIN_EXT										0x878B
#define GL_OP_SET_GE_EXT									0x878C
#define GL_OP_SET_LT_EXT									0x878D
#define GL_OP_CLAMP_EXT										0x878E
#define GL_OP_FLOOR_EXT										0x878F
#define GL_OP_ROUND_EXT										0x8790
#define GL_OP_EXP_BASE_2_EXT								0x8791
#define GL_OP_LOG_BASE_2_EXT								0x8792
#define GL_OP_POWER_EXT										0x8793
#define GL_OP_RECIP_EXT										0x8794
#define GL_OP_RECIP_SQRT_EXT								0x8795
#define GL_OP_SUB_EXT										0x8796
#define GL_OP_CROSS_PRODUCT_EXT								0x8797
#define GL_OP_MULTIPLY_MATRIX_EXT							0x8798
#define GL_OP_MOV_EXT										0x8799
#define GL_OUTPUT_VERTEX_EXT								0x879A
#define GL_OUTPUT_COLOR0_EXT								0x879B
#define GL_OUTPUT_COLOR1_EXT								0x879C
#define GL_OUTPUT_TEXTURE_COORD0_EXT						0x879D
#define GL_OUTPUT_TEXTURE_COORD1_EXT						0x879E
#define GL_OUTPUT_TEXTURE_COORD2_EXT						0x879F
#define GL_OUTPUT_TEXTURE_COORD3_EXT						0x87A0
#define GL_OUTPUT_TEXTURE_COORD4_EXT						0x87A1
#define GL_OUTPUT_TEXTURE_COORD5_EXT						0x87A2
#define GL_OUTPUT_TEXTURE_COORD6_EXT						0x87A3
#define GL_OUTPUT_TEXTURE_COORD7_EXT						0x87A4

#define GL_OUTPUT_TEXTURE_COORD8_EXT						0x87A5
#define GL_OUTPUT_TEXTURE_COORD9_EXT						0x87A6
#define GL_OUTPUT_TEXTURE_COORD10_EXT						0x87A7
#define GL_OUTPUT_TEXTURE_COORD11_EXT						0x87A8
#define GL_OUTPUT_TEXTURE_COORD12_EXT						0x87A9
#define GL_OUTPUT_TEXTURE_COORD13_EXT						0x87AA
#define GL_OUTPUT_TEXTURE_COORD14_EXT						0x87AB
#define GL_OUTPUT_TEXTURE_COORD15_EXT						0x87AC
#define GL_OUTPUT_TEXTURE_COORD16_EXT						0x87AD
#define GL_OUTPUT_TEXTURE_COORD17_EXT						0x87AE
#define GL_OUTPUT_TEXTURE_COORD18_EXT						0x87AF
#define GL_OUTPUT_TEXTURE_COORD19_EXT						0x87B0
#define GL_OUTPUT_TEXTURE_COORD20_EXT						0x87B1
#define GL_OUTPUT_TEXTURE_COORD21_EXT						0x87B2
#define GL_OUTPUT_TEXTURE_COORD22_EXT						0x87B3
#define GL_OUTPUT_TEXTURE_COORD23_EXT						0x87B4
#define GL_OUTPUT_TEXTURE_COORD24_EXT						0x87B5
#define GL_OUTPUT_TEXTURE_COORD25_EXT						0x87B6
#define GL_OUTPUT_TEXTURE_COORD26_EXT						0x87B7
#define GL_OUTPUT_TEXTURE_COORD27_EXT						0x87B8
#define GL_OUTPUT_TEXTURE_COORD28_EXT						0x87B9
#define GL_OUTPUT_TEXTURE_COORD29_EXT						0x87BA
#define GL_OUTPUT_TEXTURE_COORD30_EXT						0x87BB
#define GL_OUTPUT_TEXTURE_COORD31_EXT						0x87BC
#define GL_OUTPUT_FOG_EXT									0x87BD
#define GL_SCALAR_EXT										0x87BE
#define GL_VECTOR_EXT										0x87BF
#define GL_MATRIX_EXT										0x87C0
#define GL_VARIANT_EXT										0x87C1
#define GL_INVARIANT_EXT									0x87C2
#define GL_LOCAL_CONSTANT_EXT								0x87C3
#define GL_LOCAL_EXT										0x87C4
#define GL_MAX_VERTEX_SHADER_INSTRUCTIONS_EXT				0x87C5
#define GL_MAX_VERTEX_SHADER_VARIANTS_EXT					0x87C6
#define GL_MAX_VERTEX_SHADER_INVARIANTS_EXT					0x87C7
#define GL_MAX_VERTEX_SHADER_LOCAL_CONSTANTS_EXT			0x87C8
#define GL_MAX_VERTEX_SHADER_LOCALS_EXT						0x87C9
#define GL_MAX_OPTIMIZED_VERTEX_SHADER_INSTRUCTIONS_EXT		0x87CA
#define GL_MAX_OPTIMIZED_VERTEX_SHADER_VARIANTS_EXT			0x87CB
#define GL_MAX_OPTIMIZED_VERTEX_SHADER_INVARIANTS_EXT		0x87CC
#define GL_MAX_OPTIMIZED_VERTEX_SHADER_LOCAL_CONSTANTS_EXT	0x87CD
#define GL_MAX_OPTIMIZED_VERTEX_SHADER_LOCALS_EXT			0x87CE
#define GL_VERTEX_SHADER_INSTRUCTIONS_EXT					0x87CF
#define GL_VERTEX_SHADER_VARIANTS_EXT						0x87D0
#define GL_VERTEX_SHADER_INVARIANTS_EXT						0x87D1
#define GL_VERTEX_SHADER_LOCAL_CONSTANTS_EXT				0x87D2
#define GL_VERTEX_SHADER_LOCALS_EXT							0x87D3
#define GL_VERTEX_SHADER_OPTIMIZED_EXT						0x87D4
#define GL_X_EXT											0x87D5
#define GL_Y_EXT											0x87D6
#define GL_Z_EXT											0x87D7
#define GL_W_EXT											0x87D8
#define GL_NEGATIVE_X_EXT									0x87D9
#define GL_NEGATIVE_Y_EXT									0x87DA
#define GL_NEGATIVE_Z_EXT									0x87DB
#define GL_NEGATIVE_W_EXT									0x87DC
#define GL_ZERO_EXT											0x87DD
#define GL_ONE_EXT											0x87DE
#define GL_NEGATIVE_ONE_EXT									0x87DF
#define GL_NORMALIZED_RANGE_EXT								0x87E0
#define GL_FULL_RANGE_EXT									0x87E1
#define GL_CURRENT_VERTEX_EXT								0x87E2
#define GL_MVP_MATRIX_EXT									0x87E3
#define GL_VARIANT_VALUE_EXT								0x87E4
#define GL_VARIANT_DATATYPE_EXT								0x87E5
#define GL_VARIANT_ARRAY_STRIDE_EXT							0x87E6
#define GL_VARIANT_ARRAY_TYPE_EXT							0x87E7
#define GL_VARIANT_ARRAY_EXT								0x87E8
#define GL_VARIANT_ARRAY_POINTER_EXT						0x87E9
#define GL_INVARIANT_VALUE_EXT								0x87EA
#define GL_INVARIANT_DATATYPE_EXT							0x87EB
#define GL_LOCAL_CONSTANT_VALUE_EXT							0x87EC
#define GL_LOCAL_CONSTANT_DATATYPE_EXT						0x87ED

typedef GLvoid    (APIENTRY * PFNGLBEGINVERTEXSHADEREXTPROC) (void);
typedef GLvoid    (APIENTRY * PFNGLENDVERTEXSHADEREXTPROC) (void);
typedef GLvoid    (APIENTRY * PFNGLBINDVERTEXSHADEREXTPROC) (GLuint id);
typedef GLuint    (APIENTRY * PFNGLGENVERTEXSHADERSEXTPROC) (GLuint range);
typedef GLvoid    (APIENTRY * PFNGLDELETEVERTEXSHADEREXTPROC) (GLuint id);

typedef GLvoid    (APIENTRY * PFNGLSHADEROP1EXTPROC) (GLenum op, GLuint res, GLuint arg1);
typedef GLvoid    (APIENTRY * PFNGLSHADEROP2EXTPROC) (GLenum op, GLuint res, GLuint arg1, GLuint arg2);
typedef GLvoid    (APIENTRY * PFNGLSHADEROP3EXTPROC) (GLenum op, GLuint res, GLuint arg1, GLuint arg2, GLuint arg3);
typedef GLvoid    (APIENTRY * PFNGLSWIZZLEEXTPROC) (GLuint res, GLuint in, GLenum outX,	GLenum outY, GLenum outZ, GLenum outW);
typedef GLvoid    (APIENTRY * PFNGLWRITEMASKEXTPROC) (GLuint res, GLuint in, GLenum outX, GLenum outY, GLenum outZ, GLenum outW);
typedef GLvoid    (APIENTRY * PFNGLINSERTCOMPONENTEXTPROC) (GLuint res, GLuint src, GLuint num);
typedef GLvoid    (APIENTRY * PFNGLEXTRACTCOMPONENTEXTPROC) (GLuint res, GLuint src, GLuint num);
typedef GLuint    (APIENTRY * PFNGLGENSYMBOLSEXTPROC) (GLenum dataType, GLenum storageType,  GLenum range, GLuint components);

typedef GLvoid    (APIENTRY * PFNGLSETINVARIANTEXTPROC) (GLuint id, GLenum type, GLvoid *addr);
typedef GLvoid    (APIENTRY * PFNGLSETLOCALCONSTANTEXTPROC) (GLuint id, GLenum type, GLvoid *addr);
typedef GLvoid    (APIENTRY * PFNGLVARIANTBVEXTPROC) (GLuint id, GLbyte *addr);
typedef GLvoid    (APIENTRY * PFNGLVARIANTSVEXTPROC) (GLuint id, GLshort *addr);
typedef GLvoid    (APIENTRY * PFNGLVARIANTIVEXTPROC) (GLuint id, GLint *addr);
typedef GLvoid    (APIENTRY * PFNGLVARIANTFVEXTPROC) (GLuint id, GLfloat *addr);
typedef GLvoid    (APIENTRY * PFNGLVARIANTDVEXTPROC) (GLuint id, GLdouble *addr);
typedef GLvoid    (APIENTRY * PFNGLVARIANTUBVEXTPROC) (GLuint id, GLubyte *addr);
typedef GLvoid    (APIENTRY * PFNGLVARIANTUSVEXTPROC) (GLuint id, GLushort *addr);
typedef GLvoid    (APIENTRY * PFNGLVARIANTUIVEXTPROC) (GLuint id, GLuint *addr);
typedef GLvoid    (APIENTRY * PFNGLVARIANTPOINTEREXTPROC) (GLuint id, GLenum type, GLuint stride, GLvoid *addr);
typedef GLvoid    (APIENTRY * PFNGLENABLEVARIANTCLIENTSTATEEXTPROC) (GLuint id);
typedef GLvoid    (APIENTRY * PFNGLDISABLEVARIANTCLIENTSTATEEXTPROC) (GLuint id);

typedef GLuint    (APIENTRY * PFNGLBINDLIGHTPARAMETEREXTPROC) (GLenum light, GLenum value);
typedef GLuint    (APIENTRY * PFNGLBINDMATERIALPARAMETEREXTPROC) (GLenum face, GLenum value);
typedef GLuint    (APIENTRY * PFNGLBINDTEXGENPARAMETEREXTPROC) (GLenum unit, GLenum coord, GLenum value);
typedef GLuint    (APIENTRY * PFNGLBINDTEXTUREUNITPARAMETEREXTPROC) (GLenum unit, GLenum value);
typedef GLuint    (APIENTRY * PFNGLBINDPARAMETEREXTPROC) (GLenum value);
typedef GLboolean (APIENTRY * PFNGLISVARIANTENABLEDEXTPROC) (GLuint id, GLenum cap);
typedef GLvoid    (APIENTRY * PFNGLGETVARIANTBOOLEANVEXTPROC) (GLuint id, GLenum value, GLboolean *data);
typedef GLvoid    (APIENTRY * PFNGLGETVARIANTINTEGERVEXTPROC) (GLuint id, GLenum value, GLint *data);
typedef GLvoid    (APIENTRY * PFNGLGETVARIANTFLOATVEXTPROC) (GLuint id, GLenum value, GLfloat *data);
typedef GLvoid    (APIENTRY * PFNGLGETVARIANTPOINTERVEXTPROC) (GLuint id, GLenum value, GLvoid **data);
typedef GLvoid    (APIENTRY * PFNGLGETINVARIANTBOOLEANVEXTPROC) (GLuint id, GLenum value, GLboolean *data);
typedef GLvoid    (APIENTRY * PFNGLGETINVARIANTINTEGERVEXTPROC) (GLuint id, GLenum value, GLint *data);
typedef GLvoid    (APIENTRY * PFNGLGETINVARIANTFLOATVEXTPROC) (GLuint id, GLenum value, GLfloat *data);
typedef GLvoid    (APIENTRY * PFNGLGETLOCALCONSTANTBOOLEANVEXTPROC) (GLuint id, GLenum value, GLboolean *data);
typedef GLvoid    (APIENTRY * PFNGLGETLOCALCONSTANTINTEGERVEXTPROC) (GLuint id, GLenum value, GLint *data);
typedef GLvoid    (APIENTRY * PFNGLGETLOCALCONSTANTFLOATVEXTPROC) (GLuint id, GLenum value,  GLfloat *data);

extern PFNGLBEGINVERTEXSHADEREXTPROC  glBeginVertexShaderEXT;
extern PFNGLENDVERTEXSHADEREXTPROC    glEndVertexShaderEXT;
extern PFNGLBINDVERTEXSHADEREXTPROC   glBindVertexShaderEXT;
extern PFNGLGENVERTEXSHADERSEXTPROC   glGenVertexShadersEXT;
extern PFNGLDELETEVERTEXSHADEREXTPROC glDeleteVertexShaderEXT;

extern PFNGLSHADEROP1EXTPROC          glShaderOp1EXT;
extern PFNGLSHADEROP2EXTPROC          glShaderOp2EXT;
extern PFNGLSHADEROP3EXTPROC          glShaderOp3EXT;
extern PFNGLSWIZZLEEXTPROC            glSwizzleEXT;
extern PFNGLWRITEMASKEXTPROC          glWriteMaskEXT;
extern PFNGLINSERTCOMPONENTEXTPROC    glInsertComponentEXT;
extern PFNGLEXTRACTCOMPONENTEXTPROC   glExtractComponentEXT;
extern PFNGLGENSYMBOLSEXTPROC         glGenSymbolsEXT;

extern PFNGLSETINVARIANTEXTPROC       glSetInvariantEXT;
extern PFNGLSETLOCALCONSTANTEXTPROC   glSetLocalConstantEXT;
extern PFNGLVARIANTBVEXTPROC          glVariantbvEXT;
extern PFNGLVARIANTSVEXTPROC          glVariantsvEXT;
extern PFNGLVARIANTIVEXTPROC          glVariantivEXT;
extern PFNGLVARIANTFVEXTPROC          glVariantfvEXT;
extern PFNGLVARIANTDVEXTPROC          glVariantdvEXT;
extern PFNGLVARIANTUBVEXTPROC         glVariantubvEXT;
extern PFNGLVARIANTUSVEXTPROC         glVariantusvEXT;
extern PFNGLVARIANTUIVEXTPROC         glVariantuivEXT;
extern PFNGLVARIANTPOINTEREXTPROC     glVariantPointerEXT;
extern PFNGLENABLEVARIANTCLIENTSTATEEXTPROC  glEnableVariantClientStateEXT;
extern PFNGLDISABLEVARIANTCLIENTSTATEEXTPROC glDisableVariantClientStateEXT;

extern PFNGLBINDLIGHTPARAMETEREXTPROC       glBindLightParameterEXT;
extern PFNGLBINDMATERIALPARAMETEREXTPROC    glBindMaterialParameterEXT;
extern PFNGLBINDTEXGENPARAMETEREXTPROC      glBindTexGenParameterEXT;
extern PFNGLBINDTEXTUREUNITPARAMETEREXTPROC glBindTextureUnitParameterEXT;
extern PFNGLBINDPARAMETEREXTPROC            glBindParameterEXT;
extern PFNGLISVARIANTENABLEDEXTPROC         glIsVariantEnabledEXT;
extern PFNGLGETVARIANTBOOLEANVEXTPROC       glGetVariantBooleanvEXT;
extern PFNGLGETVARIANTINTEGERVEXTPROC       glGetVariantIntegervEXT;
extern PFNGLGETVARIANTFLOATVEXTPROC         glGetVariantFloatvEXT;
extern PFNGLGETVARIANTPOINTERVEXTPROC       glGetVariantPointervEXT;
extern PFNGLGETINVARIANTBOOLEANVEXTPROC     glGetInvariantBooleanvEXT;
extern PFNGLGETINVARIANTINTEGERVEXTPROC     glGetInvariantIntegervEXT;
extern PFNGLGETINVARIANTFLOATVEXTPROC       glGetInvariantFloatvEXT;
extern PFNGLGETLOCALCONSTANTBOOLEANVEXTPROC glGetLocalConstantBooleanvEXT;
extern PFNGLGETLOCALCONSTANTINTEGERVEXTPROC glGetLocalConstantIntegervEXT;
extern PFNGLGETLOCALCONSTANTFLOATVEXTPROC   glGetLocalConstantFloatvEXT;













// GL_GL2_fragment_shader
#define GL_FRAGMENT_SHADER_GL2              0x7011

#define GL_MAX_FRAGMENT_UNIFORM_FLOATS_GL2  0x7040
#define GL_MAX_FP_TEXTURE_UNITS_GL2         0x7041


// GL_GL2_shader_objects
#define GL_PROGRAM_OBJECT_GL2               0x7000

typedef char         GLchar;
typedef unsigned int GLhandle;

typedef GLvoid         (APIENTRY *PFNGLDELETEOBJECTGL2PROC)(GLhandle obj);
typedef GLvoid         (APIENTRY *PFNGLDELETEOBJECTSGL2PROC)(GLsizei n, GLhandle *objs);
typedef GLhandle       (APIENTRY *PFNGLGETHANDLEGL2PROC)(GLenum pname);
typedef GLvoid         (APIENTRY *PFNGLDETACHOBJECTGL2PROC)(GLhandle containerObj, GLhandle attachedObj);

typedef GLvoid         (APIENTRY *PFNGLOBJECTPARAMETERFGL2PROC)(GLhandle obj, GLenum pname, GLfloat param);
typedef GLvoid         (APIENTRY *PFNGLOBJECTPARAMETERIGL2PROC)(GLhandle obj, GLenum pname, GLint param);
typedef GLvoid         (APIENTRY *PFNGLGETOBJECTPARAMETERFVGL2PROC)(GLhandle obj, GLenum pname, GLfloat *params);
typedef GLvoid         (APIENTRY *PFNGLGETOBJECTPARAMETERIVGL2PROC)(GLhandle obj, GLenum pname, GLint *params);

typedef GLhandle       (APIENTRY *PFNGLCREATESHADEROBJECTGL2PROC)(GLenum shaderType);
typedef GLvoid         (APIENTRY *PFNGLLOADSHADERGL2PROC)(GLhandle shaderObj, GLuint nseg, const GLchar **seg, const GLint *length);
typedef GLvoid         (APIENTRY *PFNGLAPPENDSHADERGL2PROC)(GLhandle shaderObj, const GLchar *seg, GLint length);
typedef GLboolean      (APIENTRY *PFNGLCOMPILESHADERGL2PROC)(GLhandle shaderObj);
typedef const GLchar * (APIENTRY *PFNGLGETINFOLOGGL2PROC)(GLhandle obj, GLint *length);
typedef const GLchar * (APIENTRY *PFNGLGETSHADERSTRINGGL2PROC)(GLhandle shaderObj, GLuint *length);
typedef GLhandle       (APIENTRY *PFNGLCREATEPROGRAMOBJECTGL2PROC)(GLvoid);
typedef GLboolean      (APIENTRY *PFNGLATTACHSHADEROBJECTGL2PROC)(GLhandle programObj, GLhandle shaderObj);
typedef GLboolean      (APIENTRY *PFNGLLINKPROGRAMGL2PROC)(GLhandle programObj);
typedef GLboolean      (APIENTRY *PFNGLUSEPROGRAMOBJECTGL2PROC)(GLhandle programObj);

typedef GLvoid         (APIENTRY *PFNGLLOADUNIFORM1FGL2PROC)(GLint location, GLfloat f0);
typedef GLvoid         (APIENTRY *PFNGLLOADUNIFORM2FGL2PROC)(GLint location, GLfloat f0, GLfloat f1);
typedef GLvoid         (APIENTRY *PFNGLLOADUNIFORM3FGL2PROC)(GLint location, GLfloat f0, GLfloat f1, GLfloat f2);
typedef GLvoid         (APIENTRY *PFNGLLOADUNIFORM4FGL2PROC)(GLint location, GLfloat f0, GLfloat f1, GLfloat f2, GLfloat f3);
typedef GLvoid         (APIENTRY *PFNGLLOADUNIFORM1FVGL2PROC)(GLint location, GLfloat *f);
typedef GLvoid         (APIENTRY *PFNGLLOADUNIFORM2FVGL2PROC)(GLint location, GLfloat *f);
typedef GLvoid         (APIENTRY *PFNGLLOADUNIFORM3FVGL2PROC)(GLint location, GLfloat *f);
typedef GLvoid         (APIENTRY *PFNGLLOADUNIFORM4FVGL2PROC)(GLint location, GLfloat *f);
typedef GLvoid         (APIENTRY *PFNGLLOADUNIFORMINTGL2PROC)(GLint location, GLint i0);
typedef GLvoid         (APIENTRY *PFNGLLOADUNIFORMBOOLGL2PROC)(GLint location, GLboolean b0);

typedef GLvoid         (APIENTRY *PFNGLLOADUNIFORMARRAY1FVGL2PROC)(GLint location, GLint start, GLuint count, GLfloat *f);
typedef GLvoid         (APIENTRY *PFNGLLOADUNIFORMARRAY2FVGL2PROC)(GLint location, GLint start, GLuint count, GLfloat *f);
typedef GLvoid         (APIENTRY *PFNGLLOADUNIFORMARRAY3FVGL2PROC)(GLint location, GLint start, GLuint count, GLfloat *f);
typedef GLvoid         (APIENTRY *PFNGLLOADUNIFORMARRAY4FVGL2PROC)(GLint location, GLint start, GLuint count, GLfloat *f);
typedef GLvoid         (APIENTRY *PFNGLLOADUNIFORMMATRIX2FVGL2PROC)(GLint location, GLfloat *f);
typedef GLvoid         (APIENTRY *PFNGLLOADUNIFORMMATRIX3FVGL2PROC)(GLint location, GLfloat *f);
typedef GLvoid         (APIENTRY *PFNGLLOADUNIFORMMATRIX4FVGL2PROC)(GLint location, GLfloat *f);
typedef GLvoid         (APIENTRY *PFNGLLOADUNIFORMMATRIXARRAY2FVGL2PROC)(GLint location, GLint start, GLuint count, GLfloat *f);
typedef GLvoid         (APIENTRY *PFNGLLOADUNIFORMMATRIXARRAY3FVGL2PROC)(GLint location, GLint start, GLuint count, GLfloat *f);
typedef GLvoid         (APIENTRY *PFNGLLOADUNIFORMMATRIXARRAY4FVGL2PROC)(GLint location, GLint start, GLuint count, GLfloat *f);

typedef GLint          (APIENTRY *PFNGLGETUNIFORMLOCATIONGL2PROC)(GLhandle programObj, const GLchar *name, GLint length);
typedef GLvoid         (APIENTRY *PFNGLGETUNIFORMFVGL2PROC)(GLhandle programObj, GLint location, GLfloat *params);
typedef GLvoid         (APIENTRY *PFNGLGETUNIFORMIVGL2PROC)(GLhandle programObj, GLint location, GLint *params);

extern PFNGLDELETEOBJECTGL2PROC  glDeleteObjectGL2;
extern PFNGLDELETEOBJECTSGL2PROC glDeleteObjectsGL2;
extern PFNGLGETHANDLEGL2PROC     glGetHandleGL2;
extern PFNGLDETACHOBJECTGL2PROC  glDetachObjectGL2;

extern PFNGLOBJECTPARAMETERFGL2PROC     glObjectParameterfGL2;
extern PFNGLOBJECTPARAMETERIGL2PROC     glObjectParameteriGL2;
extern PFNGLGETOBJECTPARAMETERFVGL2PROC glGetObjectParameterfvGL2;
extern PFNGLGETOBJECTPARAMETERIVGL2PROC glGetObjectParameterivGL2;

extern PFNGLCREATESHADEROBJECTGL2PROC  glCreateShaderObjectGL2;
extern PFNGLLOADSHADERGL2PROC          glLoadShaderGL2;
extern PFNGLAPPENDSHADERGL2PROC        glAppendShaderGL2;
extern PFNGLCOMPILESHADERGL2PROC       glCompileShaderGL2;
extern PFNGLGETINFOLOGGL2PROC          glGetInfoLogGL2;
extern PFNGLGETSHADERSTRINGGL2PROC     glGetShaderStringGL2;
extern PFNGLCREATEPROGRAMOBJECTGL2PROC glCreateProgramObjectGL2;
extern PFNGLATTACHSHADEROBJECTGL2PROC  glAttachShaderObjectGL2;
extern PFNGLLINKPROGRAMGL2PROC         glLinkProgramGL2;
extern PFNGLUSEPROGRAMOBJECTGL2PROC    glUseProgramObjectGL2;

extern PFNGLLOADUNIFORM1FGL2PROC   glLoadUniform1fGL2;
extern PFNGLLOADUNIFORM2FGL2PROC   glLoadUniform2fGL2;
extern PFNGLLOADUNIFORM3FGL2PROC   glLoadUniform3fGL2;
extern PFNGLLOADUNIFORM4FGL2PROC   glLoadUniform4fGL2;
extern PFNGLLOADUNIFORM1FVGL2PROC  glLoadUniform1fvGL2;
extern PFNGLLOADUNIFORM2FVGL2PROC  glLoadUniform2fvGL2;
extern PFNGLLOADUNIFORM3FVGL2PROC  glLoadUniform3fvGL2;
extern PFNGLLOADUNIFORM4FVGL2PROC  glLoadUniform4fvGL2;
extern PFNGLLOADUNIFORMINTGL2PROC  glLoadUniformIntGL2;
extern PFNGLLOADUNIFORMBOOLGL2PROC glLoadUniformBoolGL2;

extern PFNGLLOADUNIFORMARRAY1FVGL2PROC  glLoadUniformArray1fvGL2;
extern PFNGLLOADUNIFORMARRAY2FVGL2PROC  glLoadUniformArray2fvGL2;
extern PFNGLLOADUNIFORMARRAY3FVGL2PROC  glLoadUniformArray3fvGL2;
extern PFNGLLOADUNIFORMARRAY4FVGL2PROC  glLoadUniformArray4fvGL2;
extern PFNGLLOADUNIFORMMATRIX2FVGL2PROC glLoadUniformMatrix2fvGL2;
extern PFNGLLOADUNIFORMMATRIX3FVGL2PROC glLoadUniformMatrix3fvGL2;
extern PFNGLLOADUNIFORMMATRIX4FVGL2PROC glLoadUniformMatrix4fvGL2;
extern PFNGLLOADUNIFORMMATRIXARRAY2FVGL2PROC glLoadUniformMatrixArray2fvGL2;
extern PFNGLLOADUNIFORMMATRIXARRAY3FVGL2PROC glLoadUniformMatrixArray3fvGL2;
extern PFNGLLOADUNIFORMMATRIXARRAY4FVGL2PROC glLoadUniformMatrixArray4fvGL2;

extern PFNGLGETUNIFORMLOCATIONGL2PROC glGetUniformLocationGL2;
extern PFNGLGETUNIFORMFVGL2PROC glGetUniformfvGL2;
extern PFNGLGETUNIFORMIVGL2PROC glGetUniformivGL2;


// GL_GL2_vertex_shader
#define GL_VERTEX_SHADER_GL2                0x7010

#define GL_SHADER_OBJECT_GL2                0x7001

#define GL_MAX_VERTEX_UNIFORM_FLOATS_GL2    0x7030
#define GL_MAX_VARYING_FLOATS_GL2           0x7031
#define GL_MAX_ACTIVE_ATTRIBUTES_GL2        0x7032

#define GL_OBJECT_TYPE_GL2                  0x7020
#define GL_OBJECT_SUBTYPE_GL2               0x7021

typedef GLvoid (APIENTRY *PFNGLBINDATTRIBUTELOCATIONGL2PROC)(GLhandle programObj, GLuint index, const GLchar *name, GLint length);
typedef GLboolean (APIENTRY *PFNGLGETACTIVEATTRIBUTESGL2PROC)(GLhandle programObj, GLint *count, const GLchar **atributes, GLint **length);

extern PFNGLBINDATTRIBUTELOCATIONGL2PROC glBindAttributeLocationGL2;
extern PFNGLGETACTIVEATTRIBUTESGL2PROC   glGetActiveAttributesGL2;













// GL_HP_occlusion_test
#define GL_OCCLUSION_TEST_HP              0x8165
#define GL_OCCLUSION_TEST_RESULT_HP       0x8166













// GL_NV_occlusion_query
#define GL_PIXEL_COUNTER_BITS_NV           0x8864
#define GL_CURRENT_OCCLUSION_QUERY_ID_NV   0x8865
#define GL_PIXEL_COUNT_NV                  0x8866
#define GL_PIXEL_COUNT_AVAILABLE_NV        0x8867

typedef GLvoid    (APIENTRY * PFNGLGENOCCLUSIONQUERIESNVPROC) (GLsizei n, GLuint *ids);
typedef GLvoid    (APIENTRY * PFNGLDELETEOCCLUSIONQUERIESNVPROC) (GLsizei n, const GLuint *ids);
typedef GLboolean (APIENTRY * PFNGLISOCCLUSIONQUERYNVPROC) (GLuint id);
typedef GLvoid    (APIENTRY * PFNGLBEGINOCCLUSIONQUERYNVPROC) (GLuint id);
typedef GLvoid    (APIENTRY * PFNGLENDOCCLUSIONQUERYNVPROC) (GLvoid);
typedef GLvoid    (APIENTRY * PFNGLGETOCCLUSIONQUERYIVNVPROC) (GLuint id, GLenum pname, GLint *params);
typedef GLvoid    (APIENTRY * PFNGLGETOCCLUSIONQUERYUIVNVPROC) (GLuint id, GLenum pname, GLuint *params);

extern PFNGLGENOCCLUSIONQUERIESNVPROC    glGenOcclusionQueriesNV;
extern PFNGLDELETEOCCLUSIONQUERIESNVPROC glDeleteOcclusionQueriesNV;
extern PFNGLISOCCLUSIONQUERYNVPROC       glIsOcclusionQueryNV;
extern PFNGLBEGINOCCLUSIONQUERYNVPROC    glBeginOcclusionQueryNV;
extern PFNGLENDOCCLUSIONQUERYNVPROC      glEndOcclusionQueryNV;
extern PFNGLGETOCCLUSIONQUERYIVNVPROC    glGetOcclusionQueryivNV;
extern PFNGLGETOCCLUSIONQUERYUIVNVPROC   glGetOcclusionQueryuivNV;


// GL_NV_register_combiners
#define GL_REGISTER_COMBINERS_NV          0x8522
#define GL_VARIABLE_A_NV                  0x8523
#define GL_VARIABLE_B_NV                  0x8524
#define GL_VARIABLE_C_NV                  0x8525
#define GL_VARIABLE_D_NV                  0x8526
#define GL_VARIABLE_E_NV                  0x8527
#define GL_VARIABLE_F_NV                  0x8528
#define GL_VARIABLE_G_NV                  0x8529
#define GL_CONSTANT_COLOR0_NV             0x852A
#define GL_CONSTANT_COLOR1_NV             0x852B
#define GL_PRIMARY_COLOR_NV               0x852C
#define GL_SECONDARY_COLOR_NV             0x852D
#define GL_SPARE0_NV                      0x852E
#define GL_SPARE1_NV                      0x852F
#define GL_DISCARD_NV                     0x8530
#define GL_E_TIMES_F_NV                   0x8531
#define GL_SPARE0_PLUS_SECONDARY_COLOR_NV 0x8532
#define GL_UNSIGNED_IDENTITY_NV           0x8536
#define GL_UNSIGNED_INVERT_NV             0x8537
#define GL_EXPAND_NORMAL_NV               0x8538
#define GL_EXPAND_NEGATE_NV               0x8539
#define GL_HALF_BIAS_NORMAL_NV            0x853A
#define GL_HALF_BIAS_NEGATE_NV            0x853B
#define GL_SIGNED_IDENTITY_NV             0x853C
#define GL_SIGNED_NEGATE_NV               0x853D
#define GL_SCALE_BY_TWO_NV                0x853E
#define GL_SCALE_BY_FOUR_NV               0x853F
#define GL_SCALE_BY_ONE_HALF_NV           0x8540
#define GL_BIAS_BY_NEGATIVE_ONE_HALF_NV   0x8541
#define GL_COMBINER_INPUT_NV              0x8542
#define GL_COMBINER_MAPPING_NV            0x8543
#define GL_COMBINER_COMPONENT_USAGE_NV    0x8544
#define GL_COMBINER_AB_DOT_PRODUCT_NV     0x8545
#define GL_COMBINER_CD_DOT_PRODUCT_NV     0x8546
#define GL_COMBINER_MUX_SUM_NV            0x8547
#define GL_COMBINER_SCALE_NV              0x8548
#define GL_COMBINER_BIAS_NV               0x8549
#define GL_COMBINER_AB_OUTPUT_NV          0x854A
#define GL_COMBINER_CD_OUTPUT_NV          0x854B
#define GL_COMBINER_SUM_OUTPUT_NV         0x854C
#define GL_MAX_GENERAL_COMBINERS_NV       0x854D
#define GL_NUM_GENERAL_COMBINERS_NV       0x854E
#define GL_COLOR_SUM_CLAMP_NV             0x854F
#define GL_COMBINER0_NV                   0x8550
#define GL_COMBINER1_NV                   0x8551
#define GL_COMBINER2_NV                   0x8552
#define GL_COMBINER3_NV                   0x8553
#define GL_COMBINER4_NV                   0x8554
#define GL_COMBINER5_NV                   0x8555
#define GL_COMBINER6_NV                   0x8556
#define GL_COMBINER7_NV                   0x8557

typedef void (APIENTRY * PFNGLCOMBINERPARAMETERFVNVPROC) (GLenum pname, const GLfloat *params);
typedef void (APIENTRY * PFNGLCOMBINERPARAMETERFNVPROC) (GLenum pname, GLfloat param);
typedef void (APIENTRY * PFNGLCOMBINERPARAMETERIVNVPROC) (GLenum pname, const GLint *params);
typedef void (APIENTRY * PFNGLCOMBINERPARAMETERINVPROC) (GLenum pname, GLint param);

typedef void (APIENTRY * PFNGLCOMBINERINPUTNVPROC) (GLenum stage, GLenum portion, GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage);
typedef void (APIENTRY * PFNGLCOMBINEROUTPUTNVPROC) (GLenum stage, GLenum portion, GLenum abOutput, GLenum cdOutput, GLenum sumOutput, GLenum scale, GLenum bias, GLboolean abDotProduct, GLboolean cdDotProduct, GLboolean muxSum);
typedef void (APIENTRY * PFNGLFINALCOMBINERINPUTNVPROC) (GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage);

typedef void (APIENTRY * PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC) (GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLfloat *params);
typedef void (APIENTRY * PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC) (GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC) (GLenum stage, GLenum portion, GLenum pname, GLfloat *params);
typedef void (APIENTRY * PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC) (GLenum stage, GLenum portion, GLenum pname, GLint *params);

typedef void (APIENTRY * PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC) (GLenum variable, GLenum pname, GLfloat *params);
typedef void (APIENTRY * PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC) (GLenum variable, GLenum pname, GLint *params);

extern PFNGLCOMBINERPARAMETERFVNVPROC glCombinerParameterfvNV;
extern PFNGLCOMBINERPARAMETERFNVPROC  glCombinerParameterfNV;
extern PFNGLCOMBINERPARAMETERIVNVPROC glCombinerParameterivNV;
extern PFNGLCOMBINERPARAMETERINVPROC  glCombinerParameteriNV;

extern PFNGLCOMBINERINPUTNVPROC       glCombinerInputNV;
extern PFNGLCOMBINEROUTPUTNVPROC      glCombinerOutputNV;
extern PFNGLFINALCOMBINERINPUTNVPROC  glFinalCombinerInputNV;

extern PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC  glGetCombinerInputParameterfvNV;
extern PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC  glGetCombinerInputParameterivNV;
extern PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC glGetCombinerOutputParameterfvNV;
extern PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC glGetCombinerOutputParameterivNV;

extern PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC glGetFinalCombinerInputParameterfvNV;
extern PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC glGetFinalCombinerInputParameterivNV;


// GL_NV_texture_env_combine4
#define GL_COMBINE4_NV                      0x8503

#define GL_SOURCE3_RGB_NV                   0x8583
#define GL_SOURCE3_ALPHA_NV                 0x858B
#define GL_OPERAND3_RGB_NV                  0x8593
#define GL_OPERAND3_ALPHA_NV                0x859B


// GL_NV_vertex_array_range
#define GL_VERTEX_ARRAY_RANGE_NV             0x851D
#define GL_VERTEX_ARRAY_RANGE_LENGTH_NV      0x851E
#define GL_VERTEX_ARRAY_RANGE_VALID_NV       0x851F
#define GL_MAX_VERTEX_ARRAY_RANGE_ELEMENT_NV 0x8520
#define GL_VERTEX_ARRAY_RANGE_POINTER_NV     0x8521


typedef void  (APIENTRY * PFNGLFLUSHVERTEXARRAYRANGENVPROC) (void);
typedef void  (APIENTRY * PFNGLVERTEXARRAYRANGENVPROC) (GLsizei size, const GLvoid *pointer);
typedef void *(APIENTRY * PFNWGLALLOCATEMEMORYNVPROC) (int size, float readfreq, float writefreq, float priority);
typedef void  (APIENTRY * PFNWGLFREEMEMORYNVPROC) (void *pointer);

extern PFNGLFLUSHVERTEXARRAYRANGENVPROC glFlushVertexArrayRangeNV;
extern PFNGLVERTEXARRAYRANGENVPROC      glVertexArrayRangeNV;
extern PFNWGLALLOCATEMEMORYNVPROC       wglAllocateMemoryNV;
extern PFNWGLFREEMEMORYNVPROC           wglFreeMemoryNV;








// GL_SGIS_generate_mipmap
#define GL_GENERATE_MIPMAP_SGIS				0x8191
#define GL_GENERATE_MIPMAP_HINT_SGIS		0x8192













#ifdef WIN32

// WGL_ARB_extensions_string
typedef char * (APIENTRY * WGL_ARB_EXTENSIONS_STRING) (HDC hdc);

extern WGL_ARB_EXTENSIONS_STRING wglGetExtensionsStringARB;


// WGL_ARB_make_current_read
typedef BOOL (WINAPI * PFNWGLMAKECONTEXTCURRENTARBPROC) (HDC hDrawDC, HDC hReadDC, HGLRC hGLRC);
typedef HDC (WINAPI * PFNWGLGETCURRENTREADDCARBPROC) (VOID);

extern PFNWGLMAKECONTEXTCURRENTARBPROC wglMakeContextCurrentARB;
extern PFNWGLGETCURRENTREADDCARBPROC wglGetCurrentReadDCARB;


// WGL_ARB_multisample
#define WGL_SAMPLE_BUFFERS_ARB          0x2041
#define WGL_SAMPLES_ARB                 0x2042


// WGL_ARB_pbuffer
#define WGL_DRAW_TO_PBUFFER_ARB              0x202D
#define WGL_MAX_PBUFFER_PIXELS_ARB           0x202E
#define WGL_MAX_PBUFFER_WIDTH_ARB            0x202F
#define WGL_MAX_PBUFFER_HEIGHT_ARB           0x2030
#define WGL_PBUFFER_LARGEST_ARB              0x2033
#define WGL_PBUFFER_WIDTH_ARB                0x2034
#define WGL_PBUFFER_HEIGHT_ARB               0x2035
#define WGL_PBUFFER_LOST_ARB				 0x2036

DECLARE_HANDLE(HPBUFFERARB);

typedef HPBUFFERARB (WINAPI * PFNWGLCREATEPBUFFERARBPROC) (HDC hDC, int iPixelFormat, int iWidth, int iHeight, const int *piAttribList);
typedef HDC (WINAPI * PFNWGLGETPBUFFERDCARBPROC) (HPBUFFERARB hPbuffer);
typedef int (WINAPI * PFNWGLRELEASEPBUFFERDCARBPROC) (HPBUFFERARB hPbuffer, HDC hDC);
typedef BOOL (WINAPI * PFNWGLDESTROYPBUFFERARBPROC) (HPBUFFERARB hPbuffer);
typedef BOOL (WINAPI * PFNWGLQUERYPBUFFERARBPROC) (HPBUFFERARB hPbuffer, int iAttribute, int *piValue);
		
extern PFNWGLCREATEPBUFFERARBPROC    wglCreatePbufferARB;
extern PFNWGLGETPBUFFERDCARBPROC     wglGetPbufferDCARB;
extern PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB;
extern PFNWGLDESTROYPBUFFERARBPROC   wglDestroyPbufferARB;
extern PFNWGLQUERYPBUFFERARBPROC     wglQueryPbufferARB;


// WGL_ARB_pixel_format
#define WGL_NUMBER_PIXEL_FORMATS_ARB		0x2000
#define WGL_DRAW_TO_WINDOW_ARB				0x2001
#define WGL_DRAW_TO_BITMAP_ARB				0x2002
#define WGL_ACCELERATION_ARB				0x2003
#define WGL_NEED_PALETTE_ARB				0x2004
#define	WGL_NEED_SYSTEM_PALETTE_ARB			0x2005
#define	WGL_SWAP_LAYER_BUFFERS_ARB			0x2006
#define	WGL_SWAP_METHOD_ARB					0x2007
#define	WGL_NUMBER_OVERLAYS_ARB				0x2008
#define	WGL_NUMBER_UNDERLAYS_ARB			0x2009
#define WGL_TRANSPARENT_ARB					0x200A
#define WGL_TRANSPARENT_RED_VALUE_ARB		0x2037
#define WGL_TRANSPARENT_GREEN_VALUE_ARB		0x2038
#define WGL_TRANSPARENT_BLUE_VALUE_ARB		0x2039
#define WGL_TRANSPARENT_ALPHA_VALUE_ARB		0x203A
#define WGL_TRANSPARENT_INDEX_VALUE_ARB		0x203B
#define WGL_SHARE_DEPTH_ARB					0x200C
#define WGL_SHARE_STENCIL_ARB				0x200D
#define WGL_SHARE_ACCUM_ARB					0x200E
#define WGL_SUPPORT_GDI_ARB					0x200F
#define WGL_SUPPORT_OPENGL_ARB				0x2010
#define WGL_DOUBLE_BUFFER_ARB				0x2011
#define WGL_STEREO_ARB						0x2012
#define WGL_PIXEL_TYPE_ARB					0x2013
#define WGL_COLOR_BITS_ARB					0x2014
#define WGL_RED_BITS_ARB					0x2015
#define WGL_RED_SHIFT_ARB					0x2016
#define WGL_GREEN_BITS_ARB					0x2017
#define WGL_GREEN_SHIFT_ARB					0x2018
#define WGL_BLUE_BITS_ARB					0x2019
#define WGL_BLUE_SHIFT_ARB					0x201A
#define WGL_ALPHA_BITS_ARB					0x201B
#define WGL_ALPHA_SHIFT_ARB					0x201C
#define WGL_ACCUM_BITS_ARB					0x201D
#define WGL_ACCUM_RED_BITS_ARB				0x201E
#define WGL_ACCUM_GREEN_BITS_ARB			0x201F
#define WGL_ACCUM_BLUE_BITS_ARB				0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB			0x2021
#define WGL_DEPTH_BITS_ARB					0x2022
#define WGL_STENCIL_BITS_ARB				0x2023
#define WGL_AUX_BUFFERS_ARB					0x2024
#define WGL_NO_ACCELERATION_ARB				0x2025
#define WGL_GENERIC_ACCELERATION_ARB		0x2026
#define WGL_FULL_ACCELERATION_ARB			0x2027
#define WGL_SWAP_EXCHANGE_ARB				0x2028
#define WGL_SWAP_COPY_ARB					0x2029
#define WGL_SWAP_UNDEFINED_ARB				0x202A
#define WGL_TYPE_RGBA_ARB					0x202B
#define WGL_TYPE_COLORINDEX_ARB				0x202C


typedef BOOL (WINAPI * PFNWGLGETPIXELFORMATATTRIBIVARBPROC) (
                                        HDC hDC,
                                        int iPixelFormat,
                                        int iLayerPlane,
                                        UINT nAttributes,
                                        const int *piAttributes,
                                        int *piValues);
typedef BOOL (WINAPI * PFNWGLGETPIXELFORMATATTRIBFVARBPROC) (
                                        HDC hDC,
                                        int iPixelFormat,
                                        int iLayerPlane,
                                        UINT nAttributes,
                                        const int *piAttributes,
                                        FLOAT *pfValues);
typedef BOOL (WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC) (
                                        HDC hDC,
                                        const int *piAttribIList,
                                        const FLOAT *pfAttribFList,
                                        UINT nMaxFormats,
                                        int *piFormats,
                                        UINT *nNumFormats);

extern PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB;
extern PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB;
extern PFNWGLCHOOSEPIXELFORMATARBPROC      wglChoosePixelFormatARB;


// WGL_ARB_render_texture
#define WGL_BIND_TO_TEXTURE_RGB_ARB			0x2070
#define WGL_BIND_TO_TEXTURE_RGBA_ARB		0x2071
#define WGL_TEXTURE_FORMAT_ARB				0x2072
#define WGL_TEXTURE_TARGET_ARB				0x2073
#define WGL_MIPMAP_TEXTURE_ARB				0x2074
#define WGL_TEXTURE_RGB_ARB					0x2075
#define WGL_TEXTURE_RGBA_ARB				0x2076
#define WGL_NO_TEXTURE_ARB					0x2077
#define WGL_TEXTURE_CUBE_MAP_ARB			0x2078
#define WGL_TEXTURE_1D_ARB					0x2079
#define WGL_TEXTURE_2D_ARB					0x207A
#define WGL_MIPMAP_LEVEL_ARB				0x207B
#define WGL_CUBE_MAP_FACE_ARB				0x207C
#define WGL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB	0x207D
#define WGL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB 0x207E
#define WGL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB	0x207F
#define WGL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB 0x2080
#define WGL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB	0x2081
#define WGL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB 0x2082
#define WGL_FRONT_LEFT_ARB					0x2083
#define WGL_FRONT_RIGHT_ARB					0x2084
#define WGL_BACK_LEFT_ARB					0x2085
#define WGL_BACK_RIGHT_ARB					0x2086
#define WGL_AUX0_ARB						0x2087
#define WGL_AUX1_ARB						0x2088
#define WGL_AUX2_ARB						0x2089
#define WGL_AUX3_ARB						0x208A
#define WGL_AUX4_ARB						0x208B
#define WGL_AUX5_ARB						0x208C
#define WGL_AUX6_ARB						0x208D
#define WGL_AUX7_ARB						0x208E
#define WGL_AUX8_ARB						0x208F
#define WGL_AUX9_ARB						0x2090

typedef BOOL (WINAPI * PFNWGLBINDTEXIMAGEARBPROC)(HPBUFFERARB hPbuffer, int iBuffer);
typedef BOOL (WINAPI * PFNWGLRELEASETEXIMAGEARBPROC)(HPBUFFERARB hPbuffer, int iBuffer);
typedef BOOL (WINAPI * PFNWGLSETPBUFFERATTRIBARBPROC)(HPBUFFERARB hPbuffer, const int *piAttribList);

extern PFNWGLBINDTEXIMAGEARBPROC     wglBindTexImageARB;
extern PFNWGLRELEASETEXIMAGEARBPROC  wglReleaseTexImageARB;
extern PFNWGLSETPBUFFERATTRIBARBPROC wglSetPbufferAttribARB;










// WGL_ATI_pixel_format_float
#define WGL_TYPE_RGBA_FLOAT_ATI             0x21A0
#define GL_TYPE_RGBA_FLOAT_ATI              0x8820
#define GL_COLOR_CLEAR_UNCLAMPED_VALUE_ATI  0x8835









// WGL_EXT_swap_control
typedef void (APIENTRY * PFNWGLEXTSWAPINTERVALPROC) (int i); 

extern PFNWGLEXTSWAPINTERVALPROC wglSwapIntervalEXT;

#endif // WIN32

/*#ifdef LINUX

// GLX_EXT_visual_info
#define GLX_X_VISUAL_TYPE_EXT			0x22
#define GLX_TRANSPARENT_TYPE_EXT		0x23
#define GLX_TRANSPARENT_INDEX_VALUE_EXT	0x24
#define GLX_TRANSPARENT_RED_VALUE_EXT	0x25
#define GLX_TRANSPARENT_GREEN_VALUE_EXT	0x26
#define GLX_TRANSPARENT_BLUE_VALUE_EXT	0x27
#define GLX_TRANSPARENT_ALPHA_VALUE_EXT	0x28

#define GLX_TRUE_COLOR_EXT			0x8002
#define GLX_DIRECT_COLOR_EXT		0x8003
#define GLX_PSEUDO_COLOR_EXT		0x8004
#define GLX_STATIC_COLOR_EXT		0x8005
#define GLX_GRAY_SCALE_EXT			0x8006
#define GLX_STATIC_GRAY_EXT			0x8007

#define GLX_NONE_EXT				0x8000
#define GLX_TRANSPARENT_RGB_EXT		0x8008
#define GLX_TRANSPARENT_INDEX_EXT	0x8009


#endif // LINUX
*/

/* ------------------------------------------------------------------------- */

bool findExtension(char *extensions, char *extension);

#define isExtensionSupported(str)     (findExtension(extensions,     str))
#define isWGLXExtensionSupported(str) (findExtension(wglxExtensions, str) || findExtension(extensions, str))

#if defined(WIN32)
void initExtensions(HDC hdc);
#elif defined(LINUX)
void initExtensions(Display *display, int screen);
#endif

#endif // _GLEXTENSIONS_H_
