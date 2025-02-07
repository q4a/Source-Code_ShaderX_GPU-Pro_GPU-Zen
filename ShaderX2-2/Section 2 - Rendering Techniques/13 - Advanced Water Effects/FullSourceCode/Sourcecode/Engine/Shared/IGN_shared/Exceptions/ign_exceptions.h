
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Subproject:			Graphic-Subsystem	(Ign_sub_graphic)
//*				File:				ign_exceptions.h
//*				Description:		Exception handling
//*
//********************************************************************************************************************************************************************
//*
//*				Author:				Kurt Pelzer
//*									Copyright (c) 2002 Kurt Pelzer
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

#ifndef			IGN_EXCEPTIONS_H_INCLUDED
#define			IGN_EXCEPTIONS_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> ign_exceptions.h\n")
#endif
#endif 




//------------- DEPENDENCIES -----------------------------------------------------------------------------------------------------------------------------------------

#include		<assert.h>




//------------- FORWARD DECLARATIONS ---------------------------------------------------------------------------------------------------------------------------------

struct			IGN_STRUCT_Error;




//------------- DEFINITIONS ------------------------------------------------------------------------------------------------------------------------------------------

#define			IGN_EXCEPTION_TRY				//try {
#define			IGN_EXCEPTION_CATCH				//} \
												//catch( IGN_STRUCT_Error a_Error ) { return a_Error.Process(); }

#define			IGN_ASSERT_THIS					assert( NULL!=this );

#define			IGN_ERROR( a )					IGN_STRUCT_Error( a );




//------------- DECLARATIONS -----------------------------------------------------------------------------------------------------------------------------------------

struct			IGN_STRUCT_Error
{
				IGN_STRUCT_Error						(	const	char* a_pChar )				
														{ m_pChar = a_pChar; };

protected:
			const char*									m_pChar;

public:
	inline	IGNRESULT	Process							(			void )
														{ return IGN_FAILED; };
};




//------------- PREPROCESSOR END -------------------------------------------------------------------------------------------------------------------------------------

#endif // IGN_EXCEPTIONS_H_INCLUDED
