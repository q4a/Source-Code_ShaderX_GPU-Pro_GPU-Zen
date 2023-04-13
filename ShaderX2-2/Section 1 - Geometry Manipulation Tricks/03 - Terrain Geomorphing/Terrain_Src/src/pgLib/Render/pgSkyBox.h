///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgSkyBox
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgSkyBox.h,v 1.1.1.1 2002/10/20 09:33:03 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __SKYBOX_HEADERFILE__
#define __SKYBOX_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Render/pgD3DObject.h>
#include <pgLib/Render/pgMesh.h>


/// Creates and renders a skybox
/**
 *  The skybox is automatically rotated to the direction
 *  retrieved from pgIDirectX
 */
class PGLIB_API pgSkyBox : public pgD3DObject
{
public:
	enum  {  NUM_SIDES = 6,
			 NUM_VERTICES = NUM_SIDES*4  };

	enum SIDE  {  FRONT=0, BACK, LEFT, RIGHT, UP, DOWN  };


	/// If nDownSide is false the bottom texture of the skybox is not drawn
	/**
	 *  In order to save texture memory nDown should be passed as NULL if nDownSide is true.
	 */
	pgSkyBox(bool nDownSide=true);


	/// Sets the textures for the six sides of the skybox cube.
	void setTextures(pgTexture* nFront, pgTexture* nBack,
					 pgTexture* nLeft, pgTexture* nRight,
					 pgTexture* nUp, pgTexture* nDown);


	/// Creates the skybox mesh
	void create();


	/// Renders the skybox
	void render();


	void deleteDeviceObjects();
	bool restoreDeviceObjects();

protected:
	pgVec3 sizeH;

	bool						downSide;
	pgTexturePtr				textures[6];
	PDIRECT3DVERTEXBUFFER8		vBuffer;
};


#endif //__SKYBOX_HEADERFILE__
