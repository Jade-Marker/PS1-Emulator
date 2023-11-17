#include "Maths.hpp"

bool ElementsAreEqual(const std::vector<uint32>& vec)
{
    for(int i = 1; i < vec.size(); i++)
    {
        if(vec[i] != vec[i-1])
            return false;
    }
    
    return true;
}

int LowestIndex(const std::vector<uint32>& vec)
{
    int lowestIndex = 0;
    
    for(int i = 1; i < vec.size(); i++)
    {
        if(vec[i] < vec[lowestIndex])
            lowestIndex = i;
    }
    
    return lowestIndex;
}


uint32 LeastCommonMultiple(const std::vector<uint32>& vec)
{
    if(vec.size() == 1)
        return vec[0];
    else if (vec.size() == 0)
        return 0;
    
    std::vector<uint32> temp = std::vector<uint32>(vec);
    
    while(!ElementsAreEqual(temp))
    {
        int lowestIndex = LowestIndex(temp);
        
        temp[lowestIndex]+= vec[lowestIndex];
    }
    
    return temp[0];
}
