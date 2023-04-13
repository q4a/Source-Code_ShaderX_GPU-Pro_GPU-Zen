// --------------------------------------------------------------
#include "CDARain.h"
// --------------------------------------------------------------
#define DROP_PROB        40
#define DRY_SCALE_COEFF  0.02
#define DROP_SPEED       0.005
#define NB_DROP          100
#define DROP_IMPACT_PROB 0
#define DROP_NB_IMPACTS  30
// --------------------------------------------------------------
CDARain::CDARain(CMyD3DApplication *tsm) : CDropAnimator(tsm)
{
	m_dDryCoeff=DRY_SCALE_COEFF;
}
// --------------------------------------------------------------
void CDARain::init()
{
	m_iFrame=0;
	m_Drops.clear();
	m_TSpriteManager->ClearOffsetMap();
	m_TSpriteManager->ClearWetAreas();
}
// --------------------------------------------------------------
void CDARain::step()
{
	if (m_iFrame > 60)
	{
		if (rand() % 100 < RAIN_IMPACT_PROB)
			dropImpact();
	}
	CDropAnimator::step();
}
// --------------------------------------------------------------
void CDARain::interact(double x,double y,int key)
{

}
// --------------------------------------------------------------
void CDARain::dropPerturb(drop& d)
{
	double agl,n,s;
	
	// angle from speed
	n=sqrt(d.t.x*d.t.x+d.t.y*d.t.y);
	agl=atan2(d.t.y/n,d.t.x/n);
	d.rot=3.0*M_PI/2.0-agl;
	// speed variation
	s=sqrt(d.t.x*d.t.x+d.t.y*d.t.y);
	n=s-s*0.1*drand;
	if (n > DROP_SPEED)
		n=DROP_SPEED;
	d.t.x=d.t.x*n/s;
	d.t.y=d.t.y*n/s;
}
//-----------------------------------------------------------------------------
bool CDARain::dropImpactNew(double x,double y,double sz)
{
	int  iter=0;
	bool fail=false;
	
#define MAX_ITER 10
	do
	{
		double agl;
		drop   d;
		
		d.last=m_iFrame;
		d.grid=rand() % NB_OFFSET_MAP;
		
		double r=drand;
		double a=drand;
		agl=a*(2.0*M_PI);

		d.p.x=x+m_dImpactSize*r*cos(agl);
		d.p.y=y+m_dImpactSize*r*sin(agl);
		
		if (!m_TSpriteManager->Overlap(d.grid,d.p.x,d.p.y))
		{
			d.t.x=cos(agl);
			d.t.y=sin(agl);
			// rotation
			d.rot=3.0*M_PI/2.0-agl;
			// size
			double v=drand;
			d.scale=(0.7+0.3*v);
			// speed
			d.t.x*=DROP_SPEED*(0.5*v+0.5);
			d.t.y*=DROP_SPEED*(0.5*v+0.5);
			// put drop
			fail=!m_TSpriteManager->PutSprite(d.grid,d.p.x,d.p.y,d.rot,d.scale);
			if (!fail)
				m_Drops.push_back(d);
		}
		else
			fail=true;
	}while (fail && iter++ < MAX_ITER);
	return (!fail);
}
//-----------------------------------------------------------------------------
void CDARain::dropImpact()
{
	double x=drand;
	double y=drand;
	m_dImpactSize=(0.01+0.06*drand)*1.5;
	m_TSpriteManager->Impact(x,y,m_dImpactSize);
	for (int i=0;i<RAIN_NB_DROPS;i++)
		dropImpactNew(x,y,m_dImpactSize);
}
// --------------------------------------------------------------
