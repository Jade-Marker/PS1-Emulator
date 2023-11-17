#include "Component.hpp"

Component::Component(uint32 frequency):
_frequency(frequency)
{
    
}

uint32 Component::GetFrequency()
{
    return _frequency;
}

void Component::Cycle()
{
}
