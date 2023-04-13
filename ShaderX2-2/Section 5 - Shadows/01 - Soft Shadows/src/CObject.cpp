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

#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <d3d9.h>
#include <d3dx9.h> 
#include <math.h>

#include "CObject.h"
#include "CUtils.h"

///
///     Calculates a plane from 3 vertices
///
void c_object::calculatePlane(float *plane, const s_stdVertex& v0, const s_stdVertex& v1,
    const s_stdVertex& v2)
{
    plane[0] = v0.m_pos[1]*(v1.m_pos[2]-v2.m_pos[2]) + v1.m_pos[1]*(v2.m_pos[2]-v0.m_pos[2]) + v2.m_pos[1]*(v0.m_pos[2]-v1.m_pos[2]);
	plane[1] = v0.m_pos[2]*(v1.m_pos[0]-v2.m_pos[0]) + v1.m_pos[2]*(v2.m_pos[0]-v0.m_pos[0]) + v2.m_pos[2]*(v0.m_pos[0]-v1.m_pos[0]);
	plane[2] = v0.m_pos[0]*(v1.m_pos[1]-v2.m_pos[1]) + v1.m_pos[0]*(v2.m_pos[1]-v0.m_pos[1]) + v2.m_pos[0]*(v0.m_pos[1]-v1.m_pos[1]);
	plane[3] = -( v0.m_pos[0]*( v1.m_pos[1]*v2.m_pos[2] - v2.m_pos[1]*v1.m_pos[2] ) +
				v1.m_pos[0]*(v2.m_pos[1]*v0.m_pos[2] - v0.m_pos[1]*v2.m_pos[2]) +
				v2.m_pos[0]*(v0.m_pos[1]*v1.m_pos[2] - v1.m_pos[1]*v0.m_pos[2]) );
}

///
///     Renders the object
///
bool c_object::render(const int primID)
{
    int start = m_primsData[primID].m_start;
	int size = m_primsData[primID].m_size;
    c_texture *tex = m_primsData[primID].m_mat->m_tex;

    FFAIL(m_lpDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
    tex->bind(0);
    tex->modulate(0);
    FFAIL(m_lpDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
    FFAIL(m_lpDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP));
    
    FFAIL(m_lpDevice->SetStreamSource(0, m_stdVB[m_curFrame], 0, sizeof(s_stdVertex)));
    FFAIL(m_lpDevice->SetFVF(STD_VERTEX_FORMAT));

    FFAIL(m_lpDevice->SetIndices(m_stdIB));
    FFAIL(m_lpDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_nbVerts, start, size / 3));

    return true;
}
bool c_object::render2(const int primID)
{
    int start = m_primsData[primID].m_start;
	int size = m_primsData[primID].m_size;
    c_texture *tex = m_primsData[primID].m_mat->m_tex;

    m_lpDevice->SetStreamSource(0, m_extVB[m_curFrame], 0, sizeof(s_extVertex));
    m_lpDevice->SetIndices(m_stdIB);
    m_lpDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_nbVerts, start, size / 3);

    return true;
}
bool c_object::render1(const int primID)
{
    int start = m_primsData[primID].m_start;
	int size = m_primsData[primID].m_size;

    m_lpDevice->SetStreamSource(0, m_stdVB[m_curFrame], 0, sizeof(s_stdVertex));
    m_lpDevice->SetIndices(m_stdIB);
    m_lpDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_nbVerts, start, size / 3);

    return true;
}
bool c_object::render3(const int primID)
{
    int start = m_primsData[primID].m_start;
	int size = m_primsData[primID].m_size;
    c_texture *tex = m_primsData[primID].m_mat->m_tex;

    m_lpDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    tex->bind(0);
    tex->modulate(0);
    m_lpDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    m_lpDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    
    m_lpDevice->SetStreamSource(0, m_stdVB[m_curFrame], 0, sizeof(s_stdVertex));
    m_lpDevice->SetFVF(STD_VERTEX_FORMAT);
    m_lpDevice->SetIndices(m_stdIB);
    m_lpDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_nbVerts, start, size / 3);

    return true;
}
bool c_object::renderDbg(const int primID)
{
    int start = m_primsData[primID].m_start;
	int size = m_primsData[primID].m_size;

    m_lpDevice->SetStreamSource(0, m_dbgVB, 0, sizeof(s_colVertex1));
    m_lpDevice->SetFVF(COL_VERTEX_FORMAT1);
    m_lpDevice->DrawPrimitive(D3DPT_LINELIST, 0, m_nbVerts);

    return true;
}

///
///     Creates an empty object
///
c_object::c_object(IDirect3DDevice9 *a_device)
{
    m_lpDevice = a_device;

    m_curFrame = 0;
    m_nbFrames = 1;
    m_nbVerts = 0;
    for (int f = 0; f < 128; f++)
    {
        m_verts[f] = NULL;
        m_stdVB[f] = NULL;
        m_extVB[f] = NULL;
    }
    m_nbIndices = 0;
    m_indices = NULL;
    m_stdIB = NULL;
    m_extIB = NULL;
    m_nbExtIndices = 0;

    m_nbPrims = 0;
    m_primsData = NULL;

    m_connVerts = NULL;
    m_nbConnVerts = 0;
    m_connTris = NULL;
}

///
///		Creates the mesh of the object
///
bool c_object::create()
{
    /// get bounding box:
    copy3f(m_min, m_verts[0][0].m_pos);
    copy3f(m_max, m_verts[0][0].m_pos);
    copy3f(m_center, m_verts[0][0].m_pos);
    m_center[3] = 1.0f;
    for (int i = 1; i < m_nbVerts; i++)
    {
        const float *v = m_verts[0][i].m_pos;
        if (v[0] < m_min[0])
            m_min[0] = v[0];
        if (v[1] < m_min[1])
            m_min[1] = v[1];
        if (v[2] < m_min[2])
            m_min[2] = v[2];
        if (v[0] > m_max[0])
            m_max[0] = v[0];
        if (v[1] > m_max[1])
            m_max[1] = v[1];
        if (v[2] > m_max[2])
            m_max[2] = v[2];

        add3f(m_center, v);
        m_center[3]++;
    }
    m_center[0] /= m_center[3];
    m_center[1] /= m_center[3];
    m_center[2] /= m_center[3];

    genConnectivity();

    /// Generate IBs:
    FFAIL(m_lpDevice->CreateIndexBuffer(m_nbIndices * sizeof(int),
        0, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &m_stdIB, NULL));
    int *iptr = NULL;
    FFAIL(m_stdIB->Lock(0, 0, (void **)(&iptr), 0));
    memcpy(iptr, m_indices, m_nbIndices * sizeof(int));
    FFAIL(m_stdIB->Unlock());

    /// Generate extrusion VBs:
    for (int f = 0; f < m_nbFrames; f++)
    {
        char str[256];
        sprintf(str, "Generating extrusion data for frame %d / %d\n\r", f, m_nbFrames);
        warning(str);

        FFAIL(m_lpDevice->CreateVertexBuffer(m_nbVerts * 2 * sizeof(s_extVertex),
            D3DUSAGE_WRITEONLY, EXT_VERTEX_FORMAT, D3DPOOL_DEFAULT, &(m_extVB[f]), NULL));
        s_extVertex *evptr = NULL;
        FFAIL(m_extVB[f]->Lock(0, 0, (void **)(&evptr), 0));

        /// fusion normals:
        for (i = 0; i < m_nbVerts; i++)
        {
            set3f(evptr[i].m_normal, 0);
            evptr[i].m_weight = 0;
        }
        copy3f(evptr[0].m_normal, m_verts[f][0].m_normal);
        evptr[0].m_weight = 1.0f;
        for (i = 1; i < m_nbVerts; i++)
        {
            for (int j = 0; j < i; j++)
            {
                if (vecEq(m_verts[f][i].m_pos, m_verts[f][j].m_pos))
                {
                    evptr[j].m_weight += 1.0f;
                    add3f(evptr[j].m_normal, m_verts[f][i].m_normal);
                    break;
                }
            }
            if (j == i)
            {
                copy3f(evptr[i].m_normal, m_verts[f][i].m_normal);
                evptr[i].m_weight = 1.0f;
            }
        }
        for (i = 0; i < m_nbVerts; i++)
        {
            if (evptr[i].m_weight > 0)
            {
                evptr[i].m_normal[0] /= evptr[i].m_weight;
                evptr[i].m_normal[1] /= evptr[i].m_weight;
                evptr[i].m_normal[2] /= evptr[i].m_weight;
                normalize(evptr[i].m_normal[0], evptr[i].m_normal[1], evptr[i].m_normal[2]);
            }
        }
        for (i = 1; i < m_nbVerts; i++)
        {
            for (int j = 0; j < i; j++)
            {
                if (vecEq(m_verts[f][i].m_pos, m_verts[f][j].m_pos))
                {
                    assert(evptr[i].m_weight == 0);
                    copy3f(evptr[i].m_normal, evptr[j].m_normal);
                    break;
                }
            }
        }
        for (i = 0; i < m_nbVerts; i++)
        {
            for (int j = 0; j < m_nbVerts; j++)
            {
                if (vecEq(m_verts[f][i].m_pos, m_verts[f][j].m_pos))
                {
                    assert(vecEq(evptr[i].m_normal, evptr[j].m_normal));
                }
            }
        }
        for (i = 0; i < m_nbVerts; i++)
        {
            copy3f(evptr[i].m_pos, m_verts[f][i].m_pos);
            copy3f(evptr[i + m_nbVerts].m_pos, m_verts[f][i].m_pos);
            copy3f(evptr[i + m_nbVerts].m_normal, evptr[i].m_normal);
            evptr[i].m_weight = 0.0005f;
            evptr[i + m_nbVerts].m_weight = 1.0f;

            /*
            /// experimental jitter:
            copy3f(evptr[i].m_normal, m_verts[f][i].m_normal);
            copy3f(evptr[i + m_nbVerts].m_normal, m_verts[f][i].m_normal);
            */
//            copy3f(m_verts[f][i].m_normal, evptr[i].m_normal);
        }
        FFAIL(m_extVB[f]->Unlock());
    }

    /// Generate VBs:
    for (f = 0; f < m_nbFrames; f++)
    {
        FFAIL(m_lpDevice->CreateVertexBuffer(m_nbVerts * sizeof(s_stdVertex),
            D3DUSAGE_WRITEONLY, STD_VERTEX_FORMAT, D3DPOOL_DEFAULT, &(m_stdVB[f]), NULL));
        s_stdVertex *vptr = NULL;
        FFAIL(m_stdVB[f]->Lock(0, 0, (void **)(&vptr), 0));
        memcpy(vptr, m_verts[f], m_nbVerts * sizeof(s_stdVertex));
        FFAIL(m_stdVB[f]->Unlock());
    }

    FFAIL(m_lpDevice->CreateVertexBuffer(m_nbVerts * 2 * sizeof(s_colVertex1),
        D3DUSAGE_WRITEONLY, COL_VERTEX_FORMAT1, D3DPOOL_DEFAULT, &m_dbgVB, NULL));
    s_colVertex1 *cvptr = NULL;
    FFAIL(m_dbgVB->Lock(0, 0, (void **)(&cvptr), 0));
    for (i = 0; i < m_nbVerts; i++)
    {
        copy3f(cvptr[i * 2 + 0].m_pos, m_verts[0][i].m_pos);
        cvptr[i * 2 + 0].m_color = 0xFF00FF00;
        copy3f(cvptr[i * 2 + 1].m_pos, m_verts[0][i].m_pos);
        cvptr[i * 2 + 1].m_color = 0xFF00FF00;
        cvptr[i * 2 + 1].m_pos[0] += m_verts[0][i].m_normal[0] * 5.0f;
        cvptr[i * 2 + 1].m_pos[1] += m_verts[0][i].m_normal[1] * 5.0f;
        cvptr[i * 2 + 1].m_pos[2] += m_verts[0][i].m_normal[2] * 5.0f;
    }
    FFAIL(m_dbgVB->Unlock());

    int l_maxExtIndices = ((m_nbIndices / 3) * ((3 * 6) + (2 * 3)));
    FFAIL(m_lpDevice->CreateIndexBuffer(l_maxExtIndices * sizeof(int),
        0, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &m_extIB, NULL));

    return true;
}

///
///     Destroys the object
///
c_object::~c_object()
{
    for (int f = 0; f < m_nbFrames; f++)
    {
        if (m_stdVB[f])
            m_stdVB[f]->Release();
        if (m_extVB[f])
            m_extVB[f]->Release();
        if (m_verts[f])
            free(m_verts[f]);
    }
    if (m_stdIB)
        m_stdIB->Release();
    if (m_extIB)
        m_extIB->Release();

    if (m_indices)
        free(m_indices);
    if (m_primsData)
        free(m_primsData);
}

///
///		Sets the position of the light source illuminating the object
///     (in object space)
///
void c_object::setLightSource(const float *lPos)
{
    copy3f(m_lightPos, lPos);
}

///
///		Sets the position of the viewer looking at this object
///
void c_object::setViewer(const float *a_pos)
{
    copy3f(m_viewPos, a_pos);
}

///
///     Generates and draws a shadow volume for this object
///
bool c_object::extend()
{
    /// Generate the indices for shadow volume faces

    int *eiptr = NULL;
    HRESULT hRes = m_extIB->Lock(0, 0, (void **)(&eiptr), 0);
    if (FAILED(hRes))
        return false;

    m_nbExtIndices = 0;
    for (int i = 0; i < m_nbIndices / 3; i++)
    {
        int v0 = m_connTris[i].m_verts[0];
        int v1 = m_connTris[i].m_verts[1];
        int v2 = m_connTris[i].m_verts[2];

        if (!m_connTris[i].m_visible)
        {
            eiptr[m_nbExtIndices++] = v0 + m_nbVerts;
            eiptr[m_nbExtIndices++] = v1 + m_nbVerts;
            eiptr[m_nbExtIndices++] = v2 + m_nbVerts;
            continue;
        }
        else
        {
            eiptr[m_nbExtIndices++] = v0;
            eiptr[m_nbExtIndices++] = v1;
            eiptr[m_nbExtIndices++] = v2;
        }

        for (int e = 0; e < 3; e++)
        {
            int adjTri = m_connTris[i].m_edges[e];

            if (adjTri == -1 || !m_connTris[adjTri].m_visible)
            {
                /// edge of shadow volume

                int v0 = m_connTris[i].m_verts[e];
                int v1 = m_connTris[i].m_verts[(e + 1) % 3];
                
                eiptr[m_nbExtIndices++] = v1;
                eiptr[m_nbExtIndices++] = v0;
                eiptr[m_nbExtIndices++] = v0 + m_nbVerts;
                
                eiptr[m_nbExtIndices++] = v1;
                eiptr[m_nbExtIndices++] = v0 + m_nbVerts;
                eiptr[m_nbExtIndices++] = v1 + m_nbVerts;
            }
        }
    }

    hRes = m_extIB->Unlock();
    if (FAILED(hRes))
        return false;

    return true;
}

///
///     Draws the shadow volume
///
bool c_object::drawShadows()
{
    m_lpDevice->SetStreamSource(0, m_extVB[m_curFrame], 0, sizeof(s_extVertex));
    m_lpDevice->SetIndices(m_extIB);
    m_lpDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_nbVerts * 2, 0,
        m_nbExtIndices / 3);

    return true;
}

///
///     Compares 2 connected vertices
///
int c_object::connVertComp(
    const void          *arg1,              // first entity
    const void          *arg2               // second entity
    )
{
    s_connVert *v1 = (s_connVert *)arg1;
    s_connVert *v2 = (s_connVert *)arg2;

    if (v1->m_pos[0] < v2->m_pos[0])
        return(-1);

    if (v1->m_pos[0] > v2->m_pos[0])
        return(1);

    if (v1->m_pos[1] < v2->m_pos[1])
        return(-1);

    if (v1->m_pos[1] > v2->m_pos[1])
        return(1);

    if (v1->m_pos[2] < v2->m_pos[2])
        return(-1);

    if (v1->m_pos[2] > v2->m_pos[2])
        return(1);

    return(0);
}

///     
///     Generates the connectivity informations for the triangles
///
void c_object::genConnectivity()
{
    m_connTris = new s_connTri[m_nbIndices / 3];
    int nbTris = m_nbIndices / 3;
    m_connVerts = new s_connVert[m_nbVerts];
    m_nbConnVerts = m_nbVerts;

    for (int f = 0; f < m_nbFrames; f++)
    {
        m_planes[f] = new float[nbTris * 4];
    }

    char str[256];
    sprintf(str, "Gen connectivity for %d triangles..\n", nbTris);
    warning(str);

    for (int i = 0; i < nbTris; i++)
    {
        int v0 = m_indices[i * 3 + 0];
        int v1 = m_indices[i * 3 + 1];
        int v2 = m_indices[i * 3 + 2];
        m_connTris[i].m_verts[0] = v0;
        m_connTris[i].m_verts[1] = v1;
        m_connTris[i].m_verts[2] = v2;
        sprintf(str, "  Triangle %d: (%d %d %d)\n", i, v0, v1, v2);
        warning(str);

        for (int f = 0; f < m_nbFrames; f++)
        {
            calculatePlane(&m_planes[f][i * 4], m_verts[f][v0], m_verts[f][v1],
                m_verts[f][v2]);
            normalizePlane(&m_planes[f][i * 4]);
        }

        m_connTris[i].m_visible = true;
        m_connTris[i].m_edges[0] = -1;
        m_connTris[i].m_edges[1] = -1;
        m_connTris[i].m_edges[2] = -1;
        m_connTris[i].m_count[0] = 1;
        m_connTris[i].m_count[1] = 1;
        m_connTris[i].m_count[2] = 1;
    }
    for (i = 0; i < m_nbVerts; i++)
    {
        sprintf(str, "  Vertex %d: (%.2f %.2f %.2f)\n", i,
            m_verts[0][i].m_pos[0], m_verts[0][i].m_pos[1], m_verts[0][i].m_pos[2]);
        warning(str);

        copy3f(m_connVerts[i].m_pos, m_verts[0][i].m_pos);
        m_connVerts[i].m_count = 1;
        m_connVerts[i].m_id = i;
    }

    /// Fusion the vertex positions, to have a correctly smoothed mesh silhouette..
    qsort(m_connVerts, m_nbVerts, sizeof(s_connVert), connVertComp);
    int *assoc = new int[m_nbVerts];
    for (i = 0; i < m_nbVerts; i++)
    {
        assoc[m_connVerts[i].m_id] = i;
    }
    int start = 0;

    for (i = 1; i < m_nbVerts; i++)
    {
        /// fusion normals that are the same

        if (connVertComp(&(m_connVerts[i - 1]), &(m_connVerts[i])) == 0)
        {
            assoc[m_connVerts[i].m_id] = start;
            m_connVerts[start].m_count++;
        }
        else
            start = i;
    }
    for (i = 0; i < nbTris; i++)
    {
        /// update faces indices
        m_connTris[i].m_cverts[0] = assoc[m_connTris[i].m_verts[0]];
        m_connTris[i].m_cverts[1] = assoc[m_connTris[i].m_verts[1]];
        m_connTris[i].m_cverts[2] = assoc[m_connTris[i].m_verts[2]];
    }
    delete [] assoc;

    /// generate neighbooring infos..
    for (i = 0; i < nbTris; i++)
    {
        if (i % 100 == 0)
        {
            sprintf(str, "%d / %d..\n", i, nbTris);
            warning(str);
        }

        if (m_connTris[i].m_verts[0] == -1)
            continue;

        for (int e0 = 0; e0 < 3; e0++)
        {
            int ev0 = m_connTris[i].m_verts[e0];
            int ev1 = m_connTris[i].m_verts[(e0 + 1) % 3];

            for (int j = i + 1; j < nbTris; j++)
            {
                if (m_connTris[j].m_verts[0] == -1)
                    continue;

                for (int e1 = 0; e1 < 3; e1++)
                {
                    int ev2 = m_connTris[j].m_verts[e1];
                    int ev3 = m_connTris[j].m_verts[(e1 + 1) % 3];

                    if ((vecEq(m_verts[0][ev0].m_pos, m_verts[0][ev2].m_pos)
                     && vecEq(m_verts[0][ev1].m_pos, m_verts[0][ev3].m_pos)) ||
                        (vecEq(m_verts[0][ev0].m_pos, m_verts[0][ev3].m_pos)
                     && vecEq(m_verts[0][ev1].m_pos, m_verts[0][ev2].m_pos)))
                    {
                        /// set link..

                        if (m_connTris[i].m_edges[e0] != -1 || m_connTris[j].m_edges[e1] != -1)
                        {
                            char str[256];
                            sprintf(str, "ev0 = %.3f %.3f %.3f (%f %f %f)\n",
                                m_verts[0][ev0].m_pos[0],
                                m_verts[0][ev0].m_pos[1],
                                m_verts[0][ev0].m_pos[2],
                                m_verts[0][ev0].m_pos[0],
                                m_verts[0][ev0].m_pos[1],
                                m_verts[0][ev0].m_pos[2]);
                            OutputDebugString(str);
                            sprintf(str, "ev1 = %.3f %.3f %.3f (%f %f %f)\n",
                                m_verts[0][ev1].m_pos[0],
                                m_verts[0][ev1].m_pos[1],
                                m_verts[0][ev1].m_pos[2],
                                m_verts[0][ev1].m_pos[0],
                                m_verts[0][ev1].m_pos[1],
                                m_verts[0][ev1].m_pos[2]);
                            OutputDebugString(str);
                            sprintf(str, "ev2 = %.3f %.3f %.3f (%f %f %f)\n",
                                m_verts[0][ev2].m_pos[0],
                                m_verts[0][ev2].m_pos[1],
                                m_verts[0][ev2].m_pos[2],
                                m_verts[0][ev2].m_pos[0],
                                m_verts[0][ev2].m_pos[1],
                                m_verts[0][ev2].m_pos[2]);
                            OutputDebugString(str);
                            sprintf(str, "ev3 = %.3f %.3f %.3f (%f %f %f)\n",
                                m_verts[0][ev3].m_pos[0],
                                m_verts[0][ev3].m_pos[1],
                                m_verts[0][ev3].m_pos[2],
                                m_verts[0][ev3].m_pos[0],
                                m_verts[0][ev3].m_pos[1],
                                m_verts[0][ev3].m_pos[2]);
                            OutputDebugString(str);

                            sprintf(str, "Object is not 2-manifold: 3-triangle edge:\nX%.3f Y%.3f Z%.3f\nto\nX%.3f Y%.3f Z%.3f",
                                m_verts[0][ev0].m_pos[2], m_verts[0][ev0].m_pos[0], m_verts[0][ev0].m_pos[1],
                                m_verts[0][ev1].m_pos[2], m_verts[0][ev1].m_pos[0], m_verts[0][ev1].m_pos[1]);
                            MessageBox(0, str, "Can't import object", 0);
                            exit(1036);
                        }

                        m_connTris[i].m_edges[e0] = j;
                        m_connTris[j].m_edges[e1] = i;

                        // add normals of neighboor to each other's..
                        m_connTris[i].m_count[e0]++;
                        m_connTris[i].m_count[(e0 + 1) % 3]++;

                        m_connTris[j].m_count[e1]++;
                        m_connTris[j].m_count[(e1 + 1) % 3]++;
                    }
                }
            }
        }
    }

    sprintf(str, "Checking %d triangles manifold..\n", nbTris);
    warning(str);

    for (i = 0; i < nbTris; i++)
    {
        for (int e = 0; e < 3; e++)
        {
            if (m_connTris[i].m_edges[e] == -1)
            {
                int v0 = m_connTris[i].m_verts[e];
                int v1 = m_connTris[i].m_verts[(e + 1) % 3];

                char str[256];
                sprintf(str, "Object is not 2-manifold: hole along edge:\nX%.3f Y%.3f Z%.3f\nto\nX%.3f Y%.3f Z%.3f",
                    m_verts[0][v0].m_pos[2], m_verts[0][v0].m_pos[0], m_verts[0][v0].m_pos[1],
                    m_verts[0][v1].m_pos[2], m_verts[0][v1].m_pos[0], m_verts[0][v1].m_pos[1]);
                MessageBox(0, str, "Can't import object", 0);
                exit(1036);
            }
        }
    }
}

///
///     Finds the faces visible from the light
///
void c_object::visFromLight()
{
    for (int i = 0; i < m_nbIndices / 3; i++)
    {
        const float *plane = &(m_planes[m_curFrame][i * 4]);
        m_connTris[i].m_visible = (plane[0] * m_lightPos[0] + plane[1] * m_lightPos[1] +
            plane[2] * m_lightPos[2] + plane[3]) > 0;
    }
}

///
///     Creates the mesh from another mesh
///
bool c_object::create(const char *name, const int nbVerts, const int nbFrames,
    const float *xyz, const float *uv, const float *norm, const int nbTris,
    const int *indices, s_material *mat)
{
    strcpy(m_name, name);

    m_nbVerts = nbVerts;
    m_curFrame = 0;
    m_nbFrames = nbFrames;

    for (int f = 0; f < nbFrames; f++)
    {
        m_verts[f] = (s_stdVertex *)malloc(sizeof(s_stdVertex) * nbVerts);
	    memset(m_verts[f], 0, nbVerts * sizeof(s_stdVertex));

        for (int i = 0; i < m_nbVerts; i++)
        {
            memcpy(m_verts[f][i].m_pos, xyz + (f * nbVerts * 3) + i * 3, 3 * sizeof(float));
            memcpy(m_verts[f][i].m_normal, norm + (f * nbVerts * 3) + i * 3, 3 * sizeof(float));
            memcpy(m_verts[f][i].m_tc, uv + i * 2, 2 * sizeof(float));
        }
    }

    m_indices = (int *)malloc(sizeof(int) * nbTris * 3);
    m_nbIndices = nbTris * 3;
    memcpy(m_indices, indices, sizeof(int) * m_nbIndices);

    s_material *sceneMat = c_scene::ms_scene->addMaterial(mat);
    m_nbPrims = 1;
    m_primsData = (s_primitive *)malloc(sizeof(s_primitive));
    m_primsData[0].m_start = 0;
    m_primsData[0].m_mat = sceneMat;
    m_primsData[0].m_size = nbTris * 3;

    return(create());
}
