///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgInSettingsFile
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgSettingsFile.h,v 1.2 2002/10/27 18:36:14 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __SETTINGSFILE_HEADERFILE__
#define __SETTINGSFILE_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Base/pgList.h>
#include <pgLib/Base/pgString.h>

class pgVec2;
class pgVec3;
class pgVec4;


/// This class loads settings (ini) files.
/**
 *  Each setting consists of the parts:
 *  a name and a value. Values can consists of
 *  more than one item. E.g.: a vec3 consists of
 *  three floats.
 *  In values consisting of more than one element
 *  all elements must be seperated by a space (no "," oder ";")
 *  All lines which are empty or start with # are ignored
 */
class PGLIB_API pgSettingsFile
{
public:
	pgSettingsFile();
	~pgSettingsFile();


	/// Removes all names and values which where retrieved by a load() call
	void reset();


	/// Opens a setting file and reads all names and values from it.
	/**
	 *  The list of names and values is not cleared, which allows
	 *  to gather information from several setting files and merge
	 *  them in one pgSettingsFile object.
	 */
	bool load(const pgString& nFileName);


	/// Activates or deactivates logging.
	/**
	 *  If logging is enabled all names which are not found
	 *  in the name list are logged as errors.
	 */
	void setLogging(bool nSet)  {  doLogging = nSet;  }


	/// Returns true if the value of nName is 'yes'
	/**
	 *  If the nName is not found false is returned.
	 */
	bool getValueYes(const pgString& nName, bool& nYes) const;


	/// Returns the value of nName as a string.
	/**
	 *  If the nName is not found false is returned.
	 */
	bool getValueString(const pgString& nName, pgString& nString) const;


	/// Returns the value of nName as an integer.
	/**
	 *  If the nName is not found false is returned.
	 */
	bool getValueInt(const pgString& nName, int& nInt) const;


	/// Returns the value of nName as a float.
	/**
	 *  If the nName is not found false is returned.
	 */
	bool getValueFloat(const pgString& nName, float& nFloat) const;


	/// Returns the value of nName as a vec2.
	/**
	 *  If the nName is not found or the value can not
	 *  be converted into a vec2, false is returned.
	 */
	bool getValueVec2(const pgString& nName, pgVec2& nVec2) const;


	/// Returns the value of nName as a vec3.
	/**
	 *  If the nName is not found or the value can not
	 *  be converted into a vec3, false is returned.
	 */
	bool getValueVec3(const pgString& nName, pgVec3& nVec3) const;


	/// Returns the value of nName as a vec4.
	/**
	 *  If the nName is not found or the value can not
	 *  be converted into a vec4, false is returned.
	 */
	bool getValueVec4(const pgString& nName, pgVec4& nVec4) const;


protected:
	int getNameIndex(const pgString& nName) const;
	bool logMissingName(const pgString& nName) const;

	pgString			fileName;
	pgList<pgString>	names, values;
	bool				doLogging;
};


#endif //__SETTINGSFILE_HEADERFILE__
