///
///     ASE file loader
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

#ifndef ASEFILE_H_
#define ASEFILE_H_

#include <windows.h>
#include <stdio.h>

#include "CTexture.h"
#include "CUtils.h"

class c_object;
class c_scene;

///
///     A face
///
struct s_face
{
    int                 m_pos[3];           // position index
    int                 m_norm[3];          // normal index
    int                 m_tc[3];            // tex coord index
    int                 m_subID;            // sub material ID
};

///
///     A normal
///
struct s_normal
{
    int                 id;                 // normal id
    float               x, y, z;            // normal components
};

///
///     A tex coord
///
struct s_texcoord
{
    int                 id;                 // tex coord id
    float               u, v;               // texture coord
};

///
///     A triplet index
///
struct s_triplet
{
    int                 id;                 // triplet id
    int                 pos, tc, norm;      // indices in triplet
    int                 index;              // index in IA
};

class c_aseFile
{
    friend class c_entity;

    protected:

    IDirect3DDevice9        *m_lpDevice;            // D3D device

    char                    m_gname[128];           // group name
    FILE                    *m_file;                // file being read

	char				    m_name[128];		    // current object's name
    int                     m_matID;                // current material ID
    int                     m_subMatID;             // sub material ID
    int                     m_lightID;              // light ID
    bool                    m_inDiffuse;            // in diffuse material?
    bool                    m_inBump;               // in bumpmap material?
    bool                    m_inSpecular;           // in specular material?
    bool                    m_inGeom;               // is in geometry chunk?
    bool                    m_inReflection;         // is in reflection chunk?
    bool                    m_inRefraction;         // is in refraction chunk?
    bool                    m_inLight;              // is in light chunk?

    s_material              m_mat;                  // current material
    s_light                 m_light;                // current light

	float				    m_rotMat0[3];		    // rotation matrix 0
	float				    m_rotMat1[3];		    // rotation matrix 1
	float				    m_rotMat2[3];		    // rotation matrix 2

    int                     m_nbVPos;               // number of vertex positions
    float                   *m_VPos;                // vertex positions
    int                     m_nbVTC;                // number of texture coords.
    s_texcoord              *m_VTC;                 // texture coordinates
    int                     m_nbVNorm;              // number of normals
    s_normal                *m_VNorm;               // vertex normals
    int                     m_normFace;             // normal face index
    int                     m_normOffset;           // normal offset in face
    s_material              **m_mats;               // materials / sub-materials table

    int                     m_nbFaces;              // number of faces
    s_face                  *m_faces;               // array of faces

    c_object                *m_objs[256];           // created objects
    int                     m_nbObjs;               // number of created objects

    ///
    ///     Validates the object: calculate the vertex data and indices
    ///
    void validate(const int matIndex);

    ///
    ///     Reads & analyzes a new line from the current ASE file
    ///
    void analyze();

    ///
    ///     Validates a material
    ///
    void validateMat();

    ///
    ///     Validates a light
    ///
    void validateLight();

    public:

    ///
    ///     Creates an ASE file
    ///
    c_aseFile(IDirect3DDevice9 *a_device, const char *fname, const char *gname = "");

    ///
    ///     Reads the ASE file
    ///
    void load();

	///
	///		Destroys the ASE file
	///
    ~c_aseFile();
};

#endif

