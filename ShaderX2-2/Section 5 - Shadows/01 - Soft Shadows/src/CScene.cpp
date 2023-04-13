///
///     Scene file
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
///
///		Shaders description:
///		ZBuff vertex shader: simple vertex shader to do T&L of vertex positions only.
///		Shadow volume vertex shader: vertex shader to do light jittering and volume extrusion.
///		Paint vertex shader: final vertex shader to evaluate the lighting equation (do
///			screen-space projection too)
///		Lights vertex shader: additionall lighting pass for 4 smaller lights (non-casting
///			shadows)
///		Blur pixel shader: pixel shader to blur the shadow maps
///		Final pixel shader: pixel shader to evaluate the lighting equation
///

//#define INFLATE
#define SHOW_LIGHTS

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

c_scene *c_scene::ms_scene = NULL;          // main scene

extern float xpos, ypos, zpos;
extern float yaw ,pitch;
extern float g_shadowScale;
extern float g_penumbra;
extern int g_count;
extern float g_fix1;
extern float g_fix2;
extern float g_lightOffset;

extern bool g_hasPS14;
extern bool g_hasVS11;

float tt = 0;

#define MAP_SIZE        512

///
///     Compares 2 rendering commands
///
int c_scene::cmdComp(
    const void          *arg1,              // first entity
    const void          *arg2               // second entity
    )
{
    s_cmd *cmd1 = (s_cmd *)arg1;
    s_cmd *cmd2 = (s_cmd *)arg2;

    if (cmd1->m_dist < cmd2->m_dist)
        return(1);

    if (cmd1->m_dist > cmd2->m_dist)
        return(-1);

    if (cmd1->m_mat < cmd2->m_mat)
        return(1);
    
    if (cmd1->m_mat > cmd2->m_mat)
        return(-1);

    return(0);
}

///
///     Creates an empty scene
///
c_scene::c_scene(IDirect3DDevice9 *a_device)
{
    ms_scene = this;

    m_lpDevice = a_device;

    m_nbObjs = 0;
    m_nbLights = 0;
    m_nbMats = 0;
    m_nbEntities = 0;
    m_nbCmds = 0;

    /// Create vertex shader for shadow volumes extrusion:

    D3DVERTEXELEMENT9 declShadowVolume[] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0}, 
        { 0, 12, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_BLENDWEIGHT, 0},        
        { 0, 16, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL, 0},
        D3DDECL_END()
    };

    RFAIL(m_lpDevice->CreateVertexDeclaration(declShadowVolume, &m_shadowVolumeDecl));

    LPD3DXBUFFER shaderBuf;
    LPD3DXBUFFER shaderLog;
    HRESULT hRes = D3DXAssembleShaderFromFile("shadowVolume.vsh", NULL, NULL, D3DXSHADER_DEBUG,
        &shaderBuf, &shaderLog);
    if (FAILED(hRes))
    {
        char *ptr = (char *)shaderLog->GetBufferPointer();
        warning(ptr);
        warning("\n\r");
        exit(1);
        return;
    }

    RFAIL(m_lpDevice->CreateVertexShader((DWORD *)shaderBuf->GetBufferPointer(),
        &m_shadowVolumeShader));
    RFAIL(shaderBuf->Release());

    /// final paint vertex shader:

    hRes = D3DXAssembleShaderFromFile("paint.vsh", NULL, NULL, D3DXSHADER_DEBUG,
        &shaderBuf, &shaderLog);
    if (FAILED(hRes))
    {
        char *ptr = (char *)shaderLog->GetBufferPointer();
        warning(ptr);
        warning("\n\r");
        exit(1);
        return;
    }

    RFAIL(m_lpDevice->CreateVertexShader((DWORD *)shaderBuf->GetBufferPointer(),
        &m_paintShader));
    RFAIL(shaderBuf->Release());

    /// z-buffer vertex shader:

    hRes = D3DXAssembleShaderFromFile("zbuff.vsh", NULL, NULL, D3DXSHADER_DEBUG,
        &shaderBuf, &shaderLog);
    if (FAILED(hRes))
    {
        char *ptr = (char *)shaderLog->GetBufferPointer();
        warning(ptr);
        warning("\n\r");
        exit(1);
        return;
    }

    RFAIL(m_lpDevice->CreateVertexShader((DWORD *)shaderBuf->GetBufferPointer(),
        &m_zbuffShader));
    RFAIL(shaderBuf->Release());

    /// lights vertex shader:

    hRes = D3DXAssembleShaderFromFile("Lights.vsh", NULL, NULL, D3DXSHADER_DEBUG,
        &shaderBuf, &shaderLog);
    if (FAILED(hRes))
    {
        char *ptr = (char *)shaderLog->GetBufferPointer();
        warning(ptr);
        warning("\n\r");
        exit(1);
        return;
    }

    RFAIL(m_lpDevice->CreateVertexShader((DWORD *)shaderBuf->GetBufferPointer(),
        &m_lightShader));
    RFAIL(shaderBuf->Release());

    /// debug vertex shader:

    hRes = D3DXAssembleShaderFromFile("Debug.vsh", NULL, NULL, D3DXSHADER_DEBUG,
        &shaderBuf, &shaderLog);
    if (FAILED(hRes))
    {
        char *ptr = (char *)shaderLog->GetBufferPointer();
        warning(ptr);
        warning("\n\r");
        exit(1);
        return;
    }

    RFAIL(m_lpDevice->CreateVertexShader((DWORD *)shaderBuf->GetBufferPointer(),
        &m_dbgShader));
    RFAIL(shaderBuf->Release());

    /// Full-screen quad VB:
    RFAIL(m_lpDevice->CreateVertexBuffer(4 * sizeof(s_colVertex),
        D3DUSAGE_WRITEONLY, COL_VERTEX_FORMAT, D3DPOOL_DEFAULT, &m_quadVB, NULL));

    s_colVertex quadVertices[4] = {
        {0, 0, 0.5f, 1.0f, 0xFF000000},
        {MAP_SIZE, 0, 0.5f, 1.0f, 0xFF000000},
        {MAP_SIZE, MAP_SIZE, 0.5f, 1.0f, 0xFF000000},
        {0, MAP_SIZE, 0.5f, 1.0f, 0xFF000000}
        };

    float *vptr = NULL;
    RFAIL(m_quadVB->Lock(0, 0, (void **)(&vptr), 0));
    memcpy(vptr, quadVertices, 4 * sizeof(s_colVertex));
    RFAIL(m_quadVB->Unlock());

    /// Render to texture:
    
    for (int i = 0; i < 3; i ++)
    {
        RFAIL(m_lpDevice->CreateTexture(MAP_SIZE, MAP_SIZE, 0,
            D3DUSAGE_RENDERTARGET,
            D3DFMT_A8R8G8B8,
            D3DPOOL_DEFAULT,
            &(m_shadowMap[i]), NULL));
    }

    for (i = 0; i < 2; i++)
    {
        RFAIL(m_lpDevice->CreateTexture(MAP_SIZE, MAP_SIZE, 0,
            D3DUSAGE_RENDERTARGET,
            D3DFMT_A8R8G8B8,
            D3DPOOL_DEFAULT,
            &(m_lightMap[i]), NULL));
    }

    RFAIL(m_lpDevice->CreateDepthStencilSurface(MAP_SIZE, MAP_SIZE, D3DFMT_D24S8,
        D3DMULTISAMPLE_NONE, 0, TRUE, &m_depthMap, NULL));

    /// Blur quad VB:
    RFAIL(m_lpDevice->CreateVertexBuffer(4 * sizeof(s_blurVertex),
        D3DUSAGE_WRITEONLY, BLUR_VERTEX_FORMAT, D3DPOOL_DEFAULT, &m_blurVB, NULL));

    s_blurVertex blurVertices[4] = {
        {0, 0, 0.5f, 1.0f, 0xFFFFFFFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        {MAP_SIZE, 0, 0.5f, 1.0f, 0xFFFFFFFF, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
        {MAP_SIZE, MAP_SIZE, 0.5f, 1.0f, 0xFFFFFFFF, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {0, MAP_SIZE, 0.5f, 1.0f, 0xFFFFFFFF, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1}
        };

    RFAIL(m_blurVB->Lock(0, 0, (void **)(&vptr), 0));
    memcpy(vptr, blurVertices, 4 * sizeof(s_blurVertex));
    RFAIL(m_blurVB->Unlock());
    
    /// Blurring pixel shader:
    hRes = D3DXAssembleShaderFromFile("blurShader.ps", NULL, NULL, D3DXSHADER_DEBUG,
        &shaderBuf, &shaderLog);
    if (FAILED(hRes))
    {
        char *ptr = (char *)shaderLog->GetBufferPointer();
        warning(ptr);
        warning("\n\r");
        exit(1);
        
        return;
    }

    RFAIL(m_lpDevice->CreatePixelShader((DWORD*)shaderBuf->GetBufferPointer(), 
        &m_blurPixelShader));
    RFAIL(shaderBuf->Release());

    /// Final pixel shader:
    hRes = D3DXAssembleShaderFromFile("finalShader.ps", NULL, NULL, D3DXSHADER_DEBUG,
        &shaderBuf, &shaderLog);
    if (FAILED(hRes))
    {
        char *ptr = (char *)shaderLog->GetBufferPointer();
        warning(ptr);
        warning("\n\r");
        exit(1);
        
        return;
    }

    RFAIL(m_lpDevice->CreatePixelShader((DWORD*)shaderBuf->GetBufferPointer(), 
        &m_finalPixelShader));
    RFAIL(shaderBuf->Release());
}

///
///		Destroys the scene
///
c_scene::~c_scene()
{
    ms_scene = NULL;
}

///
///     Cleans the states
///
void c_scene::clean()
{
}

///
///     Prepares the scene
///
void c_scene::prepare(const float *a_pos, const D3DXMATRIXA16 *a_rot)
{
    /// group the meshes by material & transparency order..
    m_nbCmds = 0;

    for (int i = 0; i < m_nbLights; i++)
    {
        s_light *light = m_lights[i];
        if (light == NULL)
            continue;
        light->m_nbObj = 0;
    }

    for (i = 0; i < m_nbEntities; i++)
    {
        c_entity *entity = m_entities[i];
        if (entity == NULL)
            continue;
        if (!entity->isShown() && !entity->isCastingShadows())
            continue;

        int nbMesh = entity->getNbMesh();
        for (int j = 0; j < nbMesh; j++)
        {
            c_object *mesh = entity->getMesh(j);
            int nbPrims = mesh->getNbPrims();
            float meshPos[3];
            copy3f(meshPos, entity->getPos());
            const float *boxMin = mesh->getBoxMin();
            const float *boxMax = mesh->getBoxMax();
            float meshRadius = 0;
            meshRadius = MAX(meshRadius, (boxMax[0] - boxMin[0]) * 0.71f);
            meshRadius = MAX(meshRadius, (boxMax[1] - boxMin[1]) * 0.71f);
            meshRadius = MAX(meshRadius, (boxMax[2] - boxMin[2]) * 0.71f);
            meshPos[0] += ((boxMin[0] + boxMax[0]) * 0.5f);
            meshPos[1] += ((boxMin[1] + boxMax[1]) * 0.5f);
            meshPos[2] += ((boxMin[2] + boxMax[2]) * 0.5f);
            float viewerToMeshDist = vecDist(a_pos, meshPos);
                
            /// find influence lights..
            int nbLights = 0;
            s_light *lights[8];
            for (int k = 0; k < m_nbLights && nbLights <= 7; k++)
            {
                if (m_lights[k]->m_disabled || m_lights[k]->m_radius == 0)
                    continue;

                float d = vecDist(m_lights[k]->m_pos, meshPos);
                if (d <= m_lights[k]->m_radius + meshRadius)
                {
                    /// this one's influencing the current object..
                    lights[nbLights++] = m_lights[k];
                    int id = (m_lights[k]->m_nbObj++);
                    m_lights[k]->m_objs[id] = mesh;
                    m_lights[k]->m_entities[id] = entity;
                    memcpy(m_lights[k]->m_transform[id], entity->getMatrix(),
                        sizeof(D3DXMATRIXA16));
                }
            }

            for (k = 0; k < nbPrims; k++)
            {
                s_primitive *prim = mesh->getPrim(k);
                s_cmd cmd;
                cmd.m_dist = viewerToMeshDist;
                cmd.m_mat = prim->m_mat;
                cmd.m_obj = mesh;
                copy3f(cmd.m_pos, entity->getPos());
                cmd.m_transform = entity->getMatrix();
                cmd.m_primID = k;
                cmd.m_nbLights = nbLights;
                cmd.m_alpha = 1.0f;
                cmd.m_entity = entity;
                memcpy(cmd.m_lights, lights, nbLights * sizeof(s_light *));

                m_cmds[m_nbCmds++] = cmd;
            }
        }
    }

    /// sort them all
    qsort(m_cmds, m_nbCmds, sizeof(s_cmd), cmdComp);
}

///
///     Sets the vertex shader constants
///
void c_scene::setShaderConstants(const float *a_pos, const float *a_plane,
    const D3DXMATRIXA16 *a_proj, const D3DXMATRIXA16 *a_view,
    const D3DXMATRIXA16 *a_world, D3DXVECTOR3 a_lightPos)
{
    /// Vertex shader operations use transposed matrices
    D3DXMATRIXA16 mat, matCamera, matTranspose, matCameraTranspose;
    D3DXMATRIXA16 matViewTranspose, matProjTranspose;
    D3DXMatrixMultiply(&matCamera, a_world, a_view);
    D3DXMatrixMultiply(&mat, &matCamera, a_proj);
    D3DXMatrixTranspose(&matTranspose, &mat);
    D3DXMatrixTranspose(&matCameraTranspose, &matCamera);
    D3DXMatrixTranspose(&matViewTranspose, a_view);
    D3DXMatrixTranspose(&matProjTranspose, a_proj);

    /// Set the vertex shader constants
    D3DXVECTOR4 vZero(0.0f, 0.5f, 1.0f, 0.0f);
    D3DXVECTOR4 vOne(20000.0f, 20000.0f, 20000.0f, 20000.0f);
    D3DXVECTOR4 vCst(0.001f, 0.0001f, 0.0f, 0.0f);
    D3DXVECTOR4 vJitter1(0, 0, 0, 0);
    m_lpDevice->SetVertexShaderConstantF(0, (float *)&vZero, 1);
    m_lpDevice->SetVertexShaderConstantF(1, (float *)&vOne, 1);
    m_lpDevice->SetVertexShaderConstantF(2, (float *)&vCst, 1);
    m_lpDevice->SetVertexShaderConstantF(3, (float *)&vJitter1, 1);
    m_lpDevice->SetVertexShaderConstantF(4, (float *)&matTranspose, 4);
    m_lpDevice->SetVertexShaderConstantF(8, (float *)&matCameraTranspose, 4);
    m_lpDevice->SetVertexShaderConstantF(12, (float *)&matViewTranspose, 4);
    m_lpDevice->SetVertexShaderConstantF(20, (float *)&(a_lightPos.x), 1);
    m_lpDevice->SetVertexShaderConstantF(28, (float *)&matProjTranspose, 4);
    m_lpDevice->SetVertexShaderConstantF(21, (float *)a_plane, 4);
}

///     Generates the first shadow map, by using standard shadow volumes
///     and 2 light positions
///
void c_scene::genShadowMap(const float *a_pos, const float *a_plane, const D3DXMATRIXA16 *a_proj,
    const D3DXMATRIXA16 *a_view, const D3DXMATRIXA16 *a_world, D3DXVECTOR3 a_lightPos)
{
    D3DXVECTOR4 vJitter1(0, 0, 0, 0);
    D3DXVECTOR4 vJitter2(g_penumbra, 1, 1, 1);

    /// clear target
    RFAIL(m_lpDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
        D3DCOLOR_COLORVALUE(0.0, 0.0, 0.0, 0.0), 1.0, 128));

    setShaderConstants(a_pos, a_plane, a_proj, a_view, a_world, a_lightPos);

    /// disable texture mapping:
    m_lpDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
    m_lpDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

    /// do not draw on the color buffer:
    m_lpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_lpDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
    m_lpDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

    /// format setup:
    m_lpDevice->SetVertexShader(m_zbuffShader);
#ifndef INFLATE
    m_lpDevice->SetFVF(STD_VERTEX_FORMAT);
#else
    m_lpDevice->SetFVF(EXT_VERTEX_FORMAT);
#endif

    /// experimental:
    m_lpDevice->SetVertexShaderConstantF(3, (float *)&vJitter1, 1);

    /// first pass: render to zbuffer only
    for (int i = 0; i < m_nbCmds; i++)
    {
        s_cmd *cmd = m_cmds + i;
        if (!cmd->m_entity->isShown())
            continue;

        c_object *mesh = cmd->m_obj;
        m_lpDevice->SetTransform(D3DTS_WORLD, &(cmd->m_transform));
        m_lpDevice->SetVertexShaderConstantF(26, mesh->getCenter(), 1);
#ifndef INFLATE
        mesh->render1(cmd->m_primID);
#else
        mesh->render2(cmd->m_primID);
#endif
    }

    m_lpDevice->SetVertexShader(NULL);

    /// prepare states and stencil for stencil passes:
    m_lpDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
    m_lpDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);
    m_lpDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_lpDevice->SetVertexDeclaration(m_shadowVolumeDecl);
    m_lpDevice->SetVertexShader(m_shadowVolumeShader);
    m_lpDevice->SetRenderState(D3DRS_STENCILFUNC,  D3DCMP_ALWAYS);
    m_lpDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
    m_lpDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_INCR); // zfail
    m_lpDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
    m_lpDevice->SetRenderState(D3DRS_STENCILREF, 128);
    m_lpDevice->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
    m_lpDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);

    /// pass 1:
    m_lpDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
    for (i = 0; i < m_nbCmds; i++)
    {
        s_cmd *cmd = m_cmds + i;
        if (!cmd->m_entity->isShown())
            continue;
        c_object *mesh = cmd->m_obj;
        mesh->setLightSource(&(a_lightPos.x));
        mesh->visFromLight();
        mesh->extend();
        m_lpDevice->SetTransform(D3DTS_WORLD, &(cmd->m_transform));
        m_lpDevice->SetVertexShaderConstantF(26, mesh->getCenter(), 1);
        mesh->drawShadows();
    }

    /// pass 2:
    m_lpDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    m_lpDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_DECR); // zfail

    for (i = 0; i < m_nbCmds; i++)
    {
        s_cmd *cmd = m_cmds + i;
        if (!cmd->m_entity->isShown())
            continue;
        c_object *mesh = cmd->m_obj;
        mesh->setLightSource(&(a_lightPos.x));
        mesh->visFromLight();
        mesh->extend();
        m_lpDevice->SetTransform(D3DTS_WORLD, &(cmd->m_transform));
        m_lpDevice->SetVertexShaderConstantF(26, mesh->getCenter(), 1);
        mesh->drawShadows();
    }

    m_lpDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_lpDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
    m_lpDevice->SetVertexShader(NULL);
    m_lpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    /// Draws a full-screen quad:
    s_colVertex *vptr = NULL;
    RFAIL(m_quadVB->Lock(0, 0, (void **)(&vptr), 0));
    vptr[0].m_color = 0x00FF0000;
    vptr[1].m_color = 0x00FF0000;
    vptr[2].m_color = 0x00FF0000;
    vptr[3].m_color = 0x00FF0000;
    RFAIL(m_quadVB->Unlock());
    m_lpDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
    m_lpDevice->SetRenderState(D3DRS_STENCILREF, 128);
    m_lpDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
    m_lpDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
    m_lpDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
    m_lpDevice->SetStreamSource(0, m_quadVB, 0, sizeof(s_colVertex));
    m_lpDevice->SetFVF(COL_VERTEX_FORMAT);
    m_lpDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

    m_lpDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

    /// Second step: jitter the light
#ifdef INFLATE 
    RFAIL(m_lpDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
        D3DCOLOR_COLORVALUE(0.0, 0.0, 0.0, 0.0), 1.0, 128));
#else
    RFAIL(m_lpDevice->Clear(0, NULL, D3DCLEAR_STENCIL,
        D3DCOLOR_COLORVALUE(0.0, 0.0, 0.0, 0.0), 1.0, 128));
#endif

    m_lpDevice->SetVertexShaderConstantF(3, (float *)&vJitter2, 1);
    m_lpDevice->SetVertexShaderConstantF(20, (float *)&(a_lightPos.x), 1);

    /// do not draw on the color buffer:
    m_lpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_lpDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
    m_lpDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

#ifdef INFLATE
    /// format setup:
    m_lpDevice->SetVertexShader(m_zbuffShader);
    m_lpDevice->SetFVF(EXT_VERTEX_FORMAT);

    m_lpDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
    m_lpDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
    m_lpDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
    m_lpDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
    m_lpDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    /// first pass: render to zbuffer only
    for (i = 0; i < m_nbCmds; i++)
    {
        s_cmd *cmd = m_cmds + i;
        if (!cmd->m_entity->isShown())
            continue;

        c_object *mesh = cmd->m_obj;
        m_lpDevice->SetTransform(D3DTS_WORLD, &(cmd->m_transform));
        m_lpDevice->SetVertexShaderConstantF(26, mesh->getCenter(), 1);
        mesh->render2(cmd->m_primID);
    }

    m_lpDevice->SetVertexShader(NULL);
    m_lpDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
#endif

    /// prepare states and stencil for stencil passes:
    m_lpDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
    m_lpDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);
    m_lpDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_lpDevice->SetVertexDeclaration(m_shadowVolumeDecl);
    m_lpDevice->SetVertexShader(m_shadowVolumeShader);
    m_lpDevice->SetRenderState(D3DRS_STENCILFUNC,  D3DCMP_ALWAYS);
    m_lpDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
    m_lpDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_INCR); // zfail
    m_lpDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
    m_lpDevice->SetRenderState(D3DRS_STENCILREF, 128);
    m_lpDevice->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
    m_lpDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);

    /// pass 1:
    m_lpDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
    for (i = 0; i < m_nbCmds; i++)
    {
        s_cmd *cmd = m_cmds + i;
        if (!cmd->m_entity->isShown())
            continue;
        c_object *mesh = cmd->m_obj;
        mesh->setLightSource(&(a_lightPos.x));
        mesh->visFromLight();
        mesh->extend();
        m_lpDevice->SetTransform(D3DTS_WORLD, &(cmd->m_transform));
        m_lpDevice->SetVertexShaderConstantF(26, mesh->getCenter(), 1);
        mesh->drawShadows();
    }

    /// pass 2:
    m_lpDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    m_lpDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_DECR); // zfail

    for (i = 0; i < m_nbCmds; i++)
    {
        s_cmd *cmd = m_cmds + i;
        if (!cmd->m_entity->isShown())
            continue;
        c_object *mesh = cmd->m_obj;
        m_lpDevice->SetTransform(D3DTS_WORLD, &(cmd->m_transform));
        m_lpDevice->SetVertexShaderConstantF(26, mesh->getCenter(), 1);
        mesh->drawShadows();
    }

    m_lpDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_lpDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
    m_lpDevice->SetVertexShader(NULL);
    m_lpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    /// Draws a full-screen quad:
    vptr = NULL;
    RFAIL(m_quadVB->Lock(0, 0, (void **)(&vptr), 0));
    vptr[0].m_color = 0x0000FF00;
    vptr[1].m_color = 0x0000FF00;
    vptr[2].m_color = 0x0000FF00;
    vptr[3].m_color = 0x0000FF00;
    RFAIL(m_quadVB->Unlock());
    
    m_lpDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_GREEN);
    m_lpDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
    m_lpDevice->SetRenderState(D3DRS_STENCILREF, 128);
    m_lpDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
    m_lpDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
    m_lpDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
    m_lpDevice->SetStreamSource(0, m_quadVB, 0, sizeof(s_colVertex));
    m_lpDevice->SetFVF(COL_VERTEX_FORMAT);
    m_lpDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

    m_lpDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);
    m_lpDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
}

///
///     Generates a light's lightmap
///
void c_scene::genLightMap(const float *a_pos, const float *a_plane, const D3DXMATRIXA16 *a_proj,
    const D3DXMATRIXA16 *a_view, const D3DXMATRIXA16 *a_world, D3DXVECTOR3 a_lightPos,
    IDirect3DTexture9 *a_lightMap)
{
    /// toggle off-screen rendering to shadow map:
    IDirect3DSurface9 *l_mapSurf = NULL;
    if (g_count == 0)
    {
        RFAIL(a_lightMap->GetSurfaceLevel(0, &l_mapSurf));
    }
    else
    {
        RFAIL(m_shadowMap[0]->GetSurfaceLevel(0, &l_mapSurf));
    }
    RFAIL(m_lpDevice->SetRenderTarget(0, l_mapSurf));
    RFAIL(m_lpDevice->SetDepthStencilSurface(m_depthMap));

    genShadowMap(a_pos, a_plane, a_proj, a_view, a_world, a_lightPos);

#ifdef STENCIL_TEST_TRICK
    /// setup stencil test:
    m_lpDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
    m_lpDevice->SetRenderState(D3DRS_STENCILREF, 128);
    m_lpDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
    m_lpDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
    m_lpDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
    m_lpDevice->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
    m_lpDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
    m_lpDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);
#else
    m_lpDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
#endif

    /// States for blurring:
    m_lpDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
    m_lpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_lpDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
    m_lpDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_lpDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

    RFAIL(m_lpDevice->SetPixelShader(m_blurPixelShader));
    for (int step = 0; step < 3 && g_count > 0 && step < g_count; step++)
    {
        l_mapSurf = NULL;

        /// Now's the step to blur the shadow map:
        if ((step + 1) == g_count)
        {
            RFAIL(a_lightMap->GetSurfaceLevel(0, &l_mapSurf));
        }
        else
        {
            RFAIL(m_shadowMap[step + 1]->GetSurfaceLevel(0, &l_mapSurf));
        }
        RFAIL(m_lpDevice->SetRenderTarget(0, l_mapSurf));

#ifdef STENCIL_TEST_TRICK
        RFAIL(m_lpDevice->Clear(0, NULL, D3DCLEAR_TARGET,
            D3DCOLOR_COLORVALUE(0.0, 0.0, 0.0, 0.0), 1.0, 128));
#endif

        blurShadowMap(m_shadowMap[step], g_shadowScale);
    }

    RFAIL(m_lpDevice->SetPixelShader(NULL));
    m_lpDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
    m_lpDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    m_lpDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_lpDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
}

///
///     Renders the scene
///
void c_scene::render(const float *a_pos, const float *a_plane, const D3DXMATRIXA16 *a_proj,
    const D3DXMATRIXA16 *a_view, const D3DXMATRIXA16 *a_world)
{
    int i;

    clean();

    /// toggle off-screen rendering to shadow map:
    /// sets shadow map #0 as render target:
    IDirect3DSurface9 *oldZStencilSurf = NULL;
    IDirect3DSurface9 *oldRenderTarget = NULL;
    RFAIL(m_lpDevice->GetRenderTarget(0, &oldRenderTarget));
    RFAIL(m_lpDevice->GetDepthStencilSurface(&oldZStencilSurf));

    /// light 0
    D3DXVECTOR3 lightPos0(139.886f, 2838.217f, 4439.732f);
    D3DXVECTOR3 lightCol0(0.749f, 0.815f, 1.0f);
    lightCol0.x *= 0.66f;
    lightCol0.y *= 0.66f;
    lightCol0.z *= 0.66f;
    tt += 0.01f;
    lightPos0.x += cosf(tt) * 17.5f;
    lightPos0.y += fabsf(cosf(2 * tt) * 25.0f);
    lightPos0.z += sinf(tt) * 14.5f;
    float penumbra = g_penumbra;
    g_penumbra *= 50.0f;
    genLightMap(a_pos, a_plane, a_proj, a_view, a_world, lightPos0, m_lightMap[0]);
    g_penumbra = penumbra;

    /// Clean states:
    m_lpDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    m_lpDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_lpDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    m_lpDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
    m_lpDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    m_lpDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_lpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    /// light 1
    D3DXVECTOR3 lightPos1(-72.6f + g_lightOffset, 1.91f, -100.8f);
    D3DXVECTOR3 lightCol1(0.8627f, 0.6549f, 0.2902f);
    lightCol1.x *= 0.8f;
    lightCol1.y *= 0.8f;
    lightCol1.z *= 0.8f;
    lightPos1.x += cosf(tt) * 17.5f;
    lightPos1.y += fabsf(cosf(2 * tt) * 25.0f);
    lightPos1.z += sinf(tt) * 14.5f;

    genLightMap(a_pos, a_plane, a_proj, a_view, a_world, lightPos1, m_lightMap[1]);

    RFAIL(m_lpDevice->SetRenderTarget(0, oldRenderTarget));
    RFAIL(m_lpDevice->SetDepthStencilSurface(oldZStencilSurf));

    /// Clean states:
    m_lpDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    m_lpDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_lpDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    m_lpDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
    m_lpDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    m_lpDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_lpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    /// paint pass:
    RFAIL(m_lpDevice->SetTexture(1, m_lightMap[0]));
    m_lpDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_lpDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_lpDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
    m_lpDevice->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    m_lpDevice->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

    RFAIL(m_lpDevice->SetTexture(2, m_lightMap[1]));
    m_lpDevice->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_lpDevice->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_lpDevice->SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
    m_lpDevice->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    m_lpDevice->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

    RFAIL(m_lpDevice->SetPixelShader(m_finalPixelShader));
    RFAIL(m_lpDevice->SetVertexShader(m_paintShader));

    RFAIL(m_lpDevice->SetVertexShaderConstantF(22, (float *)&(lightPos0.x), 1));
    RFAIL(m_lpDevice->SetVertexShaderConstantF(23, (float *)&(lightCol0.x), 1));
    RFAIL(m_lpDevice->SetVertexShaderConstantF(24, (float *)&(lightPos1.x), 1));
    RFAIL(m_lpDevice->SetVertexShaderConstantF(25, (float *)&(lightCol1.x), 1));

    for (i = 0; i < m_nbCmds; i++)
    {
        s_cmd *cmd = m_cmds + i;
        if (!cmd->m_entity->isShown())
            continue;

        c_object *mesh = cmd->m_obj;
        RFAIL(m_lpDevice->SetTransform(D3DTS_WORLD, &(cmd->m_transform)));
        mesh->render3(cmd->m_primID);
    }

    RFAIL(m_lpDevice->SetVertexShader(NULL));
    RFAIL(m_lpDevice->SetPixelShader(NULL));
    
    /// ultimate additive 4 lights pass:
    m_lpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_lpDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    m_lpDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
    RFAIL(m_lpDevice->SetVertexShader(m_lightShader));
    for (i = 0; i < m_nbCmds; i++)
    {
        s_cmd *cmd = m_cmds + i;
        if (!cmd->m_entity->isShown())
            continue;
        c_object *mesh = cmd->m_obj;
        RFAIL(m_lpDevice->SetTransform(D3DTS_WORLD, &(cmd->m_transform)));
        mesh->render3(cmd->m_primID);
    }
    RFAIL(m_lpDevice->SetVertexShader(NULL));
    m_lpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

#ifdef SHOW_LIGHTS
    m_lpDevice->SetFVF(D3DFVF_XYZ);
    float pointSize = 8.0f;
    m_lpDevice->SetRenderState(D3DRS_POINTSIZE, *((DWORD *)&pointSize));
    m_lpDevice->DrawPrimitiveUP(D3DPT_POINTLIST, 1, &lightPos0, sizeof(D3DXVECTOR3));
    m_lpDevice->DrawPrimitiveUP(D3DPT_POINTLIST, 1, &lightPos1, sizeof(D3DXVECTOR3));
#endif
}

///
///     Renders the scene, no PS1.4
///
void c_scene::renderNoPS14(const float *a_pos, const float *a_plane, const D3DXMATRIXA16 *a_proj,
    const D3DXMATRIXA16 *a_view, const D3DXMATRIXA16 *a_world)
{
    int i;

    clean();

    D3DXVECTOR3 lightPos1(-72.6f, 1.91f, -100.8f);
    setShaderConstants(a_pos, a_plane, a_proj, a_view, a_world, lightPos1);

    m_lpDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    m_lpDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_lpDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    m_lpDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
    m_lpDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    m_lpDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_lpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    D3DXVECTOR4 vJitter2(g_penumbra, 1, 1, 1);
    m_lpDevice->SetVertexShaderConstantF(3, (float *)&vJitter2, 1);

    m_lpDevice->SetVertexShader(m_dbgShader);
    for (i = 0; i < m_nbCmds; i++)
    {
        s_cmd *cmd = m_cmds + i;
        if (!cmd->m_entity->isShown())
            continue;

        c_object *mesh = cmd->m_obj;
        m_lpDevice->SetTransform(D3DTS_WORLD, &(cmd->m_transform));
        mesh->render(cmd->m_primID);
    }
    m_lpDevice->SetVertexShader(NULL);

    /// disable texture mapping:
    m_lpDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
    m_lpDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    for (i = 0; i < m_nbCmds; i++)
    {
        s_cmd *cmd = m_cmds + i;
        if (!cmd->m_entity->isShown())
            continue;

        c_object *mesh = cmd->m_obj;
        m_lpDevice->SetTransform(D3DTS_WORLD, &(cmd->m_transform));
        mesh->renderDbg(cmd->m_primID);
    }

#ifdef SHOW_LIGHTS
    m_lpDevice->SetFVF(D3DFVF_XYZ);
    float pointSize = 64.0f;
    m_lpDevice->SetRenderState(D3DRS_POINTSIZE, *((DWORD *)&pointSize));
    m_lpDevice->DrawPrimitiveUP(D3DPT_POINTLIST, 1, &lightPos1, sizeof(D3DXVECTOR3));
#endif
}

///
///     Blurs the shadow map
///
void c_scene::blurShadowMap(IDirect3DTexture9 *shadowMap, const float scale)
{
    float offsets[6 * 2] = {
        0.0f, 0.0f,
        0.0f, -1.0f,
        0.7f, -0.2f,
        0.4f, 0.5f,
        -0.4f, 0.5f,
        -0.7f, -0.2f
        };
    HRESULT hRes;

    s_blurVertex *bv = NULL;
    RFAIL(m_blurVB->Lock(0, 0, (void **)(&bv), 0));

    for (int i = 0; i < 6; i++)
    {
        float *tcs = bv[0].m_tc0 + 2 * i;
        const float ofix1 = g_fix1 / 512.0f;
        const float ofix2 = g_fix2 / 512.0f;

        tcs[0] = 0 + offsets[i * 2 + 0] / 512.0f * scale + ofix1;
        tcs[1] = 0 + offsets[i * 2 + 1] / 512.0f * scale + ofix2;

        tcs = bv[1].m_tc0 + 2 * i;
        tcs[0] = 1 + offsets[i * 2 + 0] / 512.0f * scale + ofix1;
        tcs[1] = 0 + offsets[i * 2 + 1] / 512.0f * scale + ofix2;

        tcs = bv[2].m_tc0 + 2 * i;
        tcs[0] = 1 + offsets[i * 2 + 0] / 512.0f * scale + ofix1;
        tcs[1] = 1 + offsets[i * 2 + 1] / 512.0f * scale + ofix2;

        tcs = bv[3].m_tc0 + 2 * i;
        tcs[0] = 0 + offsets[i * 2 + 0] / 512.0f * scale + ofix1;
        tcs[1] = 1 + offsets[i * 2 + 1] / 512.0f * scale + ofix2;
    }

    RFAIL(m_blurVB->Unlock());

    for (i = 0; i < 6; i++)
    {
        RFAIL(m_lpDevice->SetTexture(i, shadowMap));

        if (i != 0)
        {
            m_lpDevice->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
            m_lpDevice->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
            m_lpDevice->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
        }
        else
        {
            m_lpDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
            m_lpDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
            m_lpDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
        }
        m_lpDevice->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        m_lpDevice->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
        m_lpDevice->SetSamplerState(i, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
    }

    m_lpDevice->SetStreamSource(0, m_blurVB, 0, sizeof(s_blurVertex));
    m_lpDevice->SetFVF(BLUR_VERTEX_FORMAT);
    m_lpDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

    for (i = 0; i < 6; i++)
        RFAIL(hRes = m_lpDevice->SetTexture(i, NULL));
}

///
///     Materials management
///
s_material *c_scene::addMaterial(const s_material *mat)
{
    int matID = -1;

    for (int i = 0; i < m_nbMats; i++)
    {
        if (m_mats[i] == NULL)
        {
            matID = i;
            continue;
        }

        if (m_mats[i]->m_tex != mat->m_tex
         || m_mats[i]->m_ambient != mat->m_ambient
         || m_mats[i]->m_recvShadows != mat->m_recvShadows)
            /// materials are different..
            continue;

        return(m_mats[i]);
    }

    if (matID == -1)
    {
        matID = m_nbMats;
        m_nbMats++;
    }

    m_mats[matID] = new s_material();
    m_mats[matID]->m_tex = mat->m_tex;
    m_mats[matID]->m_ambient = mat->m_ambient;
    m_mats[matID]->m_recvShadows = mat->m_recvShadows;
    
    char str[256];
    sprintf(str, "Adding material %d\n", matID);
    warning(str);

    return(m_mats[matID]);
}

void c_scene::addObject(c_object *obj)
{
    m_objs[m_nbObjs++] = obj;
}

c_object *c_scene::getObject(const char *name)
{
    for (int i = 0; i < m_nbObjs; i++)
    {
        if (m_objs[i] == NULL)
            continue;

        if (stricmp(name, m_objs[i]->m_name) == 0)
            return(m_objs[i]);
    }
    return(NULL);
}

int c_scene::getObjects(const char *name, const int maxObjs, c_object **objs)
{
    int n = 0;
    for (int i = 0; i < m_nbObjs && n < maxObjs; i++)
    {
        if (m_objs[i] == NULL)
            continue;

        char *oname = m_objs[i]->m_name;
        if (strlen(oname) < strlen(name))
            continue;

        if (strnicmp(oname, name, strlen(name)) == 0)
            objs[n++] = m_objs[i];
    }
    return n;
}

void c_scene::addEntity(c_entity *entity)
{
    m_entities[m_nbEntities++] = entity;
}

void c_scene::addLight(s_light *light)
{
    m_lights[m_nbLights++] = light;
}

s_light *c_scene::getLight(const char *name)
{
    for (int i = 0; i < m_nbLights; i++)
    {
        if (stricmp(name, m_lights[i]->m_name) == 0)
            return(m_lights[i]);
    }
    return(NULL);
}

///
///     Updates the scene
///
void c_scene::update(const float elapsed)
{
}

c_entity *c_scene::getEntity(const char *name)
{
    for (int i = 0; i < m_nbEntities; i++)
    {
        if (stricmp(name, m_entities[i]->getName()) == 0)
            return(m_entities[i]);
    }
    return(NULL);
}

