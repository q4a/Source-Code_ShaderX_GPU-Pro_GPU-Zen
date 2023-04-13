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

#include "Config.h"

#include "Tokenizer.h"
#include "../Platform.h"

#include <stdio.h>

Config::Config(){


}

Config::~Config(){
	clear();
}

void Config::clear(){
	int index = vars.getSize();

	while (index){
		index--;
		delete vars[index].name;
		delete vars[index].value;
	}
	vars.clear();
}


#include <stdio.h>

bool Config::loadFromFile(const char *fileName){
	clear();

	Tokenizer tok;
	char str[256];

	if (!tok.setFile(fileName)) return false;

	Var var;
	while (tok.next(str)){
		var.nameLen = strlen(str);
		var.name = new char[var.nameLen + 1];
		strcpy(var.name, str);

		while (tok.next(str) && str[0] != '=');
		if (!tok.next(str)) str[0] = '\0';

		var.valueLen = strlen(str);
		var.value = new char[var.valueLen + 1];
		strcpy(var.value, str);

		while (tok.next(str) && str[0] != ';');

		vars.add(var);
	}

	return true;
}

bool Config::writeToFile(const char *fileName){
	FILE *file;

	if ((file = fopen(fileName, "wb")) == NULL) return false;

	int i, len = vars.getSize();
	int nChars = 0;
	for (i = 0; i < len; i++){
		nChars += strlen(vars[i].name) + strlen(vars[i].value);
	}
	nChars += len * 6;

	char *buf = new char[nChars];
	char *dest = buf;
	for (i = 0; i < len; i++){
		strncpy(dest, vars[i].name, vars[i].nameLen);
		dest += vars[i].nameLen;
		strncpy(dest, " = ", 3);
		dest += 3;
		strncpy(dest, vars[i].value, vars[i].valueLen);
		dest += vars[i].valueLen;
		strncpy(dest, ";\r\n", 3);
		dest += 3;
	}

	fwrite(buf, nChars, 1, file);
	fclose(file);

	delete buf;

	return false;
}

bool Config::find(const char *name, int *index){
	int i, len = vars.getSize();

	for (i = 0; i < len; i++){
		if (stricmp(vars[i].name, name) == 0){
			if (index != NULL) *index = i;
			return true;
		}
	}
	return false;
}

bool Config::remove(const char *name){
	int index;
	if (find(name, &index)){
		delete vars[index].name;
		delete vars[index].value;
		vars.remove(index);

		return true;
	}
	return false;
}

bool Config::getBool(const char *name, bool &value){
	int index;
	if (find(name, &index)){
		value = (atoi(vars[index].value) != 0);
		return true;
	}
	return false;
}

bool Config::getInt(const char *name, int &value){
	int index;
	if (find(name, &index)){
		value = atoi(vars[index].value);
		return true;
	}
	return false;
}

char *Config::getString(const char *name){
	int index;
	if (find(name, &index))	return vars[index].value;
	return NULL;
}

void Config::setBool(const char *name, bool value){
	int index;

	if (find(name, &index)){
		delete vars[index].value;
		vars[index].valueLen = 1;
		vars[index].value = new char[2];
		vars[index].value[0] = value? '1' : '0';
		vars[index].value[1] = '\0';
	} else {
		Var var;
		var.nameLen = strlen(name);
		var.name = new char[var.nameLen + 1];
		strcpy(var.name, name);
		var.valueLen = 1;
		var.value = new char[2];
		var.value[0] = value? '1' : '0';
		var.value[1] = '\0';
		vars.add(var);
	}
}

void Config::setInt(const char *name, int value){
	char str[16];
	int index;
	int len = sprintf(str, "%d", value);

	if (find(name, &index)){
		delete vars[index].value;
		vars[index].valueLen = len;
		vars[index].value = new char[len + 1];
		strcpy(vars[index].value, str);
	} else {
		Var var;
		var.nameLen = strlen(name);
		var.name = new char[var.nameLen + 1];
		strcpy(var.name, name);
		var.valueLen = len;
		var.value = new char[len + 1];
		strcpy(var.value, str);
		vars.add(var);
	}
}

void Config::setString(const char *name, char *value){
	int index;
	int len = strlen(value);

	if (find(name, &index)){
		delete vars[index].value;
		vars[index].valueLen = len;
		vars[index].value = new char[len + 1];
		strcpy(vars[index].value, value);
	} else {
		Var var;
		var.nameLen = strlen(name);
		var.name = new char[var.nameLen + 1];
		strcpy(var.name, name);
		vars[index].valueLen = len;
		var.value = new char[len + 1];
		strcpy(var.value, value);
		vars.add(var);
	}
}
