///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgIGame.cpp
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgIGame.cpp,v 1.1.1.1 2002/10/20 09:33:01 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgIGame.h"

#include <pgLib/Input/pgIInput.h>


pgIGame::pgIGame()
{
	input = NULL;
}


void
pgIGame::callIInput(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(input)
		input->processWindowMsg(hWnd, uMsg, wParam, lParam);
}
