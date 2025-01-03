#include "R3000A.hpp"
#include "Constants.hpp"
#include "Instruction.hpp"
#include <iostream>

#define str(x) #x

R3000A::R3000A(Memory* pMemory, uint32 programCounter, const std::array<uint32,32>& initRegisterValues):
    _pMemory(pMemory), _programCounter(programCounter),
    _mulHigh(0), _mulLow(0), _registers(initRegisterValues), _numCyclesTillFree(0)
{
    
}

void R3000A::Run()
{
    _shouldBreakout = false;

    while(!_shouldBreakout)
    {
        if (_numCyclesTillFree != 0)
        {
            _numCyclesTillFree--;
            continue;
        }

        //std::cout << "CPU cycle" << std::endl;
        Instruction instruction = Instruction::GetInstruction(_pMemory->ReadWord(_programCounter));
        _programCounter += 4;

        //std::cout << "Instruction: " << instruction.ToString() << " = 0x" << std::hex << instruction.asInteger << '\n';

        switch (instruction.asIType.op)
        {
            case MipsInstruction::INSTRUCTION_SPECIAL:
                switch (instruction.asRtype.funct)
                {
                case SpecialInstruction::INSTRUCTION_SLL:
                    _registers[instruction.asRtype.rd] = _registers[instruction.asRtype.rt] << instruction.asRtype.shamt;
                    break;

                case SpecialInstruction::INSTRUCTION_SRL:
                    _registers[instruction.asRtype.rd] = _registers[instruction.asRtype.rt] >> instruction.asRtype.shamt;
                    break;

                case SpecialInstruction::INSTRUCTION_SRA:
                    _registers[instruction.asRtype.rd] = (int32)_registers[instruction.asRtype.rt] >> instruction.asRtype.shamt;
                    break;

                case SpecialInstruction::INSTRUCTION_JR:
                    _programCounter = _registers[instruction.asRtype.rs];
                    break;

                case SpecialInstruction::INSTRUCTION_MFHI:
                    _registers[instruction.asRtype.rd] = _mulHigh;
                    break;

                case SpecialInstruction::INSTRUCTION_MFLO:
                    _registers[instruction.asRtype.rd] = _mulLow;
                    break;

                case SpecialInstruction::INSTRUCTION_MULT:
                    {
                        int64 temp = (int64)_registers[instruction.asRtype.rs] * (int64)_registers[instruction.asRtype.rt];
                        _mulHigh = temp >> 32;
                        _mulLow = temp;
                    }
                    break;

                case SpecialInstruction::INSTRUCTION_MULTU:
                {
                    uint64 temp = (uint64)_registers[instruction.asRtype.rs] * (uint64)_registers[instruction.asRtype.rt];
                    _mulHigh = temp >> 32;
                    _mulLow = temp;
                }
                break;

                case SpecialInstruction::INSTRUCTION_DIV:
                {
                    _mulLow = (int32)_registers[instruction.asRtype.rs] / (int32)_registers[instruction.asRtype.rt];
                    _mulHigh = (int32)_registers[instruction.asRtype.rs] % (int32)_registers[instruction.asRtype.rt];
                }
                    break;

                case SpecialInstruction::INSTRUCTION_ADDU:
                    _registers[instruction.asRtype.rd] = _registers[instruction.asRtype.rs] + _registers[instruction.asRtype.rt];
                    break;

                case SpecialInstruction::INSTRUCTION_SUBU:
                    _registers[instruction.asRtype.rd] = _registers[instruction.asRtype.rs] - _registers[instruction.asRtype.rt];
                    break;

                case SpecialInstruction::INSTRUCTION_OR:
                    _registers[instruction.asRtype.rd] = _registers[instruction.asRtype.rs] | _registers[instruction.asRtype.rt];
                    break;

                case SpecialInstruction::INSTRUCTION_AND:
                    _registers[instruction.asRtype.rd] = _registers[instruction.asRtype.rs] & _registers[instruction.asRtype.rt];
                    break;

                case SpecialInstruction::INSTRUCTION_SLT:
                    if ((int32)_registers[instruction.asRtype.rs] < (int32)_registers[instruction.asRtype.rt])
                        _registers[instruction.asRtype.rd] = 1;
                    else
                        _registers[instruction.asRtype.rd] = 0;
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

            case MipsInstruction::INSTRUCTION_BCOND:
                if (_registers[instruction.asIType.rs] >= 0)
                    _programCounter = _programCounter + (((int32)(int16)instruction.asIType.immediate) << 2);
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

            case MipsInstruction::INSTRUCTION_BLEZ:
                if (_registers[instruction.asIType.rs] <= 0)
                    _programCounter = _programCounter + (((int32)(int16)instruction.asIType.immediate) << 2);
                break;

            case MipsInstruction::INSTRUCTION_ADDIU:
                _registers[instruction.asIType.rt] = _registers[instruction.asIType.rs] + ((int16)instruction.asIType.immediate);
                break;

            case MipsInstruction::INSTRUCTION_SLTI:
                if ((int32)_registers[instruction.asIType.rs] < (int32)instruction.asIType.immediate)
                    _registers[instruction.asIType.rt] = 1;
                else
                    _registers[instruction.asIType.rt] = 0;
                break;

            case MipsInstruction::INSTRUCTION_SLTIU:
                if (_registers[instruction.asIType.rs] < instruction.asIType.immediate)
                    _registers[instruction.asIType.rt] = 1;
                else
                    _registers[instruction.asIType.rt] = 0;

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
            
            case MipsInstruction::INSTRUCTION_LH:
                _registers[instruction.asIType.rt] = (int16)(_pMemory->ReadHalfWord(_registers[instruction.asIType.rs] + ((int16)instruction.asIType.immediate)));
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

        //OutputCPUState();
    }
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

void R3000A::BreakoutOfLoop()
{
    _shouldBreakout = true;
}
