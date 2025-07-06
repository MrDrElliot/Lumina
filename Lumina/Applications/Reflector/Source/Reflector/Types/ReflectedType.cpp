#include "ReflectedType.h"

#include <iostream>

#include "Properties/ReflectedProperty.h"


namespace Lumina::Reflection
{
    void FReflectedType::GenerateMetadata(const eastl::string& InMetadata)
    {
        FMetadataParser Parser(InMetadata);
        Metadata = eastl::move(Parser.Metadata);
    }

    void FReflectedEnum::DefineConstructionStatics(eastl::string& Stream)
    {
    }

    void FReflectedEnum::DefineInitialHeader(eastl::string& Stream, const eastl::string& FileID)
    {
        Stream += "enum class " + DisplayName + " : uint8;\n";
        Stream += "template<> Lumina::CEnum* StaticEnum<" +  DisplayName + ">();\n\n";
    }

    void FReflectedEnum::DefineSecondaryHeader(eastl::string& Stream, const eastl::string& FileID)
    {
    }

    void FReflectedEnum::DeclareImplementation(eastl::string& Stream)
    {
        Stream += "static Lumina::FEnumRegistrationInfo Registration_Info_CEnum_" + DisplayName + ";\n\n";

        Stream += "struct Construct_CEnum_" + DisplayName + "_Statics\n";
        Stream +="{\n";

        Stream += "\tstatic constexpr Lumina::FEnumeratorParam Enumerators[] = {\n";
        for (FReflectedEnum::FConstant Constant : Constants)
        {
            Stream += "\t\t{ \"" + DisplayName + "::" + Constant.Label + "\", " + eastl::to_string(Constant.Value) + " },\n";
        }
        Stream += "\t};\n\n";

        Stream += "\tstatic const Lumina::FEnumParams EnumParams;\n";
        Stream += "};\n";
        Stream += "const Lumina::FEnumParams Construct_CEnum_" + DisplayName + "_Statics::EnumParams = {\n";
        Stream += "\t\"" + DisplayName + "\",\n";
        Stream += "\t Construct_CEnum_" + DisplayName + "_Statics::Enumerators,\n";
        Stream += "\tstd::size(Construct_CEnum_" + DisplayName + "_Statics::Enumerators),\n";
        Stream += "};\n\n";
        Stream += "Lumina::CEnum* Construct_CEnum_" + DisplayName + "()\n";
        Stream += "{\n";
        Stream += "\tif(!Registration_Info_CEnum_" + DisplayName + ".InnerSingleton)" + "\n";
        Stream += "\t{\n";
        Stream += "\t\tLumina::ConstructCEnum(&Registration_Info_CEnum_" + DisplayName + ".InnerSingleton, Construct_CEnum_" + DisplayName + "_Statics::EnumParams);" + "\n";
        Stream += "\t}\n";
        Stream += "\treturn Registration_Info_CEnum_" + DisplayName + ".InnerSingleton;" + "\n";
        Stream += "}\n";
        Stream += "\n";
        Stream += "template<> Lumina::CEnum* StaticEnum<" + DisplayName + ">()\n";
        Stream += "{\n";
        Stream += "\tif (!Registration_Info_CEnum_" + DisplayName + ".OuterSingleton)\n";
        Stream += "\t{\n";
        Stream += "\t\tRegistration_Info_CEnum_" + DisplayName + ".OuterSingleton = Construct_CEnum_" + DisplayName + "();\n";
        Stream += "\t}\n";
        Stream += "\treturn Registration_Info_CEnum_" + DisplayName + ".OuterSingleton;\n";
        Stream += "}\n";
    }

    void FReflectedEnum::DeclareStaticRegistration(eastl::string& Stream)
    {
        Stream += "\t{ Construct_CEnum_" + DisplayName + ", TEXT(\"" + DisplayName + "\") },\n";
    }


    //---------------------------------------------------------------------------------------------------------------------

    
    
    FReflectedStruct::~FReflectedStruct()
    {
    }

    void FReflectedStruct::PushProperty(eastl::shared_ptr<FReflectedProperty> NewProperty)
    {
        if (Namespace.empty())
        {
            NewProperty->Outer = DisplayName;
        }
        else
        {
            NewProperty->Outer = Namespace + "::" + DisplayName;
        }
        Props.push_back(NewProperty);
    }

    void FReflectedStruct::DefineConstructionStatics(eastl::string& Stream)
    {
        Stream += "struct Construct_CStruct_" + Namespace + "_" + DisplayName + "_Statics\n{\n\n";
    }

    void FReflectedStruct::DefineInitialHeader(eastl::string& Stream, const eastl::string& FileID)
    {
        
    }

    void FReflectedStruct::DefineSecondaryHeader(eastl::string& Stream, const eastl::string& FileID)
    {
        Stream += "#define " + FileID + "_" + eastl::to_string(GeneratedBodyLineNumber) + "_GENERATED_BODY \\\n";
        Stream += "\tstatic class CStruct* StaticStruct();\n\n";

        Stream += "\n\n";
    }

    void FReflectedStruct::DeclareImplementation(eastl::string& Stream)
    {
        Stream += "\n\n";
        
        Stream += "// Begin " + DisplayName + "\n";
        Stream += "static Lumina::FStructRegistrationInfo Registration_Info_CStruct_" + Namespace + "_" + DisplayName + ";\n\n";
        
        DefineConstructionStatics(Stream);

        for (const eastl::shared_ptr<FReflectedProperty>& Prop : Props)
        {
            Stream += "\tstatic const Lumina::" + eastl::string(Prop->GetPropertyParamType()) + " " + Prop->Name + ";\n";
        }
        Stream += "\t//...\n\n";
        
        Stream += "\tstatic const Lumina::FStructParams StructParams;\n";
        if (!Props.empty())
        {
            Stream += "\tstatic const Lumina::FPropertyParams* const PropPointers[];\n";
        }
        Stream += "};\n\n";
        
        Stream += "Lumina::CStruct* Construct_CStruct_" + Namespace + "_" + DisplayName + "()\n";
        Stream += "{\n";
        Stream += "\tif (!Registration_Info_CStruct_" + Namespace + "_" + DisplayName + ".OuterSingleton)\n";
        Stream += "\t{\n";
        Stream += "\t\tLumina::ConstructCStruct(&Registration_Info_CStruct_" + Namespace + "_" + DisplayName + ".OuterSingleton, Construct_CStruct_" + Namespace + "_" + DisplayName + "_Statics::StructParams);\n";
        Stream += "\t}\n";
        Stream += "\treturn Registration_Info_CStruct_" + Namespace + "_" + DisplayName + ".OuterSingleton;\n";
        Stream += "}\n\n";
        
        Stream += "class Lumina::CStruct* " + Namespace + "::" + DisplayName + "::StaticStruct()\n";
        Stream += "{\n";
        Stream += "\tif (!Registration_Info_CStruct_" + Namespace + "_" + DisplayName + ".OuterSingleton)\n";
        Stream += "\t{\n";
        Stream += "\t\tRegistration_Info_CStruct_" + Namespace + "_" + DisplayName + ".OuterSingleton = Construct_CStruct_" + Namespace + "_" + DisplayName + "();\n";
        Stream += "\t}\n";
        Stream += "\treturn Registration_Info_CStruct_" + Namespace + "_" + DisplayName + ".OuterSingleton;\n";
        Stream += "}\n";

        if (!Props.empty())
        {
            for (const eastl::shared_ptr<FReflectedProperty>& Prop : Props)
            {
                Stream += "const Lumina::" + eastl::string(Prop->GetPropertyParamType()) + " Construct_CStruct_" + Namespace + "_" + DisplayName + "_Statics::" + Prop->Name + " = ";
                Prop->AppendDefinition(Stream);
            }
            
            Stream += "\n";
            Stream += "const Lumina::FPropertyParams* const Construct_CStruct_" + Namespace + "_" + DisplayName + "_Statics::PropPointers[] = {\n";
        
            for (const auto& Prop : Props)
            {
                Stream += "\t(const Lumina::FPropertyParams*)&Construct_CStruct_" + Namespace + "_" + DisplayName + "_Statics::" + Prop->Name + ",\n";
            }
        
            Stream += "};\n\n";
        }

        Stream += "const Lumina::FStructParams Construct_CStruct_" + Namespace + "_" + DisplayName + "_Statics::StructParams = {\n";
        Stream += "\tnullptr,\n";
        Stream += "\t\"" + DisplayName + "\",\n";
        
        if (!Props.empty())
        {
            Stream += "\tConstruct_CStruct_" + Namespace + "_" + DisplayName + "_Statics::PropPointers,\n";
            Stream += "\t(uint32)std::size(Construct_CStruct_" + Namespace + "_" + DisplayName + "_Statics::PropPointers),\n";
        }
        else
        {
            Stream += "\tnullptr,\n";
            Stream += "\t0\n";
        }

        Stream += "\tsizeof(" + Namespace + "::" + DisplayName + "),\n";
        Stream += "\talignof(" + Namespace + "::" + DisplayName + ")\n";
        
        Stream += "};\n\n";

        Stream += "//~ End " + DisplayName + "\n\n";
        Stream += "//------------------------------------------------------------\n\n";
    }

    void FReflectedStruct::DeclareStaticRegistration(eastl::string& Stream)
    {
        Stream += "\t{ Construct_CStruct_" + Namespace + "_" + DisplayName + ", TEXT(\"" + DisplayName + "\") },\n";
    }


    //---------------------------------------------------------------------------------------------------------------------


    
    void FReflectedClass::DefineConstructionStatics(eastl::string& Stream)
    {
        Stream += "struct Construct_CClass_" + Namespace + "_" + DisplayName + "_Statics\n{\n";
    }

    void FReflectedClass::DefineInitialHeader(eastl::string& Stream, const eastl::string& FileID)
    {
        eastl::string LowerProject = Project;
        LowerProject.make_lower();
        eastl::string PackageName = "script://" + LowerProject;

        if (!Namespace.empty())
        {
            Stream += "namespace " + Namespace + "\n";
            Stream += "{\n";
            Stream += "\tclass " + DisplayName + ";\n";
            Stream += "}\n";
        }
        else
        {
            Stream += "\tclass " + DisplayName + ";\n";
        }

        
        Stream += "#define " + FileID + "_" + eastl::to_string(LineNumber) + "_CLASS \\\n";
        Stream += "private: \\\n";
        Stream += "\\\n";
        Stream += "public: \\\n";
        Stream += "\tDECLARE_CLASS(" + Namespace + ", " + DisplayName + ", " + Parent + ", \"" + PackageName.c_str() + "\", NO_API" + ") \\\n";
        Stream += "\tDEFINE_DEFAULT_CONSTRUCTOR_CALL(" + Namespace + "::" + DisplayName + ") \\\n";
        Stream += "\tDECLARE_SERIALIZER(" + Namespace + ", " + DisplayName + ") \\\n";
        Stream += "\n\n";
    }

    void FReflectedClass::DefineSecondaryHeader(eastl::string& Stream, const eastl::string& FileID)
    {
        Stream += "#define " + FileID + "_" + eastl::to_string(GeneratedBodyLineNumber) + "_GENERATED_BODY \\\n";
        Stream += "public: \\\n";
        Stream += "\t" + FileID + "_" + eastl::to_string(LineNumber) + "_CLASS \\\n";
        Stream += "private: \\\n";
        Stream += "\n\n";
    }

    void FReflectedClass::DeclareImplementation(eastl::string& Stream)
    {
        
        Stream += "// Begin " + DisplayName + "\n";
        Stream += "IMPLEMENT_CLASS(" + Namespace + ", " + DisplayName + ")\n";
        DefineConstructionStatics(Stream);

        for (const eastl::shared_ptr<FReflectedProperty>& Prop : Props)
        {
            if (Prop->Metadata.empty())
            {
                continue;
            }
            
            Stream += "\tstatic constexpr Lumina::FMetaDataPairParam " + Prop->Name + "_Metadata[] = {\n";

            for (const FMetadataPair& Metadata : Prop->Metadata)
            {
                Stream += "\t\t{ \"" + Metadata.Key + "\", \"" + Metadata.Value + "\" },\n";    
            }
            
            Stream += "\n\t};\n";
        }

        Stream += "\n";
        
        for (const eastl::shared_ptr<FReflectedProperty>& Prop : Props)
        {
            Stream += "\tstatic const Lumina::" + eastl::string(Prop->GetPropertyParamType()) + " " + Prop->Name + ";\n";
        }
        
        Stream += "\t//...\n\n";
        
        Stream += "\tstatic const Lumina::FClassParams ClassParams;\n";
        if (!Props.empty())
        {
            Stream += "\tstatic const Lumina::FPropertyParams* const PropPointers[];\n";
        }
        Stream += "};\n\n";
        
        Stream += "Lumina::CClass* Construct_CClass_" + Namespace + "_" + DisplayName + "()\n";
        Stream += "{\n";
        Stream += "\tif (!Registration_Info_CClass_" + Namespace + "_" + DisplayName + ".OuterSingleton)\n";
        Stream += "\t{\n";
        Stream += "\t\tLumina::ConstructCClass(&Registration_Info_CClass_" + Namespace + "_" + DisplayName + ".OuterSingleton, Construct_CClass_" + Namespace + "_" + DisplayName + "_Statics::ClassParams);\n";
        Stream += "\t}\n";
        Stream += "\treturn Registration_Info_CClass_" + Namespace + "_" + DisplayName + ".OuterSingleton;\n";
        Stream += "}\n\n";
        
        if (!Props.empty())
        {
            for (const eastl::shared_ptr<FReflectedProperty>& Prop : Props)
            {
                Stream += "const Lumina::" + eastl::string(Prop->GetPropertyParamType()) + " Construct_CClass_" + Namespace + "_" + DisplayName + "_Statics::" + Prop->Name + " = ";
                Prop->AppendDefinition(Stream);
            }
            
            Stream += "\n";
            Stream += "const Lumina::FPropertyParams* const Construct_CClass_" + Namespace + "_" + DisplayName + "_Statics::PropPointers[] = {\n";
        
            for (const eastl::shared_ptr<FReflectedProperty>& Prop : Props)
            {
                Stream += "\t(const Lumina::FPropertyParams*)&Construct_CClass_" + Namespace + "_" + DisplayName + "_Statics::" + Prop->Name + ",\n";
            }
        
            Stream += "};\n\n";
        }
        
        
        Stream += "const Lumina::FClassParams Construct_CClass_" + Namespace + "_" + DisplayName + "_Statics::ClassParams = {\n";
        Stream += "\t&" + Namespace + "::" + DisplayName + "::StaticClass,\n";
        if (!Props.empty())
        {
            Stream += "\tConstruct_CClass_" + Namespace + "_" + DisplayName + "_Statics::PropPointers,\n";
            Stream += "\t(uint32)std::size(Construct_CClass_" + Namespace + "_" + DisplayName + "_Statics::PropPointers),\n";
        }
        else
        {
            Stream += "\tnullptr,\n";
            Stream += "\t0\n";
        }

        Stream += "};\n\n";
        
        
        Stream += "//~ End " + DisplayName + "\n\n";
        Stream += "//------------------------------------------------------------\n\n";
    }

    void FReflectedClass::DeclareStaticRegistration(eastl::string& Stream)
    {
        Stream += "\t{ Construct_CClass_" + Namespace + "_" + DisplayName + ", TEXT(\"script://\"), TEXT(\"" + DisplayName + "\") },\n";
    }
}
