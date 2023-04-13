// --------------------------------------------------------------
// CDAShower.h
//
//  Drops falling along the surface
//
//  inherits CDropAnimator
//
//  Author: Sylvain Lefebvre - http://www.aracknea.net
// --------------------------------------------------------------
#ifndef __CDAShower__
#define __CDAShower__
// --------------------------------------------------------------
#include "CDropAnimator.h"
// --------------------------------------------------------------
class CDAShower : public CDropAnimator
{
  protected:

	virtual void dropPerturb(drop& d);
	bool         dropNew();

	bool         m_bUniform;

  public:
    
	CDAShower(CMyD3DApplication *);

	virtual void init();
	virtual void step();
	virtual void interact(double x,double y,int key);

};
// --------------------------------------------------------------
#endif
// --------------------------------------------------------------
