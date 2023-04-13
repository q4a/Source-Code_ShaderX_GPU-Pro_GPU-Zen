#include "stdafx.h"
#include "DXObject.h"
#include <cassert>


// ------------------------------------------------------------------

HRESULT CDXObject::onCreateDevice( IDirect3DDevice8* device )
{
	assert( device );
	mDevice = device;
	return S_OK;
}

HRESULT CDXObject::onLostDevice()
{
	return S_OK;
}

HRESULT CDXObject::onResetDevice()
{
	return S_OK;
}

HRESULT CDXObject::onDestroyDevice()
{
	return S_OK;
}

void CDXObject::render( float lastFrameTime, float time )
{
}


// ------------------------------------------------------------------

CDXObjectContainer::~CDXObjectContainer()
{
	TDXObjectList::iterator it;
	for( it = mObjects.begin(); it != mObjects.end(); ++it )
		delete *it;
}

HRESULT CDXObjectContainer::onCreateDevice( IDirect3DDevice8* device )
{
	CDXObject::onCreateDevice( device );

	TDXObjectList::iterator it;
	for( it = mObjects.begin(); it != mObjects.end(); ++it )
		(*it)->onCreateDevice( device );

	return S_OK;
}

HRESULT CDXObjectContainer::onLostDevice()
{
	CDXObject::onLostDevice();
	
	TDXObjectList::iterator it;
	for( it = mObjects.begin(); it != mObjects.end(); ++it )
		(*it)->onLostDevice();
	
	return S_OK;
}

HRESULT CDXObjectContainer::onResetDevice()
{
	CDXObject::onResetDevice();
	
	TDXObjectList::iterator it;
	for( it = mObjects.begin(); it != mObjects.end(); ++it )
		(*it)->onResetDevice();

	return S_OK;
}

HRESULT CDXObjectContainer::onDestroyDevice()
{
	CDXObject::onDestroyDevice();

	TDXObjectList::iterator it;
	for( it = mObjects.begin(); it != mObjects.end(); ++it )
		(*it)->onDestroyDevice();
	
	return S_OK;
}

void CDXObjectContainer::render( float lastFrameTime, float time )
{
	TDXObjectList::iterator it;
	for( it = mObjects.begin(); it != mObjects.end(); ++it )
		(*it)->render( lastFrameTime, time );
}
