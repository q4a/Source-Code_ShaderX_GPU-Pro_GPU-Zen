///
///     MD3 Object file
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

#ifndef COBJMD3_H_
#define COBJMD3_H_

#include "CObject.h"
#include "CTexture.h"

struct md3_fileHeader
{
   int  ident;
   int  version;
   char name[64]; 
   int  flags;
   int  numFrames;
   int  numTags;       
   int  numSurfaces;
   int  numSkins;
   int  ofsFrames;
   int  ofsTags; 
   int  ofsSurfaces; 
   int  ofsEnd; 
};

struct md3_frame
{
    float               bounds[2][3];
    float               origin[3];
    float               radius;
    char                name[64];
};

struct md3_surf
{
    char                name[64];
    int                 numFrames;
    int                 numVerts; 
    int                 numTris;
    int                 *indices;
    float               *pos;
    float               *norm;
    float               *tex;
};

struct md3_surfHeader
{
   int  ident;   
   char name[64];  
   int  flags;
   int  numFrames;  
   int  numShaders;
   int  numVerts;
   int  numTriangles;
   int  ofsTriangles;
   int  ofsShaders; 
   int  ofsSt; 
   int  ofsXyzNormals;
   int  ofsEnd;  
};

struct md3_step
{
    int                 m_stepID;           // this step's ID
    int                 m_startFrame;       // starting frame (inclusive)
    int                 m_endFrame;         // ending frame (inclusive)
    int                 m_nextStepID;       // next step ID, same to loop!
};

struct md3_state
{
    int                 m_stepID;           // current step's ID
    int                 m_start;            // starting frame
    float               m_time;             // [0-1], time into current frame
    int                 m_end;              // ending frame
};

class c_objMd3
{
    protected:

    IDirect3DDevice9    *m_lpDevice;        // D3D device
    c_object            *m_mesh;            // mesh of the MD3
    int                 m_nbFrames;         // number of frames
    int                 m_nbTags;           // number of tags
    int                 m_nbSurfaces;       // number of surfaces
    int                 m_nbSkins;          // number of skins
    md3_frame           *m_frames;          // array of frames
    md3_surf            *m_surfs;           // array of surfaces
    int                 m_nbSteps;          // number of steps
    md3_step            *m_steps;           // array of steps

    ///
    ///     Registers an animation step
    ///
    void registerStep(const int start, const int end, const int next);

    ///
    ///     Creates the object
    ///
    bool create(const char *name, const char *fname, s_material *mat, const float *translation);

    public:

    ///
    ///     Default constructor
    ///
    c_objMd3(IDirect3DDevice9 *a_device, const char *name, const char *fname,
        s_material *mat, const float *translation);

    ///
    ///     Default destructor
    ///
    ~c_objMd3();

    ///
    ///     Returns the object's mesh
    ///
    c_object *getMesh() const               { return(m_mesh); }

    ///
    ///     Fills-in data to prepare the object's mesh
    ///
    void update(const float elapsed, md3_state *state);
};

#endif

