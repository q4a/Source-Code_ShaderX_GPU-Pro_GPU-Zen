#include "Dx9ShaderDisasm.h"
#include "StdTypes.h"

#include <StdIO.h>
#include <IO.h>
#include <FCNTL.H>

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf(
			"DisAsm File.vso\r\n"
			"DisAsm File.pso\r\n");
		return 1;
	}
	int	File = _open(argv[1], _O_BINARY|_O_RDONLY);
	if (File==-1) {
		printf("Can't open \"%s\"", argv[1]);
		return 1;
	}
	int			Size = _filelength(File);
	Natural		cToken = Size/4;
	Nat32*		Tokens = new Nat32[cToken];
	_read(File, Tokens, cToken*4);
	CDx9ShaderDisasm::StrNewLine = "\n";
	printf(CDx9ShaderDisasm::DisAssemble(Tokens, cToken));
	delete [] Tokens;
	_close(File);
	return 0;
}