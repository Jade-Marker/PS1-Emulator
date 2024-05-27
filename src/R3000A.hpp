#pragma once

#include <array>

#include "Memory.hpp"

class R3000A
{
private:
    Memory* _pMemory;
    uint32 _numCyclesTillFree;

    uint32 _programCounter;
    union
    {
        uint64 _mulHigh; // Multiplication 64 bit high result or division remainder
        uint64 _divRemainder;
    };
    union
    {
        uint64 _mulLow; //Multiplication 64 bit low result or division quotient
        uint64 _divQuotient;
    };

    union {
        std::array<uint32, 32> _registers;

        struct
        {
            uint32 _constantZero;  //constant zero

            uint32 _assemblerReserved;  //Reserved for assembler

            uint32 _values0;  //Values for results and expression evaluation
            uint32 _values1;

            uint32 _arg0;  //Arguments
            uint32 _arg1;
            uint32 _arg2;
            uint32 _arg3;
            
            uint32 _temp0;  //Temporaries (not preserved across call)
            uint32 _temp1;
            uint32 _temp2;
            uint32 _temp3;
            uint32 _temp4;
            uint32 _temp5;
            uint32 _temp6;
            uint32 _temp7;

            uint32 _saved0;  //Saved (preserved across call)
            uint32 _saved1;
            uint32 _saved2;
            uint32 _saved3;
            uint32 _saved4;
            uint32 _saved5;
            uint32 _saved6;
            uint32 _saved7;

            uint32 _temp8;  //Temporaries (not preserved across call)
            uint32 _temp9;

            uint32 _kernelReserved0;  //Reserved for OS Kernel
            uint32 _kernelReserved1;

            uint32 _globalPointer;  //Global pointer

            uint32 _stackPointer;  //Stack pointer

            uint32 _framePointer;  //Frame pointer

            uint32 _returnAddress;  //Return address (set by function call)
        };
    };

public:
    R3000A(Memory* pMemory, uint32 programCounter, const std::array<uint32, 32>& initRegisterValues);
        
    void RunNCycles(uint32 numCycles);

    void OutputCPUState();
};

