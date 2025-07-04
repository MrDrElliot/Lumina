﻿#pragma once

#include "Module/API.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    class CObjectBase;
}

namespace Lumina
{
    class FCObjectAllocator
    {
    public:
        
        FCObjectAllocator()
        {
            
        }
        
        LUMINA_API CObjectBase* AllocateCObject(int32 Size, int32 Alignment);
        LUMINA_API void FreeCObject(CObjectBase* Ptr);

    private:
        
    };

    extern LUMINA_API FCObjectAllocator GUObjectAllocator;

}
