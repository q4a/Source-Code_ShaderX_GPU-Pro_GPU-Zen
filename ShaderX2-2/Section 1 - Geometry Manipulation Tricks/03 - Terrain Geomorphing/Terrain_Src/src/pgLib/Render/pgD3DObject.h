///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgD3DObject
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgD3DObject.h,v 1.2 2003/01/05 19:55:53 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __D3DOBJECT_HEADERFILE__
#define __D3DOBJECT_HEADERFILE__


#include <pgLib/pgLib.h>

#include <d3d8.h>


/// This class is the base class of all pgLib objects which directly use the render device
class PGLIB_API pgD3DObject
{
public:
	pgD3DObject() : pd3dDevice(NULL)  {}

	virtual ~pgD3DObject()  {}

	/// Checks if the render device is set or can be retrieved by pgDirectX
	/**
	 *  If the the render device can not be retrieved, the message
	 *  nMsg is logged as an error and false is returned.
	 */
	bool checkDevice(const char* nMsg);


	/// The implementing object has to destroy all device dependent objects
	/**
	 *  When switching to fullscreen or changing window size
	 *  the render device has to be destroyed and afterwards
	 *  recreated. This enforces, that all device depended
	 *  objects are destroyed and recreated too.
	 *  Its the duty of the implementating object destroy
	 *  all device dependent objects and call all
	 *  sub-objects' deleteDeviceObjects() methods.
	 */
	virtual void deleteDeviceObjects() = 0;


	/// The implementing object has to recreate all device dependent objects
	/**
	 *  Its the duty of the implementating object recreate
	 *  all device dependent objects and call all
	 *  sub-objects' restoreDeviceObjects() methods.
	 */
	virtual bool restoreDeviceObjects() = 0;


protected:
	LPDIRECT3DDEVICE8	pd3dDevice;
};


#endif //__D3DOBJECT_HEADERFILE__
