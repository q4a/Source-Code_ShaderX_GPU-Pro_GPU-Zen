///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgIImageTool
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgIImageTool.h,v 1.1.1.1 2002/10/20 09:33:01 daniel Exp $
///////////////////////////////////////////////////////////////////

#ifndef __IMAGETOOL_HEADERFILE__
#define __IMAGETOOL_HEADERFILE__


#include <pgLib/pgLib.h>

class pgImage;
class pgVec3n;

/// This interface provides image utitlity methods
namespace pgIImageTool
{
	/// calculates a lightmap image from a heightmap image
	/**
	 *  Light is calculated as direction light. The light's direction
	 *  is set using the nDir parameter. nAmbient and nDiffuse are used
	 *  to calculate each destination pixel's luminance.
	 */
	PGLIB_API pgImage* createLightmapFromHeightmap(const pgImage* nHeightmap, const pgVec3n& nDir,
										 float nAmbient, float nDiffuse);


};


#endif //__IMAGETOOL_HEADERFILE__

