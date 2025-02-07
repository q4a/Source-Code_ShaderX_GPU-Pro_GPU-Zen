/******************************************************************************

 @File         PVRESParser.cpp

 @Title        Simple parser for PVRES files

 @Version      

 @Copyright    Copyright (C)  Imagination Technologies Limited.

 @Platform     Independent

 @Description  A simple script parser for use with PVREngine

******************************************************************************/
#include "PVRESParser.h"
#include "PVREngine.h"
#include <string.h>


using namespace pvrengine;

/******************************************************************************/

PVRESParser::PVRESParser()
{
	m_strError = "";
	m_pcPVRES = new PVRES;
	m_bScriptFileSpecified=false;
}

/******************************************************************************/

PVRESParser::~PVRESParser()
{
	PVRDELETE(m_pcPVRES);
}

/******************************************************************************/

bool getline(CPVRTString& strSrc, CPVRTString& strLine, const char delimiter)
{
	if(strSrc.size()>0)
	{
		size_t i32LineLength = strSrc.find_first_of(delimiter);
		if(!i32LineLength || i32LineLength==CPVRTString::npos)
		{
			strLine = strSrc;
			strSrc.clear();
		}
		else
		{
			strLine = strSrc.substr(0,i32LineLength-1);
			strSrc = strSrc.substr(i32LineLength+1);
		}
		return true;
	}
	return false;
}

/******************************************************************************/

PVRES PVRESParser::Parse()
{
	if(m_bScriptFileSpecified)
	{	// file has been specified
		// read script file; seriously unsubtle atm
		FILE *file = fopen(m_pcPVRES->getScriptFileName().c_str(),"rb");
		if(!file)
		{
			m_strError = CPVRTString("Couldn't open file.");
			return *m_pcPVRES;
		}
		// obtain file size:
		fseek (file , 0 , SEEK_END);
		long lSize = ftell(file);
		fclose(file);			// some OSs don't understand rewind
		file = fopen(m_pcPVRES->getScriptFileName().c_str(), "rb");
		if(!lSize)
		{
			m_strError = CPVRTString("File is empty.");
			return *m_pcPVRES;
		}
		// Allocate an array large enough
		char* allData = new char[lSize+1];

		if(!allData)
		{
			m_strError = CPVRTString("Couldn't allocate memory for reading script.");
			return *m_pcPVRES;
		}

		fread(allData,1,lSize,file);
		fclose(file);

		allData[lSize]='0';	// terminate the file properly

		m_strScript = CPVRTString(allData);
		PVRDELETEARRAY(allData);
	}
	else
	{
		return *m_pcPVRES;	//use default PVRES that should outlive the PVRESParser class
	}

	// parse the script file
	CPVRTString strLine;

	char cBool;
	int i32Int;


	// TODO: do this in a slightly less pre-school way
	while(getline(m_strScript, strLine, '\n'))
	{
		if (strLine.substr(0,strlen("fullscreen:")).compare("fullscreen:")==0)
		{
			cBool = strLine[strlen("fullscreen:")];
			m_pcPVRES->setFullScreen(cBool=='Y' || cBool=='y');
		}
		else if (strLine.substr(0,strlen("title:")).compare("title:")==0)
		{
			m_pcPVRES->setTitle(strLine.substr(strlen("title:")));
		}
		else if (strLine.substr(0,strlen("pod:")).compare("pod:")==0)
		{
			m_pcPVRES->setPODFileName(strLine.substr(strlen("pod:")));
		}
		else if (strLine.substr(0,strlen("startframe:")).compare("startframe:")==0)
		{
			m_pcPVRES->setStartFrame((float)atof(strLine.c_str() + strlen("startframe:")));
		}
		else if (strLine.substr(0,strlen("showfps:")).compare("showfps:")==0)
		{
			cBool = strLine[strlen("showfps:")];
			m_pcPVRES->setShowFPS(cBool=='Y' || cBool=='y');
		}
		else if (strLine.substr(0,strlen("animspeed:")).compare("animspeed:")==0)
		{
			float fAnimationSpeed;
			fAnimationSpeed = (float) atof(strLine.c_str() + strlen("animspeed:"));
			m_pcPVRES->setAnimationSpeed(fAnimationSpeed);
		}
		else if (strLine.substr(0,strlen("vertsync:")).compare("vertsync:")==0)
		{
			cBool = strLine[strlen("vertsync:")];
			m_pcPVRES->setVertSync(cBool=='Y' || cBool=='y');
		}
		else if (strLine.substr(0,strlen("logtofile:")).compare("logtofile:")==0)
		{
			cBool = strLine[strlen("logtofile:")];
			m_pcPVRES->setLogToFile(cBool=='Y' || cBool=='y');
		}
		else if (strLine.substr(0,strlen("defaultlight:")).compare("defaultlight:")==0)
		{
			cBool = strLine[strlen("defaultlight:")];
			m_pcPVRES->setDefaultLight(cBool=='Y' || cBool=='y');
		}
		else if (strLine.substr(0,strlen("frustumcull:")).compare("frustumcull:")==0)
		{
			cBool = strLine[strlen("frustumcull:")];
			m_pcPVRES->setFrustumCull(cBool=='Y' || cBool=='y');
		}
		else if (strLine.substr(0,strlen("powersaving:")).compare("powersaving:")==0)
		{
			cBool = strLine[strlen("powersaving:")];
			m_pcPVRES->setPowerSaving(cBool=='Y' || cBool=='y');
		}
		else if (strLine.substr(0,strlen("fsaa:")).compare("fsaa:")==0)
		{
			i32Int = atoi(strLine.c_str() +  strlen("fsaa:"));
			m_pcPVRES->setFSAA(i32Int);
		}
		else if (strLine.substr(0,strlen("height:")).compare("height:")==0)
		{
			i32Int = atoi(strLine.c_str() + strlen("height:"));
			m_pcPVRES->setHeight(i32Int);
		}
		else if (strLine.substr(0,strlen("width:")).compare("width:")==0)
		{
			i32Int = atoi(strLine.c_str() + strlen("width:"));
			m_pcPVRES->setWidth(i32Int);
		}
		else if (strLine.substr(0,strlen("posx:")).compare("posx:")==0)
		{
			i32Int = atoi(strLine.c_str() + strlen("posx:"));
			m_pcPVRES->setPosX(i32Int);
		}
		else if (strLine.substr(0,strlen("posy:")).compare("posy:")==0)
		{
			i32Int = atoi(strLine.c_str() + strlen("posy:"));
			m_pcPVRES->setPosY(i32Int);
		}
		else if (strLine.substr(0,strlen("quitaftertime:")).compare("quitaftertime:")==0)
		{
			m_pcPVRES->setQuitAfterTime((float)atof(strLine.c_str() + strlen("quitaftertime:")));
		}
		else if (strLine.substr(0,strlen("quitafterframe:")).compare("quitafterframe:")==0)
		{
			i32Int = atoi(strLine.c_str() + strlen("quitafterframe:"));
			m_pcPVRES->setQuitAfterFrame(i32Int);
		}
		else if (strLine.substr(0,strlen("drawmode:")).compare("drawmode:")==0)
		{
			i32Int = atoi(strLine.c_str() + strlen("drawmode:"));
			m_pcPVRES->setDrawMode(i32Int);
		}
	}

	return *m_pcPVRES;	//PVRES should outlive the PVRESParser class
}

/******************************************************************************/

void PVRESParser::setScriptFileName(const CPVRTString& strScriptFileName)
{
	m_pcPVRES->setScriptFileName(strScriptFileName);
	m_bScriptFileSpecified = true;
}

/******************************************************************************/

void PVRESParser::setPODFileName(const CPVRTString& strPODFileName)
{
	m_pcPVRES->setPODFileName(strPODFileName);
}

/******************************************************************************/

void PVRESParser::setScript(const CPVRTString& strScript)
{
	m_strScript = strScript;
	m_bScriptFileSpecified = false;
}

/******************************************************************************/

void PVRESParser::clearScript()
{
	m_strScript = "";
}

/******************************************************************************/

void PVRESParser::clearScriptFileName()
{
	m_pcPVRES->setPODFileName("");
}

/******************************************************************************
End of file (PVRESParser.cpp)
******************************************************************************/

