                  /*--.          .-"-.
                 /   ._.        / ´ ` \
                 \_  (__\       \_°v°_/
                 //   \\        //   \\
                ((     ))      ((     ))
 ¤¤¤¤¤¤¤¤¤¤¤¤¤¤--""---""--¤¤¤¤--""---""--¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
 ¤                 |||            |||                                               ¤
 ¤  Author's note:  |              |                                                ¤
 ¤                                                                                  ¤
 ¤  This file is part of work done by Emil Persson, aka Humus.                      ¤
 ¤  You're free to use the code in any way you like, modified, unmodified           ¤
 ¤  or cut'n'pasted into your own work. But for the good of the whole               ¤
 ¤  programming community I will ask you to as long as possible and to the          ¤
 ¤  extent you find suitable to follow these points:                                ¤
 ¤   * If you use this file and it's contents unmodified, please leave this note.   ¤
 ¤   * If you use a major part of the file in your app, please credit the author(s) ¤
 ¤     in one way or the other as you find suitable, for instance a small notice    ¤
 ¤     in the readme file may do, or perhaps a link to the authors site.            ¤
 ¤   * Share your work and ideas too as much as you can.                            ¤
 ¤    _______                                                                       ¤
 ¤  /` _____ `\;,    Humus                                                          ¤
 ¤ (__(^===^)__)';,  emiper-8@student.luth.se                         ___           ¤
 ¤   /  :::  \   ,;  ICQ #47010716                                  /^   ^\         ¤
 ¤  |   :::   | ,;'                                                ( Ö   Ö )        ¤
 ¤¤¤'._______.'`¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤--°oOo--(_)--oOo°--¤¤*/

#ifndef _STRING_H_
#define _STRING_H_

#include <string.h>
#include <stdlib.h>

class String {
private:
	char *str;
	unsigned int length;
	unsigned int capasity;
public:
	String();
	String(unsigned int cap);
	String(const char *string);
	String(const char *string, unsigned int len);
	String(const String &string);
	~String();
	
	void setLength(const unsigned int newLength);

	operator const char *() const { return str; }
	unsigned int getLength() const { return length; }
	bool isEmpty() const { return length == 0; }

	int toInt() const { return atoi(str); }
	float toFloat() const { return (float) atof(str); }
	double toDouble() const { return atof(str); }

	void makeLowerCase();

	void operator =  (const char *string);
	void operator =  (const String &string);
	void operator += (const char *string);
	void operator += (const String &string);

	void assign(const char *string, const unsigned int len);
	void append(const char *string, const unsigned int len);

	unsigned int replace(const char oldCh, const char newCh);
	unsigned int replace(const char *oldStr, const char *newStr);

	bool  find(const char ch, unsigned int *index = NULL) const;
	bool rfind(const char ch, unsigned int *index = NULL) const;
	bool  find(const char *string, unsigned int *index = NULL) const;

	void trimRight(const char *chars);

	void sprintf(char *formatStr, ... );
};

String operator + (const String &string, const String &string2);
String operator + (const String &string, const char   *string2);
String operator + (const char   *string, const String &string2);

bool operator == (const String &string, const String &string2);
bool operator == (const String &string, const char   *string2);
bool operator == (const char   *string, const String &string2);

bool operator != (const String &string, const String &string2);
bool operator != (const String &string, const char   *string2);
bool operator != (const char   *string, const String &string2);

bool operator > (const String &string, const String &string2);
bool operator > (const String &string, const char   *string2);
bool operator > (const char   *string, const String &string2);

bool operator < (const String &string, const String &string2);
bool operator < (const String &string, const char   *string2);
bool operator < (const char   *string, const String &string2);

bool operator >= (const String &string, const String &string2);
bool operator >= (const String &string, const char   *string2);
bool operator >= (const char   *string, const String &string2);

bool operator <= (const String &string, const String &string2);
bool operator <= (const String &string, const char   *string2);
bool operator <= (const char   *string, const String &string2);

#endif // _STRING_H_
