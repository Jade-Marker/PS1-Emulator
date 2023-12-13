#pragma once
#include "Types.hpp"

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
	INSTRUCTION_ADDI,
	INSTRUCTION_ADDIU,
	INSTRUCTION_SLTI,
	INSTRUCTION_SLTIU,
	INSTRUCTION_ANDI,
	INSTRUCTION_ORI,
	INSTRUCTION_XORI,
	INSTRUCTION_LUI,
	INSTRUCTION_COP0,
	INSTRUCTION_COP1,
	INSTRUCTION_COP2,
	INSTRUCTION_COP3,

	INSTRUCTION_LB = 32,
	INSTRUCTION_LH,
	INSTRUCTION_LWL,
	INSTRUCTION_LW,
	INSTRUCTION_LBU,
	INSTRUCTION_LHU,
	INSTRUCTION_LWR,

	INSTRUCTION_SB = 40,
	INSTRUCTION_SH,
	INSTRUCTION_SWL,
	INSTRUCTION_SW,

	INSTRUCTION_SWR = 46,

	INSTRUCTION_LWC0 = 48,
	INSTRUCTION_LWC1,
	INSTRUCTION_LWC2,
	INSTRUCTION_LWC3,

	INSTRUCTION_SWC0 = 56,
	INSTRUCTION_SWC1,
	INSTRUCTION_SWC2,
	INSTRUCTION_SWC3,
};

struct RTypeInstruction
{
	uint32 funct : 6;
	uint32 shamt : 5;
	uint32 rd : 5;
	uint32 rt : 5;
	uint32 rs : 5;
	MipsInstruction op : 6;
};

struct ITypeInstruction
{
	uint32 immediate : 16;
	uint32 rt : 5;
	uint32 rs : 5;
	MipsInstruction op : 6;
};

struct JTypeInstruction
{
	uint32 target : 26;
	MipsInstruction op : 6;
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

	Instruction();
	Instruction(uint32 asInteger, InstructionType instructionType);

	static Instruction GetInstruction(uint32 asInteger);
};