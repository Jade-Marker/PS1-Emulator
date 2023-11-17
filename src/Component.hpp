#pragma once

#include "Types.hpp"

class Component
{
private:
    uint32 _frequency;

public:
    Component(uint32 frequency);
    
    uint32 GetFrequency();
    
    virtual void Cycle();
};

