#pragma once
#include "Types.hpp"
#include "Memory.hpp"

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

class Parser
{
private:
	Memory* _pMemory;
	uint32 _programCounter;

public:
	Parser(Memory* pMemory, uint32 programCounter);

	void Parse();
};

