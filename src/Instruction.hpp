#pragma once
#include "Types.hpp"

struct RTypeInstruction
{
	uint32 funct : 6;
	uint32 shamt : 5;
	uint32 rd : 5;
	uint32 rt : 5;
	uint32 rs : 5;
	uint32 op : 6;
};

struct ITypeInstruction
{
	uint32 immediate : 16;
	uint32 rt : 5;
	uint32 rs : 5;
	uint32 op : 6;
};

struct JTypeInstruction
{
	uint32 target : 26;
	uint32 op : 6;
};

enum class InstructionType
{
	RType,
	IType,
	JType
};

enum class MipsInstruction
{
	INSTRUCTION_SPECIAL = 0,
	INSTRUCTION_BCOND,
	INSTRUCTION_J,
	INSTRUCTION_JAL,
	INSTRUCTION_BEQ,
	INSTRUCTION_BNE,
	INSTRUCTION_BLEZ,
	INSTRUCTION_BGTZ,


};

struct Instruction
{
	union
	{
		uint32 asInteger;
		RTypeInstruction asRtype;
		ITypeInstruction asIType;
		JTypeInstruction asJType;
	};

	InstructionType instructionType;

	Instruction(uint32 integer);
};