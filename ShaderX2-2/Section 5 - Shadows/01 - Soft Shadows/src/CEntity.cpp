///
///     Entity file
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
#include <d3d9.h>
#include <d3dx9.h> 
#include <math.h>

#include "CEntity.h"
#include "CASEFile.h"

///
///     Default constructor
///
c_entity::c_entity(const char *name, c_object *mesh)
{
    strcpy(m_name, name);
    set3f(m_pos, 0);
    set3f(m_radius, 1);
    D3DXMatrixIdentity(&m_transform);
    m_mesh[0] = mesh;
    m_nbMesh = 0;
    m_show = true;
    m_castShadows = true;
    if (mesh != NULL)
        m_nbMesh = 1;
}

c_entity::c_entity(const char *name, const char *meshName)
{
    strcpy(m_name, name);
    set3f(m_pos, 0);
    set3f(m_radius, 1);
    D3DXMatrixIdentity(&m_transform);
    m_mesh[0] = c_scene::ms_scene->getObject(meshName);
    if (m_mesh[0] != NULL)
        m_nbMesh = 1;
}

///
///     Default destructor
///
c_entity::~c_entity()
{
}

///
///     Updates the entity. By default, does nothing..
///
void c_entity::update(const float elapsed)
{
}

///
///     Sets the entity's mesh #n
///
void c_entity::setMesh(c_object *mesh, const int meshID)
{
    if (mesh == NULL)
        warning("Can't set NULL mesh to entity!\n");

    if (meshID >= m_nbMesh)
    {
        for (int i = m_nbMesh; i <= meshID; i++)
            m_mesh[i] = NULL;

        m_nbMesh = meshID + 1;
    }

    m_mesh[meshID] = mesh;
}

void c_entity::addMesh(c_object *mesh)
{
    if (mesh == NULL)
        warning("Can't add NULL mesh to entity!\n");
    m_mesh[m_nbMesh++] = mesh;
}

void c_entity::addMesh(const char *meshName)
{
    c_object *mesh = c_scene::ms_scene->getObject(meshName);
    if (mesh != NULL)
        addMesh(mesh);
}

void c_entity::giveMeshes(c_aseFile *aseFile)
{
    for (int i = 0; i < aseFile->m_nbObjs; i++)
        addMesh(aseFile->m_objs[i]);
}

void c_entity::giveMeshes(const char *name)
{
    char altname[256];
    sprintf(altname, "%s_", name);
    c_object *objs[256];
    int n = c_scene::ms_scene->getObjects(altname, 250, objs);
    for (int i = 0; i < n ; i++)
        addMesh(objs[i]);
}

void c_entity::setRot(const D3DXMATRIXA16& m)
{
    m_rot = m;
    
    D3DXMATRIXA16 t;
    D3DXMatrixTranslation(&t, m_pos[0], m_pos[1], m_pos[2]);
    D3DXMatrixMultiply(&m_transform, &m_rot, &t);
}

void c_entity::setPos(const float *p)
{
    copy3f(m_pos, p);

    D3DXMATRIXA16 t;
    D3DXMatrixTranslation(&t, m_pos[0], m_pos[1], m_pos[2]);
    D3DXMatrixMultiply(&m_transform, &m_rot, &t);
}
