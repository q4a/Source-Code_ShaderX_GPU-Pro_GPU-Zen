///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgTerrainPVS
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgTerrainPVS.h,v 1.1.1.1 2002/10/20 09:33:03 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __TERRAINPVS_HEADERFILE__
#define __TERRAINPVS_HEADERFILE__


#include <pgLib/pgLib.h>


class pgTerrainPVS
{
public:
	pgTerrainPVS();
	~pgTerrainPVS();

	void calc(const unsigned char* nMinData,
			  const unsigned char* nMaxData,
			  unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nDZ);

	void shrink();

	bool save(const char* nFileName);
	bool load(const char* nFileName);

	bool isVisible(int nFromX, int nFromY, int nFromZ, int nDestX, int nDestY) const;


	int getDataX() const  {  return dataX;  }
	int getDataY() const  {  return dataY;  }
	int getDataZ() const  {  return dataZ;  }
	int getDataDZ() const  {  return dataDZ;  }

private:
	void setVisible(int nX0, int nY0, int nZ0, int nX1, int nY1, bool nValue);
	unsigned int getIdx(int nX0, int nY0, int nZ0, int nX1, int nY1) const;


	unsigned int dataX,dataY,dataZ, dataDZ;
	unsigned char* data;
	unsigned int dataSize;

};


#endif //__TERRAINPVS_HEADERFILE__
