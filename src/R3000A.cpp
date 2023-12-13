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
            case MipsInstruction::INSTRUCTION_LUI:
                break;
        }

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
