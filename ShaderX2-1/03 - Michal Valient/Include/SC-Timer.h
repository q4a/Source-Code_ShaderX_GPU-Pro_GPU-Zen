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


// SC-Timer.h: interface for the CTimer class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BaseClass.h"

class CTimer : public CBaseClass {
protected:
    bool    HighResTimer;       ///if we have hires timer
    bool    Paused;             ///Timer was paused
    __int64 HW_CountsPerSecond; ///How many counts per second the timer has
    __int64 HW_StartTickCount;  ///Timer's tick count when started.
    __int64 App_TickCount;      ///Application tick count (correctly handles freezes and resumes).
    __int64 App_OverleapCount;  ///How many times has App_TickCount overleapped
    __int64 HW_OverleapCount;   ///How many times has been the HW tick counter overleapped

    __int64 HW_LastGetTime;     ///Previous TickCountSample (real from hardware)
    __int64 TickDelta;          ///Difference between previous and current tick count (handles overleapping)
    
    __int64 Paused_TickCount;   ///Application tick count before pause.

    void GetTimerHi();
    void GetTimerLo();
public:
    CTimer(char *pObjName,CEngine *pOwnerEngine);
    virtual ~CTimer();

    ///Zeroes timer tick count
    void ZeroTimer(bool Paused);

    __int64 GetTicksPerSecond() {
        return HW_CountsPerSecond;
    }

    bool GetPaused() {
        return Paused;
    }
    
    ///Returns application running time in ticks
    __int64 GetTimer();

    ///Returns application running time in seconds
    double GetTimerSecond();

    ///Converts tick count to seconds
    double ToSeconds(__int64 TickCount);

    void PauseTimer();

    void ResumeTimer();

    MAKE_CLSCREATE(CTimer);
    MAKE_CLSNAME("CTimer");
    MAKE_DEFAULTGARBAGE();
};

