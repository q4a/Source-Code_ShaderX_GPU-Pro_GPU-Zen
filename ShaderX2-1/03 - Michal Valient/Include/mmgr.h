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


/*
Used to enable memory manager macros
*/

#ifdef	new
#undef	new
#endif

#ifdef	delete
#undef	delete
#endif

//. Debug Memmanager versions...
#ifdef D3_DEBUG_MEMORY_MANAGER
    #define AllocMem(X,Y) xAllocMem_Debug(X,Y,__FILE__,__LINE__, __FUNCTION__)
    #define FreeMem(X) xFreeMem_Debug(X,__FILE__,__LINE__, __FUNCTION__)
    #define ReAllocMem(X,Y,Z) xReAllocMem_Debug(X,Y,Z,__FILE__,__LINE__, __FUNCTION__)
    #define new	(MemMgr->SetOwner(__FILE__,__LINE__,__FUNCTION__),false) ? NULL : new
    #define delete (MemMgr->SetOwner(__FILE__,__LINE__,__FUNCTION__),false) ? MemMgr->SetOwner("",0,"") : delete
#else
    #define AllocMem(X,Y) xAllocMem_Release(X,Y)
    #define FreeMem(X) xFreeMem_Release(X)
    #define ReAllocMem(X,Y,Z) xReAllocMem_Release(X,Y,Z)
#endif //D3_DEBUG_MEMORY_MANAGER
