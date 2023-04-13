///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgString
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgString.h,v 1.1.1.1 2002/10/20 09:33:01 daniel Exp $
///////////////////////////////////////////////////////////////////

#ifndef __STRING_HEADERFILE__
#define __STRING_HEADERFILE__


#include <pgLib/pgLib.h>

#include <string.h>
#include <assert.h>

/// Class for storing and formating single byte character strings
class PGLIB_API pgString
{
public:
	/// Creates an empty string
	pgString(){	string = NULL;};

	/// Creates a string as a copy of another pgString object
	pgString(const pgString& other);

	/// Creates a string as a copy of a char pointer
	pgString(const char *nString);

	~pgString();

	/// Formats the given string and stores it.
	pgString&
		set(const char *nString,...);

	/// Formats the given string and adds it.
	 pgString&
		cat(const char *nString);

	/// Sets a new buffer directly
	/**
	 *  CAUTION: the pgString object takes the ownership of the buffer; so do not delete it.
	 */
	void setBuffer(char* nBuffer);


	/// Returns the length of the string.
	int
		length() const;


	/// Returns the length of the string.
	int
		getLength() const  {  return length();  }


	/// Does the string consists just the chars of the cSet??
	/**
	 *  If the string only contains the characters in cSet
	 *  true is returned.
	 */
	bool
		consistsJustOf(const pgString& cSet) const;

	/// Returns the first/last occurence of c; 
	int
		getIndex(const char c,bool forward=true) const;


	/// Returns the first/last occurence of c; after start. 
	/**
	 *  If start is -1, then the search begins either at index 0 or Last()
	 */
	 int 
		getIndex(int start,const char c,bool forward=true) const;


	/// Returns the first/last char that is not c.
	int
		getIndexNot(const char c,bool forward=true) const;


	/// Returns the first/last char that is not c.
	/// If start==-1, then the search begins either at index 0 or Last().
	int
		getIndexNot(int start,const char c,bool forward=true) const;


	/// Returns the first/last occurence of str;
	int
		getIndex(const pgString& other,bool forward=true) const;


	/// Returns the first/last occurence of str
	/// If start==-1, then the search begins either at index 0 or Last()
	int
		getIndex(int start,const pgString& other,bool forward=true) const;


	/// Returns the amount of occurence of c
	int
		getCNum(const char c) const;


	/// Gives direct access to the string's data
	const char *
		get() const  { return (string!=NULL ? string : ""); }

	/// Returns a substring
	pgString
		getSubString(int from,int count) const;

	/// Converts all charater to lower characters.
	pgString&
		toLower();

	/// Converts all charater to upper characters.
	pgString&
		toUpper();

	/// Returns the lower of the passed character
	static char 
		toLower(char nC);

	/// Returns the upper of the passed character
	static char 
		toUpper(char nC);

	/// Cuts all leading/ending c-s
	pgString&
		cutC(const char c,bool forward=true);

	/// Cuts all chars c of
	pgString&
		cut(char c);

	pgString&
		cutS(const pgString& cs, bool forward=true);

	pgString&
		cut(const pgString& cs);

	/// Returns the index of the substring. Returns -1 of not found
	int
		find(const pgString& nSubString);

	/// concanates two strings
    friend inline pgString 
		operator+( const pgString& left, const pgString& right);


     bool 
		operator<( const pgString& ) const;
     bool 
		operator>( const pgString& ) const;
     bool 
		operator==( const pgString& ) const;
     bool 
		operator==( const char* str ) const;
     bool 
		operator!=( const pgString& ) const;
     bool 
		operator!=( const char* str ) const;
     pgString&
		operator=( const pgString& );
     pgString&
		operator+=( const pgString& );
     char 
		operator[](int index) const 
		{ assert(index<length()&&index>=0);return string[index]; }

     operator const char *() const 
		{ return get(); }
protected:
	char *string;
};


/// pgStringEx has a formating contructor
class PGLIB_API pgStringEx : public pgString
{
public:
	/// Lets you create a pgString object with a formated string
	/**
	 *  Formating works same as sprintf()
	 */
	pgStringEx(const char *nString,...);

	~pgStringEx();
};

inline pgString 
operator+( const pgString& left, const pgString& right)
{
	pgString ret(left);
	ret.cat(right);
	return ret;
}


#endif //__STRING_HEADERFILE__
