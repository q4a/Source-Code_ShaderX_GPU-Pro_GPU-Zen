///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgTransformable
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgTransformable.h,v 1.6 2003/01/06 12:51:12 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __TRANSFORMABLE_HEADERFILE__
#define __TRANSFORMABLE_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Math/pgTransform.h>




/// base class for objects which need pgTransform support
class PGLIB_API pgTransformable
{

public:
	virtual ~pgTransformable()  {}
	
	/// creates the transform object if there is none yet
	void createTransform();

	/// destroys the transform object if there is one
	void destroyTransform();


	void setPosition(const pgVec3& nPos)	{  if (transform)  transform->setPosition(nPos);}
	void setRotation(const pgVec3& nRot)	{  if (transform)  transform->setRotation(nRot);}
	void setScale(const pgVec3& nScale)		{  if (transform)  transform->setScale (nScale);}

	void move(const pgVec3& nPos)			{  if (transform)  transform->move  (nPos);		}
	void rotate(const pgVec3& nRot)			{  if (transform)  transform->rotate(nRot);		}
	void scale(const pgVec3& nScale)		{  if (transform)  transform->scalef(nScale);	}


	void setUseHierarchical(bool nSet)  {  useHierarchical = nSet;  }

	bool getUseHierarchical() const  {  return useHierarchical;  }

	pgTransform* getTransform()  {  return transform;  }

	void setTransform(pgTransform* nTrans);

	void applyTransformation();
	void undoTransformation();


protected:
	pgTransformable();			// prevent from beeing created directly...

	pgTransform*	transform;
	bool			useHierarchical;
};


#endif //__TRANSFORMABLE_HEADERFILE__
