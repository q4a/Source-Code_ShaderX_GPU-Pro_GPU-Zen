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

#ifndef SCENE_H_
#define SCENE_H_

#include <windows.h>
#include <stdio.h>

#include "CTexture.h"
#include "CUtils.h"

class c_object;
class c_entity;
class c_objMd3;

struct s_material
{
    c_texture           *m_tex;             // diffuse texture
    float               m_ambient;          // ambient color level [0-1]
    bool                m_recvShadows;      // material receives shadows?
};

///
///     A light for that model
///
struct s_light
{
    float               m_opos[3];          // original light's position
    float               m_pos[3];           // light position in world
    float               m_ocolor[4];        // original light's diffuse color
    float               m_color[4];         // light's diffuse color
    bool                m_castShadows;      // light's casting shadows?
    char                m_name[32];         // light's name
    float               m_oradius;          // original light's radius
    float               m_radius;           // light's radius
    bool                m_disabled;         // light is disabled?
    int                 m_nbObj;            // number of objects/entities influenced..
    c_entity            *m_entities[1024];  // entities influenced by that light
    c_object            *m_objs[1024];      // objects influenced by that light
    D3DXMATRIXA16       m_transform[1024];  // transformation matrix for each object
};

///
///     A rendering command
///
struct s_cmd
{
    c_entity            *m_entity;          // entity root
    c_object            *m_obj;             // object to render
    float               m_pos[3];           // translation
    int                 m_primID;           // primitive ID to render
    s_material          *m_mat;             // material to use
    float               m_dist;             // sorting distance..
    D3DXMATRIXA16       m_transform;        // transformation matrix
    int                 m_nbLights;         // number of lights influencing object
    s_light             *m_lights[8];       // lights infleuncing that object
    float               m_alpha;            // transparency coefficient
};

class c_scene
{
    protected:

    IDirect3DDevice9            *m_lpDevice;            // D3D device
    IDirect3DVertexDeclaration9 *m_shadowVolumeDecl;    // shadow volume shader declaration
    IDirect3DVertexShader9      *m_shadowVolumeShader;  // shadow volume vertex shader
    IDirect3DVertexShader9      *m_zbuffShader;         // z-buffer pass vertex shader
    IDirect3DVertexShader9      *m_lightShader;         // small lights vertex shader
    IDirect3DVertexShader9      *m_dbgShader;           // debug smoothed normals shader
    IDirect3DVertexBuffer9      *m_quadVB;              // vertex buffer for screen quad
    IDirect3DVertexShader9      *m_paintShader;         // paint vertex shader
    IDirect3DTexture9           *m_shadowMap[3];        // shadow map to render volumes to 
    IDirect3DSurface9           *m_depthMap;            // depth/stencil shadow map
    IDirect3DVertexBuffer9      *m_blurVB;              // blur quad vertex buffer
    IDirect3DPixelShader9       *m_blurPixelShader;     // blur pixel shader
    IDirect3DPixelShader9       *m_finalPixelShader;    // final pixel shader

    IDirect3DTexture9           *m_lightMap[2];         // one map per light

    int                         m_nbObjs;               // number of objects
    c_object                    *m_objs[1024];          // array of objects
    int                         m_nbLights;             // number of lights
    s_light                     *m_lights[256];         // lights in scene
    int                         m_nbMats;               // number of materials
    s_material                  *m_mats[256];           // array of materials
    int                         m_nbEntities;           // number of entities
    c_entity                    *m_entities[4096];      // entities in scene
    int                         m_nbCmds;               // number of rendering commands
    s_cmd                       m_cmds[4096];           // rendering commands
    
    ///
    ///     Compares 2 rendering commands
    ///
    static int cmdComp(
        const void          *arg1,              // first entity
        const void          *arg2               // second entity
        );

    ///
    ///     Blurs the shadow map
    ///
    void blurShadowMap(IDirect3DTexture9 *shadowMap, const float scale);

    ///
    ///     Generates the first shadow map, by using standard shadow volumes
    ///     and 2 light positions
    ///
    void genShadowMap(const float *a_pos, const float *a_plane, const D3DXMATRIXA16 *a_proj,
        const D3DXMATRIXA16 *a_view, const D3DXMATRIXA16 *a_world, D3DXVECTOR3 a_lightPos);

    ///
    ///     Sets the vertex shader constants
    ///
    void setShaderConstants(const float *a_pos, const float *a_plane,
        const D3DXMATRIXA16 *a_proj, const D3DXMATRIXA16 *a_view,
        const D3DXMATRIXA16 *a_world, D3DXVECTOR3 a_lightPos);

    ///
    ///     Generates a light's lightmap
    ///
    void genLightMap(const float *a_pos, const float *a_plane, const D3DXMATRIXA16 *a_proj,
        const D3DXMATRIXA16 *a_view, const D3DXMATRIXA16 *a_world, D3DXVECTOR3 a_lightPos,
        IDirect3DTexture9 *a_lightMap);

    public:

    static c_scene      *ms_scene;          // main scene

    ///
    ///     Creates an empty scene
    ///
    c_scene(IDirect3DDevice9 *a_device);

	///
	///		Destroys the scene
	///
    ~c_scene();

    ///
    ///     Prepares the scene
    ///
    void prepare(const float *a_pos, const D3DXMATRIXA16 *a_rot);

    ///
    ///     Renders the scene
    ///
    void render(const float *a_pos, const float *a_plane, const D3DXMATRIXA16 *a_proj,
        const D3DXMATRIXA16 *a_view, const D3DXMATRIXA16 *a_world);

    ///
    ///     Renders the scene, no PS1.4
    ///
    void renderNoPS14(const float *a_pos, const float *a_plane, const D3DXMATRIXA16 *a_proj,
        const D3DXMATRIXA16 *a_view, const D3DXMATRIXA16 *a_world);

    ///
    ///     Cleans the rendering
    ///
    void clean();

    ///
    ///     Updates the scene
    ///
    void update(const float elapsed);

    ///
    ///     Scene management
    ///
    s_material *addMaterial(const s_material *mat);
    void addObject(c_object *obj);
    c_object *getObject(const char *name);
    void addEntity(c_entity *entity);
    void addLight(s_light *light);
    s_light *getLight(const char *name);
    c_entity *getEntity(const char *name);
    int getObjects(const char *name, const int maxObjs, c_object **objs);
};

#endif
