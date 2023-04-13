///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgInSettingsFile
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgSettingsFile.cpp,v 1.3 2002/10/27 18:36:14 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgSettingsFile.h"

#include <pgLib/Base/pgInTextFile.h>
#include <pgLib/Base/pgLog.h>
#include <pgLib/Base/pgIStringTool.h>
#include <pgLib/Math/pgVec2.h>
#include <pgLib/Math/pgVec3.h>
#include <pgLib/Math/pgVec4.h>


pgSettingsFile::pgSettingsFile()
{
	doLogging = true;
}


pgSettingsFile::~pgSettingsFile()
{
	reset();
}


void
pgSettingsFile::reset()
{
	names.removeAll();
	values.removeAll();
}


bool
pgSettingsFile::load(const pgString& nFileName)
{
	pgInTextFile	inFile;
	char			name[512], value[1024];

	fileName = nFileName;

	if(!inFile.open(nFileName))
	{
		pgLog::error("opening settings file '%s' failed", nFileName);
		return false;
	}
	else
		pgLog::info("settings file '%s' opened", nFileName);

	while(!inFile.eof())
	{
		const char *line, * line0 = inFile.readLine();

		if(pgIStringTool::startsWith(line0, "#") || pgIStringTool::isEmpty(line0))
			continue;																	// ignore this line

		line = pgIStringTool::skipNonSpaces(line0);

		strncpy(name, line0, line-line0);
		name[line-line0] = 0;

		line = pgIStringTool::skipSpaces(line);

		assert(strlen(line)<1024);
		strcpy(value, line);

		names.addTail(name);
		names.getTail().toLower();
		values.addTail(value);
	}

	return true;
}


bool
pgSettingsFile::getValueYes(const pgString& nName, bool& nYes) const
{
	int idx = getNameIndex(nName);

	if(idx==-1)
		return logMissingName(nName);

	pgString v = values[idx];
	v.toLower();

	nYes = (v=="yes");
	return true;
}

bool
pgSettingsFile::getValueString(const pgString& nName, pgString& nString) const
{
	int idx = getNameIndex(nName);

	if(idx==-1)
	{
		nString = "";
		return logMissingName(nName);
	}

	nString = values[idx];
	return true;
}


bool
pgSettingsFile::getValueInt(const pgString& nName, int& nInt) const
{
	int idx = getNameIndex(nName);

	if(idx==-1)
		return logMissingName(nName);

	nInt = atoi(values[idx]);
	return true;
}


bool
pgSettingsFile::getValueFloat(const pgString& nName, float& nFloat) const
{
	int idx = getNameIndex(nName);

	if(idx==-1)
		return logMissingName(nName);

	nFloat = (float)atof(values[idx]);
	return true;
}


bool
pgSettingsFile::getValueVec2(const pgString& nName, pgVec2& nVec2) const
{
	int idx = getNameIndex(nName);

	if(idx==-1)
		return logMissingName(nName);

	return pgIStringTool::readVec2(values[idx], nVec2);
}


bool
pgSettingsFile::getValueVec3(const pgString& nName, pgVec3& nVec3) const
{
	int idx = getNameIndex(nName);

	if(idx==-1)
		return logMissingName(nName);

	return pgIStringTool::readVec3(values[idx], nVec3);
}


bool
pgSettingsFile::getValueVec4(const pgString& nName, pgVec4& nVec4) const
{
	int idx = getNameIndex(nName);

	if(idx==-1)
		return logMissingName(nName);

	return pgIStringTool::readVec4(values[idx], nVec4);
}


int
pgSettingsFile::getNameIndex(const pgString& nName) const
{
	int i, num=names.getSize();

	pgString name = nName;
	name.toLower();

	for(i=0; i<num; i++)
		if(names[i]==name)
			return i;

	return -1;
}


bool
pgSettingsFile::logMissingName(const pgString& nName) const
{
	if(doLogging)
		pgLog::error("name %s in settings file '%s' not found", nName, fileName);
	return false;
}
