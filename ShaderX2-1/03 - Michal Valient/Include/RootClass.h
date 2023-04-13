//*****************************************************************************
//* This file is part of the Dimension3 renderer library                      *
//* Copyright (C) 2003  Michal Valient (valient@host.sk)                      *
//*                                                                           *
//* Dimension3 renderer can be found at http://www.dimension3.host.sk         *
//*                                                                           *
//* This program is free software; you can redistribute it and/or modify      *
//* it under the terms of the GNU General Public License as published by      *
//* the Free Software Foundation; either version 2 of the License, or         *
//* (at your option) any later version.                                       *
//*                                                                           *
//* This program is distributed in the hope that it will be useful,           *
//* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
//* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
//* GNU General Public License for more details.                              *
//*                                                                           *
//* You should have received a copy of the GNU General Public License         *
//* along with this program; if not, write to the Free Software               *
//* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA * 
//*****************************************************************************
//hdr_utility_remove_tag


#pragma once

#include "Global.h"

#include "nommgr.h"
/**
 * @brief A common ancestor class
 * @author Michal Valient
 * @version 1.0
 * 
 * This class is common ancestor for all classes defined in project.
 * It has overrided operators for \c new and \c delete that support memory management
 * and memory leak protection defined in CMemoryManager (in debug versions).
 * 
 * If advanced base class is needed, use CBaseClass. It has some additional parameters
 * like <b>class name</b>, <b>object name</b> or <b>garbage collection</b> and 
 * <b>runtime creation</b> capabilities
 *
 * Use CRootClass as ancestor if your class is frequently used utility class like CMString, 
 * CHashTable2 or CStrHashTable are.
 */
class CRootClass {
public:
    /**
     * @brief Class constructor
     *
     * Currently does nothing
     */
	CRootClass();

    /**
     * @brief Class destructor
     *
     * Currently does nothing
     */
	virtual ~CRootClass();

    /**
     * @brief The release version of \c new operator
     * @param s Size of object - passed in by compiler
     * @return a pointer to buffer where object will resist.
     * @retval NULL means error (and no memory for object)
     *
     * Allocates memory for object.
     * If memory manager is presented, it uses release version of \c new method.
     * Else it uses \c malloc method.
     */
    void *operator new(size_t s);

    /**
     * @brief The release version of \c delete operator
     * @param p pointer to object
     *
     * Dealocates memory used by object.
     * If memory manager is presented and \c D3_DEBUG_MEMORY_MANAGER is defined, 
     * it uses debug version of \c delete method. If no \c D3_DEBUG_MEMORY_MANAGER is 
     * presented and memory manager is online it uses release version of \c delete.
     * If no memory manager is available it uses \c free method.
     */
    void operator delete(void *p);
};

#include "mmgr.h"

