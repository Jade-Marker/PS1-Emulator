#pragma once

#include <vector>
#include "Component.hpp"

class ComponentManager
{
private:
    std::vector<Component*> _components;
    std::vector<uint32> _ticksRequired;
    std::vector<uint32> _tickCount;
    uint32 _tickFrequency;
    
    void CalculateTickFrequency();
    
public:
    
    void Tick();
    
    void AddComponent(Component* component);
};

