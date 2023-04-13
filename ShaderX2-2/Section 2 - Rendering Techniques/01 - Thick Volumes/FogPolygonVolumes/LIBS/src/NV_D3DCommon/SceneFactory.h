/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  SceneFactory.h

Copyright NVIDIA Corporation 2002
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
*AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.



Comments:

A base class which ties geometry, matrices, and textures together to form a
scene.  The class handles allocation and cleanup of each type of object.

Derive your own classes from this to handle other types of objects.

Uses master lists of object pointers for allocation / cleanup, and instance
lists of object pointers to describe one or more scenes.


******************************************************************************/

#ifndef _H_NVD3DCOMMON_SCENEFACTOR_H_
#define _H_NVD3DCOMMON_SCENEFACTOR_H_


#include <NVD3DVers.h>		// Your app must make it's own version of this
							// file and ensure that it is in the #include path
							// This file controls the switch for D3D8 vs. D3D9
							// compilation.

#include <vector>
#include <string>
using namespace std;

class SimpleObject;
class SimpleVBObject;
class ShadowVolumeVBObject;
class ShadowVolumeObject;
class MatrixV;
class MatrixNode;


//#define TRACE0	FMsg
#define TRACE0	NullFunc



#ifndef MASTERLIST_DELETE
// Deletes entire master list and objects it points to
// A vector of handles point to pointers.  This macro deletes
//	 the data pointed to by the pointers, deletes the pointers
//   and clears the vector
//
// v = vector object containing handles to pointers allocated
//		with new and whos objects were allocated with new.
#define MASTERLIST_DELETE( v )						\
{	for( UINT mldel=0; mldel < v.size(); mldel++ )	\
	{	if( v.at(mldel) != NULL )					\
		{	SAFE_DELETE( *(v.at(mldel)) );			\
			delete v.at(mldel);						\
			v.at(mldel) = NULL;						\
		}											\
	}												\
	v.clear();										\
}
#endif

/////////////////////////////////////////////


class SceneFactory
{
public:
	// If you want your object instance lists to be automaticaly cleaned
	//  up at the right times (when the master lists are cleared),
	//  derive them from this base class.
	class InstanceListBase
	{
	public:

		InstanceListBase()			{ TRACE0("InstanceListBase()\n"); };
		virtual	~InstanceListBase() { TRACE0("~InstanceListBase()\n"); };
		virtual void SetAllNull()	{ };
	};

	// Function that searches for a file name and returns 
	//  the full path to the file.
	typedef std::string (*GetFilePathCallback)(const std::string &);

public:
	// You don't have to use this vector of instance lists, but
	//  its here in the base class for your convenience

	vector< InstanceListBase * >	m_vInstanceLists;



public:
	SceneFactory();
	virtual ~SceneFactory();
	virtual void SetAllNull();

	virtual HRESULT		Free();
	virtual HRESULT		FreeInstanceLists();
	virtual HRESULT		FreeMasterLists();		// will also FreeInstanceLists()

	virtual HRESULT		Initialize( NV_D3DDEVICETYPE * pDev,
									GetFilePathCallback file_path_callback = NULL );

	// Uses m_pGetFilePathCallback to try to find the file
	std::string			GetFilePath( const char * filename );

	//////////////////////////////////////////////

	MatrixNode ** NewMatrixNode( MatrixV ** ppLocalMatrix, MatrixNode ** ppParent = NULL );
	MatrixNode ** NewMatrixNode( MatrixV ** ppLocalMatrix,
									const char * pName,
									MatrixNode ** ppParent = NULL );


	MatrixV		** NewMatrix( const D3DXMATRIX & matrix );
	MatrixV		** NewMatrix();		// initializes to identity


	SimpleObject	** NewSimpleObject();
	SimpleVBObject	** NewSimpleVBObject( SimpleObject * pObj,
										  bool bDynamic = false );

	D3DXVECTOR4		** NewVector4( const D3DXVECTOR4 & vec4 );

	NV_IDIRECT3DTEXTURE **	NewTexture( char * filename, bool verbose = false );
	HRESULT					FreeTexture( NV_IDIRECT3DTEXTURE ** ppTex );


	void	TrackNames( bool track_on );	// false to disable storing of names

protected:

	// Master lists for tracking allocation & free
	//
	// When a new object, like SimpleVBObject, is created, this is 
	//  what happens:
	//  SimpleVBObject ** ppObj = new SimpleVBObject *;
	//  *ppObj = new SimpleVBObject;
	//  m_vML_SimpleVBObjects.push_back( ppObj );
	//
	// The pointer itself is not stored, but a handle (a pointer to the pointer)
	//	is kept in the Master List vectors.
	// To free an object, de-reference the handle to get the pointer.
	//   If the pointer is not null, delete the object it points to
	//   by "delete *ppObj".  Then set the pointer to NULL.
	// The NULL pointer sticks around and is not deleted until the
	//  SceneFactory is destroyed or a clean up operation is done.
	//  The master list vectors do not shrink unless the clean up
	//  is done and the pointers themselves are destroyed.
	// This ensures that instance lists can always safely and quickly
	//  de-reference their handles to find their objects.

	vector< MatrixV ** >				m_vML_MatrixVs;
	vector< MatrixNode ** >				m_vML_MatrixNodes;

	vector< SimpleVBObject ** >			m_vML_SimpleVBObjects;
	vector< SimpleObject ** >			m_vML_SimpleObjects;
	vector< D3DXVECTOR4 ** >			m_vML_Vector4s;
	vector< NV_IDIRECT3DTEXTURE ** >	m_vML_ppTextures;

	///////////////////////////////////////////////
	
	NV_D3DDEVICETYPE * 	m_pD3DDev;

	// Function that finds the full path of a given filename, 
	// so apps can pass in their own file finding routines
	GetFilePathCallback	m_pGetFilePathCallback;

	bool	m_bTrackNames;		// false to disable storing the names of things

};



#endif

