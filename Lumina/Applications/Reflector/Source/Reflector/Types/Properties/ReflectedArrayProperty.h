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
        bool HasAccessors() override;
        bool DeclareAccessors(eastl::string& Stream, const eastl::string& FileID) override;
        bool DefineAccessors(eastl::string& Stream, Reflection::FReflectedType* ReflectedType) override;

        eastl::string ElementTypeName;
    };
}
