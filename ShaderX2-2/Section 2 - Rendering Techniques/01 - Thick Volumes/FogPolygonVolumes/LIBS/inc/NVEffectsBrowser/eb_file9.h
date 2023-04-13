/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/

// eb_file.h -- Common file loading functions
//
//////////////////////////////////////////////////////////////////////

#ifndef __EBFILE_H
#define __EBFILE_H

#include <windows.h>
#include <NVEffectsBrowser/eb_string.h>
#include <NVEffectsBrowser/nvfilesearch.h>
#include <NVEffectsBrowser/nvdebug.h>
#include <d3d9.h>

typedef enum tagEBSHADERTYPE
{
	// D3D shader types
	SHADERTYPE_VERTEXSHADER  = 0,
	SHADERTYPE_PIXELSHADER   = 1,

	// OpenGL shader types
	SHADERTYPE_VERTEXPROG    = 2,
	SHADERTYPE_TEXTURESHADER = 3,
	SHADERTYPE_REGCOMBSTATE  = 4,

    // Cg shader types
	SHADERTYPE_VERTEXCG      = 5,
	SHADERTYPE_PIXELCG       = 6,

	// D3D shader types (old names)
	SHADERTYPE_VERTEX        = 0,
	SHADERTYPE_PIXEL         = 1,

	SHADERTYPE_FORCEDWORD    = 0xFFFFFFFF
} EBSHADERTYPE;

namespace effect_api
{
	//////////////////////////////////////////////////////////////////////////////
	// These variables provide information to effects at creation time

	// Set when the browser is rescanning for new effects...
	extern bool Rescanning;

	// This variable provides a way for effects to save a piece of state at scan
	// time that will again be available when the effect is later created.
	//
	// If the browser is rescanning, it will save the value of this variable in
	// the registry for each effect.  The variable should be set as a side-effect
	// by the creation of each effect.
	//
	// If the browser is NOT rescanning, it restore this variable to the value it
	// contained when this effect was created during the original scan.
	//
	extern EBString EffectState;

	// This variable allows us to recursively search for media
	class NVMediaSearch : public NVFileSearch
	{
		public:
		std::string m_strPath;
		virtual bool FileFoundCallback(const WIN32_FIND_DATA& FindData, const std::string& strDir)
		{
			m_strPath = strDir;
			
			return false;
		}
	};
	//////////////////////////////////////////////////////////////////////////////
	// These functions and static object remember the path to base of the current
	// effect.  Each effect has its own copy of this static string object.  The
	// object is constructed by a macro defined in eb_effect.h.
	extern std::string strLastError;
	extern std::string strStartPath;
	inline std::string GetModulePath() { return strStartPath; }
	inline void        SetModulePath(const std::string &strPath)
	{
		std::string::size_type Pos = strPath.find_last_of("\\", strPath.size());
		if (Pos != strPath.npos)
			strStartPath = strPath.substr(0, Pos);
		else
			strStartPath = ".";
	}
	
	inline std::string GetFullPath(const std::string &strRelativePath)
	{
		char buf[MAX_PATH];
		char *pAdr;
		GetFullPathName(strRelativePath.data(), MAX_PATH, buf, &pAdr);
		return buf;
	}

    // Recursively searchs the given path until it finds the file. Returns "" if 
    // file can't be found
    inline std::string FindMediaFile(const std::string &strFilename, const std::string &mediaPath)
    {
        WIN32_FIND_DATA findInfo;
        HANDLE hFind;
        char savedDirectory[MAX_PATH];
        std::string result;
    
        // Save current directory
        GetCurrentDirectory(MAX_PATH, savedDirectory);
        
        // Switch to directory to be removed
        if (!SetCurrentDirectory(mediaPath.data()))
            return "";

        // check if file is in current directory
        FILE *fp;
        fp = fopen(strFilename.data(), "r");
        if (fp)
        {
            fclose(fp);
            return mediaPath + "\\" + strFilename;
        }
    
        // if file not in current directory, search for all directories
        // and search inside them until the file is found
        hFind = FindFirstFile("*.*", &findInfo);
        if (hFind == INVALID_HANDLE_VALUE)
            return "";
        
        result = "";
        do
        {
            if (findInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                // Make sure we don't try to delete the '.' or '..' directories
                if ((strcmp(findInfo.cFileName, "..") == 0) || (strcmp(findInfo.cFileName, ".") == 0))
                    continue;
                
                result = FindMediaFile(strFilename, mediaPath + "\\" + findInfo.cFileName);
                if (result != "")
                    break;                    
            }
        } while (FindNextFile(hFind, &findInfo));
    
        FindClose(hFind);
        
        // return to previous directory
        SetCurrentDirectory(savedDirectory);
        
        return result;
    }

	//////////////////////////////////////////////////////////////////////////////
	// Gets the full path to the requested file by searching in the MEDIA 
    // directory
	inline std::string GetFilePath(const std::string& strFileName)
	{
        // Check if strFilename already points to a file
        FILE *fp;
        fp = fopen(strFileName.data(), "r");
        if (fp)
        {
            fclose(fp);
            return strFileName;
        }

		std::string strMediaPath = GetModulePath();
    	strMediaPath = strMediaPath.substr(0, strMediaPath.find_last_of("\\/"));
	    strMediaPath = strMediaPath.substr(0, strMediaPath.find_last_of("\\/"));
	    strMediaPath = strMediaPath.substr(0, strMediaPath.find_last_of("\\/"));
	    strMediaPath = strMediaPath.substr(0, strMediaPath.find_last_of("\\/"));
        strMediaPath += "\\MEDIA";
		
		// Must call SetModulePath before using this function
		assert(!strMediaPath.empty());
		
		std::string result;
        result = FindMediaFile(strFileName, strMediaPath);
           
        if (result != "")
            return result;

		// Can't find it in the MEDIA directory, return it so that other API's 
        // can report the find fail.
		NVASSERT(0, "Couldn't find: " << strFileName);
		return strFileName;
	};
    
}; // namespace effect_api

#endif