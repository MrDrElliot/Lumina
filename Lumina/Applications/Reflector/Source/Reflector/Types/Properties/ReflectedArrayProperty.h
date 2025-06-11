#pragma once
#include "ReflectedProperty.h"
#include "EASTL/vector.h"

namespace Lumina
{
    class FReflectedArrayProperty : public FReflectedProperty
    {
    public:

        const char* GetPropertyParamType() const override { return "FArrayPropertyParams"; }
        void AppendDefinition(eastl::string& Stream) const override;
        const char* GetTypeName() override { return nullptr; }
        
    };
}
