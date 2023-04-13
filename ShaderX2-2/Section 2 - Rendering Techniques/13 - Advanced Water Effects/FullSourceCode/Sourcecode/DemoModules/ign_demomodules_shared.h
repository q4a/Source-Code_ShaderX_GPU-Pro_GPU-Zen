
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Subproject:			Demo-Modules
//*				File:				ign_demomodules_shared.h
//*				Description:		Shared file with DLL and OBJ interfaces
//*
//********************************************************************************************************************************************************************
//*
//*				Author:				Kurt Pelzer
//*									Copyright (c) 2002, 2003 Kurt Pelzer
//*									All rights reserved.
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Status:				In progress
//*
//********************************************************************************************************************************************************************
//*
//*				History:
//*
//*				04/25/03| x.x.x		## Special ShaderX-2 release	(kp) ##
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************




//------------- PREPROCESSOR -----------------------------------------------------------------------------------------------------------------------------------------

#ifndef			IGN_DEMOMODULES_SHARED_H_INCLUDED
#define			IGN_DEMOMODULES_SHARED_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> ign_demomodules_shared.h\n")
#endif
#endif 




//------------- DEPENDENCIES -----------------------------------------------------------------------------------------------------------------------------------------

#include "../Engine/Shared/IGN_shared/Types/ign_types_shared.h"
#include "../Engine/Shared/IGN_shared/Modules/Subsystems/ign_graphic_sub_shared.h"
#include <stdio.h>





//------------- FORWARD DECLARATIONS ---------------------------------------------------------------------------------------------------------------------------------

struct			IGN_STRUCT_IFDemoModule;
struct			IGN_STRUCT_DMVersion;
struct			IGN_STRUCT_DMEnableTable;




//------------- DEFINITIONS ------------------------------------------------------------------------------------------------------------------------------------------

//********************************************************************************************************************************************************************
//				Typedefs for all interface methods
//********************************************************************************************************************************************************************

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// DLL export methods
//
typedef			IGNRESULT	IGN_DLLCALL	IGN_METHOD_GetSubHookDemoModule						(			IGN_STRUCT_IFDemoModule**		a_ppInterface				);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// DLL interface methods
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_DMGetVersion								(			IGN_STRUCT_DMVersion*			a_pVersion					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GetEnableTable							(			IGN_STRUCT_DMEnableTable*		a_pEnableTable				);																										
typedef			IGNU32		IGN_OBJCALL	IGN_METHOD_GetResolutionCount						(			void														);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GetResolution							(	const	IGNU32							a_u32ResolutionIndex,
																										char**							a_ppResolutionDesc			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_GetModuleDescriptionText					(			char**							a_ppcharText				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_ToggleFullscreen							(	const	IGNU32							a_u32Fullscreen				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_SelectDirectX90							(			void														);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_SelectDirectX81							(			void														);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_SelectDirectX80							(			void														);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_SelectOpenGL14							(			void														);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_SelectOpenGL20							(			void														);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_SelectResolution							(	const	IGNU32							a_u32ResolutionIndex		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_Run										(			void														);




//------------- DECLARATIONS -----------------------------------------------------------------------------------------------------------------------------------------

struct	IGN_STRUCT_IFDemoModule
{
	IGN_METHOD_DMGetVersion*							m_pGetVersion;								// Pointer to GetVersion(..) method
	IGN_METHOD_GetEnableTable*							m_pGetEnableTable;							// Pointer to GetEnableTable(..) method
	IGN_METHOD_GetResolutionCount*						m_pGetResolutionCount;						// Pointer to GetResolutionCount(..) method
	IGN_METHOD_GetResolution*							m_pGetResolution;							// Pointer to GetResolution(..) method
	IGN_METHOD_GetModuleDescriptionText*				m_pGetModuleDescriptionText;				// Pointer to GetModuleDescriptionText(..) method
	IGN_METHOD_ToggleFullscreen*						m_pToggleFullscreen;						// Pointer to ToggleFullscreen(..) method
	IGN_METHOD_SelectDirectX90*							m_pSelectDirectX90;							// Pointer to SelectDirectX90(..) method
	IGN_METHOD_SelectDirectX81*							m_pSelectDirectX81;							// Pointer to SelectDirectX81(..) method
	IGN_METHOD_SelectDirectX80*							m_pSelectDirectX80;							// Pointer to SelectDirectX80(..) method
	IGN_METHOD_SelectOpenGL14*							m_pSelectOpenGL14;							// Pointer to SelectOpenGL14(..) method
	IGN_METHOD_SelectOpenGL20*							m_pSelectOpenGL20;							// Pointer to SelectOpenGL20(..) method
	IGN_METHOD_SelectResolution*						m_pSelectResolution;						// Pointer to SelectResolution(..) method
	IGN_METHOD_Run*										m_pRun;										// Pointer to Run(..) method
};

struct	IGN_STRUCT_DMVersion
{
	IGNCHAR												m_pcharIdentifier[256];						// Module identifier string
	IGNCHAR												m_pcharVendor[256];							// Vendor string
	IGNU32												m_u32MainVersionNumber;						// Main version number of module
	IGNU32												m_u32SubVersionNumber;						// Sub version number of module
	IGNCHAR												m_pcharReserved[256];						// Reserved (not in use)
};

struct	IGN_STRUCT_DMEnableTable
{
	IGNBOOL												m_bAboutDemoModule;							// TRUE enables 'AboutDemoModule' button
	IGNBOOL												m_bFullscreen;								// TRUE enables 'Fullscreen' button
	IGNBOOL												m_bDirectX90;								// TRUE enables 'DirectX9.0' button
	IGNBOOL												m_bDirectX81;								// TRUE enables 'DirectX8.1' button
	IGNBOOL												m_bDirectX80;								// TRUE enables 'DirectX8.0' button
	IGNBOOL												m_bOpenGL14;								// TRUE enables 'OpenGL1.4' button
	IGNBOOL												m_bOpenGL20;								// TRUE enables 'OpenGL2.0' button
	IGNBOOL												m_bRun;										// TRUE enables 'Run' button
};




//------------- PREPROCESSOR END -------------------------------------------------------------------------------------------------------------------------------------

#endif // IGN_DEMOMODULES_SHARED_H_INCLUDED





