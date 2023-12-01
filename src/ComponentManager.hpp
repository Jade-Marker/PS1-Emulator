#pragma once

#include <vector>
#include "Component.hpp"

class ComponentManager
{
private:
    std::vector<Component*> _components;
    std::vector<uint64> _ticksRequired;
    std::vector<uint64> _tickCount;
    uint64 _tickFrequency;
    
    void CalculateTickFrequency();
    
public:
    
    void Tick();
    
    void AddComponent(Component* component);
};

