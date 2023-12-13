#include <map>
#include <stdexcept>
#include "Instruction.hpp"

std::map<MipsInstruction, Instruction> instructionsMap
{
	{MipsInstruction::INSTRUCTION_SPECIAL, Instruction((int)MipsInstruction::INSTRUCTION_SPECIAL, InstructionType::RType)},


	{MipsInstruction::INSTRUCTION_LUI, Instruction((int)MipsInstruction::INSTRUCTION_LUI, InstructionType::IType)},
};

Instruction::Instruction():
	asInteger(0), instructionType()
{
}

Instruction::Instruction(uint32 asInteger, InstructionType instructionType):
	asInteger(asInteger), instructionType(instructionType)
{

}

Instruction Instruction::GetInstruction(uint32 asInteger)
{
	Instruction instruction;
	instruction.asInteger = asInteger;

	if(instructionsMap.find(instruction.asIType.op) == instructionsMap.end())
		throw std::runtime_error("Instruction not found");

	instruction = instructionsMap[instruction.asIType.op];
	instruction.asInteger = asInteger;

	return instruction;
}
