// --------------------------------------------------------------
// --------------------------------------------------------------
//
// CDropAnimator.h
//
//    Drops of Water source code
//
//                      ShaderX2 - www.shaderx2.com
//
//
//    Author:   Sylvain Lefebvre - 04/2003
//    ------
//
//    -= Check for updates at http://www.aracknea.net =-
//
//    File contents:
//    -------------
//
//      Animate drops over a surface.
//      virtual class - all drops animator inherits from this class
//
// --------------------------------------------------------------
// --------------------------------------------------------------
#ifndef __CDROPANIMATOR__
#define __CDROPANIMATOR__
// --------------------------------------------------------------
#include <math.h>
#include "drop.h"
#include "dow.h"
// --------------------------------------------------------------
class CMyD3DApplication;
// --------------------------------------------------------------
#define drand  (((double)(rand() % 256))/255.0)
#define sdrand ((((double)(rand() % 256))/255.0)*2.0-1.0)
// --------------------------------------------------------------
class CDropAnimator
{
  protected:

	CMyD3DApplication *m_TSpriteManager;
    drops              m_Drops;
	int                m_iFrame;

	int                m_iLostDrops;
	int                m_iMergedDrops;
	double             m_dDryCoeff;

	drops::iterator    dropFindOverlaping(drops::iterator);
	virtual void       dropPerturb(drop&)=0;

  public:

    CDropAnimator(CMyD3DApplication *tsm) {m_TSpriteManager=tsm; 
										   m_iLostDrops=0; 
										   m_iMergedDrops=0;
										   m_iFrame=0;
										   m_dDryCoeff=0.01;}

	// called at init or restart
	virtual void init()=0;
	// called at each frame
	virtual void step();
	// called when the user press a key or a mouse button
	virtual void interact(double x,double y,int key)=0;

	// number of lost drops (discarded)
	int getLostDrops() const {return (m_iLostDrops);}
	// number of drops merged last step
	int getMergedDrops() const {return (m_iMergedDrops);}

	// drying color for wet areas texture
	virtual DWORD dryingColor() const {return (0x00FAFAFA);}

};
// --------------------------------------------------------------
#endif
// --------------------------------------------------------------
