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


//------------------------------------------------------------------------
// I rewrote these classes from those made by David Poon (6 May 2001) to
// be included into Dimension3 Rendering framework. Also a little bit of 
// enhancements is done
// Originals are copyright (C)2001 David Poon and are public under GNU GPL
//------------------------------------------------------------------------
#pragma once

#include "baseclass.h"

///The critical section class
class CThreadCriticalSection : public CRootClass {
private:
	CRITICAL_SECTION CritSection;
public:
	CThreadCriticalSection();
	virtual ~CThreadCriticalSection();

    ///Enter critical section - wait until it is free free
	void Enter();
    ///Leave critical section
	void Leave();
    ///Try to enter critical section - return false if not inside
    bool TryEnter();
};

/// A CThreadLock provides a safe way to acquire and release a CThreadCriticalSection. 
/// The CThreadCriticalSection is acquired when the CThreadLock it created.
/// The CThreadCriticalSection is released when the CThreadLock goes out of scope.
class CThreadLock : public CRootClass {
public:
	CThreadLock(CThreadCriticalSection &Section);
	virtual ~CThreadLock();
private:
	CThreadCriticalSection *Section;
};

class CThread : public CBaseClass {
private:
	static friend DWORD WINAPI ThreadFunction(LPVOID args);
	
	HANDLE                      Thread;
	DWORD                       ThreadID;
	volatile bool               ShutdownRequest;
	volatile bool               Suspended;
	CThreadCriticalSection     *ThreadSection;
protected:
	bool GetShutdownRequest();
	virtual void Run() = 0;
public:
	CThread(char *ObjName, CEngine *OwnerEngine);
	virtual ~CThread();

	bool Create(DWORD ThreadStackSize);
	DWORD GetThreadId() const;
	void Start();
	void WaitTo();

	void Resume();
	void Suspend();
	void Shutdown();

    MAKE_CLSNAME("CThread");
};
