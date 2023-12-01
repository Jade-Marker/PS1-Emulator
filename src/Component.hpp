#pragma once

#include "Types.hpp"

class Component
{
private:
    uint64 _frequency;

public:
    Component(uint64 frequency);
    
    uint64 GetFrequency();
    
    virtual void Cycle();
};

