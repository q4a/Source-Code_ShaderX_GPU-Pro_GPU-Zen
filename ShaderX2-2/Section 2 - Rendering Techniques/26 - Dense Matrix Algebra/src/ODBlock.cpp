/*-------------------------------------------------------------*\
|																|
|	ODBlock class - object description script					|
|																|
| Copyright (C) 1999 Adam Moravanszky							|
|																|
|                                                               |
\*-------------------------------------------------------------*/
/*-Revision----------------------\
| At: 10/3/99 5:51:37 PM
|
| Changed vector container from 
| storing val to refernce.
\-------------------------------*/
/*-Revision----------------------\
| At: 5/18/00 9:29:43 PM
| changed static sized indentifiers
| to arbitrary dynamic identifiers.
|
\-------------------------------*/
/*-Revision----------------------\
| At: 6/19/02 
| added multiline comments.
|
|
\-------------------------------*/

#include <ODBlock.h>
#include <string.h>


#ifdef P_OS_XSOLARIS
#define ios_base ios
#define widen(exp) exp
#else
#define widen(exp) is.widen(exp)
#endif


using namespace std;

const char * ODBlock::lastError = NULL;
unsigned ODBlock::cryptState = 0;

static char map[95][5] = {
{16, 23, 93, 4, 59},
{82, 91, 10, 41, 14},
{5, 58, 33, 22, 31},
{51, 67, 4, 33, 71},
{91, 6, 89, 7, 93},
{60, 20, 90, 44, 82},
{78, 63, 60, 43, 66},
{68, 79, 26, 36, 34},
{81, 26, 85, 14, 41},
{53, 60, 88, 80, 80},
{55, 51, 58, 26, 2},
{86, 16, 64, 20, 22},
{75, 33, 6, 83, 88},
{84, 68, 57, 75, 3},
{85, 30, 50, 32, 49},
{52, 75, 20, 81, 75},
{15, 50, 5, 51, 6},
{74, 81, 92, 38, 48},
{12, 66, 70, 86, 72},
{47, 37, 51, 91, 84},
{49, 88, 3, 82, 52},
{80, 94, 63, 48, 81},
{56, 22, 38, 10, 69},
{30, 9, 35, 53, 38},
{90, 48, 84, 6, 89},
{35, 43, 52, 9, 40},
{63, 71, 23, 46, 37},
{42, 69, 79, 35, 60},
{25, 72, 29, 39, 58},
{19, 36, 54, 60, 11},
{8, 12, 80, 62, 92},
{13, 54, 12, 92, 45},
{67, 87, 11, 37, 91},
{11, 7, 72, 1, 90},
{38, 32, 19, 29, 57},
{48, 73, 91, 54, 70},
{77, 1, 66, 93, 1},
{58, 76, 0, 8, 79},
{73, 21, 36, 45, 9},
{93, 5, 17, 0, 26},
{34, 38, 41, 79, 67},
{20, 14, 7, 73, 65},
{64, 28, 82, 70, 77},
{29, 0, 76, 49, 30},
{83, 29, 22, 15, 32},
{7, 85, 74, 56, 85},
{92, 3, 25, 40, 68},
{33, 70, 68, 17, 43},
{40, 78, 32, 58, 4},
{1, 17, 48, 18, 15},
{76, 35, 37, 2, 76},
{70, 13, 75, 89, 61},
{17, 10, 16, 67, 53},
{57, 24, 24, 42, 19},
{23, 11, 49, 52, 46},
{2, 90, 44, 30, 28},
{44, 2, 15, 25, 8},
{66, 55, 42, 74, 94},
{14, 92, 53, 50, 74},
{69, 27, 27, 47, 27},
{10, 31, 69, 11, 16},
{54, 42, 28, 64, 35},
{65, 84, 59, 71, 55},
{18, 77, 14, 78, 36},
{94, 39, 86, 65, 87},
{26, 40, 40, 90, 25},
{45, 44, 83, 77, 24},
{61, 47, 34, 59, 10},
{36, 74, 73, 57, 29},
{88, 83, 1, 13, 54},
{79, 25, 18, 94, 12},
{89, 41, 55, 27, 83},
{0, 46, 30, 85, 73},
{50, 52, 45, 28, 39},
{71, 34, 2, 12, 21},
{21, 64, 13, 69, 62},
{46, 15, 46, 34, 23},
{39, 19, 39, 72, 44},
{4, 65, 77, 31, 78},
{87, 53, 56, 3, 86},
{6, 61, 43, 76, 18},
{37, 59, 21, 24, 47},
{28, 93, 71, 23, 33},
{72, 82, 61, 63, 42},
{41, 49, 65, 68, 7},
{43, 80, 67, 19, 20},
{22, 45, 62, 66, 5},
{32, 18, 94, 21, 17},
{31, 62, 31, 88, 50},
{3, 89, 47, 55, 13},
{59, 86, 81, 61, 63},
{62, 4, 8, 5, 56},
{24, 57, 87, 87, 51},
{27, 8, 78, 84, 64},
{9, 56, 9, 16, 0}};

static char invmap[95][5] = {
{72, 43, 37, 39, 94},
{49, 36, 69, 33, 36},
{55, 56, 74, 50, 10},
{89, 46, 20, 79, 13},
{78, 91, 3, 0, 48},
{2, 39, 16, 91, 86},
{80, 4, 12, 24, 16},
{45, 33, 41, 4, 84},
{30, 93, 91, 37, 56},
{94, 23, 94, 25, 38},
{60, 52, 1, 22, 67},
{33, 54, 32, 60, 29},
{18, 30, 31, 74, 70},
{31, 51, 75, 69, 89},
{58, 41, 63, 8, 1},
{16, 76, 56, 44, 49},
{0, 11, 52, 94, 60},
{52, 49, 39, 47, 87},
{63, 87, 70, 49, 80},
{29, 77, 34, 85, 53},
{41, 5, 15, 11, 85},
{75, 38, 81, 87, 74},
{86, 22, 44, 2, 11},
{54, 0, 26, 82, 76},
{92, 53, 53, 81, 66},
{28, 70, 46, 56, 65},
{65, 8, 7, 10, 39},
{93, 59, 59, 71, 59},
{82, 42, 61, 73, 55},
{43, 44, 28, 34, 68},
{23, 14, 72, 55, 43},
{88, 60, 88, 78, 2},
{87, 34, 48, 14, 44},
{47, 12, 2, 3, 82},
{40, 74, 67, 76, 7},
{25, 50, 23, 27, 61},
{68, 29, 38, 7, 63},
{81, 19, 50, 32, 26},
{34, 40, 22, 17, 23},
{77, 64, 77, 28, 73},
{48, 65, 65, 46, 25},
{84, 71, 40, 1, 8},
{27, 61, 57, 53, 83},
{85, 25, 80, 6, 47},
{56, 66, 55, 5, 77},
{66, 86, 73, 38, 31},
{76, 72, 76, 26, 54},
{19, 67, 89, 59, 81},
{35, 24, 49, 21, 17},
{20, 84, 54, 43, 14},
{73, 16, 14, 58, 88},
{3, 10, 19, 16, 92},
{15, 73, 25, 54, 20},
{9, 79, 58, 23, 52},
{61, 31, 29, 35, 69},
{10, 57, 71, 89, 62},
{22, 94, 79, 45, 91},
{53, 92, 13, 68, 34},
{37, 2, 10, 48, 28},
{90, 81, 62, 67, 0},
{5, 9, 6, 29, 27},
{67, 80, 83, 90, 51},
{91, 88, 86, 30, 75},
{26, 6, 21, 83, 90},
{42, 75, 11, 61, 93},
{62, 78, 84, 64, 41},
{57, 18, 36, 86, 6},
{32, 3, 85, 52, 40},
{7, 13, 47, 84, 46},
{59, 27, 60, 75, 22},
{51, 47, 18, 42, 35},
{74, 26, 82, 62, 3},
{83, 28, 33, 77, 18},
{38, 35, 68, 41, 72},
{17, 68, 45, 57, 58},
{12, 15, 51, 13, 15},
{50, 37, 43, 80, 50},
{36, 63, 78, 66, 42},
{6, 48, 93, 63, 78},
{70, 7, 27, 40, 37},
{21, 85, 30, 9, 9},
{8, 17, 90, 15, 21},
{1, 83, 42, 20, 5},
{44, 69, 66, 12, 71},
{13, 62, 24, 93, 19},
{14, 45, 8, 72, 45},
{11, 90, 64, 18, 79},
{79, 32, 92, 92, 64},
{69, 20, 9, 88, 12},
{71, 89, 4, 51, 24},
{24, 55, 5, 65, 33},
{4, 1, 35, 19, 32},
{46, 58, 17, 31, 30},
{39, 82, 0, 36, 4},
{64, 21, 87, 70, 57}};

char ODBlock::encrypt(char c, unsigned & cryptState)
	{
	char t;
	if (c > ' ' && c <= 127)
		{
		c -= (' '+1);
		t			= map[c][cryptState-1];
		cryptState  = 1+((cryptState+c+t)%5);
		c = t;
		c += (' '+1);
		//assert(c < 127);
		}
	return c;
	}

char ODBlock::decrypt(char c, unsigned & cryptState)
	{
	char t;
	if (c > ' ' && c <= 127)
		{
		c -= (' '+1);
		t			= invmap[c][cryptState-1];
		cryptState  = 1+((cryptState+c+t)%5);
		c = t;
		c += (' '+1);
		}
	return c;
	}

void ODBlock::encrypt(char * c, std::ostream & s , unsigned & cryptState)
	{
	while (*c)
		{
		s << encrypt(*c , cryptState);
		c++;
		}
	}



char * ODBlock::ODSyntaxError::asString()
	{
	static char unex1[] = "Quote unexpected.";
	static char unex2[] = "Opening brace unexpected.";
	static char unex3[] = "Closing brace unexpected.";
	static char unex4[] = "Literal or semicolon unexpected.";
	static char unex5[] = "Unexpected end of file found.";
	static char encun[] = "Unknown encryption algo code.";
	switch (err)
		{
		case ODSE_UNEX_QUOTE:
			return unex1;
		case ODSE_UNEX_OBRACE:
			return unex2;
		case ODSE_UNEX_CBRACE:
			return unex3;
		case ODSE_UNEX_LITERAL:
			return unex4;
		case ODSE_UNEX_EOF:
			return unex5;
		case ODSE_ENC_UNKNOWN:
			return encun;
		default:
			return NULL;
		}
	}


ODBlock *ODBlock::loadScript(istream &is)
	{//create ODBlock from file
	ODBlock * o;
	char c;

	try {
		is.setf(ios_base::skipws);			//skip whitespace 
		is >> c;
		if (c == '!')
			{
			is >> c;	//the character after the ! is a code for the encryption algo used.  We only support version 1 at the moment.
			if (c != '1')
				throw new ODSyntaxError(ODSyntaxError::ODSE_ENC_UNKNOWN);
			else
				{
				cryptState = 1;
				o = new ODBlock(is,true);
				}
			}
		else
			o = new ODBlock(is,false,c);
		}
	catch (ODSyntaxError * c)
		{
		lastError = c->asString();
		return NULL;
		}
	return o;
	}

ODBlock::ODBlock(istream &is,bool encrypt,char cc)			//loads block from script, including subbocks
						//crypt is 0 for unencrypted, 1 for seed.
	{
	char c,currindex = 0;
	bool bQuote = false;					//if we're inside a pair of quotes
	identifier = NULL;
	identSize = 0;
	bTerminal = true;
	termiter = NULL;

	enum {WAIT_IDENT,IDENT,WAIT_BLOCK,BLOCK} state;
#ifdef P_OS_XSOLARIS
	//gcc error workaround: sorry, not implemented: initializer contains unrecognized tree code
#define WAIT_IDENT 0
#define IDENT 1
#define WAIT_BLOCK 2
#define BLOCK 3
#endif
	
	state = WAIT_IDENT;


	//0) read possible comments starting with # to end of line
	//1) read our identifier. -->forget quotes for now.

	while(!is.fail())
		{
		if (cc)		//a char that might have been read out of the stream by our caller -- process it instead of getting a new one.
			c = cc, cc=0;
		else
			{
			is >> c;
			if (encrypt)
				c = decrypt(c,cryptState);
			}
		if  (c == '"')
			{
			if (state == BLOCK)
				{
				bTerminal = false;
				addStatement(*(new ODBlock(is,encrypt,c)));
				}
			else
				{
				  if (state != WAIT_IDENT && state != IDENT) 
					  throw new ODSyntaxError(ODSyntaxError::ODSE_UNEX_QUOTE);
				bQuote = !bQuote;
				if (bQuote)
					is.unsetf(ios_base::skipws);
				else
					is.setf(ios_base::skipws);
				state = IDENT;
				}
			}
		else
			{
			if (bQuote)
				{
				assert(state == IDENT);
				if (currindex >= identSize)
					{
					identSize = 2*currindex + 4;
					char * newi = new char [identSize+1];
					if (identifier)
						strcpy(newi,identifier);
					sdelete(identifier);
					identifier = newi;
					}
				identifier[currindex] = c;
				identifier[currindex+1] = 0;
				currindex++;
				}
			else
				{
				switch (c)
					{
					case '#':
						is.ignore(0xFFFF,widen('\n'));
						break;
					case '/':
						is.ignore(0xFFFF,'/');
						break;
					case '{': 
						if (state != WAIT_BLOCK && state != IDENT) 
							throw new ODSyntaxError(ODSyntaxError::ODSE_UNEX_OBRACE);	//identifier can be followed by block w/o whitespace inbetween
						state = BLOCK;
						break;
					case '}':
						if (state != BLOCK) 
							throw new ODSyntaxError(ODSyntaxError::ODSE_UNEX_CBRACE);
						return;
					default:
						//text
						//our identifier?
						if (state == BLOCK)
							{
							bTerminal = false;
							addStatement(*(new ODBlock(is,encrypt,c)));
							}
						else
							{
							if (state != WAIT_IDENT && state != IDENT) 
								throw new ODSyntaxError(ODSyntaxError::ODSE_UNEX_LITERAL);
							if (state == IDENT && c == ';')		//terminal ended
								return;
							state = IDENT;

							if (currindex >= identSize)
								{
								identSize = 2*currindex + 4;
								char * newi = new char [identSize+1];
								if (identifier)
									strcpy(newi,identifier);
								sdelete(identifier);
								identifier = newi;
								}
							identifier[currindex] = c;
							identifier[currindex+1] = 0;
							currindex++;
							}
					}
				}
			}
		}
	//eof and block didn't close
	throw new ODSyntaxError(ODSyntaxError::ODSE_UNEX_EOF);
	}


ODBlock::ODBlock()
	{
	//create empty block
	identifier = NULL;
	identSize = 0;
	bTerminal = true;
	termiter = NULL;
	}

ODBlock::~ODBlock()
	{
	//free the contents
	ODBlockList::iterator i;
	for (i =  subBlocks.begin(); i != subBlocks.end(); ++i)
		sdelete((*i));
	//free the pointers
	subBlocks.clear();

	sdelete(identifier);
	identifier = NULL;
	identSize = 0;
	}

void ODBlock::saveFile(ostream & os,bool quote, bool encrypt)
	{
	if (encrypt)
		{
		os << "!1";
		cryptState = 1;
		saveScript(os,true,true);
		}
	else
		saveScript(os,quote, false);
	}
void ODBlock::saveScript(ostream & os,bool bQuote, bool encr)
	{
	static int tablevel = 1;
	int j;
	//save the script in said file  should be stream!!
	if (!encr)
		{
		for (j=0; j<tablevel-1; j++) os << '\t'; 
		if (bQuote) os << '"';
		if (identifier)
			os << identifier;
		else
			os << "_noname";
		if (bQuote) os << '"';
		if (!bTerminal)
			{
			os << "\n";
			for (j=0; j<tablevel; j++) os << '\t';		os << "{\n";
			tablevel ++;
			ODBlockList::iterator i;
			for (i =  subBlocks.begin(); i != subBlocks.end(); ++i)
				(*i)->saveScript(os,bQuote);
			tablevel --;
			for (j=0; j<tablevel; j++) os << '\t';		os << "}\n";
			}
		else 
			os << ";\n";
		}
	else
		{
		os << encrypt('"',cryptState);
		if (identifier)
			encrypt(identifier, os,cryptState);
		else
			encrypt("_noname", os,cryptState);
		os << encrypt('"',cryptState);
		if (!bTerminal)
			{
			os << encrypt('{',cryptState);
			ODBlockList::iterator i;
			for (i =  subBlocks.begin(); i != subBlocks.end(); ++i)
				(*i)->saveScript(os,bQuote, encr);
			os << encrypt('}',cryptState);
			}
		else 
			{
			os << encrypt(';',cryptState);
			}
		}
	}


void ODBlock::saveScript(FILE * fp,bool bQuote)
	{
	static int tablevel = 1;
	int j;
	//save the script in said file  should be stream!!
	for (j=0; j<tablevel-1; j++)
		fprintf(fp,"\t"); 
	if (bQuote) fprintf(fp,"\"");
	if (identifier)
		fprintf(fp,identifier);
	else
		fprintf(fp,"_noname");
	if (bQuote) fprintf(fp,"\"");
	if (!bTerminal)
		{
		fprintf(fp,"\n");
		for (j=0; j<tablevel; j++) 
			fprintf(fp,"\t");	
		fprintf(fp,"{\n");
		tablevel ++;
		ODBlockList::iterator i;
		for (i =  subBlocks.begin(); i != subBlocks.end(); ++i)
			(*i)->saveScript(fp,bQuote);
		tablevel --;
		for (j=0; j<tablevel; j++) 
			fprintf(fp,"\t");
		fprintf(fp,"}\n");
		}
	else 
		fprintf(fp,";\n");
	}


::string ODBlock::ident()
	{
	static char noname[] = "_noname";
	if (identifier)
		return identifier;
	else
		return noname;
	}


bool ODBlock::isTerminal()
	{
	return bTerminal;
	}


 void ODBlock::ident(::string i)
	{
	if (!i) return;
	unsigned l = strlen(i)+1;
	if (identSize < l)
		{
		sdelete(identifier);
		identifier = new char[l+1];
		identSize = l;
		}
	strcpy(identifier,i);
	}


void ODBlock::addStatement(ODBlock &b)
	{
	bTerminal = false;
	subBlocks.push_back(&b);
	}

void ODBlock::reset()								//prepares to get first immediate terminal child of current block
	{
	termiter = subBlocks.begin();
	}

bool ODBlock::moreTerminals()						//returns true if more terminals are available
	{
	//skip any nonterminals
	while ( (termiter != subBlocks.end()) && !(*termiter)->bTerminal)
		++termiter;
	return  termiter != subBlocks.end();			//return true if not the end yet
	}

::string ODBlock::nextTerminal()								//returns a pointer to the next terminal string.  
	{
	::string  s = (*termiter)->identifier;
	++termiter;

	static char noname[] = "_noname";
	if (s)
		return s;
	else
		return noname;
	}

bool ODBlock::moreSubBlocks()						//returns true if more sub blocks are available
	{
	return  termiter != subBlocks.end();			//return true if not the end yet
	}
ODBlock * ODBlock::nextSubBlock()					//returns a pointer to the next sub block.  
	{
	ODBlock * b =  (*termiter);
	++termiter;
	return b;
	}


ODBlock * ODBlock::getBlock(::string ident,bool bRecursiveSearch)		//returns block with given identifier, or NULL.
	{
	ODBlock * b;
	if (identifier && strncmp(identifier,ident,OD_MAXID) == 0)
		return this;
	else
		{
		if (bTerminal) 
			return NULL;
		else
			{
			ODBlockList::iterator i;
			for (i =  subBlocks.begin(); i != subBlocks.end(); ++i)
				if (bRecursiveSearch)
					{
					if (b = (*i)->getBlock(ident,true)) 
						return b;
					}
				else
					{
					if ((*i)->identifier && strncmp((*i)->identifier,ident,OD_MAXID) == 0)
						return (*i);
					}
			return NULL;
			}
		}
		
	}

// hig level macro functs, return true on success:
bool ODBlock::getBlockInt(char * ident, int *p)		//reads blocks of form:		ident{ 123;}
	{
	ODBlock * temp;
	if (temp = getBlock(ident))
		{
		temp->reset(); 
		if (temp->moreTerminals())	
			{ 
			sscanf(temp->nextTerminal(),"%d",p); 
			return true; 
			}
		}
	return false;
	}
bool ODBlock::getBlockString(char *  ident,  char **  p)		//of form:				ident{abcdef;}
	{
	ODBlock * temp;
	if (temp = getBlock(ident))
		{
		temp->reset(); 
		if (temp->moreTerminals())	
			{ 
			*p = temp->nextTerminal();
			return true; 
			}
		}
	return false;
	}
bool ODBlock::getBlockFloat(char *  ident, float *p)		//of form:				ident{123.456;}
	{
	ODBlock * temp;
	if (temp = getBlock(ident))
		{
		temp->reset(); 
		if (temp->moreTerminals())	
			{ 
			sscanf(temp->nextTerminal(),"%f",p); 
			return true; 
			}
		}
	return false;
	}
bool ODBlock::getBlockFloats(char *  ident, float *p, unsigned numfloats)//form:		ident{12.3; 12.3; 12.3; ... };
	{
	ODBlock * temp;
	if (temp = getBlock(ident))
		{
		temp->reset();
		for (int n=0; n<numfloats; n++)
			if (temp->moreTerminals())	sscanf(temp->nextTerminal(),"%f",&(p[n]));

		return true;
		}
	return false;
	}

