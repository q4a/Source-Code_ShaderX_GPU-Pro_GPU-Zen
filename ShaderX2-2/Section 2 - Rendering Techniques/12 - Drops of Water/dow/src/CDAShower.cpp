// --------------------------------------------------------------
#include "CDAShower.h"
// --------------------------------------------------------------
#define DROP_PROB        10
#define DRY_SCALE_COEFF  0.001
#define DROP_SPEED       0.005
#define NB_DROP          10
// --------------------------------------------------------------
CDAShower::CDAShower(CMyD3DApplication *tsm) : CDropAnimator(tsm)
{
    m_bUniform=false;
	m_dDryCoeff=DRY_SCALE_COEFF;
}
// --------------------------------------------------------------
void CDAShower::init()
{
	m_iFrame=0;
	m_Drops.clear();
	m_TSpriteManager->ClearOffsetMap();
	m_TSpriteManager->ClearWetAreas();
	for (int i=0;i<NB_DROP;i++)
		dropNew();
}
// --------------------------------------------------------------
void CDAShower::step()
{
	if (rand() % 100 < DROP_PROB)
	{
		for (int i=0;i<NB_DROP;i++)
			dropNew();
	}
	CDropAnimator::step();
}
// --------------------------------------------------------------
void CDAShower::interact(double x,double y,int key)
{

}
// --------------------------------------------------------------
void CDAShower::dropPerturb(drop& d)
{
	double a,agl,n;

	if (((m_iFrame-d.last) > 5 && (rand() % 100 > 70)))
	{
		d.last=m_iFrame;
		a=sdrand;
		agl=(-M_PI/2.0)+a*(M_PI/12.0);
		n=sqrt(d.t.x*d.t.x+d.t.y*d.t.y);
		// speed variation
		a=drand;
	    n=DROP_SPEED*(0.4+0.8*a);
		d.t.x=n*cos(agl);
	    d.t.y=n*sin(agl);
		d.rot=(3.0*M_PI/2.0-agl);
	}
}
//-----------------------------------------------------------------------------
bool CDAShower::dropNew()
{
	int  iter=0;
	bool fail=false;
	
	// limit density
	if (m_TSpriteManager->GetOffsetMapRes() == 2 && m_Drops.size() >= 1)
		return (false);
	if (m_TSpriteManager->GetOffsetMapRes() == 4 && m_Drops.size() >= 2)
		return (false);
	if (m_TSpriteManager->GetOffsetMapRes() == 8 && m_Drops.size() >= 8)
		return (false);
	
#define MAX_ITER 10
	do
	{
		double agl;
		drop   d;
		
		d.last=m_iFrame;
		d.grid=0;
		
		d.p.x=drand;
		if (m_bUniform)
			d.p.y=drand;
		else
			d.p.y=1.0-0.1*drand;
		
		if (!m_TSpriteManager->Overlap(d.p.x,d.p.y))
		{
			double a=2.0*(drand-0.5);
			agl=(-M_PI/2.0)+a*(M_PI/12.0);
			//std::cerr << a << std::endl;
			d.t.x=cos(agl);
			d.t.y=sin(agl);
			// rotation
			d.rot=(3.0*M_PI/2.0-agl);
			// size
			double v=drand;
			d.scale=(0.4+0.6*v);
			// speed
			d.t.x*=DROP_SPEED*(0.8+0.4*v);
			d.t.y*=DROP_SPEED*(0.8+0.4*v);
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
