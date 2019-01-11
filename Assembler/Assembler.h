#pragma once
#ifndef _XEDPARSE_H
#include "../XDEParse/XEDParse.h"
#endif
namespace assembler
{
	bool assembler(XEDPARSE * parse);
	bool autoAssembler(char * instr, bool x64);
};

