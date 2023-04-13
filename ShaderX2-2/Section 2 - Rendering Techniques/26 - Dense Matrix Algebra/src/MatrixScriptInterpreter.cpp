#pragma warning( disable : 4786)

#include "MatrixScriptInterpreter.h"
#include "ODBlock.h"
#include "Matrix.h"
#include "Time.h"

/*
#script format
B3D
	{
	load { var; fname; }			#load var from fname
	save { var; fname; }			#save var to fname
	zeros { var; nrows; ncols; }	#set var to the nrows*ncols zero matrix
	identity { var; n; }			#set var to the n*n identity matrix
	set { var; src; }				#set var to the matrix source
	add	{ var; src1; src2; }		#set var to the matrix src1 + src2
	multiply { var; src1; src2; }	#set var to the matrix src1 * src2
	multiply_at{ var; src1; src2; }	#set var to the matrix src1' * src2

  # complex operations:
	conjugateGradients { destVec; srcMx; srcVec; }	# set destMx so that:  srcMx*destVec = srcVec  holds.
													# unless destVec is 0x0, it will be warmstarted from.  So set it to zero to prevent a warmstart.
													# srcMx has to be PSD.  Note that one could implement the 
													#biconjugate gradients algo for general matrices.
	lcpProjectedJacobi { z; M; d; q; iters; }		#set z so that:  
													#z >= 0,  M*z + q >= 0, and z'(M*z + q) = 0  holds.
													#z will be warmstarted from, unless its empty, so the caller may want to zero it first.
													#  d should be set to omega ./ diag(M), where diag(M) is the vector with the diagonal elements
													# of M, ./ is array division, and omega is a small negative magic number that steers convergence.
													# something like -0.85 works ok for omega in most cases.  Advanced versions of the algo don't need
													# to be provided with an omega.

	randPD { M; d; q; size; }						# creates a random problem for the LCP above.



  # variations on the above stuff:
	multiply_scalar{ var; const_scalar; }	#set var to var * const_scalar
	dot { src1; src2; }				#display src1 . src2
	mad { var; src1; src2; const_scalar; }	#set var to src1 + src2 * const_scalar

  #todo: implement:
	transpose { var; src; }			#set var to the matrix src'
	subtract { var; src1; src2; }	#set var to the matrix src1 - src2
	while { conditionalBlock bodyBlock }
	if { conditionalBlock thenBlock [elseIfBlock][elseBlock] }
	}
*/

Time timer;

MatrixScriptInterpreter::MatrixScriptInterpreter()
	{
	profile = false;
	}

MatrixScriptInterpreter::~MatrixScriptInterpreter()
	{
	//clean up the matrices.
	VariableMap::iterator i;
	for (i = variables.begin(); i != variables.end(); i++)
		{
		if ((*i).second.matrix)
			{
			delete (*i).second.matrix;
			(*i).second.matrix = 0;
			}
		}
	}

void MatrixScriptInterpreter::beginCmd()
	{
	if (profile)
		timer.GetElapsedSeconds();

	}

void MatrixScriptInterpreter::endCmd(Matrix & dest, ODBlock & block)
	{
	if (profile)
		{
		dest.flush();
		second t = timer.GetElapsedSeconds();
		std::cout << block.ident()  << " : " << t <<  " seconds." << std::endl;
		}
	}


void MatrixScriptInterpreter::execute(std::istream & input)
	{
	ODBlock * script = 0;
	script = ODBlock::loadScript(input);
	if (!script)
		{
		std::cout << ODBlock::lastError;
		return;
		}
	try
		{
		script->reset();
		while (script->moreSubBlocks())
			{
			execute(*script->nextSubBlock());
			}
		delete script;
		}
	catch (char * err)
		{
		std::cout << err << std::endl;
		}
	catch (...)
		{
		std::cout << "aborted: script error." << std::endl;
		}

	}


Matrix & MatrixScriptInterpreter::fetchMatrix(char * name)
	{
	//see if we have this variable already
		

	VariableMap::iterator i = variables.find(name);


	if(i == variables.end())
		{
		//doesn't exist yet, instance it!
		Variable & v = variables[name];
		v.matrix = new Matrix(name);
		return *v.matrix;
		}
	else
		{
		//right type?

		if ((*i).second.matrix)
			return *(*i).second.matrix;
		else
			{
			std::cout << name << " is not a matrix" << std::endl;
			throw 0;
			}

		}
	}


float  & MatrixScriptInterpreter::fetchScalar(char * name)
	{
	//see if we have this variable already
		

	VariableMap::iterator i = variables.find(name);


	if(i == variables.end())
		{
		//doesn't exist yet, instance it!
		Variable & v = variables[name];
		v.matrix = 0;
		v.scalar = 0.0f;
		return v.scalar;
		}
	else
		{
		//right type?

		if ((*i).second.matrix)
			{
			std::cout << name << " is not a matrix" << std::endl;
			throw 0;
			}
		else
			return (*i).second.scalar;

		}	
	}


bool MatrixScriptInterpreter::execute(ODBlock & block)
	{

	char * name = block.ident();
	//decide which exec funct to call
	if (!strcmp(name, "profile"))
		{
		profile = true;
		std::cout << "starting profiling, please wait..." << std::endl; 
		return true;
		}
	else if (!strcmp(name, "load"))
		return cmdLoad(block);
	else if (!strcmp(name, "save"))
		return cmdSave(block);
	else if (!strcmp(name, "zeros"))
		return cmdZeros(block);
	else if (!strcmp(name, "identity"))
		return cmdIdentity(block);
	else if (!strcmp(name, "set"))
		return cmdSet(block);
	else if (!strcmp(name, "randPD"))
		return cmdRandPD(block);
	else if (!strcmp(name, "add"))
		return cmdAdd(block);
	else if (!strcmp(name, "dot"))
		return cmdDot(block);
	else if (!strcmp(name, "mad"))
		return cmdMad(block);
	else if (!strcmp(name, "multiply"))
		return cmdMultiply(block);
	else if (!strcmp(name, "multiply_at"))
		return cmdMultiplyAT(block);
	else if (!strcmp(name, "multiply_scalar"))
		return cmdMultiplyScalar(block);
	else if (!strcmp(name, "transpose"))
		return cmdTranspose(block);
	else if (!strcmp(name, "subtract"))
		return cmdSubtract(block);
	else if (!strcmp(name, "conjugateGradients"))
		return cmdConjugateGradients(block);
	else if (!strcmp(name, "lcpProjectedJacobi"))
		return cmdLcpProjectedJacobi(block);
	else
		{
		std::cout << "error: unknown command <" << name << ">"<< std::endl;
		throw 0;
		}
	
	}

bool MatrixScriptInterpreter::fetch2Args(ODBlock & block, char ** one, char ** two)
	{
	block.reset();
	if (block.moreTerminals())
		{
		*one = block.nextTerminal();
		if (block.moreTerminals())
			{
			*two = block.nextTerminal();
			return true;
			}
		}

	std::cout << "error: 2 args expected for command " << block.ident() <<std::endl;
	throw 0;
	}

bool MatrixScriptInterpreter::fetch3Args(ODBlock & block, char ** one, char ** two, char ** three)
	{
	block.reset();
	if (block.moreTerminals())
		{
		*one = block.nextTerminal();
		if (block.moreTerminals())
			{
			*two = block.nextTerminal();
			if (block.moreTerminals())
				{
				*three = block.nextTerminal();
				return true;
				}
			}
		}

	std::cout << "error: 3 args expected for command " << block.ident() <<std::endl;
	throw 0;
	}

bool MatrixScriptInterpreter::fetch4Args(ODBlock & block, char ** one, char ** two, char ** three, char ** four)
	{
	block.reset();
	if (block.moreTerminals())
		{
		*one = block.nextTerminal();
		if (block.moreTerminals())
			{
			*two = block.nextTerminal();
			if (block.moreTerminals())
				{
				*three = block.nextTerminal();
				if (block.moreTerminals())
					{
					*four = block.nextTerminal();
					return true;
					}
				}
			}
		}
	std::cout << "error: 4 args expected for command " << block.ident() <<std::endl;
	throw 0;
	}

bool MatrixScriptInterpreter::fetch5Args(ODBlock & block, char ** one, char ** two, char ** three, char ** four, char ** five)
//yes, this is stupid, but copy & paste goes faster than writing a decent loop.  :-)
	{
	block.reset();
	if (block.moreTerminals())
		{
		*one = block.nextTerminal();
		if (block.moreTerminals())
			{
			*two = block.nextTerminal();
			if (block.moreTerminals())
				{
				*three = block.nextTerminal();
				if (block.moreTerminals())
					{
					*four = block.nextTerminal();
					if (block.moreTerminals())
						{
						*five = block.nextTerminal();
						return true;
						}
					}
				}
			}
		}
	std::cout << "error: 5 args expected for command " << block.ident() <<std::endl;
	throw 0;
	}



bool MatrixScriptInterpreter::cmdLoad(ODBlock & block)
//	load { var; fname; }			#load var from fname
	{
	char * var, * fname;
	fetch2Args(block, &var, &fname);
	std::cout << "loading file: " << fname << std::endl; 
	//execute command
	fetchMatrix(var).loadFromFile(fname);
	return true;
	}

bool MatrixScriptInterpreter::cmdSave(ODBlock & block)
// save { var; fname; }			#save var to fname
	{
	char * var, * fname;
	fetch2Args(block, &var, &fname);
	std::cout << "saving file: " << fname << std::endl; 
	//execute command
	fetchMatrix(var).saveToFile(fname);
	return true;
	}

bool MatrixScriptInterpreter::cmdSet(ODBlock & block)
//	set { var; src; }				#set var to the matrix source
	{
	char * var, * src;
	fetch2Args(block, &var, &src);
	Matrix & vm = fetchMatrix(var);
	Matrix & sm = fetchMatrix(src);
	beginCmd();
	vm.copy(sm);
	endCmd(vm, block);
	return true;
	}

bool MatrixScriptInterpreter::cmdRandPD(ODBlock & block)
//  randPD { M; d; q; size; }						# creates a random problem for the LCP above.
	{
	char * cM, * cd, *cq, *csize;
	fetch4Args(block, &cM, &cd, &cq, &csize);
	Matrix & M = fetchMatrix(cM);
	Matrix & d = fetchMatrix(cd);
	Matrix & q = fetchMatrix(cq);
	unsigned size;		
	sscanf(csize, "%d", &size);

	M.randPD(d,q,size);

	std::cout << "generated PD matrix problem of size " << M.getNRows() << std::endl; 
	return true;
	}

bool MatrixScriptInterpreter::cmdZeros(ODBlock & block)
//	zeros { var; nrows; ncols; }	#set var to the nrows*ncols zero matrix
	{
	char * var, * snrows, *sncols;
	unsigned r = 0, c = 0;
	fetch3Args(block, &var, &snrows, &sncols);
	sscanf(snrows, "%d", &r);
	sscanf(sncols, "%d", &c);
	if (r > 0xffff || c > 0xffff)
		{
		std::cout << "matrix size of " << r << "x" << c << " seems weird." << std::endl; 
		throw 0;
		}

	fetchMatrix(var).zeros(r,c);
	return true;
	}

bool MatrixScriptInterpreter::cmdIdentity(ODBlock & block)
//	identity { var; n; }			#set var to the n*n identity matrix
	{
	char * var, * sn;
	fetch2Args(block, &var, &sn);
	unsigned n;
	sscanf(sn, "%d", &n);
	if (n > 0xffff )
		{
		std::cout << "matrix size of " << n << "x" << n << " seems weird." << std::endl; 
		throw 0;
		}
	fetchMatrix(var).identity(n);
	return true;
	}

bool MatrixScriptInterpreter::cmdAdd(ODBlock & block)
//	add	{ var; src1; src2; }		#set var to the matrix src1 + src2
	{
	char * var, * src1, *src2;
	fetch3Args(block, &var, &src1, &src2);
	Matrix & vm = fetchMatrix(var);
	Matrix & s1 = fetchMatrix(src1);
	Matrix & s2 = fetchMatrix(src2);
	beginCmd();
	vm.add(s1, s2);
	endCmd(vm, block);
	return true;
	}

/**
This is primarily a debugging call. You write dot products into a vector destination with multiply_at(src1, src2)
*/
bool MatrixScriptInterpreter::cmdDot(ODBlock & block)
// dot { src1; src2; }		#display src1 . src2
	{
	char * src1, *src2;
	fetch2Args(block, &src1, &src2);
	float d = fetchMatrix(src1).dot(fetchMatrix(src2));
	std::cout << "dot product: " << d << std::endl;
	return true;
	}

bool MatrixScriptInterpreter::cmdMad(ODBlock & block)
// dot { val; src1; src2; const_color; }		# val = src1 + src2 * const_color
	{
	char * var, * src1, *src2, * constColor;
	fetch4Args(block, &var, &src1, &src2, &constColor);
	float scalar;
	sscanf(constColor, "%f", &scalar);
	fetchMatrix(var).mad(fetchMatrix(src1),fetchMatrix(src2), scalar);
	return true;
	}



bool MatrixScriptInterpreter::cmdSubtract(ODBlock & block)
	{
	//TODO
	return true;
	}

bool MatrixScriptInterpreter::cmdMultiply(ODBlock & block)
//	multiply { var; src1; src2; }	#set var to the matrix src1 * src2
	{
	char * var, * src1, *src2;
	fetch3Args(block, &var, &src1, &src2);
	Matrix & vm = fetchMatrix(var);
	Matrix & s1 = fetchMatrix(src1);
	Matrix & s2 = fetchMatrix(src2);
	beginCmd();
	vm.multiply(s1, s2);
	endCmd(vm, block);
	return true;
	}

bool MatrixScriptInterpreter::cmdMultiplyAT(ODBlock & block)
//	multiply { var; src1; src2; }	#set var to the matrix src1' * src2
	{
	char * var, * src1, *src2;
	fetch3Args(block, &var, &src1, &src2);
	Matrix & vm = fetchMatrix(var);
	Matrix & s1 = fetchMatrix(src1);
	Matrix & s2 = fetchMatrix(src2);
	beginCmd();
	vm.multiplyAT(s1,s2);
	endCmd(vm, block);
	return true;
	}

bool MatrixScriptInterpreter::cmdMultiplyScalar(ODBlock & block)
//	multiply { var; const_scalar; }	#set var to the matrix src1' * src2
	{
	char * var, * const_scalar;
	fetch2Args(block, &var, &const_scalar);
	float scalar;
	sscanf(const_scalar, "%f", &scalar);
	fetchMatrix(var).multiply(scalar);
	return true;
	}



bool MatrixScriptInterpreter::cmdTranspose(ODBlock &)
	{
	//TODO
	return true;
	}



bool MatrixScriptInterpreter::cmdConjugateGradients(ODBlock & block)
//	conjugateGradients { destVec; srcMx; srcVec; }	#set destMx so that:  srcMx*destVec = srcVec  holds.
//  destVec will be warmstarted from, unless its empty, so the caller may want to zero it first.
	{
	char * var, * src1, *src2;
	fetch3Args(block, &var, &src1, &src2);
	Matrix & destVec = fetchMatrix(var);
	Matrix & srcMx   = fetchMatrix(src1);
	Matrix & srcVec  = fetchMatrix(src2);

	//aliases:	  Mx = b
	Matrix & x = destVec;
	Matrix & M = srcMx;
	Matrix & b = srcVec;

	unsigned n = b.getNRows();

	if (M.getNRows() != n			//matrix - vector size agrees
		|| n < 1					//finite size
		|| M.getNCols() != n		//square matrix
		||  b.getNCols() != 1)
		{
		throw "conjugateGradients: matrix dimensions don't agree.";
		}

	beginCmd();

	//zero x if its the wrong size, else warmstart.
	if (x.getNRows() != n || x.getNCols() != 1)
		x.zeros(n, 1);		


	//use three temporary vectors:

	Matrix p("p"), r("r"), s("s");


	p.copy(b);
	r.copy(b);

	//1 x loop body for k = 1.
	float rr = r.normSquared();


	s.multiply(M,p);	//s = M*p

	float t = p.dot(s);

	float alpha = rr / t;	//scalar op


	x.mad(p, alpha); //x += p * alpha;		//MAD operation with scalar

	float rrnew = rr;	//scalar op

	unsigned iter = n;	//problem size, use smaller number for approximate solution.

	for (unsigned k = 2; k<=iter; k++)
		{
		r.mad(s, -alpha);		//r += s * (-alpha);		//1 O(n)
		rr = rrnew;				//scalar op
		rrnew = r.normSquared(); //1 O(n)   

		float beta = rrnew / rr;	//scalar op

		p.mad(r, p, beta);		//p = r + p * beta;		//1 O(n)	//argh! if 2nd arg == dest, it takes 2x as long!
    
		//body    
		s.multiply(M,p);		//1 O(n^2)

		t = p.dot(s);			//1 O(n)

		alpha = rrnew / t;		//scalar op

		x.mad(p, alpha);		//x += p * alpha;			//1 O(n)
		}

	endCmd(destVec, block);
	return true;
	}

bool MatrixScriptInterpreter::cmdLcpProjectedJacobi(ODBlock & block)
//	lcpProjectedJacobi { z; M; d; q; iters; }	#set z so that:  
//  z >= 0,  M*z + q >= 0, and z'(M*z + q) = 0  holds.
//  z will be warmstarted from, unless its empty, so the caller may want to zero it first.
//  d should be set to omega ./ diag(M), where diag(M) is the vector with the diagonal elements
// of M, ./ is array division, and omega is a small negative magic number that steers convergence.
// something like -0.85 works ok for omega in most cases.  Advanced versions of the algo don't need
// to be provided with an omega.
// uses the projected jacobi method
// does not include a termination criterion, nor a line search
// M must be symmetric PSD.
	{
	char * var, * src1, *src2, *src3, *siters;
	fetch5Args(block, &var, &src1, &src2, &src3, &siters);
	Matrix & z = fetchMatrix(var);
	Matrix & M   = fetchMatrix(src1);
	Matrix & d  = fetchMatrix(src2);
	Matrix & q  = fetchMatrix(src3);
	unsigned iter;		//number of iterations
	sscanf(siters, "%d", &iter);

	unsigned n = q.getNRows();

	if (M.getNRows() != n			//matrix - vector size agrees
		|| n < 1					//finite size
		|| M.getNCols() != n		//square matrix
		|| q.getNCols() != 1

		|| d.getNCols() != 1
		|| d.getNRows() != n
		)
		{
		throw "conjugateGradients: matrix dimensions don't agree.";
		}

	//working variables:
	Matrix w("w"), t("w2");

	beginCmd();

	//zero z if its the wrong size, else warmstart.
	if (z.getNRows() != n || z.getNCols() != 1)
		{
		z.zeros(n, 1);		
		w.copy(z);
		}
	else
		w.multiply(M, z);



	//t = z+ (q + w) .* d
	t.madad(z, q, w, d);
	//z = clamp(t)
	z.max(t, 0);
	


	for (unsigned k = 1; k<iter; k++)		//do 2 iters in one loop!
		{
		//z = max(z + (q + M * z) .* d, zeros(10, 1))
		w.multiply(M, z);	//TODO: unless we're warmstarting this is redundant the 1st time around.

		//t = z+ (q + w) .* d
		t.madad(z, q, w, d);
		//z = clamp(t)
		z.max(t, 0);
		}

	endCmd(z, block);
	return true;
	}