
#ifndef __DX_OBJECT_H
#define __DX_OBJECT_H



class CDXObject {
public:
	virtual ~CDXObject() { }

	virtual HRESULT onCreateDevice( IDirect3DDevice8* device );
	virtual HRESULT onLostDevice();
	virtual HRESULT onResetDevice();
	virtual HRESULT onDestroyDevice();

	virtual void	render( float lastFrameTime, float time );

protected:
	const IDirect3DDevice8&	getDevice() const { return *mDevice; }
	IDirect3DDevice8&	getDevice() { return *mDevice; }
	
private:
	IDirect3DDevice8*	mDevice;
};


class CDXObjectContainer : public CDXObject {
public:
	virtual ~CDXObjectContainer();

	void addObject( CDXObject& o ) { mObjects.push_back( &o ); }
	
	virtual HRESULT onCreateDevice( IDirect3DDevice8* device );
	virtual HRESULT onLostDevice();
	virtual HRESULT onResetDevice();
	virtual HRESULT onDestroyDevice();

	virtual void	render( float lastFrameTime, float time );

private:
	typedef std::list<CDXObject*>	TDXObjectList;
private:
	TDXObjectList	mObjects;
};


#endif

