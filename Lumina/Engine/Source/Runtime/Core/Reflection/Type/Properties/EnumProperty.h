#pragma once
#include "Core/Object/Class.h"
#include "Core/Object/ObjectPtr.h"
#include "Core/Reflection/Type/LuminaTypes.h"

namespace Lumina
{
    class FEnumProperty : public FProperty
    {
    public:

        FEnumProperty(FFieldOwner InOwner)
            :FProperty(InOwner)
        {}

        void SetEnum(CEnum* InEnum);

        /** Returns the pointer to the internal enum */
        FORCEINLINE CEnum* GetEnum() const { return Enum; }
        
    private:
        
        TObjectPtr<CEnum> Enum;
    
    };
}
