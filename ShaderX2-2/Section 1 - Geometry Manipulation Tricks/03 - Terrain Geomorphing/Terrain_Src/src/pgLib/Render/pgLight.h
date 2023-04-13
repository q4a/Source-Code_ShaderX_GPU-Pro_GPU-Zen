///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgLight
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgLight.h,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __LIGHT_HEADERFILE__
#define __LIGHT_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Math/pgVec3n.h>
#include <pgLib/Math/pgVec4.h>

#include <D3DX8.h>


/// pgLight stores all information about a light
/**
 *  pgLight object can be a point, a spot or a
 *  directional light. Some of the properties only
 *  make sense for specific light types.
 */
class PGLIB_API pgLight
{
friend class pgLighting;
public:
	/// Type of light source
	enum TYPE {
				TYPE_POINT = D3DLIGHT_POINT,				/**<  Point light: direction has to be calculated for each vertex  */
				TYPE_SPOT = D3DLIGHT_SPOT,					/**<  Spot light: the slowest type of light  */
				TYPE_DIRECTIONAL = D3DLIGHT_DIRECTIONAL		/**<  Directional light:  fasted type of light.  direction is same for each vertex  */
	};


	/// Creates a default white light
	pgLight();


	/// Creates a light where the most important values are set
	pgLight(TYPE nType, const pgVec3n& nDir, const pgVec3& nPos, const pgVec4& nAmbient, const pgVec4& nDiffuse, const pgVec4& nSpecular, float nRange) :
		type(nType), dir(nDir), pos(nPos), ambient(nAmbient), diffuse(nDiffuse), specular(nSpecular), range(nRange)
		{  updateAll();  }


	/// Sets all color values at once
	void setColors(const pgVec4& nAmbient, const pgVec4& nDiffuse, const pgVec4& nSpecular);


	/// Sets the type of light (point, spot, directional)
	void setType(TYPE nType)
		{  type = nType;  }


	/// Sets the direction of the light
	/**
	 *  This property is only valid for
	 *  spot or directional lights.
	 */
	void setDirection(const pgVec3n& nDir);


	/// Sets the position of the light
	/**
	 *  This property is only valid for point
	 *  and spot lights.
	 */
	void setPosition(const pgVec3& nPos);


	/// Sets the ambient color property
	void setAmbient(const pgVec4& nAmbient);


	/// Sets the diffuse color property
	void setDiffuse(const pgVec4& nDiffuse);


	/// Sets the specular color property
	void setSpecular(const pgVec4& nSpecular);


	/// Sets the range of the light
	/**
	 *  This property is only valid for point
	 *  and spot lights.
	 */
	void setRange(float nRange);


	// access methods
	//
	const pgVec3n& getDirection() const  {  return dir;  }
	const pgVec3& getPosition() const  {  return pos;  }
	const pgVec4& getAmbient() const  {  return ambient;  }
	const pgVec4& getDiffuse() const  {  return diffuse;  }
	const pgVec4& getSpecular() const  {  return specular;  }

protected:
	void updateAll();

	TYPE	type;
	pgVec3n	dir;
	pgVec3	pos;
	pgVec4	ambient, diffuse, specular;
	float	range;

	D3DLIGHT8 ddLight;
};


#endif //__LIGHT_HEADERFILE__
