#pragma once
#include "ReflectedProperty.h"


namespace Lumina
{
    class FReflectedEnumProperty : public FReflectedProperty
    {
    public:
        
        const char* GetTypeName() override;
        void AppendDefinition(std::stringstream& SS) const override;

    };
}
