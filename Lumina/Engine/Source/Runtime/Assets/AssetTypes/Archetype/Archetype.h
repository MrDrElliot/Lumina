#pragma once
#include "Core/Object/Object.h"
#include "Archetype.generated.h"

namespace Lumina
{
    LUM_CLASS()
    class LUMINA_API CArchetype : public CObject
    {
        GENERATED_BODY()
        
    public:

        bool IsAsset() const override { return true; }
    
    };
}
