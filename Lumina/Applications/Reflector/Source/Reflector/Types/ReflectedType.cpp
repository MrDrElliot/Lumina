#include "ReflectedType.h"

#include <iostream>

#include "Properties/ReflectedProperty.h"


namespace Lumina::Reflection
{
    bool FReflectedType::DeclareGettersAndSetters(eastl::string& Stream, const eastl::string& FileID)
    {
        eastl::vector<eastl::string> Getters;
        eastl::vector<eastl::string> Setters;
        for (const eastl::shared_ptr<FReflectedProperty>& Prop : Props)
        {
            if (!Prop->GetterFunc.empty())
            {
                Getters.push_back(Prop->GetterFunc);
            }

            if (!Prop->SetterFunc.empty())
            {
                Setters.push_back(Prop->SetterFunc);
            }
        }

        if (!Getters.empty() || !Setters.empty())
        {
            Stream += "#define " + FileID + "_" + eastl::to_string(GeneratedBodyLineNumber) + "ACCESSORS \\\n";
            for (size_t i = 0; i < Getters.size(); ++i)
            {
                const eastl::string& Getter = Getters[i];
                Stream += "static void " + Getter + "_WrapperImpl(const void* Object, void* OutValue);";
                if (i != Getters.size() - 1 || !Setters.empty())
                {
                    Stream += " \\\n";
                }
                else
                {
                    Stream += "\n";
                }
            }

            for (size_t i = 0; i < Setters.size(); ++i)
            {
                const eastl::string& Setter = Setters[i];
                Stream += "static void " + Setter + "_WrapperImpl(void* Object, const void* InValue);";
                if (i != Setters.size() - 1)
                {
                    Stream += " \\\n";
                }
                else
                {
                    Stream += "\n";
                }
            }

            Stream += "\n\n";
        }

        return !Getters.empty() || !Setters.empty();
    }

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

    void FReflectedStruct::PushProperty(const eastl::shared_ptr<FReflectedProperty>& NewProperty)
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
        if (!Namespace.empty())
        {
            Stream += "namespace " + Namespace + " { struct " + DisplayName + "; }\n";
        }
        else
        {
            Stream += "\tclass " + DisplayName + ";\n";
        }
        eastl::string ProjectAPI = Project + "_api";
        ProjectAPI.make_upper();

        if (ProjectAPI == "LUMINA_API")
        {
            Stream += "LUMINA_API ";
        }
        Stream += "Lumina::CStruct* Construct_CStruct_" + Namespace + "_" + DisplayName + "();\n";

        
    }

    void FReflectedStruct::DefineSecondaryHeader(eastl::string& Stream, const eastl::string& FileID)
    {
        if (FileID.find("manualreflecttypes") != eastl::string::npos)
        {
            Stream += "\n\n";
            return;
        }

        bool bDeclared = DeclareGettersAndSetters(Stream, FileID);
        
        Stream += "#define " + FileID + "_" + eastl::to_string(GeneratedBodyLineNumber) + "_GENERATED_BODY \\\n";
        if (bDeclared)
        {
            Stream += FileID + "_" + eastl::to_string(GeneratedBodyLineNumber) + "ACCESSORS \\\n";
        }
        Stream += "\tstatic class Lumina::CStruct* StaticStruct();\\\n";
        if (!Parent.empty())
        {
            Stream += "\tusing Super = " + Namespace + "::" + Parent + ";\\\n\n";
        }

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
        
        Stream += "\tstatic const Lumina::FStructParams StructParams;\n";
        if (!Props.empty())
        {
            Stream += "\tstatic const Lumina::FPropertyParams* const PropPointers[];\n";
        }
        Stream += "};\n\n";
        
        Stream += "Lumina::CStruct* Construct_CStruct_" + Namespace + "_" + DisplayName + "()\n";
        Stream += "{\n";
        Stream += "\tif (!Registration_Info_CStruct_" + Namespace + "_" + DisplayName + ".InnerSingleton)\n";
        Stream += "\t{\n";
        Stream += "\t\tLumina::ConstructCStruct(&Registration_Info_CStruct_" + Namespace + "_" + DisplayName + ".InnerSingleton, Construct_CStruct_" + Namespace + "_" + DisplayName + "_Statics::StructParams);\n";
        Stream += "\t}\n";
        Stream += "\treturn Registration_Info_CStruct_" + Namespace + "_" + DisplayName + ".InnerSingleton;\n";
        Stream += "}\n\n";

        if (HeaderID.find("manualreflecttypes") == eastl::string::npos)
        {
            Stream += "class Lumina::CStruct* " + Namespace + "::" + DisplayName + "::StaticStruct()\n";
            Stream += "{\n";
            Stream += "\tif (!Registration_Info_CStruct_" + Namespace + "_" + DisplayName + ".OuterSingleton)\n";
            Stream += "\t{\n";
            Stream += "\t\tRegistration_Info_CStruct_" + Namespace + "_" + DisplayName + ".OuterSingleton = Construct_CStruct_" + Namespace + "_" + DisplayName + "();\n";
            Stream += "\t}\n";
            Stream += "\treturn Registration_Info_CStruct_" + Namespace + "_" + DisplayName + ".OuterSingleton;\n";
            Stream += "}\n";
        }

        if (!Props.empty())
        {
            for (const eastl::shared_ptr<FReflectedProperty>& Prop : Props)
            {
                if (!Prop->GetterFunc.empty())
                {
                    Stream += "void " + QualifiedName + "::" + Prop->GetterFunc + "_WrapperImpl(const void* Object, void* OutValue)\n";
                    Stream += "{\n";
                    Stream += "\tconst " + DisplayName + "* Obj = (const " + DisplayName + "*)Object;\n";
                    Stream += "\t" + Prop->RawTypeName + "& Result = *(" + Prop->RawTypeName + "*)OutValue;\n";
                    Stream += "\tResult = (" + Prop->RawTypeName + ")Obj->" + Prop->GetterFunc + "();\n";
                    Stream += "}\n";
                }

                if (!Prop->SetterFunc.empty())
                {
                    Stream += "void " + QualifiedName + "::" + Prop->SetterFunc + "_WrapperImpl(void* Object, const void* InValue)\n";
                    Stream += "{\n";
                    Stream += "\t" + QualifiedName + "* " + "Obj = (" + QualifiedName + "*)Object;\n";
                    Stream += "\tconst " + Prop->RawTypeName + "& Value = *(const " + Prop->RawTypeName + "*)InValue;\n";
                    Stream += "\tObj->" + Prop->SetterFunc + "(Value);\n";
                    Stream += "}\n";
                }
            }
            
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
        if (Parent.empty())
        {
            Stream += "\tnullptr,\n";
        }
        else
        {
            Stream += "\t" + Namespace + "::" + DisplayName + "::" + "Super::StaticStruct,\n";
        }
        Stream += "\t\"" + DisplayName + "\",\n";
        
        if (!Props.empty())
        {
            Stream += "\tConstruct_CStruct_" + Namespace + "_" + DisplayName + "_Statics::PropPointers,\n";
            Stream += "\t(uint32)std::size(Construct_CStruct_" + Namespace + "_" + DisplayName + "_Statics::PropPointers),\n";
        }
        else
        {
            Stream += "\tnullptr,\n";
            Stream += "\t0,\n";
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
            Stream += "namespace " + Namespace + " { class " + DisplayName + "; }\n";
        }
        else
        {
            Stream += "\tclass " + DisplayName + ";\n";
        }

        if (LowerProject == "lumina")
        {
            Stream += "LUMINA_API ";
        }
        Stream += "Lumina::CClass* Construct_CClass_" + Namespace + "_" + DisplayName + "();\n";
        
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
        bool bDeclared = DeclareGettersAndSetters(Stream, FileID);
        
        Stream += "#define " + FileID + "_" + eastl::to_string(GeneratedBodyLineNumber) + "_GENERATED_BODY \\\n";
        Stream += "public: \\\n";
        if (bDeclared)
        {
            Stream += FileID + "_" + eastl::to_string(GeneratedBodyLineNumber) + "ACCESSORS \\\n";
        }
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
                if (!Prop->GetterFunc.empty())
                {
                    Stream += "void " + QualifiedName + "::" + Prop->GetterFunc + "_WrapperImpl(const void* Object, void* OutValue)\n";
                    Stream += "{\n";
                    Stream += "\tconst " + DisplayName + "* Obj = (const " + DisplayName + "*)Object;\n";
                    Stream += "\t" + Prop->RawTypeName + "& Result = *(" + Prop->RawTypeName + "*)OutValue;\n";
                    Stream += "\tResult = (" + Prop->RawTypeName + ")Obj->" + Prop->GetterFunc + "();\n";
                    Stream += "}\n";
                }

                if (!Prop->SetterFunc.empty())
                {
                    Stream += "void " + QualifiedName + "::" + Prop->SetterFunc + "_WrapperImpl(void* Object, const void* InValue)\n";
                    Stream += "{\n";
                    Stream += "\t" + QualifiedName + "* " + "Obj = (" + QualifiedName + "*)Object;\n";
                    Stream += "\tconst " + Prop->RawTypeName + "& Value = *(const " + Prop->RawTypeName + "*)InValue;\n";
                    Stream += "\tObj->" + Prop->SetterFunc + "(Value);\n";
                    Stream += "}\n";
                }
            }
            
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
        Stream += "\t{ Construct_CClass_" + Namespace + "_" + DisplayName + ", " + "TEXT(\"script://\"), TEXT(\"" + DisplayName + "\") },\n";
    }
}
