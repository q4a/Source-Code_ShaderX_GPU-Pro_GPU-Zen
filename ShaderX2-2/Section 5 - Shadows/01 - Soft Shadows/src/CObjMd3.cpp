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

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <d3d9.h>
#include <d3dx9.h> 
#include <d3dx9shader.h>

#include "CScene.h"
#include "CUtils.h"
#include "CEntity.h"
#include "CObjMd3.h"

///
///     Creates the object
///
bool c_objMd3::create(const char *name, const char *fname, s_material *mat,
    const float *translation)
{
    md3_fileHeader fh;
    char str[256];

    FILE *f = fopen(fname, "rb");

    fread(&fh, sizeof(md3_fileHeader), 1, f);
    if (fh.ident != (('3' << 24) + ('P' << 16) + ('D' << 8) + 'I'))
    {
        fclose(f);
        return false;
    }

    if (fh.version != 15)
    {
        fclose(f);
        return false;
    }

	fh.numFrames = 1;
    m_nbFrames = fh.numFrames;
    m_nbTags = fh.numTags;
    m_nbSurfaces = fh.numSurfaces;
    m_nbSkins = fh.numSkins;

    if (m_nbFrames > 1024 || m_nbSurfaces > 64 || m_nbTags > 16 || m_nbSkins > 16)
    {
        fclose(f);
        return false;
    }

    /// read frames
    m_frames = new md3_frame[m_nbFrames];
    fseek(f, fh.ofsFrames, SEEK_SET);
    for (int i = 0; i < m_nbFrames; i++)
    {
        fread(m_frames[i].bounds, sizeof(float) * 3, 2, f);
        fread(m_frames[i].origin, sizeof(float) * 3, 1, f);
        fread(&(m_frames[i].radius), sizeof(float), 1, f);
        fread(m_frames[i].name, sizeof(char) * 64, 1, f);
    }

    /// read surfaces
    m_surfs = new md3_surf[m_nbSurfaces];
    int surfSeek = fh.ofsSurfaces;
    for (i = 0; i < m_nbSurfaces; i++)
    {
        md3_surfHeader sh;
        fseek(f, surfSeek, SEEK_SET);
        fread(&sh, sizeof(md3_surfHeader), 1, f);
        
        sprintf(str, "Reading '%s', %d vertices, %d triangles..\n",
            sh.name, sh.numVerts, sh.numTriangles);
        OutputDebugString(str);

        strcpy(m_surfs[i].name, sh.name);
        m_surfs[i].numFrames = sh.numFrames;
        m_surfs[i].numTris = sh.numTriangles;
        m_surfs[i].numVerts = sh.numVerts;
        m_surfs[i].indices = new int[sh.numTriangles * 3];

        if (sh.ofsTriangles == sh.ofsSt)
            exit(36);

        fseek(f, surfSeek + sh.ofsTriangles, SEEK_SET);
        fread(m_surfs[i].indices, 1, 3 * sh.numTriangles * sizeof(int), f);
        for (int k = 0; k < 3 * sh.numTriangles; k++)
        {
            if (m_surfs[i].indices[k] < 0 || m_surfs[i].indices[k] >= sh.numVerts)
            {
                fclose(f);
                return false;
            }
        }

        fseek(f, surfSeek + sh.ofsSt, SEEK_SET);
        m_surfs[i].tex = new float[2 * sh.numVerts];
        fread(m_surfs[i].tex, 2 * sh.numVerts * sizeof(float), 1, f);
        for (k = 0; k < sh.numVerts; k++)
        {
            m_surfs[i].tex[k * 2 + 1] = 1.0f - m_surfs[i].tex[k * 2 + 1];
        }

        fseek(f, surfSeek + sh.ofsXyzNormals, SEEK_SET);
        m_surfs[i].pos = new float[3 * sh.numVerts * sh.numFrames];
        m_surfs[i].norm = new float[3 * sh.numVerts * sh.numFrames];

        for (k = 0; k < sh.numVerts * sh.numFrames; k++)
        {
            const float scale = (1.0f / 55.6f);
            short xyz[3];
            short normal;
            fread(xyz, sizeof(short) * 3, 1, f);
            fread(&normal, sizeof(short), 1, f);

            m_surfs[i].pos[k * 3 + 0] = xyz[1] * scale;
            m_surfs[i].pos[k * 3 + 1] = xyz[2] * scale;
            m_surfs[i].pos[k * 3 + 2] = xyz[0] * scale;
            unsigned char *n = (unsigned char *)(&normal);
            float longitude = n[1] * (2 * 3.1415927f / 255.0f);
            float latitude = n[0] * (2 * 3.1415927f / 255.0f);
            m_surfs[i].norm[k * 3 + 1] = cosf(latitude) * sinf(longitude);
            m_surfs[i].norm[k * 3 + 0] = sinf(latitude) * sinf(longitude);
            m_surfs[i].norm[k * 3 + 2] = cosf(longitude);
            
            if (k < sh.numVerts)
            {
                char str[256];
                sprintf(str, "Normal %d: %.3f %.3f %.3f (%.3f %.3f) (%d %d)\n", k,
                    m_surfs[i].norm[k * 3 + 0],
                    m_surfs[i].norm[k * 3 + 1],
                    m_surfs[i].norm[k * 3 + 2],
                    latitude, longitude,
                    n[0], n[1]);
                warning(str);
            }
        }

        surfSeek += sh.ofsEnd;
    }

    fclose(f);

    /// Fix normals: smooth all normals!
    /// MD3 normals don't seem correct on some models, wtf?
    /// TODO: find a good MD3 exporter

    float *nCount = new float[m_surfs[0].numVerts];

    for (i = 0; i < m_surfs[0].numFrames; i++)
    {
        float *pos = m_surfs[0].pos + m_surfs[0].numVerts * i * 3;
        float *normals = m_surfs[0].norm + m_surfs[0].numVerts * i * 3;

        for (int j = 0; j < m_surfs[0].numVerts; j++)
        {
            set3f(normals + j * 3, 0);
            nCount[j] = 0;
        }

        for (j = 0; j < m_surfs[0].numTris * 3; j += 3)
        {
            int i0 = m_surfs[0].indices[j + 0];
            int i1 = m_surfs[0].indices[j + 1];
            int i2 = m_surfs[0].indices[j + 2];

            float plane[4];
            calculatePlane(plane, pos + i0 * 3, pos + i1 * 3, pos + i2 * 3);
            normalize(plane[0], plane[1], plane[2]);

            for (int k = 0; k < m_surfs[0].numVerts; k++)
            {
                if (vecEq(pos + i0 * 3, pos + k * 3))
                {
                    add3f(normals + k * 3, plane);
                    nCount[k]++;
                }
                if (vecEq(pos + i1 * 3, pos + k * 3))
                {
                    add3f(normals + k * 3, plane);
                    nCount[k]++;
                }
                if (vecEq(pos + i2 * 3, pos + k * 3))
                {
                    add3f(normals + k * 3, plane);
                    nCount[k]++;
                }
            }
        }

        for (j = 0; j < m_surfs[0].numVerts; j++)
        {
            normals[j * 3 + 0] = -normals[j * 3 + 0] / nCount[j];
            normals[j * 3 + 1] = -normals[j * 3 + 1] / nCount[j];
            normals[j * 3 + 2] = -normals[j * 3 + 2] / nCount[j];

            add3f(pos + j * 3, translation);
        }
    }


    /// Step2: create a dummy c_object. We will fill it with the interpolated
    /// vertices at run-time. That way we can still use all the functions of
    /// c_object !!!

    m_mesh = new c_object(m_lpDevice);
    return(m_mesh->create(name, m_surfs[0].numVerts, m_surfs[0].numFrames, m_surfs[0].pos,
        m_surfs[0].tex, m_surfs[0].norm, m_surfs[0].numTris, m_surfs[0].indices, mat));
}

///
///     Registers an animation step
///
void c_objMd3::registerStep(const int start, const int end, const int next)
{
    m_steps = (md3_step *)realloc(m_steps, sizeof(md3_step) * (m_nbSteps + 1));
    m_steps[m_nbSteps].m_stepID = m_nbSteps;
    m_steps[m_nbSteps].m_startFrame = start;
    m_steps[m_nbSteps].m_endFrame = end;
    m_steps[m_nbSteps].m_nextStepID = next;
    m_nbSteps++;
}

///
///     Default constructor
///
c_objMd3::c_objMd3(IDirect3DDevice9 *a_device, const char *name, const char *fname,
    s_material *mat, const float *translation)
{
    m_lpDevice = a_device;
    m_mesh = NULL;
    m_steps = NULL;
    m_nbSteps = 0;
    create(name, fname, mat, translation);
    registerStep(0, m_nbFrames - 1, 0);
}

///
///     Default destructor
///
c_objMd3::~c_objMd3()
{
}

///
///     Fills-in data to prepare the object's mesh
///
void c_objMd3::update(const float elapsed, md3_state *state)
{
    float tinc = 20.0f * elapsed;
    if (tinc > 200.0f)
        tinc = 200.0f;

    state->m_time += tinc;
    
    while (state->m_time >= 1.0f)
    {
        state->m_time -= 1.0f;
        state->m_start = state->m_end;

        if (m_steps[state->m_stepID].m_endFrame == state->m_start)
        {
            /// jump to next step
            state->m_stepID = m_steps[state->m_stepID].m_nextStepID;
            state->m_end = m_steps[state->m_stepID].m_startFrame;
        }
        else
            state->m_end++;
    }

//    m_mesh->m_curFrame = state->m_start;

	/// No MD3 animations, due to lack of time..
    m_mesh->m_curFrame = 0;
}
