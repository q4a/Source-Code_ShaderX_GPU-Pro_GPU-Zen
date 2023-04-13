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


// WND-WindowManager.h: interface for the CWindowManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "MGR-BaseManager.h"

const int MSG_WND_CLOSED=WM_APP+1;  //this message is send to thread! , when window is closed, lparam is pointer to CWindowManager.

class CWindowManager : public CBaseManager {
public:
    char   *m_sClassName;
    char   *m_sWindowName;
    HWND    m_hWnd;
    int     m_iHeight;
    int     m_iWidth;
    int     m_iClientHeight;
    int     m_iClientWidth;
    RECT    m_tWindowRect;
    RECT    m_tClientRect;
    bool    m_bActive;

	virtual int OnClose();
	virtual int D3_FASTCALL OnCreate(CREATESTRUCT *pCS);
    //message handlers
	virtual int D3_FASTCALL OnSize(int iClientWidth,int iClientHeight);
	virtual int D3_FASTCALL OnMove(int iX,int iY);
	virtual LRESULT WindowProcedure(UINT uMsg, WPARAM wParam, LPARAM lParam);
    //informations and settings
	bool ShowWnd(int cmd);
	bool Show(bool bShow = true);
    bool IsVisible() {
        if (m_hWnd!=NULL) {
            return (IsWindowVisible(m_hWnd)==TRUE)? true : false;
        } else return false;
    }

    //base construction set of tools
    bool CreateWnd(char *sWindowName, UINT iExStyle, UINT iStyle, int iX, int iY,
        int iWidth, int iHeight, HWND hWndParent, HMENU hMenu, LPVOID lpParam);
    bool CreateWnd(char *sWindowName, UINT iExStyle, UINT iStyle, int iX, int iY,
        int iWidth, int iHeight) {
        return CreateWnd(sWindowName,iExStyle,iStyle,iX,iY,iWidth,iHeight,NULL,NULL,NULL);
    }

    //destroy set of tools
	bool DestroyWnd();
	bool DestroyWndClass();

    //constructor & destructor
	CWindowManager(char *sObjName, CEngine *pOwnerEngine);
	virtual ~CWindowManager();
    MAKE_CLSCREATE(CWindowManager);
    MAKE_CLSNAME("CWindowManager");
    MAKE_DEFAULTGARBAGE();
protected:
	bool CreateWndClass(UINT Flags);
};

