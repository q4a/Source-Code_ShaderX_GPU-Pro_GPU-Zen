/******************************************************************************

 @File         MaterialManager.h

 @Title        A simple material manager for use with PVREngine

 @Version      

 @Copyright    Copyright (C)  Imagination Technologies Limited.

 @Platform     Independent - OGL/OGLES/OGLES2 Specific at the moment

 @Description  Manager for materials in the PVREngine

******************************************************************************/

#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

/******************************************************************************
Includes
******************************************************************************/

#include "PVRTools.h"
#include "Manager.h"


namespace pvrengine
{

	class Material;
	class UniformHandler;
	/*!***************************************************************************
	* @Class MaterialManager
	* @Brief Manager for materials in the PVREngine
	* @Description Manager for materials in the PVREngine
	*****************************************************************************/
	class MaterialManager : public Manager<Material>
	{

	public:
		/*!***************************************************************************
		@Function			MaterialManager
		@Description		blank constructor.
		*****************************************************************************/
		MaterialManager();

		/*!***************************************************************************
		@Function			~MaterialManager
		@Description		destructor.
		*****************************************************************************/
		~MaterialManager();

		/*!***************************************************************************
		@Function			MaterialManager
		@Input				i32TotalMaterials
		@Description		constructor taking initial number of materials.
		*****************************************************************************/
		MaterialManager(int i32TotalMaterials);

		/*!***************************************************************************
		@Function			MaterialManager
		@Input				pUniformHandler - Uniform handler for this collection of materials
		@Return				bool whether initialisation was successful
		@Description		Initialises material manager
		*****************************************************************************/
		bool init(UniformHandler *pUniformHandler);

		/*!***************************************************************************
		@Function			LoadMaterial
		@Input				strPFXFilename	PFX filename
		@Input				strTexturePath	The texture path
		@Input				sPODMaterial	The POD material
		@Input				sPODTexture		The POD texture
		@Input				pUniformHandler	The uniform handler for this material
		@Return				A pointer to the added material
		@Description		Adds a material to the manager from a POD.
		*****************************************************************************/
		Material*	LoadMaterial(const CPVRTString& strPFXFilename,
			const	CPVRTString& strTexturePath,
			const	SPODMaterial& sPODMaterial,
			const	SPODTexture& sPODTexture,
			UniformHandler* pUniformHandler);

		/*!***************************************************************************
		@Function			getMaterial
		@Input				u32Id	index of material
		@Return				pointer to indexed material
		@Description		retrieves a pointer to the requested material from in the
		manager.
		*****************************************************************************/
		Material*	getMaterial(const unsigned int u32Id);

		/*!***************************************************************************
		@Function			getFlatMaterial
		@Return				pointer to flat material
		@Description		Sets the built-in flat material
		*****************************************************************************/
		Material*	useDefaultFlatMaterial();

		/*!***************************************************************************
		@Function			setFlatMaterial
		@Return				pointer to flat material
		@Description		sets a different material for the flat material
		*****************************************************************************/
		Material*	setCustomFlatMaterial(Material* pFlatMaterial);

		/*!***************************************************************************
		@Function			getFlatMaterial
		@Return				pointer to flat material
		@Description		retrieves a pointer to the flat material from in the
		manager.
		*****************************************************************************/
		Material*	getFlatMaterial();

		/*!***************************************************************************
		@Function			ReportActiveMaterial
		@Input				pNewMaterial the active material
		@Description		Stores a pointer to the active material and deactivates
		the previous material (if different).
		*****************************************************************************/
		void ReportActiveMaterial(Material* pNewMaterial);

	private:

		Material		*m_pFlatMaterial;		/*! the basic flat material */

		Material*		m_pActiveMaterial;		/*! the currently active material */

		/*!***************************************************************************
		@Function			sort
		@Description		shared initialisation function for constructors
		*****************************************************************************/
		void internalInit();	
	};

}
#endif // MATERIALMANAGER_H

/******************************************************************************
End of file (MaterialManager.h)
******************************************************************************/

