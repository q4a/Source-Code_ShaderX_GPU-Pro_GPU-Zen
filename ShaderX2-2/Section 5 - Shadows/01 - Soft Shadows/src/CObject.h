///
///     Object file
///
///
///		Copyright (c) 2003 F. Brebion (f.brebion@vrcontext.com)
///
///		Licence/disclaimer: This demo is provided 'as-is', without any express or implied
///		warranty. In no event can i be held liable for any damages arising from the use
///		of that code or this demo.
///
///		Permission is granted to anyone to use the code for any purpose, including
///		commercial usage, and to alter it and redistribute it freely, subject to the
///		following restrictions:
///
///		1. Proper credits must be given to the author for his contribution to the code
///		if the code is totally or partially reused. If you implement the described technic
///		yourself, credits are appreciated but not required. I'd also appreciate it
///		if you send me an email saying how you're going to use the code for.
///
///		2. The 3D models and textures in this demo belong to Daniel Cornibert
///		(danielcornibert@hotmail.com) and are included in it as pure curtesy from his
///		part. PERMISSION TO USE THIS ART IS NOT GRANTED by this license.
///

#ifndef OBJECT_H_
#define OBJECT_H_

#include <windows.h>
#include <stdio.h>

#include "CTexture.h"
#include "CUtils.h"
#include "CScene.h"

struct s_primitive
{
    int                 m_start;            // start index
    int                 m_size;             // number of indices
    s_material          *m_mat;             // material of primitive
};

/// a transparent triangle
struct s_transpTri
{
    float               m_dist;             // distance to viewer
    int                 m_v0;               // vertex 0
    int                 m_v1;               // vertex 1
    int                 m_v2;               // vertex 2
    s_material          *m_mat;             // material to use
};

#pragma pack(1)

///
///     Standard vertex data: 32 bytes
///
struct s_stdVertex
{
    float               m_pos[3];           // position of vertex                   
    float               m_normal[3];        // normal vector                        
    float               m_tc[2];            // texture coordinates TMU 0            
};
#define STD_VERTEX_FORMAT (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

///
///     Extruded vertex data: 16 bytes
///
struct s_extVertex
{
    float               m_pos[3];           // position of vertex
    float               m_weight;           // extrusion weight
    float               m_normal[3];        // normal vector
};
#define EXT_VERTEX_FORMAT (D3DFVF_XYZB1 | D3DFVF_NORMAL)

///
///     Simple colored vertex: 20 bytes
///
struct s_colVertex
{
    float               m_pos[4];           // transformed position of vertex
    DWORD               m_color;            // color of vertex
};
#define COL_VERTEX_FORMAT (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
struct s_colVertex1
{
    float               m_pos[3];           // untransformed position of vertex
    DWORD               m_color;            // color of vertex
};
#define COL_VERTEX_FORMAT1 (D3DFVF_XYZ | D3DFVF_DIFFUSE)

///
///     Simple colored & textured vertex: 28 bytes
///
struct s_texVertex
{
    float               m_pos[4];           // transformed position of vertex
    DWORD               m_color;            // color of vertex
    float               m_tc[2];            // texture coordinates TMU 0            
};
#define TEX_VERTEX_FORMAT (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

///
///     Blurred vertex: 16 bytes
///
struct s_blurVertex
{
    float               m_pos[4];           // transformed position of vertex
    DWORD               m_color;            // color of vertex
    float               m_tc0[2];           // texture coordinates TMU 0            
    float               m_tc1[2];           // texture coordinates TMU 1
    float               m_tc2[2];           // texture coordinates TMU 2
    float               m_tc3[2];           // texture coordinates TMU 3            
    float               m_tc4[2];           // texture coordinates TMU 4            
    float               m_tc5[2];           // texture coordinates TMU 5
};
#define BLUR_VERTEX_FORMAT (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX6)

#pragma pack(4)

///
///     A triangle connection
///
struct s_connTri
{
    int                 m_verts[3];         // vertices IDs
    int                 m_cverts[3];        // connected vertices..
    int                 m_count[3];         // normals count
    int                 m_edges[3];         // neighbooring triangle ID per edge, -1 if none
    bool                m_visible;          // is triangle visible from light?
};

struct s_connVert
{
    int                 m_id;
    float               m_pos[3];
    float               m_normal[3];
    int                 m_count;
};

class c_object
{
    friend class c_aseFile;
    friend class c_scene;
    friend class c_objMd3;

    protected:

    int                     m_nbFrames;             // number of keyframes, up to 128
    int                     m_curFrame;             // current keyframe to show

    IDirect3DDevice9        *m_lpDevice;            // D3D device
	char				    m_name[128];		    // current object's name
    int                     m_nbVerts;              // number of vertices
    s_stdVertex             *m_verts[128];          // array of vertices
    int                     m_nbIndices;            // number of indices
    int                     *m_indices;             // array of indices
    
    IDirect3DVertexBuffer9  *m_stdVB[128];          // standard vertex buffer
    IDirect3DIndexBuffer9   *m_stdIB;               // standard index buffer

    int                     m_nbExtIndices;         // number of extrusion indices
    IDirect3DVertexBuffer9  *m_extVB[128];          // extrusion vertex buffer
    IDirect3DIndexBuffer9   *m_extIB;               // extrusion index buffer
    IDirect3DVertexBuffer9  *m_dbgVB;               // debug vertex buffer

    int                     m_nbPrims;              // number of primitives
    s_primitive             *m_primsData;           // array of primitives

    float                   m_lightPos[3];          // light's position
    float                   m_viewPos[3];           // viewer's position
    
    s_connTri               *m_connTris;            // connected triangles
    s_connVert              *m_connVerts;           // connected vertices
    float                   *m_planes[128];         // triangle's plane
    int                     m_nbConnVerts;          // number of connected vertices

    float                   m_min[3];               // bounding box, min
    float                   m_max[3];               // bounding box, max
    float                   m_center[4];            // center point (+weight)

    ///
    ///     Calculates a plane from 3 vertices
    ///
    static void calculatePlane(float *plane, const s_stdVertex& v0, const s_stdVertex& v1,
        const s_stdVertex& v2);

    ///
    ///     Compares 2 connected vertices
    ///
    static int c_object::connVertComp(
        const void          *arg1,              // first entity
        const void          *arg2               // second entity
        );

    ///     
    ///     Generates the connectivity informations for the triangles
    ///
    void genConnectivity();

    public:

    ///
    ///     Creates an empty object
    ///
    c_object(IDirect3DDevice9 *a_device);

    ///
    ///     Destroys the object
    ///
    ~c_object();

	///
	///		Creates the mesh of the object
	///
	bool create();

    ///
    ///     Creates the mesh from another mesh
    ///
    bool create(const char *name, const int nbVerts, const int nbFrames,
        const float *xyz, const float *uv, const float *norm, const int nbTris,
        const int *indices, s_material *mat);

	///
	///		Sets the position of the light source illuminating the object
    ///     (in object space)
	///
	void setLightSource(const float *lPos);

	///
	///		Sets the position of the viewer looking at this object
	///
	void setViewer(const float *a_pos);

    ///
    ///     Renders the object
    ///
    bool render(const int primID);
    bool render1(const int primID);
    bool render2(const int primID);
    bool render3(const int primID);
    bool renderDbg(const int primID);

	///
	///		Returns the number of drawn polygons after a render()
	///
    inline int drawn() const            { return(m_nbIndices / 3); }

    ///
    ///     Generates the shadow volume indices, plus flags the vertices to extend
    ///
    bool extend();

    ///
    ///     Draws the shadow volume
    ///
    bool drawShadows();

    ///
    ///     Finds the faces visible from the light
    ///
    void visFromLight();

    ///
    ///     Draws the geometry
    ///
    void drawElements(int type, int nbElems, void *elems);

    ///
    ///     Misc. functions
    ///
    inline s_stdVertex *getVerts(const int f) const { return(m_verts[f]); }
    inline int getNbVerts() const               { return(m_nbVerts); }
    inline int *getIndices() const              { return(m_indices); }
    inline int getNbTris() const                { return(m_nbIndices / 3); }
    inline int getNbPrims() const               { return(m_nbPrims); }
    inline s_primitive *getPrim(const int id) const
    {
        return(m_primsData + id);
    }
    inline const char *getName() const          { return(m_name); }
    inline const float *getBoxMin() const       { return(m_min); }
    inline const float *getBoxMax() const       { return(m_max); }
    inline const float *getCenter() const       { return(m_center); }
};

#endif

