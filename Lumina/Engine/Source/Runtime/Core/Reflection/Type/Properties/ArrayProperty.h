#pragma once
#include "Core/Reflection/Type/LuminaTypes.h"

namespace Lumina
{
    class FArrayProperty : public FProperty
    {
    public:

        
        FArrayProperty(FFieldOwner InOwner, FPropertyParams* Params = nullptr)
            :FProperty(InOwner, Params)
        {
        }

        ~FArrayProperty() override;

        void AddProperty(FProperty* Property) override { Inner = Property; }

        FProperty* GetInternalProperty() const { return Inner; }
    
    private:

        FProperty* Inner = nullptr;
    
    };
}
