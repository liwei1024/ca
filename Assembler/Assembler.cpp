#include "Assembler.h"


bool assembler::assembler(XEDPARSE *parse)
{
	return (bool)XEDParseAssemble(parse);
}

bool assembler::autoAssembler(char * instr,bool x64)
{

}