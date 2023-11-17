#include "R3000A.hpp"

#include <iostream>

R3000A::R3000A():
    Component(1)
{
    
}

void R3000A::Cycle()
{
    std::cout << "CPU cycle" << std::endl;
}
