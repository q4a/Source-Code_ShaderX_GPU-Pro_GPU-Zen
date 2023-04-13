///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgLighting
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgLighting.h,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __LIGHTING_HEADERFILE__
#define __LIGHTING_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Base/pgPtrList.h>
#include <pgLib/Math/pgVec4.h>
#include <pgLib/Render/pgD3DObject.h>
#include <pgLib/Render/pgLight.h>

#include <D3DX8.h>


/// A pgLighting is a fix set of lights which can be applied to an object up to be rendered
/**
 *  A pgLighting can conist of up to 8 lights.
 *  Additionally a common base ambient value can be set.
 */
class PGLIB_API pgLighting : public pgD3DObject
{
public:
	pgLighting();


	/// Adds a light (only up to 8 lights are allowed)
	void addLight(pgLight* nLight)  {  lights.addTail(nLight);  }


	/// Returns how many lights are set.
	int getNumLights() const  {  return lights.getSize();  }


	/// Retrieves a specific light from the lighting
	pgLight* getLight(int nIndex)  {  return lights[nIndex];  }


	/// Removes a light
	void removeLight(int nIndex)  {  lights.removeIndex(nIndex);  }


	/// Removes all lights at once
	void removeAllLights()  {  lights.removeAll();  }


	/// Sets the base ambient color value
	/**
	 *  This ambient color value is added to all lights
	 */
	void setBaseAmbient(const pgVec4& nColor);


	const pgVec4& getBaseAmbient() const  {  return ambient;  }


	/// Applies the lighting to the render device
	/**
	 *  This method activates lighting. After this method was
	 *  called lighting is enabled until turnOffLighting() is called
	 */
	void applyLighting();


	virtual void deleteDeviceObjects()  {}
	virtual bool restoreDeviceObjects()  {  return true;  }

	/// Turns of lighting
	void turnOffLighting();

protected:
	pgPtrList<pgLight>	lights;
	pgVec4								ambient;
	DWORD								iAmbient;
};


#endif //__LIGHTING_HEADERFILE__
