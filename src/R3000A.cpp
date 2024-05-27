#include "R3000A.hpp"
#include "Constants.hpp"
#include "Instruction.hpp"
#include <iostream>

#define str(x) #x

R3000A::R3000A(Memory* pMemory, uint32 programCounter, const std::array<uint32,32>& initRegisterValues):
    Component(cR3000AClockSpeedHz), _pMemory(pMemory), _programCounter(programCounter),
    _mulHigh(0), _mulLow(0), _registers(initRegisterValues), _numCyclesTillFree(0)
{
    
}

void R3000A::Cycle()
{
    if (_numCyclesTillFree == 0)
    {
        std::cout << "CPU cycle" << std::endl;
        Instruction instruction = Instruction::GetInstruction(_pMemory->ReadWord(_programCounter));
        _programCounter += 4;

        std::cout << "Instruction: " << instruction.ToString() << " = 0x" << std::hex << instruction.asInteger << '\n';

        switch (instruction.asIType.op)
        {
            case MipsInstruction::INSTRUCTION_SPECIAL:
                switch (instruction.asRtype.funct)
                {
                    case SpecialInstruction::INSTRUCTION_SLL:
                        _registers[instruction.asRtype.rd] = _registers[instruction.asRtype.rt] << instruction.asRtype.shamt;
                        break;

                    case SpecialInstruction::INSTRUCTION_JR:
                        _programCounter = _registers[instruction.asRtype.rs];
                        break;

                    case SpecialInstruction::INSTRUCTION_ADDU:
                        _registers[instruction.asRtype.rd] = _registers[instruction.asRtype.rs] + _registers[instruction.asRtype.rt];
                        break;

                    case SpecialInstruction::INSTRUCTION_OR:
                        _registers[instruction.asRtype.rd] = _registers[instruction.asRtype.rs] | _registers[instruction.asRtype.rt];
                        break;

                    case SpecialInstruction::INSTRUCTION_AND:
                        _registers[instruction.asRtype.rd] = _registers[instruction.asRtype.rs] & _registers[instruction.asRtype.rt];
                        break;

                    case SpecialInstruction::INSTRUCTION_SLTU:
                        if (_registers[instruction.asRtype.rs] < _registers[instruction.asRtype.rt])
                            _registers[instruction.asRtype.rd] = 1;
                        else
                            _registers[instruction.asRtype.rd] = 0;

                        break;

                    default:
                        throw std::runtime_error("Instruction not implemented");
                        break;
                }
            break;

            case MipsInstruction::INSTRUCTION_J:
                _programCounter = (instruction.asJType.target << 2) + (_programCounter & 0xF0000000);
                break;

            case MipsInstruction::INSTRUCTION_JAL:
                _returnAddress = _programCounter + 4;

                _programCounter = (instruction.asJType.target << 2) + (_programCounter & 0xF0000000);
                break;

            case MipsInstruction::INSTRUCTION_BEQ:
                if (_registers[instruction.asIType.rs] == _registers[instruction.asIType.rt])
                    _programCounter = _programCounter + (((int32)(int16)instruction.asIType.immediate) << 2);
                break;
            case MipsInstruction::INSTRUCTION_BNE:
                if (_registers[instruction.asIType.rs] != _registers[instruction.asIType.rt])
                    _programCounter = _programCounter + (((int32)(int16)instruction.asIType.immediate) << 2);
                break;

            case MipsInstruction::INSTRUCTION_ADDIU:
                _registers[instruction.asIType.rt] = _registers[instruction.asIType.rs] + ((int16)instruction.asIType.immediate);
                break;

            case MipsInstruction::INSTRUCTION_ANDI:
                _registers[instruction.asIType.rt] = _registers[instruction.asIType.rs] & instruction.asIType.immediate;
                break;

            case MipsInstruction::INSTRUCTION_ORI:
                _registers[instruction.asIType.rt] = _registers[instruction.asIType.rs] | instruction.asIType.immediate;
                break;

            case MipsInstruction::INSTRUCTION_LUI:
                _registers[instruction.asIType.rt] = (instruction.asIType.immediate << 16) & 0xFFFF0000;
                break;
            
            case MipsInstruction::INSTRUCTION_LW:
                _registers[instruction.asIType.rt] = _pMemory->ReadWord(_registers[instruction.asIType.rs] + ((int16)instruction.asIType.immediate));
                break;

            case MipsInstruction::INSTRUCTION_LBU:
                _registers[instruction.asIType.rt] = (uint8)(_pMemory->ReadByte(_registers[instruction.asIType.rs] + ((int16)instruction.asIType.immediate)));
                break;

            case MipsInstruction::INSTRUCTION_LHU:
                _registers[instruction.asIType.rt] = (uint16)(_pMemory->ReadHalfWord(_registers[instruction.asIType.rs] + ((int16)instruction.asIType.immediate)));
                break;

            case MipsInstruction::INSTRUCTION_SB:
                _pMemory->WriteByte(_registers[instruction.asIType.rs] + ((int16)instruction.asIType.immediate), _registers[instruction.asIType.rt]);
                break;

            case MipsInstruction::INSTRUCTION_SH:
                _pMemory->WriteHalfword(_registers[instruction.asIType.rs] + ((int16)instruction.asIType.immediate), _registers[instruction.asIType.rt]);
                break;

            case MipsInstruction::INSTRUCTION_SW:
                _pMemory->WriteWord(_registers[instruction.asIType.rs] + ((int16)instruction.asIType.immediate), _registers[instruction.asIType.rt]);
                break;

            default:
                throw std::runtime_error("Instruction not implemented");
                break;
        }

        _constantZero = 0;

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
