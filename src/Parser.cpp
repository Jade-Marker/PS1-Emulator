#include "Parser.hpp"

Parser::Parser(Memory* pMemory, uint32 programCounter):
	_pMemory(pMemory), _programCounter(programCounter)
{
}

void Parser::Parse()
{
	Instruction instruction = Instruction(_pMemory->ReadInt(_programCounter));

	_programCounter += 4;
}

Instruction::Instruction(uint32 integer)
{
	asInteger = integer;

	if (asRtype.op == 0)
	{
		instructionType = InstructionType::RType;
		return;
	}
}
