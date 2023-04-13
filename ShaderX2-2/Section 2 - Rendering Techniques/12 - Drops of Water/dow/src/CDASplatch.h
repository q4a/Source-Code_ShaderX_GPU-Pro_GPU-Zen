// --------------------------------------------------------------
// CDASplatch.h
//
//  Project multiple drops on the surface
//
//  inherits CDropAnimator
//
//  Author: Sylvain Lefebvre - http://www.aracknea.net
// --------------------------------------------------------------
#ifndef __CDASplatch__
#define __CDASplatch__
// --------------------------------------------------------------
#include "CDropAnimator.h"
// --------------------------------------------------------------
class CDASplatch : public CDropAnimator
{
  protected:

	double                  m_dImpactSize;
	double                  m_Path[4];
	int					    m_iNbImpacts;

	virtual void dropPerturb(drop& d);
	bool         dropImpactNew(double x,double y,double sz);
	void         dropImpact();

  public:
    
	CDASplatch(CMyD3DApplication *);

	virtual void init();
	virtual void step();
	virtual void interact(double x,double y,int key);

	virtual DWORD dryingColor() const {return (0x00FFFFFF);}
};
// --------------------------------------------------------------
#endif
// --------------------------------------------------------------
