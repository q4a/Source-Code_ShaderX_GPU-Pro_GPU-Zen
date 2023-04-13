///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgString
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgString.cpp,v 1.1.1.1 2002/10/20 09:33:01 daniel Exp $
///////////////////////////////////////////////////////////////////

#pragma warning(disable: 4514)

#include "pgString.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>


const int maxStringlength = 2000;

pgStringEx::pgStringEx(const char *nString,...)
{
	char tempString[maxStringlength];
	string = NULL;
    va_list marker;
	if (nString!=NULL)
	{
		va_start(marker, nString);
		vsprintf(tempString,nString,marker);
		int size = strlen(tempString);
		assert(size<maxStringlength);
		string = new char[size+1];
		memcpy(string,tempString,size+1);
	}
}

pgString::pgString(const pgString& other)
{
	string = NULL;
	*this = other;
}

pgString::pgString(const char *nString)
{
	if(nString)
	{
		string = new char[strlen(nString)+1];
		strcpy(string, nString);
	}
	else
		string = NULL;
}

pgString::~pgString()
{
	if (string!=NULL)
		delete string;
}

pgString&
pgString::set(const char *nString,...)
{	
	char tempString[maxStringlength];
    va_list marker;
	if (string!=NULL)
		delete string;
	if (nString!=NULL)
	{
		va_start(marker, nString);
		vsprintf(tempString,nString,marker);
		assert(strlen(tempString)<maxStringlength);
		string = new char[strlen(tempString)+1];
		strcpy(string,tempString);
	}
	else
		string = NULL;

    return *this;
}

pgString&
pgString::cat(const char *nString)
{
	assert(nString!=NULL);
	if (string==NULL)
	{
		//Set(nString);
		string = new char[strlen(nString)+1];
		strcpy(string,nString);
	}
	else
	{
		char *temp = string;
		string = new char[strlen(temp)+strlen(nString)+1];
		strcpy(string,temp);
		strcat(string,nString);
		delete temp;
	}

    return *this;
}

int
pgString::length() const
{
	if (string!=NULL)
		return strlen(string);
	else 
		return 0;
}

bool
pgString::consistsJustOf(const pgString& cSet) const 
{
    int i1, i2;
    bool found;

	if (string!=NULL)
	{
		for (i1=0; i1<length(); ++i1)
        {
			for (i2=0,found=false ;i2<cSet.length(); ++i2)
				if(string[i1]==cSet[i2])
                    found = true;

            if(!found)
			    return false;
        }
	}
	return true;
}

int
pgString::getIndex(const char c,bool forward) const 
{
	if (forward)
		return getIndex(0,c,forward);
	else
		return getIndex(length(),c,forward);
}

int
pgString::getIndex(int start,const char c,bool forward) const
{
	if(start==-1)
		start = forward ? 0 : (length()-1);
	assert(start>=0 && start<=length());
	if (string!=NULL)
	{
		if (forward)
		{
			for (int i=start;i<length();++i)
				if (string[i]==c)
					return i;
		}
		else
		{
			for (int i=start;i>=0;--i)
				if (string[i]==c)
					return i;
		}
	}
	return -1;
}

int
pgString::getIndexNot(const char c,bool forward) const 
{
	if (forward)
		return getIndexNot(0,c,forward);
	else
		return getIndexNot(length(),c,forward);
}

int
pgString::getIndexNot(int start,const char c,bool forward) const
{
	if(start==-1)
		start = forward ? 0 : (length()-1);
	assert(start>=0 && start<length());
	if (string!=NULL)
	{
		if (forward)
		{
			for (int i=start;i<length();++i)
				if (string[i]!=c)
					return i;
		}
		else
		{
			for (int i=start;i>=0;--i)
				if (string[i]!=c)
					return i;
		}
	}
	return -1;
}

int
pgString::getIndex(const pgString& other,bool forward) const 
{
	if (forward)
		return getIndex(0,other,forward);
	else
		return getIndex(length()-1,other,forward);
}

int
pgString::getIndex(int start,const pgString& other,bool forward) const
{
	if(start==-1)
		start = forward ? 0 : (length()-1);
	if (string!=NULL && start>=0 && start<length())
	{
		int l1 = length();
		int l2 = other.length();
		if (forward)
		{
			for (int i=start;i<l1;++i)
			{
			        int i1,i2;
				for (i1=i,i2=0;i1<l1 && i2<l2 &&
									string[i1]==other[i2];++i1,++i2);
				if (i2==l2)
					return i;
			}
		}
		else
		{
			for (int i=start;i>=0;--i)
			{
			        int i1,i2;
				for (i1=i,i2=0;i1<l1 && i2<l2 &&
									string[i1]==other[i2];++i1,++i2);
				if (i2==l2)
					return i;
			}
		}
	}
	return -1;
}


int
pgString::getCNum(const char c) const
{
	int ret = 0;
	if (string!=NULL)
	{
		for (int i=0;i<length();++i)
			if (string[i]==c)
				++ret;
	}
	return ret;
}

pgString
pgString::getSubString(int from,int count) const
{
	char *str;
	assert(from>=0);
	if (count==-1)
		count = length()-from;
	assert((from+count)<=length());
	assert(count>=0);
	str = new char[count+1];
	int i1,i2;
	for (i1=from,i2=0;i2<count;++i1,++i2)
		str[i2] = string[i1];
	str[i2]=0;
	pgString ret;
	ret.setBuffer(str);
	return ret;
}

pgString&
pgString::toLower()
{
   char *ptr = string;
   while(*ptr)
   {
     if (isupper(*ptr))
       *ptr = static_cast<char>(tolower(*ptr));
     ++ptr;
   }
   return *this;
}

pgString&
pgString::toUpper()
{
   char *ptr = string;
   while(*ptr)
   {
     if (islower(*ptr))
       *ptr = static_cast<char>(toupper(*ptr));
     ++ptr;
   }
   return *this;
}

char 
pgString::toLower(char nC)
{
	const char a='a',A='A'; 
	if (isalpha(nC))
	{
		if (isupper(nC))
			nC = static_cast<char>(a+(nC-A));
	}
	return nC;
}

char 
pgString::toUpper(char nC)
{
	const char a='a',A='A'; 
	if (isalpha(nC))
	{
		if (islower(nC))
			nC = static_cast<char>(A+(nC-a));
	}
	return nC;
}

pgString&
pgString::cutC(const char c,bool forward)
{
	int i,index=-1;
	if (string!=NULL)
	{
		if (forward)
		{
			for (i=0;(i<length());++i)
				if (string[i]==c)
					index = i;
				else
					break;
			if (index!=-1)
				*this = getSubString(index+1,-1);
		}
		else
		{
			int l=length()-1;
			for (i=l;i>=0;--i)
				if (string[i]==c)
					index = i;
				else
					break;
			if (index!=-1)
				*this = getSubString(0,index);
		}
	}
    return *this;
}

pgString&
pgString::cut(char c)
{
	cutC(c,true);
	cutC(c,false);
    return *this;
}

pgString&
pgString::cutS(const pgString& cs,bool forward)
{
	int i,index=-1;
	if (string!=NULL)
	{
		if (forward)
		{
			for (i=0;(i<length());++i)
				if (cs.getIndex(string[i])>-1)
					index = i;
				else
					break;
			if (index!=-1)
				*this = getSubString(index+1,-1);
		}
		else
		{
			int l=length()-1;
			for (i=l;i>=0;--i)
				if (cs.getIndex(string[i])>-1)
					index = i;
				else
					break;
			if (index!=-1)
				*this = getSubString(0,index-1);
		}
	}
    return *this;
}

pgString&
pgString::cut(const pgString& cs)
{
	cutS(cs,true);
	cutS(cs,false);
    return *this;
}

int
pgString::find(const pgString& nSubString)
{
	if (string!=NULL)
	{
		char *sub = strstr(string,nSubString);
		if (sub!=NULL)
			return (sub - string +1);
	}
	return -1;
}

bool 
pgString::operator<( const pgString& other) const
{
	if ((string == NULL) || (other.string == NULL))
		return (string < other.string);
	return (strcmp(string,other.string)<0);
}

bool 
pgString::operator>( const pgString& other) const
{
	if ((string == NULL) || (other.string == NULL))
		return (string > other.string);
	return (strcmp(string,other.string)>0);
}

bool 
pgString::operator==( const pgString& other) const
{
	if ((string == NULL) || (other.string == NULL))
		return (string == other.string);
	return (!strcmp(string,other.string));
}

/*bool 
pgString::operator==( const char* str) const
{
	pgString other(str);

	if ((string == NULL) || (other.string == NULL))
		return (string == other.string);
	return (!strcmp(string,other.string));
}*/

bool 
pgString::operator==( const char* str) const
{
	if ((string == NULL) || (str == NULL))
		return (string == str);
	return (!strcmp(string,str));
}

bool 
pgString::operator!=( const pgString& other) const
{
	if ((string == NULL) || (other.string == NULL))
		return (string != other.string);
	return (strcmp(string,other.string)!=0);
}

/*bool 
pgString::operator!=( const char* str) const
{
	pgString other(str);

	if ((string == NULL) || (other.string == NULL))
		return (string != other.string);
	return (strcmp(string,other.string)!=0);
}*/

bool 
pgString::operator!=( const char* str) const
{
	if ((string == NULL) || (str == NULL))
		return (string != str);
	return (strcmp(string,str)!=0);
}

pgString&
pgString::operator=( const pgString& other)
{
	if(string)
		delete string;
	if (other.string==NULL)
	{
		string = NULL;
	}
	else
	{
		string = new char[strlen(other.string)+1];
		strcpy(string, other.string);
	}
	return *this;
}

pgString&
pgString::operator+=( const pgString& other)
{
	cat(other);
	return *this;
}

bool 
operator==(const char* left, const pgString& right)
{
    return right==left;
}

void
pgString::setBuffer(char* nBuffer)
{
	if(string)
		delete string;

	string = nBuffer;
}


pgStringEx::~pgStringEx()
{
	if(string!=NULL)
		delete string;
	string = NULL;
}
