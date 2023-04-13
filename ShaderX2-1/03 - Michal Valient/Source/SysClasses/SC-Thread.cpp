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


#include "stdafx.h"

#include "SC-Thread.h"

//The critical section class
CThreadCriticalSection::CThreadCriticalSection() : CRootClass(){
    InitializeCriticalSection(&CritSection);
    _LOG(NULL, D3_DV_GRP3_LEV0, "Created critical section %p", this);
}

CThreadCriticalSection::~CThreadCriticalSection() {
    DeleteCriticalSection(&CritSection);
    _LOG(NULL, D3_DV_GRP3_LEV0, "Deleted critical section %p", this);
}

void CThreadCriticalSection::Enter() {
    EnterCriticalSection(&CritSection);
    _LOG(NULL, D3_DV_GRP3_LEV4, "Entered critical section %p", this);
}

void CThreadCriticalSection::Leave() {
    LeaveCriticalSection(&CritSection);
    _LOG(NULL, D3_DV_GRP3_LEV4, "Leaved critical section %p", this);
}

bool CThreadCriticalSection::TryEnter() {
    bool ret = (TryEnterCriticalSection(&CritSection) == TRUE)? true : false;
    if (ret) {
        _LOG(NULL, D3_DV_GRP3_LEV4, "Tried and entered critical section %p", this);
        return true;
    } else {
        _LOG(NULL, D3_DV_GRP3_LEV4, "Tried and failed to enter critical section %p", this);
        return false;
    }
}

//Critical section lock
CThreadLock::CThreadLock(CThreadCriticalSection &Section) {
    int LogE = _LOGB(NULL, D3_DV_GRP3_LEV4, "Locking critical section %p", &Section);
    Section.Enter();
    this->Section = &Section;
    _LOGE(LogE, "Locked");
}

CThreadLock::~CThreadLock() {
    this->Section->Leave();
    _LOG(NULL, D3_DV_GRP3_LEV4, "Leaving critical section %p", &Section);
}

//Thread
CThread::CThread(char *ObjName, CEngine *OwnerEngine) : CBaseClass(ObjName, OwnerEngine) {
    int LogE = _LOGB(this, D3_DV_GRP1_LEV0, "Creating");
    Thread = 0;
	ThreadID = 0;
	ShutdownRequest = false;
	Suspended = true;
    ThreadSection = new CThreadCriticalSection;
    _LOGE(LogE, "Created with section %p",ThreadSection);
}

CThread::~CThread() {
	if (Thread) CloseHandle(Thread);
    delete ThreadSection;
    _LOG(this, D3_DV_GRP1_LEV0, "Destroyed");
}

bool CThread::GetShutdownRequest() {
    int LogE = _LOGB(this, D3_DV_GRP1_LEV4, "Requesting shutdown status");
    CThreadLock Guard(*ThreadSection);
    _LOGE(LogE, "Requested, returning '%s'",(ShutdownRequest)? "true" : "false");
	return ShutdownRequest;
}

bool CThread::Create(DWORD ThreadStackSize) {
    int LogE = _LOGB(this, D3_DV_GRP1_LEV0, "Creating thread");
    Thread = CreateThread(NULL, ThreadStackSize, ThreadFunction, this, CREATE_SUSPENDED, &ThreadID);
	
    if (Thread == NULL) {
        _WARN(this, ErrMgr->TSystem, "Windows returned error %0x", GetLastError());
        _LOGE(LogE, "Ending with failure");
        return false;
    } else {
        _LOGE(LogE, "Created thread with handle %u and id %u",Thread, ThreadID);
        return true;
    }
}

void CThread::WaitTo() {
    int LogE = _LOGB(this, D3_DV_GRP1_LEV0, "Waiting for this thread");
    WaitForSingleObject(Thread, INFINITE);
    _LOGE(LogE, "End of wait");
}

void CThread::Resume() {
    int LogE = _LOGB(this, D3_DV_GRP1_LEV1, "Resuming thread");
    if (Suspended) {
		CThreadLock Guard(*ThreadSection);
		if (Suspended) {
			ResumeThread(Thread);
			Suspended = false;
		}
	}
    _LOGE(LogE, "Resumed");
}

void CThread::Shutdown() {
    int LogE = _LOGB(this, D3_DV_GRP1_LEV1, "Requesting shutdown");
    if (!ShutdownRequest) {
		CThreadLock *Guard = new CThreadLock(*ThreadSection);
        if (!ShutdownRequest) {
			ShutdownRequest = true;
        }
        delete Guard;
		Resume();
	}
    _LOGE(LogE, "Requested");
}

void CThread::Start() {
    int LogE = _LOGB(this, D3_DV_GRP1_LEV0, "Starting thread");
    Resume();
    _LOGE(LogE, "Started");
}

void CThread::Suspend() {
    int LogE = _LOGB(this, D3_DV_GRP1_LEV0, "Suspending thread");
    if (!Suspended) {
		CThreadLock Guard(*ThreadSection);
		if (!Suspended) {
			SuspendThread(Thread);
			Suspended = true;
		}
	}
    _LOGE(LogE, "Suspended");
}

DWORD CThread::GetThreadId() const {
	return ThreadID;
}

DWORD WINAPI ThreadFunction(LPVOID args) {
	CThread *Thread = reinterpret_cast<CThread *>(args);
    if (Thread) {
        Thread->Run();
    }
	return 0;
}
