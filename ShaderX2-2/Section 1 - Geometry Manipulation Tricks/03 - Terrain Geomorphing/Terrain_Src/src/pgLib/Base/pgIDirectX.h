///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgIDirectX
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgIDirectX.h,v 1.7 2003/01/06 12:51:11 daniel Exp $
///////////////////////////////////////////////////////////////////

#ifndef __DIRECTX_HEADERFILE__
#define __DIRECTX_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Base/pgImage.h>
#include <pgLib/Math/pgVec4.h>

#include <d3dx8.h>


class pgMatrix;
class pgVec3;
class pgPlane;


/// pgIDirectX is the main connection of code outside of pgLib to directx
/**
 *  pgIDirectX allows you to take advantage of working directly with directx.
 *  If you write new classes for pgLib surely have to work with directx.
 *  pgIDirectX allows you to access the view and projection matrix, although
 *  you should be careful when changing these, since a lot of classes depend on them.
 */
namespace pgIDirectX
{
	/// Flags to be passed to renderBegin()
	/**
	 *  In order to pass more than one flag
	 *  combine them with the or-operator ('|')
	 */
	enum {
			CLEAR_Z = 1,		/**< Tells the render device to clear the z-buffer */
			CLEAR_COLOR = 2		/**< Tells the render device to clear the color-buffer */
	};

	enum POOLTYPE {
			POOL_DEFAULT =		D3DPOOL_DEFAULT,
			POOL_MANAGED =		D3DPOOL_MANAGED,
			POOL_SYSTEMMEM =	D3DPOOL_SYSTEMMEM,
			POOL_SCRATCH =		D3DPOOL_SCRATCH
	};

	/// Initializes the containter class with the given device object.
	/**
	 *  This method is automatically called by pgAppStub.
	 *  (If the application framework is used...)
	 */
	PGLIB_API void init(LPDIRECT3DDEVICE8 nDevice, LPDIRECT3D8 nD3D);


	/// This method has to be called before a new frame is rendered.
	/**
	 *  As flags pass:
	 *    CLEAR_Z if the z-buffer should be cleared
	 *    CLEAR_COLOR if the color (frame) buffer should be cleared
	 */
	PGLIB_API bool renderBegin(int nFlags = CLEAR_Z|CLEAR_COLOR);

	/// This method has to be called after rendering a complete frame
	/**
	 *  This method calls DirectX to finish the scene and present the rendered image.
	 */
	PGLIB_API void renderEnd();


	/// Sets the color, which is used to clear the color (frame) buffer.
	/**
	 *  pgColor takes 4 components as float values [0.0-1.0]:
	 *  Red, Green, Blue and Alpha
	 *
	 */
	PGLIB_API void setClearColor(const pgColor& nColor);


	/// Sets the z-value which is used to clear the z-buffer
	/**
	 *  Except in very special cases you'll always want this value to be 1.0
	 */
	PGLIB_API void setClearZ(float nZ);


	/// Returns the color, which is used to clear the color buffer
	PGLIB_API pgColor getClearColor();


	/// Returns the value which is used to clear the z-buffer
	PGLIB_API float getClearZ();


	/// Sets a new view matrix
	/**
	 *  Be aware, that strange things can (will) happen, if you change the view
	 *  matrix during rendering a frame and do not restore it, before rendering
	 *  an other object.
	 */
	PGLIB_API void setViewMatrix(const pgMatrix& nMatrix);


	/// returns a pointer to the current view (camera) matrix
	/**
	 *  this method returns the accumulated view matrix. if the pure camera matrix
	 *  is needed, getCameraMatrix should be used.
	 */
	PGLIB_API const pgMatrix& getViewMatrix();


	/// returns a pointer to the current view (camera) matrix and casts it to dx-Matrix
	PGLIB_API const D3DXMATRIX* getD3DViewMatrix();


	/// Sets a new camera matrix
	/**
	 *  only pgSteering or similiar classes should invoke this method
	 */
	PGLIB_API void setCameraMatrix(const pgMatrix& nMatrix);


	/// returns a pointer to the camera matrix
	/**
	 *  method returns a view matrix which only contains the camera's
	 *  transformation. this method is useful if a non-hierarchical
	 *  (top-level) transformation shall be done.
	 */
	PGLIB_API const pgMatrix& getCameraMatrix();


	/// returns a pointer to the current projection matrix
	PGLIB_API const pgMatrix& getProjectionMatrix();


	PGLIB_API void pushMatrix(const pgMatrix& nMatrix);

	PGLIB_API const pgMatrix& popMatrix();


	/// Sets a projection matrix directly
	/*
	 *  Be aware, that strange things can (will) happen, if you change the projection
	 *  matrix during rendering a frame and do not restore it, before rendering
	 *  an other object.
	 */
	PGLIB_API void setProjectionMatrix(const pgMatrix& nMatrix);


	/// Sets the projection matrices with the given aspect ratio, field of view, near plane and far plane
	PGLIB_API void setProjectionMatrix(float nAspectRatio, float nFOV, float nNear, float nFar);


	/// Sets a new aspect ratio and updates the projection matrix
	PGLIB_API void setAspectRatio(float nAspectRatio);


	/// Returns the currently set aspect ratio
	PGLIB_API float getAspectRatio();


	/// Returns the currently set field of view (FOV) for the y-axis.
	/**
	 *  Since normally four aspect ration won't be 1.0 the FOV for x-axis
	 *  and y-axis is different.
	 */
	PGLIB_API float getFOVY();


	/// Returns the currently set field of view (FOV) for the x-axis.
	/**
	 *  Since normally four aspect ration won't be 1.0 the FOV for x-axis
	 *  and y-axis is different.
	 */
	PGLIB_API float getFOVX();


	/// Returns the distance of the near clipping plane to the camera position
	PGLIB_API float getNearPlane();


	/// Returns the distance of the far clipping plane to the camera position
	PGLIB_API float getFarPlane();


	/// Sets a factor by which the skybox follows the user camera in height
	/**
	 *  Normally a skybox is fixed and the camera stays always in the center of the box.
	 *  For better emmersion it can make sense to let camera move a little bit in the skybox.
	 *  Be aware that if the camera moves to far away from the center, the box shape will become appearant.
	 *  Default value is 0.0
	 */
	PGLIB_API void setSkyBoxHeightFactor(float nFactor);


	/// Returns the skybox height factor
	PGLIB_API float getSkyBoxHeightFactor();

	/// Switches wireframe rendering on/off
	/**
	 *  It's up to every object that can be rendered to determine
	 *  wether it should be rendered as wireframe by calling getWireframe()
	 */
	PGLIB_API void switchFillOperation();


	/// Returns the current D3D fill mode
	PGLIB_API int getFillFlag();


	/// Returns true if current fill mode is solid
	PGLIB_API bool isFillSolid();


	/// Returns nCullOp if current fill mode is solid; otherwise 0 is returned
	PGLIB_API int getCullFlag(int nCullOp);


	/// Sets a pool type which all objects will use
	PGLIB_API void setPoolType(POOLTYPE nPool);


	/// Returns the currently set D3D memory pool type
	PGLIB_API D3DPOOL getPoolType();


	/// Switches updating object visibility on/off
	/**
	 *  It's up to every object that can be rendered to determine
	 *  wether it really stops updating visibility or not by calling getUpdateVisibility()
	 */
	PGLIB_API void switchUpdateVisibility();


	/// Returns true if the visibility updating is set to on
	PGLIB_API bool getUpdateVisibility();


	/// Sets the camera's position
	/**
	 *  This method only saves the camera's position but does not change the view matrix accordingly
	 *  It's up to the calling method to update the view matrix. The camera's position is only saved,
	 *  because some objects need to know it.
	 */
	PGLIB_API void setCameraPos(const pgVec3& nPos);


	/// Returns the camera's position as it was set with setCameraPos()
	PGLIB_API pgVec3& getCameraPos();


	/// Creates six clipping planes
	PGLIB_API void createClippingPlanes(pgPlane nPlanes[6]);


	/// Sets a color in pgVec4 (pgColor) format into a D3DCOLORVALUE color value
	/**
	 *  Use this method to convert colors from pgLib format into DirectX format
	 */
	PGLIB_API void setColor(D3DCOLORVALUE& nDst, const pgVec4& nSrc);


	/// Sets a 3d vector in pgVec3 format into a D3DVECTOR value
	/**
	 *  Use this method to convert vectors from pgLib format into DirectX format
	 */
	PGLIB_API void setD3DVecFromVec3(D3DVECTOR& nDst, const pgVec3& nSrc);


	/// Sets a 3d vector in pgVec3 format into a D3DVECTOR value
	/**
	 *  Use this method to convert vectors from pgLib format into DirectX format
	 */
	PGLIB_API void setVec3FromVecD3D(pgVec3& nDst, const D3DVECTOR& nSrc);


	PGLIB_API void setVec3FromVecD3D(pgVec3& nDst, const D3DXVECTOR4& nSrc);

	/// Resets all triangle counters.
	/**
	 *  The statistics count how many triangles have been drawn for
	 *  level, terrain, particles and other objects (non-level).
	 */
	PGLIB_API void resetStats();

	/// Adds triangles to the level triangle counter
	PGLIB_API void addLevelTris(int nNum);

	/// Adds triangles to the terrain triangle counter
	PGLIB_API void addTerrainTris(int nNum);

	/// Adds triangles to the non-level triangle counter
	PGLIB_API void addNonLevelTris(int nNum);

	/// Adds triangles to the particle triangle counter
	PGLIB_API void addParticles(int nNum);

	/// Returns how many level triangles have been counted
	PGLIB_API int getNumLevelTris();

	/// Returns how many terrain triangles have been counted
	PGLIB_API int getNumTerrainTris();

	/// Returns how many non-level triangles have been counted
	PGLIB_API int getNumNonLevelTris();

	/// Returns how many particle triangles have been counted
	PGLIB_API int getNumParticles();


	/// Returns the D3D render device
	/**
	 *  This method returns the render device which was passed to init()
	 */
	PGLIB_API LPDIRECT3DDEVICE8 getDevice();

	/// Returns the D3D device
	/**
	 *  This method returns the d3d device which was passed to init()
	 */
	PGLIB_API LPDIRECT3D8 getD3D();


	/// Returns the availble texture memoy in megabytes.
	/**
	 *  The returned value is only a very coarse value and should not
	 *  be taken for exact calculations.
	 */
	PGLIB_API int getAvailableTextureMemory();


	/// Returns the number of texture blending stages the device has
	PGLIB_API int getNumTextureBlendStages();


	/// Returns true if the device is able to work with compressed textures
	PGLIB_API bool canUseCompressedTextureFormat(pgImage::FORMAT nFormat);


	/// If nSet ist true write only buffers will not be used.
	/**
	 *  Write-Only buffers are usually faster than buffers wit
	 *  read support. Write-Only buffers is the default setting.
	 */
	PGLIB_API void setDisableWriteOnlyBuffers(bool nSet);


	/// Returns wether an object should use write-only buffers or not
	PGLIB_API bool getDisableWriteOnlyBuffers();
};


#endif //__DIRECTX_HEADERFILE__
