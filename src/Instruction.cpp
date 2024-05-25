#include <map>
#include <stdexcept>
#include "Instruction.hpp"

std::map<MipsInstruction, Instruction> instructionsMap
{
	{MipsInstruction::INSTRUCTION_SPECIAL, Instruction((int)MipsInstruction::INSTRUCTION_SPECIAL, InstructionType::RType, PipelineParts::INSTRUCT_FETCH)},

	{MipsInstruction::INSTRUCTION_J, Instruction((int)MipsInstruction::INSTRUCTION_J, InstructionType::JType, PipelineParts::INSTRUCT_FETCH)},
	{MipsInstruction::INSTRUCTION_JAL, Instruction((int)MipsInstruction::INSTRUCTION_JAL, InstructionType::JType, PipelineParts::INSTRUCT_FETCH)},
	{MipsInstruction::INSTRUCTION_BNE, Instruction((int)MipsInstruction::INSTRUCTION_BNE, InstructionType::JType, PipelineParts::INSTRUCT_FETCH)},

	{MipsInstruction::INSTRUCTION_ADDI, Instruction((int)MipsInstruction::INSTRUCTION_ADDI, InstructionType::IType, PipelineParts::INSTRUCT_FETCH)},
	{MipsInstruction::INSTRUCTION_ADDIU, Instruction((int)MipsInstruction::INSTRUCTION_ADDIU, InstructionType::IType, PipelineParts::INSTRUCT_FETCH)},
	{MipsInstruction::INSTRUCTION_ORI, Instruction((int)MipsInstruction::INSTRUCTION_ORI, InstructionType::IType, PipelineParts::INSTRUCT_FETCH)},

    {MipsInstruction::INSTRUCTION_LUI, Instruction((int)MipsInstruction::INSTRUCTION_LUI, InstructionType::IType, PipelineParts::INSTRUCT_FETCH)},
    {MipsInstruction::INSTRUCTION_COP0, Instruction((int)MipsInstruction::INSTRUCTION_COP0, InstructionType::RType, PipelineParts::INSTRUCT_FETCH)},
	{MipsInstruction::INSTRUCTION_SW, Instruction((int)MipsInstruction::INSTRUCTION_SW, InstructionType::IType, PipelineParts::INSTRUCT_FETCH)},

    //{MipsInstruction::INSTRUCTION_LUI, Instruction((int)MipsInstruction::INSTRUCTION_LUI, InstructionType::IType, PipelineParts::INSTRUCT_FETCH | PipelineParts::EXECUTE | PipelineParts::REGISTER_WRITE)},
};

Instruction::Instruction():
	asInteger(0), instructionType()
{
}

Instruction::Instruction(uint32 asInteger, InstructionType instructionType, PipelineParts pipelineUsed):
	asInteger(asInteger), instructionType(instructionType), pipelineUsed(pipelineUsed)
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
