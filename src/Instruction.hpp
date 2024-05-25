#pragma once
#include "Types.hpp"

enum class InstructionType
{
	RType,
	IType,
	JType
};

enum class PipelineParts
{
    INSTRUCT_FETCH = 1,
    REGISTER_READ = 2,
    EXECUTE = 4,
    MEMORY_ACCESS = 8,
    REGISTER_WRITE = 16,
};

enum class MipsInstruction : uint32
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

enum class SpecialInstruction : uint32
{
	INSTRUCTION_SLL = 0,

	INSTRUCTION_SRL = 2,
	INSTRUCTION_SRA,
	INSTRUCTION_SLLV,

	INSTRUCTION_SRLV = 6,
	INSTRUCTION_SRAV,
	INSTRUCTION_JR,
	INSTRUCTION_JALR,

	INSTRUCTION_SYSCALL = 12,
	INSTRUCTION_BREAK,

	INSTRUCTION_MFHI = 16,
	INSTRUCTION_MTHI,
	INSTRUCTION_MFLO,
	INSTRUCTION_MTLO,

	INSTRUCTION_MULT = 24,
	INSTRUCTION_MULTU,
	INSTRUCTION_DIV,
	INSTRUCTION_DIVU,

	INSTRUCTION_ADD = 32,
	INSTRUCTION_ADDU,
	INSTRUCTION_SUB,
	INSTRUCTION_SUBU,
	INSTRUCTION_AND,
	INSTRUCTION_OR,
	INSTRUCTION_XOR,
	INSTRUCTION_NOR,

	INSTRUCTION_SLT = 42,
	INSTRUCTION_SLTU
};

struct RTypeInstruction
{
	SpecialInstruction funct : 6;
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
    PipelineParts pipelineUsed;

	Instruction();
	Instruction(uint32 asInteger, InstructionType instructionType, PipelineParts pipelineUsed);

	static Instruction GetInstruction(uint32 asInteger);
};
