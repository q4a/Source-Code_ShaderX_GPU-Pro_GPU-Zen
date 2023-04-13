/*********************************************************************NVMH4****
Path:  SDK\LIBS\src\NV_D3DCommon
File:  NVBModelLoader.h

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
[Greg J.] 3/14/2003

Similar to functionality in NVBScene9 but with the option to load models to 
system memory or possibly to different mesh formats.

The functionality is very limited for now, and supports loading to the
SimpleObject class.


******************************************************************************/


#ifndef __H_NVBMODELLOADER_D3D_
#define __H_NVBMODELLOADER_D3D_

#include <NVD3DVers.h>			// Includes version of D3D to use

#pragma warning(disable : 4786)
#include <vector>
#include <string>

#include <nv_nvb/nv_nvb.h>

using namespace std;

class SimpleObject;
class SimpleVBObject;

///////////////////////////////////////////////

class NVBModelLoader
{
public:
	// Function that searches for a file name and returns 
	//  the full path to the file.
	typedef std::string (*GetFilePathCallback)(const std::string &);

protected:
	nv_scene	* m_pScene;
	std::vector< nv_mesh * >	m_vpMeshes;


	// Function that finds the full path of a given filename, 
	// so apps can pass in their own file finding routines
	GetFilePathCallback	m_pGetFilePathCallback;

public:
	
	HRESULT		LoadNVBFile( const char * filename, GetFilePathCallback getFilePath );
	HRESULT		FreeNVBFile();

	void		Analyze();

	//////////////////////////////////////////
	// Call these after LoadNVBFile();
	
	int		GetNumMeshes();
	HRESULT	LoadMeshToSimpleObject( int mesh_num, SimpleObject & object );
	HRESULT LoadMeshToSimpleVBObject( int mesh_num, SimpleVBObject & vb_object );
	
	NVBModelLoader();
	~NVBModelLoader();
};


#endif






