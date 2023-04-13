///////////////////////////////////////////////////////////////////
//
// project:     pgLib
// class:       pgIStringTool
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgIStringTool.h,v 1.3 2003/01/03 19:02:05 daniel Exp $
///////////////////////////////////////////////////////////////////


#ifndef __STRINGTOOL_HEADERFILE__
#define __STRINGTOOL_HEADERFILE__

#include <pgLib/pgLib.h>

#include <pgLib/Base/pgList.h>
#include <pgLib/Base/pgString.h>

#include <stdio.h>

class pgVec2;
class pgVec3;
class pgVec4;


/// This interface provides utility methods for string processing
namespace pgIStringTool
{
	/// Reads a line and returns a temporary copy for it
	/**
	 *  Be aware not to call this method from outside of
	 *  the pgLib DLL, since FILE pointers may not be passed
	 *  over DLL boundaries.
	 */
	PGLIB_API const char* readLine(FILE* nFp);


	/// Skips all spaces and tabs and returns the new pointer to the same data buffer
	PGLIB_API const char* skipSpaces(const char* nLine);


	/// Skips all non spaces and non tabs and returns the new pointer to the same data buffer
	PGLIB_API const char* skipNonSpaces(const char* nLine);


	/// Searches for the given string and returns the position after it in nLine
	PGLIB_API const char* getPosAfter(const char* nLine, const char* nSearch);


	/// Returns true if nStr starts with nStart (ignores heading white spaces)
	PGLIB_API bool startsWith(const char* nStr, const char* nStart);


	/// Returns true if nStr starts with nStart ignoring case sensitivity (ignores heading white spaces)
	PGLIB_API bool startsWithIgnoreCase(const char* nStr, const char* nStart);


	/// Returns true if the line is empty (white space do not count)
	PGLIB_API bool isEmpty(const char* nStr);


	// Returns the next part of the string, which is seperated by nSeperator starting at position nPos
	PGLIB_API const char* getNextToken(const char* nStr, unsigned int& nPos, char nSeperator);


	// Fills all segments of the string, which are speperated by nSeperator into a list of strings
	/**
	 * If nNumTokens>0 the starting list size will be set to that value.
	 */
	PGLIB_API void fillTokenList(const char* nStr, char nSeperator, pgList<pgString>& nList, bool nSkipMultipleSeperators=true, int nNumTokens=0);


	/// Takes a (probably) full path of a file and tries to remove the path and the extenions from it.
	/**
	 *  nFileName must be large enough to hold the resulting string.
	 */
	PGLIB_API void removePathAndExtension(const char* nFullPath, char* nFileName);


	/// Takes a (probably) full path of a file and tries to remove the extenions from it.
	/**
	 *  nFileName must be large enough to hold the resulting string.
	 */
	PGLIB_API void removeExtension(const char* nFullPath, char* nFileName);

	/// Takes a (probably) full path of a file and tries to remove the path from it.
	/**
	 *  nFileName must be large enough to hold the resulting string.
	 */
	PGLIB_API void removePath(const char* nFullPath, char* nFileName);


	/// Reads a Vec2 from a String.
	/**
	 *  If reading fails, false is returned.
	 */
	PGLIB_API bool readVec2(const char* nString, pgVec2& nVec2);


	/// Reads a Vec3 from a String.
	/**
	 *  If reading fails, false is returned.
	 */
	PGLIB_API bool readVec3(const char* nString, pgVec3& nVec3);


	/// Reads a Vec4 from a String.
	/**
	 *  If reading fails, false is returned.
	 */
	PGLIB_API bool readVec4(const char* nString, pgVec4& nVec4);


	/// Replaces all characters nOld with nNew in string nString
	PGLIB_API void replaceCharacter(char* nString, char nOld, char nNew);
};

#endif //__STRINGTOOL_HEADERFILE__
