#pragma once

#include <array>

#include "Component.hpp"
#include "Memory.hpp"

class R3000A : public Component
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
            uint32 constantZero;  //constant zero

            uint32 assemblerReserved;  //Reserved for assembler

            uint32 values0;  //Values for results and expression evaluation
            uint32 values1;

            uint32 arg0;  //Arguments
            uint32 arg1;
            uint32 arg2;
            uint32 arg3;
            
            uint32 temp0;  //Temporaries (not preserved across call)
            uint32 temp1;
            uint32 temp2;
            uint32 temp3;
            uint32 temp4;
            uint32 temp5;
            uint32 temp6;
            uint32 temp7;

            uint32 saved0;  //Saved (preserved across call)
            uint32 saved1;
            uint32 saved2;
            uint32 saved3;
            uint32 saved4;
            uint32 saved5;
            uint32 saved6;
            uint32 saved7;

            uint32 temp8;  //Temporaries (not preserved across call)
            uint32 temp9;

            uint32 kernelReserved0;  //Reserved for OS Kernel
            uint32 kernelReserved1;

            uint32 globalPointer;  //Global pointer

            uint32 stackPointer;  //Stack pointer

            uint32 framePointer;  //Frame pointer

            uint32 returnAddress;  //Return address (set by function call)
        };
    };

public:
    R3000A(Memory* pMemory, uint32 programCounter);
        
    virtual void Cycle();

    void OutputCPUState();
};

