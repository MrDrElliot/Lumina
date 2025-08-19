#include "ReflectedProperty.h"

#include <iostream>

#include "Reflector/Types/ReflectedType.h"
#include "Reflector/Utils/MetadataUtils.h"

namespace Lumina
{
    void FReflectedProperty::AppendPropertyDef(eastl::string& Stream, const char* PropertyFlags, const char* TypeFlags, const eastl::string& CustomData) const
    {
        eastl::string GetterFunctionName = GetterFunc.empty() ? "nullptr" : Outer + "::" + GetterFunc + "_WrapperImpl";
        eastl::string SetterFunctionName = SetterFunc.empty() ? "nullptr" : Outer + + "::" + SetterFunc + "_WrapperImpl";

        Stream += "{ \"" + Name + "\", " + PropertyFlags + ", " + TypeFlags + ", " + SetterFunctionName + ", " + GetterFunctionName + ", ";

        if (bInner)
        {
            Stream += "0";
        }
        else
        {
            Stream += "offsetof(" + Outer + ", " + Name + ")";
        }

        if (!CustomData.empty())
        {
            Stream += ", " + CustomData;
        }

        if (!Metadata.empty())
        {
            Stream += ", METADATA_PARAMS(std::size(" + Name + "_Metadata), " + Name + "_Metadata)";
        }

        Stream += " };\n";
    }

    void FReflectedProperty::GenerateMetadata(const eastl::string& InMetadata)
    {
        if (InMetadata.empty())
            return;
        
        FMetadataParser Parser(InMetadata);
        Metadata = eastl::move(Parser.Metadata);

        for (const FMetadataPair& MetadataPair : Metadata)
        {
            if (MetadataPair.Key == "Getter")
            {
                if (MetadataPair.Value.empty())
                {
                    GetterFunc = "Get" + Name;
                }
                else
                {
                    GetterFunc = MetadataPair.Value;
                }
            }

            if (MetadataPair.Key == "Setter")
            {
                if (MetadataPair.Value.empty())
                {
                    SetterFunc = "Set" + Name;
                }
                else
                {
                    SetterFunc = MetadataPair.Value;
                }
            }
        }
    }

    bool FReflectedProperty::HasAccessors()
    {
        return !GetterFunc.empty() || !SetterFunc.empty();
    }

    bool FReflectedProperty::DeclareAccessors(eastl::string& Stream, const eastl::string& FileID)
    {
        if (!GetterFunc.empty())
        {
            Stream += "static void " + GetterFunc + "_WrapperImpl(const void* Object, void* OutValue); \\\n";
        }

        if (!SetterFunc.empty())
        {
            Stream += "static void " + SetterFunc + "_WrapperImpl(void* Object, const void* InValue); \\\n";
        }
        
        return HasAccessors();
    }

    bool FReflectedProperty::DefineAccessors(eastl::string& Stream, Reflection::FReflectedType* ReflectedType)
    {
        if (!GetterFunc.empty())
        {
            Stream += "void " + ReflectedType->QualifiedName + "::" + GetterFunc + "_WrapperImpl(const void* Object, void* OutValue)\n";
            Stream += "{\n";
            Stream += "\tconst " + ReflectedType->DisplayName + "* Obj = (const " + ReflectedType->DisplayName + "*)Object;\n";
            Stream += "\t" + RawTypeName + "& Result = *(" + RawTypeName + "*)OutValue;\n";
            Stream += "\tResult = (" + RawTypeName + ")Obj->" + GetterFunc + "();\n";
            Stream += "}\n";
        }

        if (!SetterFunc.empty())
        {
            Stream += "void " + ReflectedType->QualifiedName + "::" + SetterFunc + "_WrapperImpl(void* Object, const void* InValue)\n";
            Stream += "{\n";
            Stream += "\t" + ReflectedType->QualifiedName + "* " + "Obj = (" + ReflectedType->QualifiedName + "*)Object;\n";
            Stream += "\tconst " + RawTypeName + "& Value = *(const " + RawTypeName + "*)InValue;\n";
            Stream += "\tObj->" + SetterFunc + "(Value);\n";
            Stream += "}\n";
        }

        return true;
    }
}
