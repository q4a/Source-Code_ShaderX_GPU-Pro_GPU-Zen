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
#include "eng-framerate.h"

#include "ENG-RenderSettings.h"

CFrameRate::CFrameRate(char *ObjName,CEngine *OwnerEngine) : CBaseClass(ObjName, OwnerEngine) {
    _LOG(this, D3_DV_GRP0_LEV0, "Created");
    LastRenderFrame = 0;
    LastApplicationTime = 0;

    MeasurementCount = 0;
    FPSCumulative = 0;
}

CFrameRate::~CFrameRate(void) {
    GetFinalFPS();
    _LOG(this, D3_DV_GRP0_LEV0, "Destroyed");
}

double CFrameRate::GetFPS(CRenderSettings &Settings) {
    double frames = (double)Settings.RenderID - (double)LastRenderFrame;
    double result = frames / (Settings.RenderTime - LastApplicationTime);
    LastRenderFrame = Settings.RenderID;
    LastApplicationTime = Settings.RenderTime;

    MeasurementCount++;
    FPSCumulative = FPSCumulative + result;

    _LOG(this, D3_DV_GRP0_LEV0, "Framerate is %f FPS", result);
    return result;
}

double CFrameRate::GetFinalFPS() {
    double result = FPSCumulative / (double)MeasurementCount;
    _LOG(this, D3_DV_GRP0_LEV0, "Final (average) framerate from %u measurements is %f FPS", MeasurementCount, result);
    return result;
}
