#include "Component.hpp"

Component::Component(uint64 frequency):
_frequency(frequency)
{
    
}

uint64 Component::GetFrequency()
{
    return _frequency;
}

void Component::Cycle()
{
}
