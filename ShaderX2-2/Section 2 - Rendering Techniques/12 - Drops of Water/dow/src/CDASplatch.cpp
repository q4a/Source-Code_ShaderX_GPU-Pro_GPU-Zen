// --------------------------------------------------------------
#include "CDASplatch.h"
// --------------------------------------------------------------
#define DROP_PROB        40
#define DRY_SCALE_COEFF  0.004
#define DROP_SPEED       0.005
#define NB_DROP          100
#define DROP_IMPACT_PROB 0
#define DROP_NB_IMPACTS  30
// --------------------------------------------------------------
CDASplatch::CDASplatch(CMyD3DApplication *tsm) : CDropAnimator(tsm)
{
	m_iNbImpacts=0;
	m_Path[0]=0.15;
	m_Path[1]=0.7;
	m_Path[2]=0.70;
	m_Path[3]=0.7;
	m_dDryCoeff=DRY_SCALE_COEFF;
}
// --------------------------------------------------------------
void CDASplatch::init()
{
	m_iFrame=0;
	m_iNbImpacts=0;
	m_Drops.clear();
	m_TSpriteManager->ClearOffsetMap();
	m_TSpriteManager->ClearWetAreas();
}
// --------------------------------------------------------------
void CDASplatch::step()
{
	if (m_iFrame > 60)
	{
		if (m_iNbImpacts < DROP_NB_IMPACTS)
		{
			dropImpact();
			m_iNbImpacts++;
		}
		else if (rand() % 100 < DROP_IMPACT_PROB)
		{
			dropImpact();
			m_iNbImpacts=0;
		}
	}
	CDropAnimator::step();
}
// --------------------------------------------------------------
void CDASplatch::interact(double x,double y,int key)
{
	if (key == 'b')
	{
			m_Path[0]=0.2+0.15*(drand*2.0-1.0);
			m_Path[1]=0.8+0.15*(drand*2.0-1.0);
			m_Path[2]=0.5+0.4*(drand*2.0-1.0);
			m_Path[3]=0.5+0.4*(drand*2.0-1.0);
			m_iNbImpacts=0;	
	}
}
// --------------------------------------------------------------
void CDASplatch::dropPerturb(drop& d)
{
	double agl,n,s;
	
	// angle from speed
	n=sqrt(d.t.x*d.t.x+d.t.y*d.t.y);
	agl=atan2(d.t.y/n,d.t.x/n);
	d.rot=3.0*M_PI/2.0-agl;
	// gravity
	d.t.y=d.t.y-0.0001;
	// speed variation
	s=sqrt(d.t.x*d.t.x+d.t.y*d.t.y);
	n=s-s*0.01*drand;
	if (n > DROP_SPEED)
		n=DROP_SPEED;
	d.t.x=d.t.x*n/s;
	d.t.y=d.t.y*n/s;
}
//-----------------------------------------------------------------------------
bool CDASplatch::dropImpactNew(double x,double y,double sz)
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
			d.t.x*=0.3*DROP_SPEED*(0.5*v+0.5);
			d.t.y*=0.3*DROP_SPEED*(0.5*v+0.5);
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
void CDASplatch::dropImpact()
{
	double param=m_iNbImpacts/((double)DROP_NB_IMPACTS-1.0);
	double x=m_Path[0]+(m_Path[1]-m_Path[0])*param;
	double y=m_Path[2]+(m_Path[3]-m_Path[2])*param;
	double r=(drand*2.0-1.0);
	r*=0.1*sin(M_PI*param);
	double u=m_Path[1]-m_Path[0];
	double v=m_Path[3]-m_Path[2];
	double l=sqrt(u*u+v*v);
	u/=l;
	v/=l;
	x+=v*r;
	y-=u*r;
	m_dImpactSize=0.07*drand;
	m_TSpriteManager->Impact(x,y,m_dImpactSize);
	for (int i=0;i<NB_DROP;i++)
		dropImpactNew(x,y,m_dImpactSize);
}
// --------------------------------------------------------------
