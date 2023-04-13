///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e Shared::CGeometry.
 *
 * @file
 *  Geometry.h
 * @author
 *  Carsten Wenzel
 * @date
 *  27.02.2002
 * @brief
 *  Declaration of \e Shared::CGeometry.
 */



#ifndef _GEOMETRY_H_INCLUDED_
#define _GEOMETRY_H_INCLUDED_

#pragma once



namespace Shared
{



    /**
    * A mesh generator class for primitive objects.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  27.02.2002
    * @brief
    *  A mesh generator class for primitive objects.
    */
    class CGeometry
    {
    public:
        /**
        * Creates a sphere with flexible vertex format = D3DFVF_XYZ | D3DFVF_TEX1. 
        * 
        * @param fRadius
        *  Radius of sphere.
        * @param uiRings
        *  Number of rings
        * @param uiSections
        *  Number of section
        * @param pD3DDev
        *  Pointer to D3D device.
        * @param pMesh
        *  Pointer to an \e ID3DXMesh interface representing the mesh of the generated sphere.
        * @param fScaleU
        *  Scaling factor texture u.
        * @param fScaleV
        *  Scaling factor texture v.
        * @return
        *  \b true of creation was successful else \b false.
        * @brief
        *  Creates a sphere with flexible vertex format = D3DFVF_XYZ | D3DFVF_TEX1.
        */
        static bool CreateSphere( float fRadius, unsigned int uiRings, unsigned int uiSections, 
                                  IDirect3DDevice8* pD3DDev, ID3DXMesh*& pMesh, 
                                  float fScaleU, float fScaleV );
    private:
        /**
        * Constructor.
        * 
        * @brief
        *  Constructor.
        */
        CGeometry();

        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        ~CGeometry();
    };



} // namespace Shared



#endif // #ifndef _GEOMETRY_H_INCLUDED_