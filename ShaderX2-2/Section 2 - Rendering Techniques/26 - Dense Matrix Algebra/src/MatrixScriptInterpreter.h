#ifndef __MATRIXSCRIPTINTERPRETER_H__
#define __MATRIXSCRIPTINTERPRETER_H__

#include <iostream>
#include <string>
#include <map>

class ODBlock;
class Matrix;

class Variable			//a scalar or matrix variable.
	{
	public:
	Matrix * matrix;	//if matrix is NULL then scalar below is valid
	float scalar;
	};

typedef std::map<std::string, Variable> VariableMap;	//this is for quickly retrieving a variable given its string name


class MatrixScriptInterpreter
	{
	public:
	MatrixScriptInterpreter();
	~MatrixScriptInterpreter();

	void execute(std::istream &);
	bool execute(ODBlock &);

	private:
	VariableMap variables;
	bool profile;

	//commands
	bool cmdLoad(ODBlock &);
	bool cmdSave(ODBlock &);
	bool cmdSet(ODBlock &);
	bool cmdRandPD(ODBlock &);
	bool cmdZeros(ODBlock &);
	bool cmdIdentity(ODBlock &);
	bool cmdAdd(ODBlock &);
	bool cmdSubtract(ODBlock &);
	bool cmdMultiplyScalar(ODBlock &);
	bool cmdMultiply(ODBlock &);
	bool cmdMultiplyAT(ODBlock &);
	bool cmdDot(ODBlock &);
	bool cmdMad(ODBlock &);
	bool cmdTranspose(ODBlock &);
	bool cmdConjugateGradients(ODBlock &);
	bool cmdLcpProjectedJacobi(ODBlock &);



	Matrix & fetchMatrix(char * name);
	float  & fetchScalar(char * name);

	bool fetch2Args(ODBlock & block, char ** one, char ** two);
	bool fetch3Args(ODBlock & block, char ** one, char ** two, char ** three);
	bool fetch4Args(ODBlock & block, char ** one, char ** two, char ** three, char ** four);
	bool fetch5Args(ODBlock & block, char ** one, char ** two, char ** three, char ** four, char **five);

	void beginCmd();
	void endCmd(Matrix & dest, ODBlock & block);

	};


#endif