//*****************************************************************************
//* This file is part of the Dimension3 renderer library                      *
//* Copyright (C) 2003  Michal Valient (valient@host.sk)                      *
//*                                                                           *
//* Dimension3 renderer can be found at http://www.dimension3.host.sk         *
//*                                                                           *
//* This program is free software; you can redistribute it and/or modify      *
//* it under the terms of the GNU General Public License as published by      *
//* the Free Software Foundation; either version 2 of the License, or         *
//* (at your option) any later version.                                       *
//*                                                                           *
//* This program is distributed in the hope that it will be useful,           *
//* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
//* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
//* GNU General Public License for more details.                              *
//*                                                                           *
//* You should have received a copy of the GNU General Public License         *
//* along with this program; if not, write to the Free Software               *
//* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA * 
//*****************************************************************************
//hdr_utility_remove_tag


#pragma once

#include "nommgr.h"
#include "d3dx9.h"
#include "mmgr.h"

#include "baseclass.h"

class CFrustum : public CBaseClass {
public:

	// Call this every time the camera moves to update the frustum
	void CalculateFrustum(const D3DXMATRIX &mview, const D3DXMATRIX &projection);

	// This takes a 3D point and returns TRUE if it's inside of the frustum
	bool PointInFrustum(const D3DXVECTOR3 &point);

	// This takes a 3D point and a radius and returns TRUE if the sphere is inside of the frustum
	bool SphereInFrustum(const D3DXVECTOR3 &center, const float radius);

	// This takes the center and half the length of the cube.
	bool CubeInFrustum( const D3DXVECTOR3 &center, const float size );

private:

	// This holds the A B C and D values for each side of our frustum.
	float m_Frustum[6][4];

public:
    CFrustum(char *ObjName,CEngine *OwnerEngine);
    virtual ~CFrustum(void);

    MAKE_CLSCREATE(CFrustum);
    MAKE_CLSNAME("CFrustum");
    MAKE_DEFAULTGARBAGE()
};
