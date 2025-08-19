#pragma once

#include <string>
#include <EASTL/string.h>
#include "EASTL/vector.h"
#include "Reflector/Utils/MetadataUtils.h"

namespace Lumina::Reflection
{
    class FReflectedType;
}

namespace Lumina
{
    class FReflectedProperty
    {
    public:

        virtual void AppendDefinition(eastl::string& Stream) const = 0;
        void AppendPropertyDef(eastl::string& Stream, const char* PropertyFlags, const char* TypeFlags, const eastl::string& CustomData = "") const;
        
        virtual const char* GetPropertyParamType() const { return "FPropertyParams"; }

        virtual const char* GetTypeName() = 0;
        eastl::string GetDisplayName() const { return Name; }
        void GenerateMetadata(const eastl::string& InMetadata);

        virtual void DeclareCrossModuleReference(const eastl::string& API, eastl::string& Stream) { }

        virtual bool HasAccessors();
        virtual bool DeclareAccessors(eastl::string& Stream, const eastl::string& FileID);
        virtual bool DefineAccessors(eastl::string& Stream, Reflection::FReflectedType* ReflectedType);

        eastl::string                   RawTypeName;
        eastl::string                   TypeName;
        eastl::string                   Namespace;
        eastl::string                   Name;
        eastl::string                   Outer;
        bool                            bInner = false;
        eastl::vector<FMetadataPair>    Metadata;

        eastl::string                   GetterFunc;
        eastl::string                   SetterFunc;
    };
}
