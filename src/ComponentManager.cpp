#include "ComponentManager.hpp"
#include "Maths.hpp"

void ComponentManager::CalculateTickFrequency()
{
    //we want to find the rate at which we can tick and not miss any updates for anything
    //so given a set of frequencies, it should be the lowest common multiple
    std::vector<uint64> frequencies;
    frequencies.resize(_components.size());
    
    for(int i = 0; i < _components.size(); i++)
        frequencies[i] = _components[i]->GetFrequency();
    
    _tickFrequency = LeastCommonMultiple(frequencies);
    
    //now we known the least common multiple, we need to know how many ticks each component takes to update
    _ticksRequired.resize(_components.size());
    _tickCount.resize(_components.size());
    for(int i = 0; i < _components.size(); i++)
    {
        _tickCount[i] = 0;
        _ticksRequired[i] = _tickFrequency / _components[i]->GetFrequency();
    }
}

void ComponentManager::Tick()
{
    for(int i = 0; i < _components.size(); i++)
    {
        _tickCount[i]++;
        
        if(_tickCount[i] == _ticksRequired[i])
            
        {
            _tickCount[i] = 0;
            _components[i]->Cycle();
        }
    }
}

void ComponentManager::AddComponent(Component *component)
{
    _components.push_back(component);
    CalculateTickFrequency();
}

void ComponentManager::RunNTicks(int numTicks)
{
    for (int i = 0; i < numTicks; i++)
        Tick();
}