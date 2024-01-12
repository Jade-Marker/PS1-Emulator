#include "R3000A.hpp"
#include "Constants.hpp"
#include "Instruction.hpp"
#include <iostream>

#define str(x) #x

R3000A::R3000A(Memory* pMemory, uint32 programCounter):
    Component(cR3000AClockSpeedHz), _pMemory(pMemory), _programCounter(programCounter),
    _mulHigh(0), _mulLow(0), _registers(), _numCyclesTillFree(0)
{
    
}

void R3000A::Cycle()
{
    if (_numCyclesTillFree == 0)
    {
        std::cout << "CPU cycle" << std::endl;
        Instruction instruction = Instruction::GetInstruction(_pMemory->ReadInt(_programCounter));

        std::cout << "Instruction: " << std::hex << instruction.asInteger << '\n';

        switch (instruction.asIType.op)
        {
            case MipsInstruction::INSTRUCTION_JAL:
                _programCounter = (instruction.asJType.target << 2) + (_programCounter & 0xF0000000);
                _programCounter -= 4; //So that when it gets incremeneted after, it "just works" :3
                break;

            case MipsInstruction::INSTRUCTION_ADDI:
                _registers[instruction.asIType.rt] = (int32)_registers[instruction.asIType.rs] + (int32)instruction.asIType.immediate;
                break;

            case MipsInstruction::INSTRUCTION_ADDIU:
                _registers[instruction.asIType.rt] = _registers[instruction.asIType.rs] + instruction.asIType.immediate;
                break;

            case MipsInstruction::INSTRUCTION_LUI:
                _registers[instruction.asIType.rt] = (instruction.asIType.immediate << 16) & 0xFFFF0000;
                break;

            case MipsInstruction::INSTRUCTION_SW:
                _pMemory->WriteInt(globalPointer + instruction.asIType.immediate, _registers[instruction.asIType.rt]);
                break;
        }

        constantZero = 0;

        _programCounter += 4;
        OutputCPUState();
    }
    else
        _numCyclesTillFree--;
}

void R3000A::OutputCPUState()
{
    std::cout << "\n\t CPU STATE\n";
    std::cout << "pc: " << _programCounter << "\n";

    std::cout << str(_mulHigh) << ": " << _mulHigh << "\t"; std::cout << str(_mulLow) << ": " << _mulHigh << "\n";

    for (int i = 0; i < _registers.size(); i += 4)
    {
        std::cout << "R" << std::dec << i << ": " << std::hex << _registers[i] << "\t";
        std::cout << "R" << std::dec << i + 1 << ": " << std::hex << _registers[i + 1] << "\t";
        std::cout << "R" << std::dec << i + 2 << ": " << std::hex << _registers[i + 2] << "\t";
        std::cout << "R" << std::dec << i + 3 << ": " << std::hex << _registers[i + 3] << "\t\n";
    }

    std::cout << "\n\n";
}
