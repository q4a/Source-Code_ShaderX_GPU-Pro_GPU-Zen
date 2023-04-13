// --------------------------------------------------------------
#include "CDropAnimator.h"
// --------------------------------------------------------------
void CDropAnimator::step()
{
	m_iLostDrops=0;
	m_iMergedDrops=0;
	
	m_iFrame++;

	m_TSpriteManager->ClearOffsetMap();
	drops::iterator P=m_Drops.begin();
	
	while (P!=m_Drops.end())
	{
		// is there an overlapping drop ?
		drops::iterator O=dropFindOverlaping(P);
		if (O != m_Drops.end())
		{
			// merge P with O 
			m_iMergedDrops++;
			// -> O becomes bigger
			(*O).scale=min(1.0,(*O).scale+(*P).scale);
			// -> erase P
			P=m_Drops.erase(P);
		}
		else
		{
			dropPerturb((*P));
			// ok let's update drop position
			(*P).p.x+=(*P).t.x;
			(*P).p.y+=(*P).t.y;
			if (!m_TSpriteManager->PutSprite((*P).grid,(*P).p.x,(*P).p.y,(*P).rot,(*P).scale))
			{
				int i;
				
				for (i=1;i<NB_OFFSET_MAP;i++)
				{
					// try other offset maps
					(*P).grid=((*P).grid+i) % NB_OFFSET_MAP;
					// if ok stop here
					if (m_TSpriteManager->PutSprite((*P).grid,(*P).p.x,(*P).p.y,(*P).rot,(*P).scale))
						break;
				}
				if (i == NB_OFFSET_MAP)
				{
					// not possible erase (lost drop)
					m_iLostDrops++;
					P=m_Drops.erase(P);
				}
				else
					P++;
			}
			else
			{
				(*P).scale-=m_dDryCoeff;
				if ((*P).scale < 0.2)
				{
					P=m_Drops.erase(P);
				}
				else
					P++;
			}
		}
	}
	// update offset map
	m_TSpriteManager->UpdateOffsetMap();
}
// --------------------------------------------------------------
drops::iterator CDropAnimator::dropFindOverlaping(drops::iterator D)
{
	for (drops::iterator P=m_Drops.begin();P!=m_Drops.end();P++)
	{
		if (P == D)
			continue;
		double dist=sqrt(((*D).p.x-(*P).p.x)*((*D).p.x-(*P).p.x)
			+((*D).p.y-(*P).p.y)*((*D).p.y-(*P).p.y));
		if (dist < ((*D).scale+(*P).scale)*0.3/(double)m_TSpriteManager->GetOffsetMapRes()
			&& ((*D).scale+(*P).scale) < 1.9)
			return (P);
	}
	return (m_Drops.end());
}
// --------------------------------------------------------------
