///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgD3DObject
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgD3DObject.cpp,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgD3DObject.h"
#include <pgLib/Base/pgIDirectX.h>
#include <pgLib/Base/pgLog.h>


bool
pgD3DObject::checkDevice(const char* nMsg)
{
	if(pd3dDevice==NULL)
	{
		pd3dDevice = pgIDirectX::getDevice();
		if(pd3dDevice==NULL)
		{
			pgLog::error(nMsg);
			return false;
		}
	}

	return true;
}
