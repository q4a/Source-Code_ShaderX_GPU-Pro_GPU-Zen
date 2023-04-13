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


#include "Tokenizer.h"
#include <stdio.h>
#include "../Platform.h"

bool isDelimiter(const char ch){
	return (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n');
}

bool isNumeric(const char ch){
	return (ch >= '0' && ch <= '9');
}

bool isAlphabetical(const char ch){
	return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_');
}


Tokenizer::Tokenizer(){
	str = NULL;

	reset();
}

Tokenizer::~Tokenizer(){
	if (str != NULL) delete str;
}

void Tokenizer::setString(const char *string){
	if (str != NULL) delete str;

	length = strlen(string);

	str = new char[(length + 1) * sizeof(char)];
	strcpy(str, string);

	reset();
}

bool Tokenizer::setFile(const char *fileName){
	FILE *file;

	if (str != NULL) delete str;

	if ((file = fopen(fileName, "rb")) != NULL){
		fseek(file, 0, SEEK_END);
		length = ftell(file);
		fseek(file, 0, SEEK_SET);

		str = new char[(length + 1) * sizeof(char)];
		fread(str, length, 1, file);
		str[length] = '\0';

		fclose(file);
		
		reset();
		return true;
	}
	return false;
}

void Tokenizer::reset(){
	end = 0;
}

bool Tokenizer::next(char *buffer, BOOLFUNC isAlpha){
	start = end;

	while (start < length && isDelimiter(str[start])) start++;
	if (start < length){
		
		end = start + 1;
		
		if (isNumeric(str[start])){
			while (isNumeric(str[end]) || str[end] == '.') end++;
		} else if (isAlpha(str[start])){
			while (isAlpha(str[end]) || isNumeric(str[end])) end++;
		}

		if (buffer != NULL){
			strncpy(buffer, str + start, end - start);
			buffer[end - start] = '\0';
		}
		return true;
	}
	return false;
}

bool Tokenizer::getAllRight(char *buffer){
	if (start < length){
		strcpy(buffer, str + start);
		return true;
	}
	return false;
}

bool Tokenizer::nextAfterToken(char *buffer, char *token){
	char str[256];

	while (next(str)){
		if (stricmp(str, token) == 0){
			return next(buffer);
		}
	}

	return false;
}
