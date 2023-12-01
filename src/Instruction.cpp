#include "Instruction.hpp"

Instruction::Instruction(uint32 integer)
{
	asInteger = integer;

	if (asRtype.op == 0)
	{
		instructionType = InstructionType::RType;
		return;
	}
}