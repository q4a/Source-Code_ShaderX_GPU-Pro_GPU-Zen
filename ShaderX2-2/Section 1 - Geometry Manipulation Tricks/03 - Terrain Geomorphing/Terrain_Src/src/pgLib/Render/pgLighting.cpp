///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgLighting
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgLighting.cpp,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgLighting.h"
#include <pgLib/Base/pgIDirectX.h>


pgLighting::pgLighting()
{
	iAmbient = 0x00000000;
}


void
pgLighting::setBaseAmbient(const pgVec4& nColor)
{
	ambient = nColor;
	iAmbient = D3DCOLOR_COLORVALUE(ambient[0], ambient[1], ambient[2], ambient[3]);
}


void
pgLighting::applyLighting()
{
	int i;

	if(!checkDevice("can not apply lights: render device invalid"))
		return;

	for(i=0; i<lights.getSize(); i++)
	{
		pd3dDevice->SetLight(i, &lights[i]->ddLight);
		pd3dDevice->LightEnable(i, TRUE);
	}

	// turn off all other lights
	//
	for(; i<8; i++)
		pd3dDevice->LightEnable(i, FALSE);

	pd3dDevice->SetRenderState(D3DRS_AMBIENT, iAmbient);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}


void
pgLighting::turnOffLighting()
{
	for(int i=0; i<8; i++)
		pd3dDevice->LightEnable(i, FALSE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
}
