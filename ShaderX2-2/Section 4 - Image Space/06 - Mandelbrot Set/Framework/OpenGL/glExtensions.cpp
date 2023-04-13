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

#include "glExtensions.h"

bool GL_ARB_depth_texture              = false;
bool GL_ARB_fragment_program           = false;
bool GL_ARB_multisample                = false;
bool GL_ARB_multitexture               = false;
bool GL_ARB_shadow                     = false;
bool GL_ARB_shadow_ambient             = false;
bool GL_ARB_texture_compression        = false;
bool GL_ARB_texture_cube_map           = false;
bool GL_ARB_texture_env_add            = false;
bool GL_ARB_texture_env_combine        = false;
bool GL_ARB_texture_env_crossbar       = false;
bool GL_ARB_texture_env_dot3           = false;
bool GL_ARB_transpose_matrix           = false;
bool GL_ARB_vertex_blend               = false;
bool GL_ARB_vertex_program             = false;

bool GL_ATI_element_array              = false;
bool GL_ATI_envmap_bumpmap             = false;
bool GL_ATI_fragment_shader            = false;
bool GL_ATI_map_object_buffer          = false;
bool GL_ATI_pn_triangles               = false;
bool GL_ATI_texture_float              = false;
bool GL_ATI_texture_mirror_once        = false;
bool GL_ATI_vertex_array_object        = false;
bool GL_ATI_vertex_attrib_array_object = false;
bool GL_ATI_vertex_streams             = false;
bool GL_ATIX_texture_env_combine3      = false;
bool GL_ATIX_texture_env_route         = false;

bool GL_EXT_blend_color                = false;
bool GL_EXT_blend_func_separate        = false;
bool GL_EXT_blend_minmax               = false;
bool GL_EXT_blend_subtract             = false;
bool GL_EXT_compiled_vertex_array      = false;
bool GL_EXT_draw_range_elements        = false;
bool GL_EXT_fog_coord                  = false;
bool GL_EXT_packed_pixels              = false;
bool GL_EXT_polygon_offset             = false;
bool GL_EXT_stencil_wrap               = false;
bool GL_EXT_texture3D                  = false;
bool GL_EXT_texture_compression_s3tc   = false;
bool GL_EXT_texture_edge_clamp         = false;
bool GL_EXT_texture_env_dot3           = false;
bool GL_EXT_texture_filter_anisotropic = false;
bool GL_EXT_texture_lod_bias           = false;
bool GL_EXT_texture_rectangle          = false;
bool GL_EXT_vertex_shader              = false;

bool GL_GL2_fragment_shader            = false;
bool GL_GL2_shader_objects             = false;
bool GL_GL2_vertex_shader              = false;

bool GL_HP_occlusion_test              = false;

bool GL_NV_blend_square                = false;
bool GL_NV_occlusion_query             = false;
bool GL_NV_register_combiners          = false;
bool GL_NV_texture_env_combine4        = false;
bool GL_NV_vertex_array_range          = false;

bool GL_SGIS_generate_mipmap           = false;

#if defined(WIN32)

bool WGL_ARB_extensions_string         = false;
bool WGL_ARB_make_current_read         = false;
bool WGL_ARB_multisample               = false;
bool WGL_ARB_pbuffer                   = false;
bool WGL_ARB_pixel_format              = false;
bool WGL_ARB_render_texture            = false;

bool WGL_ATI_pixel_format_float        = false;

bool WGL_EXT_swap_control              = false;

#elif defined(LINUX)

//bool GLX_EXT_visual_info               = false;

#endif

// GL_ARB_multisample
PFNGLSAMPLECOVERAGEARBPROC glSampleCoverageARB = NULL;


// GL_ARB_multitexture
PFNGLACTIVETEXTUREARBPROC       glActiveTexture       = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTexture = NULL;

PFNGLMULTITEXCOORD1FARBPROC  glMultiTexCoord1f  = NULL;
PFNGLMULTITEXCOORD1FVARBPROC glMultiTexCoord1fv = NULL;

PFNGLMULTITEXCOORD2FARBPROC  glMultiTexCoord2f  = NULL;
PFNGLMULTITEXCOORD2FVARBPROC glMultiTexCoord2fv = NULL;

PFNGLMULTITEXCOORD3FARBPROC  glMultiTexCoord3f  = NULL;
PFNGLMULTITEXCOORD3FVARBPROC glMultiTexCoord3fv = NULL;

PFNGLMULTITEXCOORD4FARBPROC  glMultiTexCoord4f  = NULL;
PFNGLMULTITEXCOORD4FVARBPROC glMultiTexCoord4fv = NULL;


// GL_ARB_texture_compression
PFNGLCOMPRESSEDTEXIMAGE1DARBPROC    glCompressedTexImage1DARB    = NULL;
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC    glCompressedTexImage2DARB    = NULL;
PFNGLCOMPRESSEDTEXIMAGE3DARBPROC    glCompressedTexImage3DARB    = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC glCompressedTexSubImage1DARB = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC glCompressedTexSubImage2DARB = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC glCompressedTexSubImage3DARB = NULL;
PFNGLGETCOMPRESSEDTEXIMAGEARBPROC   glGetCompressedTexImageARB   = NULL;


// GL_ARB_transpose_matrix
PFNGLLOADTRANSPOSEMATRIXFARBPROC glLoadTransposeMatrixfARB = NULL;
PFNGLLOADTRANSPOSEMATRIXDARBPROC glLoadTransposeMatrixdARB = NULL;
PFNGLMULTTRANSPOSEMATRIXFARBPROC glMultTransposeMatrixfARB = NULL;
PFNGLMULTTRANSPOSEMATRIXDARBPROC glMultTransposeMatrixdARB = NULL;


// GL_ARB_vertex_blend
PFNGLWEIGHTBVARBPROC  glWeightbvARB  = NULL;
PFNGLWEIGHTSVARBPROC  glWeightsvARB  = NULL;
PFNGLWEIGHTIVARBPROC  glWeightivARB  = NULL;
PFNGLWEIGHTFVARBPROC  glWeightfvARB  = NULL;
PFNGLWEIGHTDVARBPROC  glWeightdvARB  = NULL;
PFNGLWEIGHTUBVARBPROC glWeightubvARB = NULL;
PFNGLWEIGHTUSVARBPROC glWeightusvARB = NULL;
PFNGLWEIGHTUIVARBPROC glWeightuivARB = NULL;

PFNGLWEIGHTPOINTERARBPROC glWeightPointerARB = NULL;
PFNGLVERTEXBLENDARBPROC   glVertexBlendARB   = NULL;


// GL_ARB_vertex_program
PFNGLVERTEXATTRIB1SARBPROC glVertexAttrib1sARB = NULL;
PFNGLVERTEXATTRIB1FARBPROC glVertexAttrib1fARB = NULL;
PFNGLVERTEXATTRIB1DARBPROC glVertexAttrib1dARB = NULL;
PFNGLVERTEXATTRIB2SARBPROC glVertexAttrib2sARB = NULL;
PFNGLVERTEXATTRIB2FARBPROC glVertexAttrib2fARB = NULL;
PFNGLVERTEXATTRIB2DARBPROC glVertexAttrib2dARB = NULL;
PFNGLVERTEXATTRIB3SARBPROC glVertexAttrib3sARB = NULL;
PFNGLVERTEXATTRIB3FARBPROC glVertexAttrib3fARB = NULL;
PFNGLVERTEXATTRIB3DARBPROC glVertexAttrib3dARB = NULL;
PFNGLVERTEXATTRIB4SARBPROC glVertexAttrib4sARB = NULL;
PFNGLVERTEXATTRIB4FARBPROC glVertexAttrib4fARB = NULL;
PFNGLVERTEXATTRIB4DARBPROC glVertexAttrib4dARB = NULL;
PFNGLVERTEXATTRIB4NUBARBPROC glVertexAttrib4NubARB = NULL;
PFNGLVERTEXATTRIB1SVARBPROC glVertexAttrib1svARB = NULL;
PFNGLVERTEXATTRIB1FVARBPROC glVertexAttrib1fvARB = NULL;
PFNGLVERTEXATTRIB1DVARBPROC glVertexAttrib1dvARB = NULL;
PFNGLVERTEXATTRIB2SVARBPROC glVertexAttrib2svARB = NULL;
PFNGLVERTEXATTRIB2FVARBPROC glVertexAttrib2fvARB = NULL;
PFNGLVERTEXATTRIB2DVARBPROC glVertexAttrib2dvARB = NULL;
PFNGLVERTEXATTRIB3SVARBPROC glVertexAttrib3svARB = NULL;
PFNGLVERTEXATTRIB3FVARBPROC glVertexAttrib3fvARB = NULL;
PFNGLVERTEXATTRIB3DVARBPROC glVertexAttrib3dvARB = NULL;
PFNGLVERTEXATTRIB4BVARBPROC glVertexAttrib4bvARB = NULL;
PFNGLVERTEXATTRIB4SVARBPROC glVertexAttrib4svARB = NULL;
PFNGLVERTEXATTRIB4IVARBPROC glVertexAttrib4ivARB = NULL;
PFNGLVERTEXATTRIB4UBVARBPROC glVertexAttrib4ubvARB = NULL;
PFNGLVERTEXATTRIB4USVARBPROC glVertexAttrib4usvARB = NULL;
PFNGLVERTEXATTRIB4UIVARBPROC glVertexAttrib4uivARB = NULL;
PFNGLVERTEXATTRIB4FVARBPROC glVertexAttrib4fvARB = NULL;
PFNGLVERTEXATTRIB4DVARBPROC glVertexAttrib4dvARB = NULL;
PFNGLVERTEXATTRIB4NBVARBPROC glVertexAttrib4NbvARB = NULL;
PFNGLVERTEXATTRIB4NSVARBPROC glVertexAttrib4NsvARB = NULL;
PFNGLVERTEXATTRIB4NIVARBPROC glVertexAttrib4NivARB = NULL;
PFNGLVERTEXATTRIB4NUBVARBPROC glVertexAttrib4NubvARB = NULL;
PFNGLVERTEXATTRIB4NUSVARBPROC glVertexAttrib4NusvARB = NULL;
PFNGLVERTEXATTRIB4NUIVARBPROC glVertexAttrib4NuivARB = NULL;
PFNGLVERTEXATTRIBPOINTERARBPROC glVertexAttribPointerARB = NULL;
PFNGLENABLEVERTEXATTRIBARRAYARBPROC glEnableVertexAttribArrayARB = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC glDisableVertexAttribArrayARB = NULL;
PFNGLPROGRAMSTRINGARBPROC glProgramStringARB = NULL;
PFNGLBINDPROGRAMARBPROC glBindProgramARB = NULL;
PFNGLDELETEPROGRAMSARBPROC glDeleteProgramsARB = NULL;
PFNGLGENPROGRAMSARBPROC glGenProgramsARB = NULL;
PFNGLPROGRAMENVPARAMETER4DARBPROC glProgramEnvParameter4dARB = NULL;
PFNGLPROGRAMENVPARAMETER4DVARBPROC glProgramEnvParameter4dvARB = NULL;
PFNGLPROGRAMENVPARAMETER4FARBPROC glProgramEnvParameter4fARB = NULL;
PFNGLPROGRAMENVPARAMETER4FVARBPROC glProgramEnvParameter4fvARB = NULL;
PFNGLPROGRAMLOCALPARAMETER4DARBPROC glProgramLocalParameter4dARB = NULL;
PFNGLPROGRAMLOCALPARAMETER4DVARBPROC glProgramLocalParameter4dvARB = NULL;
PFNGLPROGRAMLOCALPARAMETER4FARBPROC glProgramLocalParameter4fARB = NULL;
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC glProgramLocalParameter4fvARB = NULL;
PFNGLGETPROGRAMENVPARAMETERDVARBPROC glGetProgramEnvParameterdvARB = NULL;
PFNGLGETPROGRAMENVPARAMETERFVARBPROC glGetProgramEnvParameterfvARB = NULL;
PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC glGetProgramLocalParameterdvARB = NULL;
PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC glGetProgramLocalParameterfvARB = NULL;
PFNGLGETPROGRAMIVARBPROC glGetProgramivARB = NULL;
PFNGLGETPROGRAMSTRINGARBPROC glGetProgramStringARB = NULL;
PFNGLGETVERTEXATTRIBDVARBPROC glGetVertexAttribdvARB = NULL;
PFNGLGETVERTEXATTRIBFVARBPROC glGetVertexAttribfvARB = NULL;
PFNGLGETVERTEXATTRIBIVARBPROC glGetVertexAttribivARB = NULL;
PFNGLGETVERTEXATTRIBPOINTERVARBPROC glGetVertexAttribPointervARB = NULL;
PFNGLISPROGRAMARBPROC glIsProgramARB = NULL;



// GL_ATI_element_array
PFNGLELEMENTPOINTERATIPROC        glElementPointerATI        = NULL;
PFNGLDRAWELEMENTARRAYATIPROC      glDrawElementArrayATI      = NULL;
PFNGLDRAWRANGEELEMENTARRAYATIPROC glDrawRangeElementArrayATI = NULL;


// GL_ATI_envmap_bumpmap
PFNGLTEXBUMPPARAMETERIVATIPROC    glTexBumpParameterivATI = NULL;
PFNGLTEXBUMPPARAMETERFVATIPROC    glTexBumpParameterfvATI = NULL;
PFNGLGETTEXBUMPPARAMETERIVATIPROC glGetTexBumpParameterivATI = NULL;
PFNGLGETTEXBUMPPARAMETERFVATIPROC glGetTexBumpParameterfvATI = NULL;


// GL_ATI_fragment_shader
PFNGLGENFRAGMENTSHADERSATIPROC   glGenFragmentShadersATI   = NULL;
PFNGLBINDFRAGMENTSHADERATIPROC   glBindFragmentShaderATI   = NULL;
PFNGLDELETEFRAGMENTSHADERATIPROC glDeleteFragmentShaderATI = NULL;
PFNGLBEGINFRAGMENTSHADERATIPROC  glBeginFragmentShaderATI  = NULL;
PFNGLENDFRAGMENTSHADERATIPROC    glEndFragmentShaderATI    = NULL;
PFNGLPASSTEXCOORDATIPROC         glPassTexCoordATI         = NULL;
PFNGLSAMPLEMAPATIPROC            glSampleMapATI            = NULL;

PFNGLCOLORFRAGMENTOP1ATIPROC glColorFragmentOp1ATI = NULL;
PFNGLCOLORFRAGMENTOP2ATIPROC glColorFragmentOp2ATI = NULL;
PFNGLCOLORFRAGMENTOP3ATIPROC glColorFragmentOp3ATI = NULL;

PFNGLALPHAFRAGMENTOP1ATIPROC glAlphaFragmentOp1ATI = NULL;
PFNGLALPHAFRAGMENTOP2ATIPROC glAlphaFragmentOp2ATI = NULL;
PFNGLALPHAFRAGMENTOP3ATIPROC glAlphaFragmentOp3ATI = NULL;

PFNGLSETFRAGMENTSHADERCONSTANTATIPROC glSetFragmentShaderConstantATI = NULL;


// GL_ATI_map_object_buffer
PFNGLMAPOBJECTBUFFERATIPROC   glMapObjectBufferATI   = NULL;
PFNGLUNMAPOBJECTBUFFERATIPROC glUnmapObjectBufferATI = NULL;

// GL_ATI_pn_triangles
PFNGLPNTRIANGLESIATIPROC glPNTrianglesiATI = NULL;
PFNGLPNTRIANGLESFATIPROC glPNTrianglesfATI = NULL;


// GL_ATI_vertex_array_object
PFNGLNEWOBJECTBUFFERATIPROC    glNewObjectBufferATI    = NULL;
PFNGLISOBJECTBUFFERATIPROC     glIsObjectBufferATI     = NULL;
PFNGLUPDATEOBJECTBUFFERATIPROC glUpdateObjectBufferATI = NULL;
PFNGLGETOBJECTBUFFERFVATIPROC  glGetObjectBufferfvATI  = NULL;
PFNGLGETOBJECTBUFFERIVATIPROC  glGetObjectBufferivATI  = NULL;
PFNGLFREEOBJECTBUFFERATIPROC   glDeleteObjectBufferATI = NULL;

PFNGLARRAYOBJECTATIPROC        glArrayObjectATI        = NULL;
PFNGLGETARRAYOBJECTFVATIPROC   glGetArrayObjectfvATI   = NULL;
PFNGLGETARRAYOBJECTIVATIPROC   glGetArrayObjectivATI   = NULL;
PFNGLVARIANTARRAYOBJECTATIPROC glVariantArrayObjectATI = NULL;

PFNGLGETVARIANTARRAYOBJECTFVATIPROC glGetVariantArrayObjectfvATI = NULL;
PFNGLGETVARIANTARRAYOBJECTIVATIPROC glGetVariantArrayObjectivATI = NULL;


// GL_ATI_vertex_attrib_array_object
PFNGLVERTEXATTRIBARRAYOBJECTATIPROC glVertexAttribArrayObjectATI = NULL;


// GL_ATI_vertex_streams
PFNGLCLIENTACTIVEVERTEXSTREAMATIPROC glClientActiveVertexStreamATI = NULL;
PFNGLVERTEXBLENDENVIATIPROC glVertexBlendEnviATI = NULL;
PFNGLVERTEXBLENDENVFATIPROC glVertexBlendEnvfATI = NULL;

PFNGLVERTEXSTREAM2SATIPROC  glVertexStream2sATI  = NULL;
PFNGLVERTEXSTREAM2SVATIPROC glVertexStream2svATI = NULL;
PFNGLVERTEXSTREAM2IATIPROC  glVertexStream2iATI  = NULL;
PFNGLVERTEXSTREAM2IVATIPROC glVertexStream2ivATI = NULL;
PFNGLVERTEXSTREAM2FATIPROC  glVertexStream2fATI  = NULL;
PFNGLVERTEXSTREAM2FVATIPROC glVertexStream2fvATI = NULL;
PFNGLVERTEXSTREAM2DATIPROC  glVertexStream2dATI  = NULL;
PFNGLVERTEXSTREAM2DVATIPROC glVertexStream2dvATI = NULL;

PFNGLVERTEXSTREAM3SATIPROC  glVertexStream3sATI  = NULL;
PFNGLVERTEXSTREAM3SVATIPROC glVertexStream3svATI = NULL;
PFNGLVERTEXSTREAM3IATIPROC  glVertexStream3iATI  = NULL;
PFNGLVERTEXSTREAM3IVATIPROC glVertexStream3ivATI = NULL;
PFNGLVERTEXSTREAM3FATIPROC  glVertexStream3fATI  = NULL;
PFNGLVERTEXSTREAM3FVATIPROC glVertexStream3fvATI = NULL;
PFNGLVERTEXSTREAM3DATIPROC  glVertexStream3dATI  = NULL;
PFNGLVERTEXSTREAM3DVATIPROC glVertexStream3dvATI = NULL;

PFNGLVERTEXSTREAM4SATIPROC  glVertexStream4sATI  = NULL;
PFNGLVERTEXSTREAM4SVATIPROC glVertexStream4svATI = NULL;
PFNGLVERTEXSTREAM4IATIPROC  glVertexStream4iATI  = NULL;
PFNGLVERTEXSTREAM4IVATIPROC glVertexStream4ivATI = NULL;
PFNGLVERTEXSTREAM4FATIPROC  glVertexStream4fATI  = NULL;
PFNGLVERTEXSTREAM4FVATIPROC glVertexStream4fvATI = NULL;
PFNGLVERTEXSTREAM4DATIPROC  glVertexStream4dATI  = NULL;
PFNGLVERTEXSTREAM4DVATIPROC glVertexStream4dvATI = NULL;

PFNGLNORMALSTREAM3BATIPROC  glNormalStream3bATI  = NULL;
PFNGLNORMALSTREAM3BVATIPROC glNormalStream3bvATI = NULL;
PFNGLNORMALSTREAM3SATIPROC  glNormalStream3sATI  = NULL;
PFNGLNORMALSTREAM3SVATIPROC glNormalStream3svATI = NULL;
PFNGLNORMALSTREAM3IATIPROC  glNormalStream3iATI  = NULL;
PFNGLNORMALSTREAM3IVATIPROC glNormalStream3ivATI = NULL;
PFNGLNORMALSTREAM3FATIPROC  glNormalStream3fATI  = NULL;
PFNGLNORMALSTREAM3FVATIPROC glNormalStream3fvATI = NULL;
PFNGLNORMALSTREAM3DATIPROC  glNormalStream3dATI  = NULL;
PFNGLNORMALSTREAM3DVATIPROC glNormalStream3dvATI = NULL;



// GL_EXT_blend_color
PFNGLBLENDCOLOREXTPROC glBlendColorEXT = NULL;

// GL_EXT_blend_func_separate
PFNGLBLENDFUNCSEPARATEEXTPROC glBlendFuncSeparateEXT = NULL;


// GL_EXT_blend_minmax
PFNGLBLENDEQUATIONEXTPROC glBlendEquationEXT = NULL;


// GL_EXT_compiled_vertex_array
PFNGLLOCKARRAYSEXTPROC   glLockArraysEXT   = NULL;
PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT = NULL;


// GL_EXT_draw_range_elements
PFNGLDRAWRANGEELEMENTSEXTPROC glDrawRangeElementsEXT = NULL;


// GL_EXT_fog_coord
PFNGLFOGCOORDFEXTPROC  glFogCoordfEXT  = NULL;
PFNGLFOGCOORDDEXTPROC  glFogCoorddEXT  = NULL;
PFNGLFOGCOORDFVEXTPROC glFogCoordfvEXT = NULL;
PFNGLFOGCOORDDVEXTPROC glFogCoorddvEXT = NULL;
PFNGLFOGCOORDPOINTEREXTPROC glFogCoordPointerEXT = NULL;


// GL_EXT_polygon_offset
PFNGLPOLYGONOFFSETEXTPROC glPolygonOffsetEXT = NULL;


// GL_EXT_texture3D
PFNGLTEXIMAGE3DEXTPROC glTexImage3DEXT = NULL;


// GL_EXT_vertex_shader
PFNGLBEGINVERTEXSHADEREXTPROC  glBeginVertexShaderEXT         = NULL;
PFNGLENDVERTEXSHADEREXTPROC    glEndVertexShaderEXT           = NULL;
PFNGLBINDVERTEXSHADEREXTPROC   glBindVertexShaderEXT          = NULL;
PFNGLGENVERTEXSHADERSEXTPROC   glGenVertexShadersEXT          = NULL;
PFNGLDELETEVERTEXSHADEREXTPROC glDeleteVertexShaderEXT        = NULL;

PFNGLSHADEROP1EXTPROC          glShaderOp1EXT                 = NULL;
PFNGLSHADEROP2EXTPROC          glShaderOp2EXT                 = NULL;
PFNGLSHADEROP3EXTPROC          glShaderOp3EXT                 = NULL;
PFNGLSWIZZLEEXTPROC            glSwizzleEXT                   = NULL;
PFNGLWRITEMASKEXTPROC          glWriteMaskEXT                 = NULL;
PFNGLINSERTCOMPONENTEXTPROC    glInsertComponentEXT           = NULL;
PFNGLEXTRACTCOMPONENTEXTPROC   glExtractComponentEXT          = NULL;
PFNGLGENSYMBOLSEXTPROC         glGenSymbolsEXT                = NULL;

PFNGLSETINVARIANTEXTPROC       glSetInvariantEXT              = NULL;
PFNGLSETLOCALCONSTANTEXTPROC   glSetLocalConstantEXT          = NULL;
PFNGLVARIANTBVEXTPROC          glVariantbvEXT                 = NULL;
PFNGLVARIANTSVEXTPROC          glVariantsvEXT                 = NULL;
PFNGLVARIANTIVEXTPROC          glVariantivEXT                 = NULL;
PFNGLVARIANTFVEXTPROC          glVariantfvEXT                 = NULL;
PFNGLVARIANTDVEXTPROC          glVariantdvEXT                 = NULL;
PFNGLVARIANTUBVEXTPROC         glVariantubvEXT                = NULL;
PFNGLVARIANTUSVEXTPROC         glVariantusvEXT                = NULL;
PFNGLVARIANTUIVEXTPROC         glVariantuivEXT                = NULL;
PFNGLVARIANTPOINTEREXTPROC     glVariantPointerEXT            = NULL;
PFNGLENABLEVARIANTCLIENTSTATEEXTPROC  glEnableVariantClientStateEXT  = NULL;
PFNGLDISABLEVARIANTCLIENTSTATEEXTPROC glDisableVariantClientStateEXT = NULL;

PFNGLBINDLIGHTPARAMETEREXTPROC       glBindLightParameterEXT        = NULL;
PFNGLBINDMATERIALPARAMETEREXTPROC    glBindMaterialParameterEXT     = NULL;
PFNGLBINDTEXGENPARAMETEREXTPROC      glBindTexGenParameterEXT       = NULL;
PFNGLBINDTEXTUREUNITPARAMETEREXTPROC glBindTextureUnitParameterEXT  = NULL;
PFNGLBINDPARAMETEREXTPROC            glBindParameterEXT             = NULL;
PFNGLISVARIANTENABLEDEXTPROC         glIsVariantEnabledEXT          = NULL;
PFNGLGETVARIANTBOOLEANVEXTPROC       glGetVariantBooleanvEXT        = NULL;
PFNGLGETVARIANTINTEGERVEXTPROC       glGetVariantIntegervEXT        = NULL;
PFNGLGETVARIANTFLOATVEXTPROC         glGetVariantFloatvEXT          = NULL;
PFNGLGETVARIANTPOINTERVEXTPROC       glGetVariantPointervEXT        = NULL;
PFNGLGETINVARIANTBOOLEANVEXTPROC     glGetInvariantBooleanvEXT      = NULL;
PFNGLGETINVARIANTINTEGERVEXTPROC     glGetInvariantIntegervEXT      = NULL;
PFNGLGETINVARIANTFLOATVEXTPROC       glGetInvariantFloatvEXT        = NULL;
PFNGLGETLOCALCONSTANTBOOLEANVEXTPROC glGetLocalConstantBooleanvEXT  = NULL;
PFNGLGETLOCALCONSTANTINTEGERVEXTPROC glGetLocalConstantIntegervEXT  = NULL;
PFNGLGETLOCALCONSTANTFLOATVEXTPROC   glGetLocalConstantFloatvEXT    = NULL;


// GL_GL2_shader_objects
PFNGLDELETEOBJECTGL2PROC  glDeleteObjectGL2 = NULL;
PFNGLDELETEOBJECTSGL2PROC glDeleteObjectsGL2 = NULL;
PFNGLGETHANDLEGL2PROC     glGetHandleGL2 = NULL;
PFNGLDETACHOBJECTGL2PROC  glDetachObjectGL2 = NULL;

PFNGLOBJECTPARAMETERFGL2PROC     glObjectParameterfGL2 = NULL;
PFNGLOBJECTPARAMETERIGL2PROC     glObjectParameteriGL2 = NULL;
PFNGLGETOBJECTPARAMETERFVGL2PROC glGetObjectParameterfvGL2 = NULL;
PFNGLGETOBJECTPARAMETERIVGL2PROC glGetObjectParameterivGL2 = NULL;

PFNGLCREATESHADEROBJECTGL2PROC  glCreateShaderObjectGL2 = NULL;
PFNGLLOADSHADERGL2PROC          glLoadShaderGL2 = NULL;
PFNGLAPPENDSHADERGL2PROC        glAppendShaderGL2 = NULL;
PFNGLCOMPILESHADERGL2PROC       glCompileShaderGL2 = NULL;
PFNGLGETINFOLOGGL2PROC          glGetInfoLogGL2 = NULL;
PFNGLGETSHADERSTRINGGL2PROC     glGetShaderStringGL2 = NULL;
PFNGLCREATEPROGRAMOBJECTGL2PROC glCreateProgramObjectGL2 = NULL;
PFNGLATTACHSHADEROBJECTGL2PROC  glAttachShaderObjectGL2 = NULL;
PFNGLLINKPROGRAMGL2PROC         glLinkProgramGL2 = NULL;
PFNGLUSEPROGRAMOBJECTGL2PROC    glUseProgramObjectGL2 = NULL;

PFNGLLOADUNIFORM1FGL2PROC   glLoadUniform1fGL2 = NULL;
PFNGLLOADUNIFORM2FGL2PROC   glLoadUniform2fGL2 = NULL;
PFNGLLOADUNIFORM3FGL2PROC   glLoadUniform3fGL2 = NULL;
PFNGLLOADUNIFORM4FGL2PROC   glLoadUniform4fGL2 = NULL;
PFNGLLOADUNIFORM1FVGL2PROC  glLoadUniform1fvGL2 = NULL;
PFNGLLOADUNIFORM2FVGL2PROC  glLoadUniform2fvGL2 = NULL;
PFNGLLOADUNIFORM3FVGL2PROC  glLoadUniform3fvGL2 = NULL;
PFNGLLOADUNIFORM4FVGL2PROC  glLoadUniform4fvGL2 = NULL;
PFNGLLOADUNIFORMINTGL2PROC  glLoadUniformIntGL2 = NULL;
PFNGLLOADUNIFORMBOOLGL2PROC glLoadUniformBoolGL2 = NULL;

PFNGLLOADUNIFORMARRAY1FVGL2PROC  glLoadUniformArray1fvGL2 = NULL;
PFNGLLOADUNIFORMARRAY2FVGL2PROC  glLoadUniformArray2fvGL2 = NULL;
PFNGLLOADUNIFORMARRAY3FVGL2PROC  glLoadUniformArray3fvGL2 = NULL;
PFNGLLOADUNIFORMARRAY4FVGL2PROC  glLoadUniformArray4fvGL2 = NULL;
PFNGLLOADUNIFORMMATRIX2FVGL2PROC glLoadUniformMatrix2fvGL2 = NULL;
PFNGLLOADUNIFORMMATRIX3FVGL2PROC glLoadUniformMatrix3fvGL2 = NULL;
PFNGLLOADUNIFORMMATRIX4FVGL2PROC glLoadUniformMatrix4fvGL2 = NULL;
PFNGLLOADUNIFORMMATRIXARRAY2FVGL2PROC glLoadUniformMatrixArray2fvGL2 = NULL;
PFNGLLOADUNIFORMMATRIXARRAY3FVGL2PROC glLoadUniformMatrixArray3fvGL2 = NULL;
PFNGLLOADUNIFORMMATRIXARRAY4FVGL2PROC glLoadUniformMatrixArray4fvGL2 = NULL;

PFNGLGETUNIFORMLOCATIONGL2PROC glGetUniformLocationGL2 = NULL;
PFNGLGETUNIFORMFVGL2PROC glGetUniformfvGL2 = NULL;
PFNGLGETUNIFORMIVGL2PROC glGetUniformivGL2 = NULL;

// GL_GL2_vertex_shader
PFNGLBINDATTRIBUTELOCATIONGL2PROC glBindAttributeLocationGL2 = NULL;
PFNGLGETACTIVEATTRIBUTESGL2PROC   glGetActiveAttributesGL2   = NULL;


// GL_NV_occlusion_query
PFNGLGENOCCLUSIONQUERIESNVPROC    glGenOcclusionQueriesNV    = NULL;
PFNGLDELETEOCCLUSIONQUERIESNVPROC glDeleteOcclusionQueriesNV = NULL;
PFNGLISOCCLUSIONQUERYNVPROC       glIsOcclusionQueryNV       = NULL;
PFNGLBEGINOCCLUSIONQUERYNVPROC    glBeginOcclusionQueryNV    = NULL;
PFNGLENDOCCLUSIONQUERYNVPROC      glEndOcclusionQueryNV      = NULL;
PFNGLGETOCCLUSIONQUERYIVNVPROC    glGetOcclusionQueryivNV    = NULL;
PFNGLGETOCCLUSIONQUERYUIVNVPROC   glGetOcclusionQueryuivNV   = NULL;


// GL_NV_register_combiners
PFNGLCOMBINERPARAMETERFVNVPROC glCombinerParameterfvNV = NULL;
PFNGLCOMBINERPARAMETERFNVPROC  glCombinerParameterfNV  = NULL;
PFNGLCOMBINERPARAMETERIVNVPROC glCombinerParameterivNV = NULL;
PFNGLCOMBINERPARAMETERINVPROC  glCombinerParameteriNV  = NULL;

PFNGLCOMBINERINPUTNVPROC       glCombinerInputNV       = NULL;
PFNGLCOMBINEROUTPUTNVPROC      glCombinerOutputNV      = NULL;
PFNGLFINALCOMBINERINPUTNVPROC  glFinalCombinerInputNV  = NULL;

PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC  glGetCombinerInputParameterfvNV = NULL;
PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC  glGetCombinerInputParameterivNV = NULL;
PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC glGetCombinerOutputParameterfvNV = NULL;
PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC glGetCombinerOutputParameterivNV = NULL;

PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC glGetFinalCombinerInputParameterfvNV = NULL;
PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC glGetFinalCombinerInputParameterivNV = NULL;


// GL_NV_texture_env_combine4
PFNGLFLUSHVERTEXARRAYRANGENVPROC glFlushVertexArrayRangeNV = NULL;
PFNGLVERTEXARRAYRANGENVPROC      glVertexArrayRangeNV      = NULL;
PFNWGLALLOCATEMEMORYNVPROC       wglAllocateMemoryNV       = NULL;
PFNWGLFREEMEMORYNVPROC           wglFreeMemoryNV           = NULL;



#ifdef WIN32
// WGL_ARB_extensions_string
WGL_ARB_EXTENSIONS_STRING wglGetExtensionsStringARB = NULL;


// WGL_ARB_make_current_read
PFNWGLMAKECONTEXTCURRENTARBPROC wglMakeContextCurrentARB = NULL;
PFNWGLGETCURRENTREADDCARBPROC wglGetCurrentReadDCARB = NULL;


// WGL_ARB_pbuffer
PFNWGLCREATEPBUFFERARBPROC    wglCreatePbufferARB    = NULL;
PFNWGLGETPBUFFERDCARBPROC     wglGetPbufferDCARB     = NULL;
PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB = NULL;
PFNWGLDESTROYPBUFFERARBPROC   wglDestroyPbufferARB   = NULL;
PFNWGLQUERYPBUFFERARBPROC     wglQueryPbufferARB     = NULL;


// WGL_ARB_pixel_format
PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB = NULL;
PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB = NULL;
PFNWGLCHOOSEPIXELFORMATARBPROC      wglChoosePixelFormatARB      = NULL;


// WGL_ARB_render_texture
PFNWGLBINDTEXIMAGEARBPROC     wglBindTexImageARB     = NULL;
PFNWGLRELEASETEXIMAGEARBPROC  wglReleaseTexImageARB  = NULL;
PFNWGLSETPBUFFERATTRIBARBPROC wglSetPbufferAttribARB = NULL;


// WGL_EXT_swap_control
PFNWGLEXTSWAPINTERVALPROC wglSwapIntervalEXT = NULL;

#endif // WIN32




/* ------------------------------------------------------------------ */

bool findExtension(char *extensions, char *extension){
	if (extensions){
		char *str = extensions;
		int len = strlen(extension);
		
		while ((str = strstr(str, extension)) != NULL){
			str += len;
			if (*str == ' ' || *str == '\0') return true;
		}
	}
	return false;
}

#if defined(WIN32)

void initExtensions(HDC hdc){
	char *wglxExtensions = NULL;

	if (((wglGetExtensionsStringARB = (WGL_ARB_EXTENSIONS_STRING) wglxGetProcAddress("wglGetExtensionsStringARB")) != NULL) ||
		((wglGetExtensionsStringARB = (WGL_ARB_EXTENSIONS_STRING) wglxGetProcAddress("wglGetExtensionsStringEXT")) != NULL)){
		WGL_ARB_extensions_string = true;
		wglxExtensions = wglGetExtensionsStringARB(hdc);
	}

#elif defined(LINUX)

void initExtensions(Display *display, int screen){
	//char *wglxExtensions = (char *) glXQueryExtensionsString(display, screen);

#endif

	char *extensions = (char *) glGetString(GL_EXTENSIONS);

	GL_ARB_depth_texture    = isExtensionSupported("GL_ARB_depth_texture");
	GL_ARB_fragment_program = isExtensionSupported("GL_ARB_fragment_program");

	if (GL_ARB_multisample = isExtensionSupported("GL_ARB_multisample")){
		glSampleCoverageARB = (PFNGLSAMPLECOVERAGEARBPROC) wglxGetProcAddress("glSampleCoverageARB");
	}

	if (GL_ARB_multitexture = isExtensionSupported("GL_ARB_multitexture")){
		glActiveTexture       = (PFNGLACTIVETEXTUREARBPROC)       wglxGetProcAddress("glActiveTextureARB");
		glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREARBPROC) wglxGetProcAddress("glClientActiveTextureARB");

		glMultiTexCoord1f  = (PFNGLMULTITEXCOORD1FARBPROC)  wglxGetProcAddress("glMultiTexCoord1fARB");
		glMultiTexCoord1fv = (PFNGLMULTITEXCOORD1FVARBPROC) wglxGetProcAddress("glMultiTexCoord1fvARB");
		glMultiTexCoord2f  = (PFNGLMULTITEXCOORD2FARBPROC)  wglxGetProcAddress("glMultiTexCoord2fARB");
		glMultiTexCoord2fv = (PFNGLMULTITEXCOORD2FVARBPROC) wglxGetProcAddress("glMultiTexCoord2fvARB");
		glMultiTexCoord3f  = (PFNGLMULTITEXCOORD3FARBPROC)  wglxGetProcAddress("glMultiTexCoord3fARB");
		glMultiTexCoord3fv = (PFNGLMULTITEXCOORD3FVARBPROC) wglxGetProcAddress("glMultiTexCoord3fvARB");
		glMultiTexCoord4f  = (PFNGLMULTITEXCOORD4FARBPROC)  wglxGetProcAddress("glMultiTexCoord4fARB");
		glMultiTexCoord4fv = (PFNGLMULTITEXCOORD4FVARBPROC) wglxGetProcAddress("glMultiTexCoord4fvARB");
	}

	GL_ARB_shadow         = isExtensionSupported("GL_ARB_shadow");
	GL_ARB_shadow_ambient = isExtensionSupported("GL_ARB_shadow_ambient");

	if (GL_ARB_texture_compression = isExtensionSupported("GL_ARB_texture_compression")){
		glCompressedTexImage1DARB     = (PFNGLCOMPRESSEDTEXIMAGE1DARBPROC)    wglxGetProcAddress("glCompressedTexImage1DARB");
		glCompressedTexImage2DARB     = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)    wglxGetProcAddress("glCompressedTexImage2DARB");
		glCompressedTexImage3DARB     = (PFNGLCOMPRESSEDTEXIMAGE3DARBPROC)    wglxGetProcAddress("glCompressedTexImage3DARB");
		glCompressedTexSubImage1DARB  = (PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC) wglxGetProcAddress("glCompressedTexSubImage1DARB");
		glCompressedTexSubImage2DARB  = (PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC) wglxGetProcAddress("glCompressedTexSubImage2DARB");
		glCompressedTexSubImage3DARB  = (PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC) wglxGetProcAddress("glCompressedTexSubImage3DARB");
		glGetCompressedTexImageARB    = (PFNGLGETCOMPRESSEDTEXIMAGEARBPROC)   wglxGetProcAddress("glGetCompressedTexImageARB");

		// Workaround for a driver bug in current ATi Linux drivers
		if (glCompressedTexImage2DARB == NULL) GL_ARB_texture_compression = false;
	}

	GL_ARB_texture_cube_map		= isExtensionSupported("GL_ARB_texture_cube_map");
    GL_ARB_texture_env_add      = isExtensionSupported("GL_ARB_texture_env_add");
	GL_ARB_texture_env_combine  = isExtensionSupported("GL_ARB_texture_env_combine");
	GL_ARB_texture_env_crossbar	= isExtensionSupported("GL_ARB_texture_env_crossbar");
	GL_ARB_texture_env_dot3		= isExtensionSupported("GL_ARB_texture_env_dot3");

	if (GL_ARB_transpose_matrix = isExtensionSupported("GL_ARB_transpose_matrix")){
		glLoadTransposeMatrixfARB = (PFNGLLOADTRANSPOSEMATRIXFARBPROC) wglxGetProcAddress("glLoadTransposeMatrixfARB");
		glLoadTransposeMatrixdARB = (PFNGLLOADTRANSPOSEMATRIXDARBPROC) wglxGetProcAddress("glLoadTransposeMatrixdARB");
		glMultTransposeMatrixfARB = (PFNGLMULTTRANSPOSEMATRIXFARBPROC) wglxGetProcAddress("glMultTransposeMatrixfARB");
		glMultTransposeMatrixdARB = (PFNGLMULTTRANSPOSEMATRIXDARBPROC) wglxGetProcAddress("glMultTransposeMatrixdARB");
	}

	if (GL_ARB_vertex_blend = isExtensionSupported("GL_ARB_vertex_blend")){
		glWeightbvARB  = (PFNGLWEIGHTBVARBPROC)  wglxGetProcAddress("glWeightbvARB");
		glWeightsvARB  = (PFNGLWEIGHTSVARBPROC)  wglxGetProcAddress("glWeightsvARB");
		glWeightivARB  = (PFNGLWEIGHTIVARBPROC)  wglxGetProcAddress("glWeightivARB");
		glWeightfvARB  = (PFNGLWEIGHTFVARBPROC)  wglxGetProcAddress("glWeightfvARB");
		glWeightdvARB  = (PFNGLWEIGHTDVARBPROC)  wglxGetProcAddress("glWeightdvARB");
		glWeightubvARB = (PFNGLWEIGHTUBVARBPROC) wglxGetProcAddress("glWeightubvARB");
		glWeightusvARB = (PFNGLWEIGHTUSVARBPROC) wglxGetProcAddress("glWeightusvARB");
		glWeightuivARB = (PFNGLWEIGHTUIVARBPROC) wglxGetProcAddress("glWeightuivARB");
		
		glWeightPointerARB = (PFNGLWEIGHTPOINTERARBPROC) wglxGetProcAddress("glWeightPointerARB");
		glVertexBlendARB   = (PFNGLVERTEXBLENDARBPROC)   wglxGetProcAddress("glVertexBlendARB");
	}

	if (GL_ARB_vertex_program = isExtensionSupported("GL_ARB_vertex_program")){
		glVertexAttrib1sARB = (PFNGLVERTEXATTRIB1SARBPROC) wglxGetProcAddress("glVertexAttrib1sARB");
		glVertexAttrib1fARB = (PFNGLVERTEXATTRIB1FARBPROC) wglxGetProcAddress("glVertexAttrib1fARB");
		glVertexAttrib1dARB = (PFNGLVERTEXATTRIB1DARBPROC) wglxGetProcAddress("glVertexAttrib1dARB");
		glVertexAttrib2sARB = (PFNGLVERTEXATTRIB2SARBPROC) wglxGetProcAddress("glVertexAttrib2sARB");
		glVertexAttrib2fARB = (PFNGLVERTEXATTRIB2FARBPROC) wglxGetProcAddress("glVertexAttrib2fARB");
		glVertexAttrib2dARB = (PFNGLVERTEXATTRIB2DARBPROC) wglxGetProcAddress("glVertexAttrib2dARB");
		glVertexAttrib3sARB = (PFNGLVERTEXATTRIB3SARBPROC) wglxGetProcAddress("glVertexAttrib3sARB");
		glVertexAttrib3fARB = (PFNGLVERTEXATTRIB3FARBPROC) wglxGetProcAddress("glVertexAttrib3fARB");
		glVertexAttrib3dARB = (PFNGLVERTEXATTRIB3DARBPROC) wglxGetProcAddress("glVertexAttrib3dARB");
		glVertexAttrib4sARB = (PFNGLVERTEXATTRIB4SARBPROC) wglxGetProcAddress("glVertexAttrib4sARB");
		glVertexAttrib4fARB = (PFNGLVERTEXATTRIB4FARBPROC) wglxGetProcAddress("glVertexAttrib4fARB");
		glVertexAttrib4dARB = (PFNGLVERTEXATTRIB4DARBPROC) wglxGetProcAddress("glVertexAttrib4dARB");
		glVertexAttrib4NubARB = (PFNGLVERTEXATTRIB4NUBARBPROC) wglxGetProcAddress("glVertexAttrib4NubARB");
		glVertexAttrib1svARB  = (PFNGLVERTEXATTRIB1SVARBPROC)  wglxGetProcAddress("glVertexAttrib1svARB");
		glVertexAttrib1fvARB  = (PFNGLVERTEXATTRIB1FVARBPROC)  wglxGetProcAddress("glVertexAttrib1fvARB");
		glVertexAttrib1dvARB  = (PFNGLVERTEXATTRIB1DVARBPROC)  wglxGetProcAddress("glVertexAttrib1dvARB");
		glVertexAttrib2svARB  = (PFNGLVERTEXATTRIB2SVARBPROC)  wglxGetProcAddress("glVertexAttrib2svARB");
		glVertexAttrib2fvARB  = (PFNGLVERTEXATTRIB2FVARBPROC)  wglxGetProcAddress("glVertexAttrib2fvARB");
		glVertexAttrib2dvARB  = (PFNGLVERTEXATTRIB2DVARBPROC)  wglxGetProcAddress("glVertexAttrib2dvARB");
		glVertexAttrib3svARB  = (PFNGLVERTEXATTRIB3SVARBPROC)  wglxGetProcAddress("glVertexAttrib3svARB");
		glVertexAttrib3fvARB  = (PFNGLVERTEXATTRIB3FVARBPROC)  wglxGetProcAddress("glVertexAttrib3fvARB");
		glVertexAttrib3dvARB  = (PFNGLVERTEXATTRIB3DVARBPROC)  wglxGetProcAddress("glVertexAttrib3dvARB");
		glVertexAttrib4bvARB  = (PFNGLVERTEXATTRIB4BVARBPROC)  wglxGetProcAddress("glVertexAttrib4bvARB");
		glVertexAttrib4svARB  = (PFNGLVERTEXATTRIB4SVARBPROC)  wglxGetProcAddress("glVertexAttrib4svARB");
		glVertexAttrib4ivARB  = (PFNGLVERTEXATTRIB4IVARBPROC)  wglxGetProcAddress("glVertexAttrib4ivARB");
		glVertexAttrib4ubvARB = (PFNGLVERTEXATTRIB4UBVARBPROC) wglxGetProcAddress("glVertexAttrib4ubvARB");
		glVertexAttrib4usvARB = (PFNGLVERTEXATTRIB4USVARBPROC) wglxGetProcAddress("glVertexAttrib4usvARB");
		glVertexAttrib4uivARB = (PFNGLVERTEXATTRIB4UIVARBPROC) wglxGetProcAddress("glVertexAttrib4uivARB");
		glVertexAttrib4fvARB  = (PFNGLVERTEXATTRIB4FVARBPROC)  wglxGetProcAddress("glVertexAttrib4fvARB");
		glVertexAttrib4dvARB  = (PFNGLVERTEXATTRIB4DVARBPROC)  wglxGetProcAddress("glVertexAttrib4dvARB");
		glVertexAttrib4NbvARB = (PFNGLVERTEXATTRIB4NBVARBPROC) wglxGetProcAddress("glVertexAttrib4NbvARB");
		glVertexAttrib4NsvARB = (PFNGLVERTEXATTRIB4NSVARBPROC) wglxGetProcAddress("glVertexAttrib4NsvARB");
		glVertexAttrib4NivARB = (PFNGLVERTEXATTRIB4NIVARBPROC) wglxGetProcAddress("glVertexAttrib4NivARB");
		glVertexAttrib4NubvARB = (PFNGLVERTEXATTRIB4NUBVARBPROC) wglxGetProcAddress("glVertexAttrib4NubvARB");
		glVertexAttrib4NusvARB = (PFNGLVERTEXATTRIB4NUSVARBPROC) wglxGetProcAddress("glVertexAttrib4NusvARB");
		glVertexAttrib4NuivARB = (PFNGLVERTEXATTRIB4NUIVARBPROC) wglxGetProcAddress("glVertexAttrib4NuivARB");

		glVertexAttribPointerARB = (PFNGLVERTEXATTRIBPOINTERARBPROC) wglxGetProcAddress("glVertexAttribPointerARB");
		glEnableVertexAttribArrayARB = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC) wglxGetProcAddress("glEnableVertexAttribArrayARB");
		glDisableVertexAttribArrayARB = (PFNGLDISABLEVERTEXATTRIBARRAYARBPROC) wglxGetProcAddress("glDisableVertexAttribArrayARB");

		glGetVertexAttribdvARB = (PFNGLGETVERTEXATTRIBDVARBPROC) wglxGetProcAddress("glGetVertexAttribdvARB");
		glGetVertexAttribfvARB = (PFNGLGETVERTEXATTRIBFVARBPROC) wglxGetProcAddress("glGetVertexAttribfvARB");
		glGetVertexAttribivARB = (PFNGLGETVERTEXATTRIBIVARBPROC) wglxGetProcAddress("glGetVertexAttribivARB");
		glGetVertexAttribPointervARB = (PFNGLGETVERTEXATTRIBPOINTERVARBPROC) wglxGetProcAddress("glGetVertexAttribPointervARB");
	}

	if (GL_ARB_vertex_program || GL_ARB_fragment_program){
		glProgramStringARB  = (PFNGLPROGRAMSTRINGARBPROC)  wglxGetProcAddress("glProgramStringARB");
		glBindProgramARB    = (PFNGLBINDPROGRAMARBPROC)    wglxGetProcAddress("glBindProgramARB");
		glDeleteProgramsARB = (PFNGLDELETEPROGRAMSARBPROC) wglxGetProcAddress("glDeleteProgramsARB");
		glGenProgramsARB    = (PFNGLGENPROGRAMSARBPROC)    wglxGetProcAddress("glGenProgramsARB");
		glProgramEnvParameter4dARB    = (PFNGLPROGRAMENVPARAMETER4DARBPROC)    wglxGetProcAddress("glProgramEnvParameter4dARB");
		glProgramEnvParameter4dvARB   = (PFNGLPROGRAMENVPARAMETER4DVARBPROC)   wglxGetProcAddress("glProgramEnvParameter4dvARB");
		glProgramEnvParameter4fARB    = (PFNGLPROGRAMENVPARAMETER4FARBPROC)    wglxGetProcAddress("glProgramEnvParameter4fARB");
		glProgramEnvParameter4fvARB   = (PFNGLPROGRAMENVPARAMETER4FVARBPROC)   wglxGetProcAddress("glProgramEnvParameter4fvARB");
		glProgramLocalParameter4dARB  = (PFNGLPROGRAMLOCALPARAMETER4DARBPROC)  wglxGetProcAddress("glProgramLocalParameter4dARB");
		glProgramLocalParameter4dvARB = (PFNGLPROGRAMLOCALPARAMETER4DVARBPROC) wglxGetProcAddress("glProgramLocalParameter4dvARB");
		glProgramLocalParameter4fARB  = (PFNGLPROGRAMLOCALPARAMETER4FARBPROC)  wglxGetProcAddress("glProgramLocalParameter4fARB");
		glProgramLocalParameter4fvARB = (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC) wglxGetProcAddress("glProgramLocalParameter4fvARB");
		glGetProgramEnvParameterdvARB = (PFNGLGETPROGRAMENVPARAMETERDVARBPROC) wglxGetProcAddress("glGetProgramEnvParameterdvARB");
		glGetProgramEnvParameterfvARB = (PFNGLGETPROGRAMENVPARAMETERFVARBPROC) wglxGetProcAddress("glGetProgramEnvParameterfvARB");
		glGetProgramLocalParameterdvARB = (PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC) wglxGetProcAddress("glGetProgramLocalParameterdvARB");
		glGetProgramLocalParameterfvARB = (PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC) wglxGetProcAddress("glGetProgramLocalParameterfvARB");
		glGetProgramivARB     = (PFNGLGETPROGRAMIVARBPROC)     wglxGetProcAddress("glGetProgramivARB");
		glGetProgramStringARB = (PFNGLGETPROGRAMSTRINGARBPROC) wglxGetProcAddress("glGetProgramStringARB");
		glIsProgramARB        = (PFNGLISPROGRAMARBPROC)        wglxGetProcAddress("glIsProgramARB");
	}





	if (GL_ATI_element_array = isExtensionSupported("GL_ATI_element_array")){
		glElementPointerATI        = (PFNGLELEMENTPOINTERATIPROC)        wglxGetProcAddress("glElementPointerATI");
		glDrawElementArrayATI      = (PFNGLDRAWELEMENTARRAYATIPROC)      wglxGetProcAddress("glDrawElementArrayATI");
		glDrawRangeElementArrayATI = (PFNGLDRAWRANGEELEMENTARRAYATIPROC) wglxGetProcAddress("glDrawRangeElementArrayATI");
	}

	if (GL_ATI_envmap_bumpmap = isExtensionSupported("GL_ATI_envmap_bumpmap")){
		glTexBumpParameterivATI = (PFNGLTEXBUMPPARAMETERIVATIPROC) wglxGetProcAddress("glTexBumpParameterivATI");
		glTexBumpParameterfvATI = (PFNGLTEXBUMPPARAMETERFVATIPROC) wglxGetProcAddress("glTexBumpParameterfvATI");
		glGetTexBumpParameterivATI = (PFNGLGETTEXBUMPPARAMETERIVATIPROC) wglxGetProcAddress("glGetTexBumpParameterivATI");
		glGetTexBumpParameterfvATI = (PFNGLGETTEXBUMPPARAMETERFVATIPROC) wglxGetProcAddress("glGetTexBumpParameterfvATI");
	}

	if (GL_ATI_fragment_shader = isExtensionSupported("GL_ATI_fragment_shader")){
		glGenFragmentShadersATI   = (PFNGLGENFRAGMENTSHADERSATIPROC)   wglxGetProcAddress("glGenFragmentShadersATI");
		glBindFragmentShaderATI   = (PFNGLBINDFRAGMENTSHADERATIPROC)   wglxGetProcAddress("glBindFragmentShaderATI");
		glDeleteFragmentShaderATI = (PFNGLDELETEFRAGMENTSHADERATIPROC) wglxGetProcAddress("glDeleteFragmentShaderATI");
		glBeginFragmentShaderATI  = (PFNGLBEGINFRAGMENTSHADERATIPROC)  wglxGetProcAddress("glBeginFragmentShaderATI");
		glEndFragmentShaderATI    = (PFNGLENDFRAGMENTSHADERATIPROC)    wglxGetProcAddress("glEndFragmentShaderATI");
		glPassTexCoordATI         = (PFNGLPASSTEXCOORDATIPROC)         wglxGetProcAddress("glPassTexCoordATI");
		glSampleMapATI            = (PFNGLSAMPLEMAPATIPROC)            wglxGetProcAddress("glSampleMapATI");
		
		glColorFragmentOp1ATI = (PFNGLCOLORFRAGMENTOP1ATIPROC) wglxGetProcAddress("glColorFragmentOp1ATI");
		glColorFragmentOp2ATI = (PFNGLCOLORFRAGMENTOP2ATIPROC) wglxGetProcAddress("glColorFragmentOp2ATI");
		glColorFragmentOp3ATI = (PFNGLCOLORFRAGMENTOP3ATIPROC) wglxGetProcAddress("glColorFragmentOp3ATI");
		
		glAlphaFragmentOp1ATI = (PFNGLALPHAFRAGMENTOP1ATIPROC) wglxGetProcAddress("glAlphaFragmentOp1ATI");
		glAlphaFragmentOp2ATI = (PFNGLALPHAFRAGMENTOP2ATIPROC) wglxGetProcAddress("glAlphaFragmentOp2ATI");
		glAlphaFragmentOp3ATI = (PFNGLALPHAFRAGMENTOP3ATIPROC) wglxGetProcAddress("glAlphaFragmentOp3ATI");
		
		glSetFragmentShaderConstantATI = (PFNGLSETFRAGMENTSHADERCONSTANTATIPROC) wglxGetProcAddress("glSetFragmentShaderConstantATI");
	}

	if (GL_ATI_map_object_buffer = isExtensionSupported("GL_ATI_map_object_buffer")){
		glMapObjectBufferATI   = (PFNGLMAPOBJECTBUFFERATIPROC)   wglxGetProcAddress("glMapObjectBufferATI");
		glUnmapObjectBufferATI = (PFNGLUNMAPOBJECTBUFFERATIPROC) wglxGetProcAddress("glUnmapObjectBufferATI");
	}



	if (GL_ATI_pn_triangles = isExtensionSupported("GL_ATI_pn_triangles")){
		glPNTrianglesiATI = (PFNGLPNTRIANGLESIATIPROC) wglxGetProcAddress("glPNTrianglesiATI");
		glPNTrianglesfATI = (PFNGLPNTRIANGLESFATIPROC) wglxGetProcAddress("glPNTrianglesfATI");
	}

	GL_ATI_texture_float       = isExtensionSupported("GL_ATI_texture_float");
	GL_ATI_texture_mirror_once = isExtensionSupported("GL_ATI_texture_mirror_once");

	if (GL_ATI_vertex_array_object = isExtensionSupported("GL_ATI_vertex_array_object")){
		glNewObjectBufferATI    = (PFNGLNEWOBJECTBUFFERATIPROC)    wglxGetProcAddress("glNewObjectBufferATI");
		glIsObjectBufferATI     = (PFNGLISOBJECTBUFFERATIPROC)     wglxGetProcAddress("glIsObjectBufferATI");
		glUpdateObjectBufferATI = (PFNGLUPDATEOBJECTBUFFERATIPROC) wglxGetProcAddress("glUpdateObjectBufferATI");
		glGetObjectBufferfvATI  = (PFNGLGETOBJECTBUFFERFVATIPROC)  wglxGetProcAddress("glGetObjectBufferfvATI");
		glGetObjectBufferivATI  = (PFNGLGETOBJECTBUFFERIVATIPROC)  wglxGetProcAddress("glGetObjectBufferivATI");
		glDeleteObjectBufferATI = (PFNGLFREEOBJECTBUFFERATIPROC)   wglxGetProcAddress("glDeleteObjectBufferATI");
		
		glArrayObjectATI        = (PFNGLARRAYOBJECTATIPROC)        wglxGetProcAddress("glArrayObjectATI");
		glGetArrayObjectfvATI   = (PFNGLGETARRAYOBJECTFVATIPROC)   wglxGetProcAddress("glGetArrayObjectfvATI");
		glGetArrayObjectivATI   = (PFNGLGETARRAYOBJECTIVATIPROC)   wglxGetProcAddress("glGetArrayObjectivATI");
		glVariantArrayObjectATI = (PFNGLVARIANTARRAYOBJECTATIPROC) wglxGetProcAddress("glVariantArrayObjectATI");
		
		glGetVariantArrayObjectfvATI = (PFNGLGETVARIANTARRAYOBJECTFVATIPROC) wglxGetProcAddress("glGetVariantArrayObjectfvATI");
		glGetVariantArrayObjectivATI = (PFNGLGETVARIANTARRAYOBJECTIVATIPROC) wglxGetProcAddress("glGetVariantArrayObjectivATI");
	}

	if (GL_ATI_vertex_attrib_array_object = isExtensionSupported("GL_ATI_vertex_attrib_array_object")){
		/* TODO: Fix this */
		glVertexAttribArrayObjectATI = (PFNGLVERTEXATTRIBARRAYOBJECTATIPROC) wglxGetProcAddress("glVertexAttribArrayObjectATI");
		//glGetVertexAttribArrayObjectivATI
		//glGetVertexAttribArrayObjectfvATI
		//glVertexAttribArrayObjectATI
	}

	if (GL_ATI_vertex_streams = isExtensionSupported("GL_ATI_vertex_streams")){
		glClientActiveVertexStreamATI = (PFNGLCLIENTACTIVEVERTEXSTREAMATIPROC) wglxGetProcAddress("glClientActiveVertexStreamATI");
		glVertexBlendEnviATI = (PFNGLVERTEXBLENDENVIATIPROC) wglxGetProcAddress("glVertexBlendEnviATI");
		glVertexBlendEnvfATI = (PFNGLVERTEXBLENDENVFATIPROC) wglxGetProcAddress("glVertexBlendEnvfATI");
		
		glVertexStream2sATI  = (PFNGLVERTEXSTREAM2SATIPROC)  wglxGetProcAddress("glVertexStream2sATI");
		glVertexStream2svATI = (PFNGLVERTEXSTREAM2SVATIPROC) wglxGetProcAddress("glVertexStream2svATI");
		glVertexStream2iATI  = (PFNGLVERTEXSTREAM2IATIPROC)  wglxGetProcAddress("glVertexStream2iATI");
		glVertexStream2ivATI = (PFNGLVERTEXSTREAM2IVATIPROC) wglxGetProcAddress("glVertexStream2ivATI");
		glVertexStream2fATI  = (PFNGLVERTEXSTREAM2FATIPROC)  wglxGetProcAddress("glVertexStream2fATI");
		glVertexStream2fvATI = (PFNGLVERTEXSTREAM2FVATIPROC) wglxGetProcAddress("glVertexStream2fvATI");
		glVertexStream2dATI  = (PFNGLVERTEXSTREAM2DATIPROC)  wglxGetProcAddress("glVertexStream2dATI");
		glVertexStream2dvATI = (PFNGLVERTEXSTREAM2DVATIPROC) wglxGetProcAddress("glVertexStream2dvATI");
		
		glVertexStream3sATI  = (PFNGLVERTEXSTREAM3SATIPROC)  wglxGetProcAddress("glVertexStream3sATI");
		glVertexStream3svATI = (PFNGLVERTEXSTREAM3SVATIPROC) wglxGetProcAddress("glVertexStream3svATI");
		glVertexStream3iATI  = (PFNGLVERTEXSTREAM3IATIPROC)  wglxGetProcAddress("glVertexStream3iATI");
		glVertexStream3ivATI = (PFNGLVERTEXSTREAM3IVATIPROC) wglxGetProcAddress("glVertexStream3ivATI");
		glVertexStream3fATI  = (PFNGLVERTEXSTREAM3FATIPROC)  wglxGetProcAddress("glVertexStream3fATI");
		glVertexStream3fvATI = (PFNGLVERTEXSTREAM3FVATIPROC) wglxGetProcAddress("glVertexStream3fvATI");
		glVertexStream3dATI  = (PFNGLVERTEXSTREAM3DATIPROC)  wglxGetProcAddress("glVertexStream3dATI");
		glVertexStream3dvATI = (PFNGLVERTEXSTREAM3DVATIPROC) wglxGetProcAddress("glVertexStream3dvATI");
		
		glVertexStream4sATI  = (PFNGLVERTEXSTREAM4SATIPROC)  wglxGetProcAddress("glVertexStream4sATI");
		glVertexStream4svATI = (PFNGLVERTEXSTREAM4SVATIPROC) wglxGetProcAddress("glVertexStream4svATI");
		glVertexStream4iATI  = (PFNGLVERTEXSTREAM4IATIPROC)  wglxGetProcAddress("glVertexStream4iATI");
		glVertexStream4ivATI = (PFNGLVERTEXSTREAM4IVATIPROC) wglxGetProcAddress("glVertexStream4ivATI");
		glVertexStream4fATI  = (PFNGLVERTEXSTREAM4FATIPROC)  wglxGetProcAddress("glVertexStream4fATI");
		glVertexStream4fvATI = (PFNGLVERTEXSTREAM4FVATIPROC) wglxGetProcAddress("glVertexStream4fvATI");
		glVertexStream4dATI  = (PFNGLVERTEXSTREAM4DATIPROC)  wglxGetProcAddress("glVertexStream4dATI");
		glVertexStream4dvATI = (PFNGLVERTEXSTREAM4DVATIPROC) wglxGetProcAddress("glVertexStream4dvATI");
		
		glNormalStream3bATI  = (PFNGLNORMALSTREAM3BATIPROC)  wglxGetProcAddress("glNormalStream3bATI");
		glNormalStream3bvATI = (PFNGLNORMALSTREAM3BVATIPROC) wglxGetProcAddress("glNormalStream3bvATI");
		glNormalStream3sATI  = (PFNGLNORMALSTREAM3SATIPROC)  wglxGetProcAddress("glNormalStream3sATI");
		glNormalStream3svATI = (PFNGLNORMALSTREAM3SVATIPROC) wglxGetProcAddress("glNormalStream3svATI");
		glNormalStream3iATI  = (PFNGLNORMALSTREAM3IATIPROC)  wglxGetProcAddress("glNormalStream3iATI");
		glNormalStream3ivATI = (PFNGLNORMALSTREAM3IVATIPROC) wglxGetProcAddress("glNormalStream3ivATI");
		glNormalStream3fATI  = (PFNGLNORMALSTREAM3FATIPROC)  wglxGetProcAddress("glNormalStream3fATI");
		glNormalStream3fvATI = (PFNGLNORMALSTREAM3FVATIPROC) wglxGetProcAddress("glNormalStream3fvATI");
		glNormalStream3dATI  = (PFNGLNORMALSTREAM3DATIPROC)  wglxGetProcAddress("glNormalStream3dATI");
		glNormalStream3dvATI = (PFNGLNORMALSTREAM3DVATIPROC) wglxGetProcAddress("glNormalStream3dvATI");		
	}

	GL_ATIX_texture_env_route		  = isExtensionSupported("GL_ATIX_texture_env_route");
	GL_ATIX_texture_env_combine3	  = isExtensionSupported("GL_ATIX_texture_env_combine3");





	if (GL_EXT_blend_color = isExtensionSupported("GL_EXT_blend_color")){
#ifdef LINUX
		glBlendColorEXT = (PFNGLBLENDCOLOREXTPROC) wglxGetProcAddress("glBlendColor");
		if (glBlendColorEXT == NULL)
#endif
		glBlendColorEXT = (PFNGLBLENDCOLOREXTPROC) wglxGetProcAddress("glBlendColorEXT");
	}

	if (GL_EXT_blend_func_separate = isExtensionSupported("GL_EXT_blend_func_separate")){
		glBlendFuncSeparateEXT = (PFNGLBLENDFUNCSEPARATEEXTPROC) wglxGetProcAddress("glBlendFuncSeparateEXT");
	}

	GL_EXT_blend_minmax   = isExtensionSupported("GL_EXT_blend_minmax");
	GL_EXT_blend_subtract = isExtensionSupported("GL_EXT_blend_subtract");
	if (GL_EXT_blend_minmax || GL_EXT_blend_subtract){
// Workaround for a driver bug in current ATi Linux drivers
#ifdef LINUX
		glBlendEquationEXT = (PFNGLBLENDEQUATIONEXTPROC) wglxGetProcAddress("glBlendEquation");
		if (glBlendEquationEXT == NULL)
#endif
		glBlendEquationEXT = (PFNGLBLENDEQUATIONEXTPROC) wglxGetProcAddress("glBlendEquationEXT");
	}

	if (GL_EXT_compiled_vertex_array = isExtensionSupported("GL_EXT_compiled_vertex_array")){
	    glLockArraysEXT   = (PFNGLLOCKARRAYSEXTPROC)   wglxGetProcAddress("glLockArraysEXT");
		glUnlockArraysEXT = (PFNGLUNLOCKARRAYSEXTPROC) wglxGetProcAddress("glUnlockArraysEXT");
	}

	if (GL_EXT_draw_range_elements = isExtensionSupported("GL_EXT_draw_range_elements")){
		glDrawRangeElementsEXT = (PFNGLDRAWRANGEELEMENTSEXTPROC) wglxGetProcAddress("glDrawRangeElementsEXT");
	}

	if (GL_EXT_fog_coord = isExtensionSupported("GL_EXT_fog_coord")){
		glFogCoordfEXT  = (PFNGLFOGCOORDFEXTPROC)  wglxGetProcAddress("glFogCoordfEXT");
		glFogCoorddEXT  = (PFNGLFOGCOORDDEXTPROC)  wglxGetProcAddress("glFogCoorddEXT");
		glFogCoordfvEXT = (PFNGLFOGCOORDFVEXTPROC) wglxGetProcAddress("glFogCoordfvEXT");
		glFogCoorddvEXT = (PFNGLFOGCOORDDVEXTPROC) wglxGetProcAddress("glFogCoorddvEXT");
		glFogCoordPointerEXT = (PFNGLFOGCOORDPOINTEREXTPROC) wglxGetProcAddress("glFogCoordPointerEXT");
	}

	GL_EXT_packed_pixels = isExtensionSupported("GL_EXT_packed_pixels");

	if (GL_EXT_polygon_offset = isExtensionSupported("GL_EXT_polygon_offset")){
		glPolygonOffsetEXT = (PFNGLPOLYGONOFFSETEXTPROC) wglxGetProcAddress("glPolygonOffsetEXT");
	}

	GL_EXT_stencil_wrap = isExtensionSupported("GL_EXT_stencil_wrap");
	
	if (GL_EXT_texture3D = isExtensionSupported("GL_EXT_texture3D")){
// Workaround for a driver bug in current ATi Linux drivers
#ifdef LINUX
		glTexImage3DEXT = (PFNGLTEXIMAGE3DEXTPROC) wglxGetProcAddress("glTexImage3D");
		if (glTexImage3DEXT == NULL)
#endif
		glTexImage3DEXT = (PFNGLTEXIMAGE3DEXTPROC) wglxGetProcAddress("glTexImage3DEXT");
	}

	GL_EXT_texture_compression_s3tc   = isExtensionSupported("GL_EXT_texture_compression_s3tc");
	GL_EXT_texture_edge_clamp		  = isExtensionSupported("GL_EXT_texture_edge_clamp") | isExtensionSupported("GL_SGIS_texture_edge_clamp");
	GL_EXT_texture_env_dot3			  = isExtensionSupported("GL_EXT_texture_env_dot3");
	GL_EXT_texture_filter_anisotropic = isExtensionSupported("GL_EXT_texture_filter_anisotropic");
	GL_EXT_texture_lod_bias           = isExtensionSupported("GL_EXT_texture_lod_bias");
	GL_EXT_texture_rectangle          = isExtensionSupported("GL_EXT_texture_rectangle");

	if (GL_EXT_vertex_shader = isExtensionSupported("GL_EXT_vertex_shader")){
		glBeginVertexShaderEXT  = (PFNGLBEGINVERTEXSHADEREXTPROC)  wglxGetProcAddress("glBeginVertexShaderEXT");
		glEndVertexShaderEXT    = (PFNGLENDVERTEXSHADEREXTPROC)    wglxGetProcAddress("glEndVertexShaderEXT");
		glBindVertexShaderEXT   = (PFNGLBINDVERTEXSHADEREXTPROC)   wglxGetProcAddress("glBindVertexShaderEXT");
		glGenVertexShadersEXT   = (PFNGLGENVERTEXSHADERSEXTPROC)   wglxGetProcAddress("glGenVertexShadersEXT");
		glDeleteVertexShaderEXT = (PFNGLDELETEVERTEXSHADEREXTPROC) wglxGetProcAddress("glDeleteVertexShaderEXT");
		
		glShaderOp1EXT          = (PFNGLSHADEROP1EXTPROC)          wglxGetProcAddress("glShaderOp1EXT");
		glShaderOp2EXT          = (PFNGLSHADEROP2EXTPROC)          wglxGetProcAddress("glShaderOp2EXT");
		glShaderOp3EXT          = (PFNGLSHADEROP3EXTPROC)          wglxGetProcAddress("glShaderOp3EXT");
		glSwizzleEXT            = (PFNGLSWIZZLEEXTPROC)            wglxGetProcAddress("glSwizzleEXT");
		glWriteMaskEXT          = (PFNGLWRITEMASKEXTPROC)          wglxGetProcAddress("glWriteMaskEXT");
		glInsertComponentEXT    = (PFNGLINSERTCOMPONENTEXTPROC)    wglxGetProcAddress("glInsertComponentEXT");
		glExtractComponentEXT   = (PFNGLEXTRACTCOMPONENTEXTPROC)   wglxGetProcAddress("glExtractComponentEXT");
		glGenSymbolsEXT         = (PFNGLGENSYMBOLSEXTPROC)         wglxGetProcAddress("glGenSymbolsEXT");
		
		glSetInvariantEXT       = (PFNGLSETINVARIANTEXTPROC)       wglxGetProcAddress("glSetInvariantEXT");
		glSetLocalConstantEXT   = (PFNGLSETLOCALCONSTANTEXTPROC)   wglxGetProcAddress("glSetLocalConstantEXT");
		glVariantbvEXT          = (PFNGLVARIANTBVEXTPROC)          wglxGetProcAddress("glVariantbvEXT");
		glVariantsvEXT          = (PFNGLVARIANTSVEXTPROC)          wglxGetProcAddress("glVariantsvEXT");
		glVariantivEXT          = (PFNGLVARIANTIVEXTPROC)          wglxGetProcAddress("glVariantivEXT");
		glVariantfvEXT          = (PFNGLVARIANTFVEXTPROC)          wglxGetProcAddress("glVariantfvEXT");
		glVariantdvEXT          = (PFNGLVARIANTDVEXTPROC)          wglxGetProcAddress("glVariantdvEXT");
		glVariantubvEXT         = (PFNGLVARIANTUBVEXTPROC)         wglxGetProcAddress("glVariantubvEXT");
		glVariantusvEXT         = (PFNGLVARIANTUSVEXTPROC)         wglxGetProcAddress("glVariantusvEXT");
		glVariantuivEXT         = (PFNGLVARIANTUIVEXTPROC)         wglxGetProcAddress("glVariantuivEXT");
		glVariantPointerEXT     = (PFNGLVARIANTPOINTEREXTPROC)     wglxGetProcAddress("glVariantPointerEXT");
		glEnableVariantClientStateEXT  = (PFNGLENABLEVARIANTCLIENTSTATEEXTPROC)  wglxGetProcAddress("glEnableVariantClientStateEXT");
		glDisableVariantClientStateEXT = (PFNGLDISABLEVARIANTCLIENTSTATEEXTPROC) wglxGetProcAddress("glDisableVariantClientStateEXT");
		
		glBindLightParameterEXT        = (PFNGLBINDLIGHTPARAMETEREXTPROC)       wglxGetProcAddress("glBindLightParameterEXT");
		glBindMaterialParameterEXT     = (PFNGLBINDMATERIALPARAMETEREXTPROC)    wglxGetProcAddress("glBindMaterialParameterEXT");
		glBindTexGenParameterEXT       = (PFNGLBINDTEXGENPARAMETEREXTPROC)      wglxGetProcAddress("glBindTexGenParameterEXT");
		glBindTextureUnitParameterEXT  = (PFNGLBINDTEXTUREUNITPARAMETEREXTPROC) wglxGetProcAddress("glBindTextureUnitParameterEXT");
		glBindParameterEXT             = (PFNGLBINDPARAMETEREXTPROC)            wglxGetProcAddress("glBindParameterEXT");
		glIsVariantEnabledEXT          = (PFNGLISVARIANTENABLEDEXTPROC)         wglxGetProcAddress("glIsVariantEnabledEXT");
		glGetVariantBooleanvEXT        = (PFNGLGETVARIANTBOOLEANVEXTPROC)       wglxGetProcAddress("glGetVariantBooleanvEXT");
		glGetVariantIntegervEXT        = (PFNGLGETVARIANTINTEGERVEXTPROC)       wglxGetProcAddress("glGetVariantIntegervEXT");
		glGetVariantFloatvEXT          = (PFNGLGETVARIANTFLOATVEXTPROC)         wglxGetProcAddress("glGetVariantFloatvEXT");
		glGetVariantPointervEXT        = (PFNGLGETVARIANTPOINTERVEXTPROC)       wglxGetProcAddress("glGetVariantPointervEXT");
		glGetInvariantBooleanvEXT      = (PFNGLGETINVARIANTBOOLEANVEXTPROC)     wglxGetProcAddress("glGetInvariantBooleanvEXT");
		glGetInvariantIntegervEXT      = (PFNGLGETINVARIANTINTEGERVEXTPROC)     wglxGetProcAddress("glGetInvariantIntegervEXT");
		glGetInvariantFloatvEXT        = (PFNGLGETINVARIANTFLOATVEXTPROC)       wglxGetProcAddress("glGetInvariantFloatvEXT");
		glGetLocalConstantBooleanvEXT  = (PFNGLGETLOCALCONSTANTBOOLEANVEXTPROC) wglxGetProcAddress("glGetLocalConstantBooleanvEXT");
		glGetLocalConstantIntegervEXT  = (PFNGLGETLOCALCONSTANTINTEGERVEXTPROC) wglxGetProcAddress("glGetLocalConstantIntegervEXT");
		glGetLocalConstantFloatvEXT    = (PFNGLGETLOCALCONSTANTFLOATVEXTPROC)   wglxGetProcAddress("glGetLocalConstantFloatvEXT");
	}




	GL_GL2_fragment_shader = isExtensionSupported("GL_GL2_fragment_shader");

	if (GL_GL2_shader_objects = isExtensionSupported("GL_GL2_shader_objects")){
		glDeleteObjectGL2  = (PFNGLDELETEOBJECTGL2PROC)  wglxGetProcAddress("glDeleteObjectGL2");
		glDeleteObjectsGL2 = (PFNGLDELETEOBJECTSGL2PROC) wglxGetProcAddress("glDeleteObjectsGL2");
		glGetHandleGL2     = (PFNGLGETHANDLEGL2PROC)     wglxGetProcAddress("glGetHandleGL2");
		glDetachObjectGL2  = (PFNGLDETACHOBJECTGL2PROC)  wglxGetProcAddress("glDetachObjectGL2");

		glObjectParameterfGL2     = (PFNGLOBJECTPARAMETERFGL2PROC)     wglxGetProcAddress("glObjectParameterfGL2");
		glObjectParameteriGL2     = (PFNGLOBJECTPARAMETERIGL2PROC)     wglxGetProcAddress("glObjectParameteriGL2");
		glGetObjectParameterfvGL2 = (PFNGLGETOBJECTPARAMETERFVGL2PROC) wglxGetProcAddress("glGetObjectParameterfvGL2");
		glGetObjectParameterivGL2 = (PFNGLGETOBJECTPARAMETERIVGL2PROC) wglxGetProcAddress("glGetObjectParameterivGL2");

		glCreateShaderObjectGL2  = (PFNGLCREATESHADEROBJECTGL2PROC)  wglxGetProcAddress("glCreateShaderObjectGL2");
		glLoadShaderGL2          = (PFNGLLOADSHADERGL2PROC)          wglxGetProcAddress("glLoadShaderGL2");
		glAppendShaderGL2        = (PFNGLAPPENDSHADERGL2PROC)        wglxGetProcAddress("glAppendShaderGL2");
		glCompileShaderGL2       = (PFNGLCOMPILESHADERGL2PROC)       wglxGetProcAddress("glCompileShaderGL2");
		glGetInfoLogGL2          = (PFNGLGETINFOLOGGL2PROC)          wglxGetProcAddress("glGetInfoLogGL2");
		glGetShaderStringGL2     = (PFNGLGETSHADERSTRINGGL2PROC)     wglxGetProcAddress("glGetShaderStringGL2");
		glCreateProgramObjectGL2 = (PFNGLCREATEPROGRAMOBJECTGL2PROC) wglxGetProcAddress("glCreateProgramObjectGL2");
		glAttachShaderObjectGL2  = (PFNGLATTACHSHADEROBJECTGL2PROC)  wglxGetProcAddress("glAttachShaderObjectGL2");
		glLinkProgramGL2         = (PFNGLLINKPROGRAMGL2PROC)         wglxGetProcAddress("glLinkProgramGL2");
		glUseProgramObjectGL2    = (PFNGLUSEPROGRAMOBJECTGL2PROC)    wglxGetProcAddress("glUseProgramObjectGL2");

		glLoadUniform1fGL2   = (PFNGLLOADUNIFORM1FGL2PROC)   wglxGetProcAddress("glLoadUniform1fGL2");
		glLoadUniform2fGL2   = (PFNGLLOADUNIFORM2FGL2PROC)   wglxGetProcAddress("glLoadUniform2fGL2");
		glLoadUniform3fGL2   = (PFNGLLOADUNIFORM3FGL2PROC)   wglxGetProcAddress("glLoadUniform3fGL2");
		glLoadUniform4fGL2   = (PFNGLLOADUNIFORM4FGL2PROC)   wglxGetProcAddress("glLoadUniform4fGL2");
		glLoadUniform1fvGL2  = (PFNGLLOADUNIFORM1FVGL2PROC)  wglxGetProcAddress("glLoadUniform1fvGL2");
		glLoadUniform2fvGL2  = (PFNGLLOADUNIFORM2FVGL2PROC)  wglxGetProcAddress("glLoadUniform2fvGL2");
		glLoadUniform3fvGL2  = (PFNGLLOADUNIFORM3FVGL2PROC)  wglxGetProcAddress("glLoadUniform3fvGL2");
		glLoadUniform4fvGL2  = (PFNGLLOADUNIFORM4FVGL2PROC)  wglxGetProcAddress("glLoadUniform4fvGL2");
		glLoadUniformIntGL2  = (PFNGLLOADUNIFORMINTGL2PROC)  wglxGetProcAddress("glLoadUniformIntGL2");
		glLoadUniformBoolGL2 = (PFNGLLOADUNIFORMBOOLGL2PROC) wglxGetProcAddress("glLoadUniformBoolGL2");

		glLoadUniformArray1fvGL2  = (PFNGLLOADUNIFORMARRAY1FVGL2PROC)  wglxGetProcAddress("glLoadUniformArray1fvGL2");
		glLoadUniformArray2fvGL2  = (PFNGLLOADUNIFORMARRAY2FVGL2PROC)  wglxGetProcAddress("glLoadUniformArray2fvGL2");
		glLoadUniformArray3fvGL2  = (PFNGLLOADUNIFORMARRAY3FVGL2PROC)  wglxGetProcAddress("glLoadUniformArray3fvGL2");
		glLoadUniformArray4fvGL2  = (PFNGLLOADUNIFORMARRAY4FVGL2PROC)  wglxGetProcAddress("glLoadUniformArray4fvGL2");
		glLoadUniformMatrix2fvGL2 = (PFNGLLOADUNIFORMMATRIX2FVGL2PROC) wglxGetProcAddress("glLoadUniformMatrix2fvGL2");
		glLoadUniformMatrix3fvGL2 = (PFNGLLOADUNIFORMMATRIX3FVGL2PROC) wglxGetProcAddress("glLoadUniformMatrix3fvGL2");
		glLoadUniformMatrix4fvGL2 = (PFNGLLOADUNIFORMMATRIX4FVGL2PROC) wglxGetProcAddress("glLoadUniformMatrix4fvGL2");
		glLoadUniformMatrixArray2fvGL2 = (PFNGLLOADUNIFORMMATRIXARRAY2FVGL2PROC) wglxGetProcAddress("glLoadUniformMatrixArray2fvGL2");
		glLoadUniformMatrixArray3fvGL2 = (PFNGLLOADUNIFORMMATRIXARRAY3FVGL2PROC) wglxGetProcAddress("glLoadUniformMatrixArray3fvGL2");
		glLoadUniformMatrixArray4fvGL2 = (PFNGLLOADUNIFORMMATRIXARRAY4FVGL2PROC) wglxGetProcAddress("glLoadUniformMatrixArray4fvGL2");

		glGetUniformLocationGL2 = (PFNGLGETUNIFORMLOCATIONGL2PROC) wglxGetProcAddress("glGetUniformLocationGL2");
		glGetUniformfvGL2       = (PFNGLGETUNIFORMFVGL2PROC) wglxGetProcAddress("glGetUniformfvGL2");
		glGetUniformivGL2       = (PFNGLGETUNIFORMIVGL2PROC) wglxGetProcAddress("glGetUniformivGL2");
	}

	if (GL_GL2_vertex_shader = isExtensionSupported("GL_GL2_vertex_shader")){
		glBindAttributeLocationGL2 = (PFNGLBINDATTRIBUTELOCATIONGL2PROC) wglxGetProcAddress("glBindAttributeLocationGL2");
		glGetActiveAttributesGL2   = (PFNGLGETACTIVEATTRIBUTESGL2PROC)   wglxGetProcAddress("glGetActiveAttributesGL2");
	}



	
	GL_HP_occlusion_test = isExtensionSupported("GL_HP_occlusion_test");



	GL_NV_blend_square = isExtensionSupported("GL_NV_blend_square");

	if (GL_NV_occlusion_query = isExtensionSupported("GL_NV_occlusion_query")){
		glGenOcclusionQueriesNV    = (PFNGLGENOCCLUSIONQUERIESNVPROC)    wglxGetProcAddress("glGenOcclusionQueriesNV");
		glDeleteOcclusionQueriesNV = (PFNGLDELETEOCCLUSIONQUERIESNVPROC) wglxGetProcAddress("glDeleteOcclusionQueriesNV");
		glIsOcclusionQueryNV       = (PFNGLISOCCLUSIONQUERYNVPROC)       wglxGetProcAddress("glIsOcclusionQueryNV");
		glBeginOcclusionQueryNV    = (PFNGLBEGINOCCLUSIONQUERYNVPROC)    wglxGetProcAddress("glBeginOcclusionQueryNV");
		glEndOcclusionQueryNV      = (PFNGLENDOCCLUSIONQUERYNVPROC)      wglxGetProcAddress("glEndOcclusionQueryNV");
		glGetOcclusionQueryivNV    = (PFNGLGETOCCLUSIONQUERYIVNVPROC)    wglxGetProcAddress("glGetOcclusionQueryivNV");
		glGetOcclusionQueryuivNV   = (PFNGLGETOCCLUSIONQUERYUIVNVPROC)   wglxGetProcAddress("glGetOcclusionQueryuivNV");
	}

    if (GL_NV_register_combiners = isExtensionSupported("GL_NV_register_combiners")){
        glCombinerParameterfvNV = (PFNGLCOMBINERPARAMETERFVNVPROC) wglxGetProcAddress("glCombinerParameterfvNV");
        glCombinerParameterfNV  = (PFNGLCOMBINERPARAMETERFNVPROC)  wglxGetProcAddress("glCombinerParameterfNV");
        glCombinerParameterivNV = (PFNGLCOMBINERPARAMETERIVNVPROC) wglxGetProcAddress("glCombinerParameterivNV");
        glCombinerParameteriNV  = (PFNGLCOMBINERPARAMETERINVPROC)  wglxGetProcAddress("glCombinerParameteriNV");

        glCombinerInputNV       = (PFNGLCOMBINERINPUTNVPROC)       wglxGetProcAddress("glCombinerInputNV");
        glCombinerOutputNV      = (PFNGLCOMBINEROUTPUTNVPROC)      wglxGetProcAddress("glCombinerOutputNV");
        glFinalCombinerInputNV  = (PFNGLFINALCOMBINERINPUTNVPROC)  wglxGetProcAddress("glFinalCombinerInputNV");

        glGetCombinerInputParameterfvNV  = (PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC)  wglxGetProcAddress("glGetCombinerInputParameterfvNV");
        glGetCombinerInputParameterivNV  = (PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC)  wglxGetProcAddress("glGetCombinerInputParameterivNV");
        glGetCombinerOutputParameterfvNV = (PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC) wglxGetProcAddress("glGetCombinerOutputParameterfvNV");
        glGetCombinerOutputParameterivNV = (PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC) wglxGetProcAddress("glGetCombinerOutputParameterivNV");

        glGetFinalCombinerInputParameterfvNV = (PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC) wglxGetProcAddress("glGetFinalCombinerInputParameterfvNV");
        glGetFinalCombinerInputParameterivNV = (PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC) wglxGetProcAddress("glGetFinalCombinerInputParameterivNV");
    }

	GL_NV_texture_env_combine4 = isExtensionSupported("GL_NV_texture_env_combine4");
	
	if (GL_NV_vertex_array_range = isExtensionSupported("GL_NV_vertex_array_range")){
		glFlushVertexArrayRangeNV = (PFNGLFLUSHVERTEXARRAYRANGENVPROC) wglxGetProcAddress("glFlushVertexArrayRangeNV");
		glVertexArrayRangeNV      = (PFNGLVERTEXARRAYRANGENVPROC)      wglxGetProcAddress("glVertexArrayRangeNV");
		wglAllocateMemoryNV       = (PFNWGLALLOCATEMEMORYNVPROC)       wglxGetProcAddress("wglAllocateMemoryNV");
		wglFreeMemoryNV           = (PFNWGLFREEMEMORYNVPROC)           wglxGetProcAddress("wglFreeMemoryNV");
	}




	GL_SGIS_generate_mipmap = isExtensionSupported("GL_SGIS_generate_mipmap");


#if defined(WIN32)

	if (WGL_ARB_pbuffer = isWGLXExtensionSupported("WGL_ARB_pbuffer")){
		wglCreatePbufferARB    = (PFNWGLCREATEPBUFFERARBPROC)    wglxGetProcAddress("wglCreatePbufferARB");
		wglGetPbufferDCARB     = (PFNWGLGETPBUFFERDCARBPROC)     wglxGetProcAddress("wglGetPbufferDCARB");
		wglReleasePbufferDCARB = (PFNWGLRELEASEPBUFFERDCARBPROC) wglxGetProcAddress("wglReleasePbufferDCARB");
		wglDestroyPbufferARB   = (PFNWGLDESTROYPBUFFERARBPROC)   wglxGetProcAddress("wglDestroyPbufferARB");
		wglQueryPbufferARB     = (PFNWGLQUERYPBUFFERARBPROC)     wglxGetProcAddress("wglQueryPbufferARB");
	}

	if (WGL_ARB_pixel_format = isWGLXExtensionSupported("WGL_ARB_pixel_format")){
		wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC) wglxGetProcAddress("wglGetPixelFormatAttribivARB");
		wglGetPixelFormatAttribfvARB = (PFNWGLGETPIXELFORMATATTRIBFVARBPROC) wglxGetProcAddress("wglGetPixelFormatAttribfvARB");
		wglChoosePixelFormatARB      = (PFNWGLCHOOSEPIXELFORMATARBPROC)      wglxGetProcAddress("wglChoosePixelFormatARB");
	}

    if (WGL_ARB_make_current_read  = isWGLXExtensionSupported("WGL_ARB_make_current_read")){
        wglMakeContextCurrentARB = (PFNWGLMAKECONTEXTCURRENTARBPROC) wglxGetProcAddress("wglMakeContextCurrentARB");
        wglGetCurrentReadDCARB   = (PFNWGLGETCURRENTREADDCARBPROC)   wglxGetProcAddress("wglGetCurrentReadDCARB");
    }

	WGL_ARB_multisample = isWGLXExtensionSupported("WGL_ARB_multisample");

	if (WGL_ARB_render_texture = isWGLXExtensionSupported("WGL_ARB_render_texture")){
		wglBindTexImageARB     = (PFNWGLBINDTEXIMAGEARBPROC)     wglxGetProcAddress("wglBindTexImageARB");
		wglReleaseTexImageARB  = (PFNWGLRELEASETEXIMAGEARBPROC)  wglxGetProcAddress("wglReleaseTexImageARB");
		wglSetPbufferAttribARB = (PFNWGLSETPBUFFERATTRIBARBPROC) wglxGetProcAddress("wglSetPbufferAttribARB");
	}

	WGL_ATI_pixel_format_float = isWGLXExtensionSupported("WGL_ATI_pixel_format_float");



	if (WGL_EXT_swap_control = isWGLXExtensionSupported("WGL_EXT_swap_control")){
        wglSwapIntervalEXT = (PFNWGLEXTSWAPINTERVALPROC) wglxGetProcAddress("wglSwapIntervalEXT"); 
	}

#elif defined(LINUX)

	//GLX_EXT_visual_info = isWGLXExtensionSupported("GLX_EXT_visual_info");

#endif

}
