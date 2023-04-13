///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgLensflare
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgLensflare.h,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __LENSEFLARE_HEADERFILE__
#define __LENSEFLARE_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Math/pgVec2.h>
#include <pgLib/Math/pgVec3.h>
#include <pgLib/Render/pgD3DObject.h>
#include <pgLib/Render/pgTexture.h>

#include <D3DX8.h>


/// Renders a lensflare
/**
 *  This class draws a nice lensflare onto the screen.
 *  Currently only a set of standard images can be loaded.
 *  The lensflare is drawn on a line between the set position
 *  and the center of the screen.
 */
class PGLIB_API pgLensflare : public pgD3DObject
{
public:
	enum  {  NUM_STDFLAREIMGS = 7  };

	pgLensflare();


	/// Sets the position of the lensflare.
	/**
	 *  If the lensflare shall follow a specific object
	 *  (such as a skybox) it is important, that this position
	 *  lies on the object and not just in that direction.
	 */
	void setPosition(const pgVec3& nPos)  {  pos = nPos;  }


	/// Loads a set of standard lensflare images.
	void loadStdFlareImages();


	/// Renders the lensflare. The viewing direction is retrieved from pgIDirectX
	void render();


	/// Sets the basic alpha value of the lensflare.
	/**
	 *  The lower the alpha value is, the less the lensflare
	 *  will be visible. nAlpha must be in range [0.0-1.0]
	 */
	void setBaseAlpha(float nAlpha)  {  baseAlpha = nAlpha;  }


	virtual void deleteDeviceObjects()  {}
	virtual bool restoreDeviceObjects()  {  return true;  }


protected:
	typedef struct LF_VERTEX
	{
		pgVec3	pos;
		DWORD	color;
		pgVec2	tex;
	};

	void setVertsPosSize(float nPosX, float nPosY, float nSize);
	void setVertsColor(float nRed, float nGreen, float nBlue, float nAlpha);

	void drawFlarePart(float nPos, int nIndex, float nSize, float nRed, float nGreen, float nBlue, float nAlpha=1.0f);

	int							numFlareImages;
	pgTexturePtr				*flareTex;
	pgVec3						pos;
	pgVec2						screenPos;

	float						baseAlpha;

	LF_VERTEX					verts[4];
};


#endif //__LENSEFLARE_HEADERFILE__
