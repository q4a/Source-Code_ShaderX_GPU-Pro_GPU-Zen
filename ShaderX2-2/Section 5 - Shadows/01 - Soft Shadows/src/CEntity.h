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

#ifndef ENTITY_H_
#define ENTITY_H_

#include "CObject.h"

class c_entity
{
    protected:

    char                m_name[64];         // entity's name
    D3DXMATRIXA16       m_transform;        // transformation matrix
    D3DXMATRIXA16       m_rot;              // rotation matrix
    float               m_pos[3];           // entity's position
    float               m_radius[3];        // entity's radius
    int                 m_nbMesh;           // number of meshes
    c_object            *m_mesh[128];       // entity's meshes if any..
    bool                m_show;             // show the entity?
    bool                m_castShadows;      // casts shadows?

    public:

    ///
    ///     Default constructor
    ///
    c_entity(const char *name, c_object *mesh = NULL);
    c_entity(const char *name, const char *meshName);

    ///
    ///     Default destructor
    ///
    ~c_entity();

    ///
    ///     Accessors
    ///
    const float *getRadius() const          { return(m_radius); }
    bool isShown() const                    { return(m_show); }
    inline void setShow(const bool s)       { m_show = s; }
    const char *getName() const             { return(m_name); }
    bool isCastingShadows() const           { return(m_castShadows); }
    void setCastShadows(const bool s)       { m_castShadows = s; }
    D3DXMATRIXA16 getMatrix() const         { return(m_transform); }
    void setMatrix(const D3DXMATRIXA16& m)  { m_transform = m; }
    D3DXMATRIXA16 getRot() const            { return(m_rot); }
    void setRot(const D3DXMATRIXA16& m);
    const float *getPos() const             { return(m_pos); }
    void setPos(const float *p);
    
    ///
    ///     Sets the radius of the entity
    ///
    void setRadius(const float *r)          { copy3f(m_radius, r); }
    void setRadius(const float x, const float y, const float z)
    {
        m_radius[0] = x;
        m_radius[1] = y;
        m_radius[2] = z;
    }

    ///
    ///     Sets the entity's mesh #n
    ///
    void setMesh(c_object *mesh, const int meshID = 0);
    void addMesh(c_object *mesh);
    void addMesh(const char *meshName);
    void giveMeshes(c_aseFile *aseFile);
    void giveMeshes(const char *name);

    ///
    ///     Gets the entity's mesh
    ///
    inline c_object *getMesh(const int meshID = 0) const
    {
        return(m_mesh[meshID]);
    }
    inline int getNbMesh() const            { return(m_nbMesh); }

    ///
    ///     Updates the entity. By default, does nothing..
    ///
    virtual void update(const float elapsed);
};

#endif

