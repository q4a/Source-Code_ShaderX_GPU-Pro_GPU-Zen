#ifndef __ODBLOCK_H__
#define __ODBLOCK_H__
/*-------------------------------------------------------------*\
|																|
|	ODBlock class - object description script					|
|																|
| Copyright (C) 1999 Adam Moravanszky							|
|																|
|                                                               |
\*-------------------------------------------------------------*/
/*
ObjectDescription Scripts
--------------------------
ODScript = Block
Statement = Block | Terminal
Block = indent '{' {Statement} '}'  
Terminal = ident ';'

Comments:
	# = line comment
	/*  * / = multiline comment.  The / character cannot be used in identifiers.

idents may be enclosed in quotes, and should be unique to facilitate searching.

In a typical application, program would look for known Blocks, and read out its user set terminal(s).
Particular users define semantics:

SHIPFILE 
	{
	Shipname
		{
		Client
			{
			ShipModel
				{
				MeshFile 
					{
					Filename;
					lodlevels;
					}
				Texturefile 
					{
					Filename;
					}
				}
			CockpitModel
				{
				...
				}
			}
		Server
			{
			...
			}
		}
	}
*/

#include <Smart.h>
class ODBlock;

#include <vector> 
#include <iostream>    //ostream would be enough, but gcc only has iostream

#define OD_MAXID 30								//max identifier length
												//typedef List<ODBlock> ODBlockList;
typedef std::vector<ODBlock *> ODBlockList;


class ODBlock
/*-------------------------\
| Block = indent '{' {Statement} '}'  
| Terminals are simply empty blocks
|
|
\-------------------------*/
	{
	class ODSyntaxError 
		{
		public:
		enum Error { ODSE_UNEX_QUOTE, ODSE_UNEX_OBRACE, ODSE_UNEX_CBRACE, ODSE_UNEX_LITERAL,ODSE_UNEX_EOF,ODSE_ENC_UNKNOWN };
		private:
		Error err;
		public:
		ODSyntaxError(Error e) {err = e;};
		char * asString();
		};
	char * identifier;
	unsigned identSize;							//size of above array.
	bool bTerminal;
	ODBlockList subBlocks;
	
	ODBlockList::iterator termiter;				//iterator for reading terminals

	//file scrambling
	static unsigned cryptState;
	static char decrypt(char c, unsigned & cryptState);
	static char encrypt(char c, unsigned & cryptState);
	static void encrypt(char * c, std::ostream &, unsigned & cryptState);


	public:
	static ODBlock *loadScript(std::istream &);	//loads block from script
	ODBlock();									//create a new one
	~ODBlock();
	ODBlock(std::istream &,bool encrypt,char c=0);			//loads block from script, including subbocks
	void saveScript(std::ostream &, bool bQuote, bool encrypt=0);//saves this block to scipt file.  set bQuote == true if you want to machine parse output.
	void saveScript(FILE * fp,bool bQuote);						//this is for broken MS stl where ostream doesn't work.
	void saveFile(std::ostream &,bool bQuote,bool encrypt=0);					//should be called before saveScript() when using encryption.

	//reading:
	string ident();
	bool isTerminal();							//resets to first statement returns false if its a terminal == no contained Blocks
	
	//writing:	
	void ident(string);						//identifier of the block
	void addStatement(ODBlock &);

	//queries:  return true in success
	ODBlock * getBlock(string identifier,bool bRecursiveSearch=false);	//returns block with given identifier, or NULL.

	void reset();								//prepares to get first terminal or sub block of current block

	//getting terminals:
	bool moreTerminals();						//returns true if more terminals are available
	string nextTerminal();					//returns a pointer to the next immediate terminal child of current block's identifier string.  

	//getting terminals:
	bool moreSubBlocks();						//returns true if more sub blocks (including terminals) are available
	ODBlock * nextSubBlock();					//returns a pointer to the next sub block.  

	// hig level macro functs, return true on success: (call for obj containing:)
	bool getBlockInt(string ident, int *);		//reads blocks of form:		ident{ 123;}
	bool getBlockString(string ident, char **);		//of form:				ident{abcdef;}
	bool getBlockFloat(string ident, float *);		//of form:				ident{123.456;}
	bool getBlockFloats(string ident, float *, unsigned  numfloats);//form:		ident{12.3; 12.3; 12.3; ... };

	//errors
	static const char * lastError;
	};


#endif //__ODBLOCK_H__
