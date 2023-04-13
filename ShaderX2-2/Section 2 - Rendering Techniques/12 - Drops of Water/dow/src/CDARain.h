// --------------------------------------------------------------
// CDARain.h
//
//  Drop impacts on a surface
//
//  inherits CDropAnimator
//
//  Author: Sylvain Lefebvre - http://www.aracknea.net
// --------------------------------------------------------------
#ifndef __CDARain__
#define __CDARain__
// --------------------------------------------------------------
#include "CDropAnimator.h"
// --------------------------------------------------------------
#define RAIN_IMPACT_PROB 40
#define RAIN_NB_DROPS    10
// --------------------------------------------------------------
class CDARain : public CDropAnimator
{
  protected:

	double                  m_dImpactSize;

	virtual void dropPerturb(drop& d);
	bool         dropImpactNew(double x,double y,double sz);
	void         dropImpact();

  public:
    
	CDARain(CMyD3DApplication *);

	virtual void init();
	virtual void step();
	virtual void interact(double x,double y,int key);

};
// --------------------------------------------------------------
#endif
// --------------------------------------------------------------
