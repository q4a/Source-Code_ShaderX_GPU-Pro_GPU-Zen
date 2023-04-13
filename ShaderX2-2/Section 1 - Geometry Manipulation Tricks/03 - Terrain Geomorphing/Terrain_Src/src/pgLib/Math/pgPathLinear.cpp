///////////////////////////////////////////////////////////////////
//
// project:     Geisterschloss
// class:       pgPathLinear
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgPathLinear.cpp,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////


#pragma warning(disable: 4514)

#include "pgPathLinear.h"

#include <pgLib/Base/pgInTextFile.h>
#include <pgLib/Base/pgLog.h>
#include <pgLib/Base/pgISettings.h>
#include <pgLib/Base/pgIStringTool.h>

#include <stdio.h>

pgPathLinear::pgPathLinear() : interpolRot(false)
{
	type = TYPE_LINEAR;
}


bool
pgPathLinear::load(const char* nFileName)
{
	const char*		fullName = pgISettings::tmpFullPathPath(nFileName, ".path");
	pgVec3			pos;
	int				i;
	float			len=0.0;
	pgInTextFile	inFile;

	if(!inFile.open(fullName))
	{
		pgLog::error("cannot open path file '%s'", nFileName);
		return false;
	}

	// read all positions
	//
	while(!inFile.eof())
	{
		const char* line = inFile.readLine();

		if(pgIStringTool::startsWith(line, "#") || pgIStringTool::isEmpty(line))
			continue;																	// ignore this line

		sscanf(line, "%f\t%f\t%f", &pos.x,&pos.z,&pos.y);
		posList.addTail(pos);
	}


	// create the length list, which stores, how long
	// the path already is until the given index
	lenList.addTail(0.0f);
	for(i=1; i<posList.getSize(); i++)
	{
		len += pgVec3::distance(posList[i-1], posList[i]);
		lenList.addTail(len);
	}


	// normalize the length list to 1.0
	//
	for(i=1; i<posList.getSize(); i++)
		lenList[i] /= len;

	pgLog::info("path '%s' loaded", nFileName);

	assert(lenList.getSize()==posList.getSize());
	return true;
}


pgVec3
pgPathLinear::getPosition(float nPos)
{
	pgVec3 pos;
	pgVec3n dir;

	getPosDir(nPos, pos, dir);
	return pos;
}


pgVec3n
pgPathLinear::getDirection(float nPos)
{
	pgVec3 pos;
	pgVec3n dir;

	getPosDir(nPos, pos, dir);
	return dir;
}


int
pgPathLinear::getPosDir(float nPos, pgVec3& nPosition, pgVec3n& nDirection)
{
	if(nPos<0.0f)
		nPos = 0.0f;
	if(nPos>1.0f)
		nPos = 1.0f;

	// handle some special cases which might make
	// problems later on... (begin & end of path)
	if(nPos==0.0f)
	{
		nPosition = posList[0];
		nDirection = posList[1]-posList[0];
		return 0;
	}

	if(nPos==1.0f)
	{
		nPosition = posList[posList.getSize()-1];
		nDirection = posList[posList.getSize()-1]-posList[posList.getSize()-2];
		return posList.getSize()-1;
	}


	int minIdx=0, midIdx=lenList.getSize()/2, maxIdx=lenList.getSize();
	float minVal=0.0f, midVal=lenList[midIdx], maxVal=1.0f;
	int runs=0;

	// do a binary search (which is in average
	// a lot faster than linear search:
	// ld(maxIdx) instead of maxIdx/2 steps)
	while(maxIdx-minIdx>1)
	{
		if(midVal<nPos)
		{
			minIdx = midIdx;
			minVal = midVal;
		}
		else
		{
			maxIdx = midIdx;
			maxVal = midVal;
		}

		midIdx = (minIdx+maxIdx)/2;
		midVal = lenList[midIdx];
		runs++;
	}

	const pgVec3&	pos0 = posList[minIdx],
					pos1 = posList[maxIdx];
	float			secMin = lenList[minIdx],
					secLen = lenList[maxIdx]-lenList[minIdx];

	float			f = (nPos-secMin)/secLen;

	assert(f>=0.0f && f<=1.0f);

	nPosition = pos1 * f + pos0 * (1.0f-f);
	nDirection = pos1-pos0;

	return minIdx;
}
