#include "ReflectedArrayProperty.h"

#include "Reflector/Types/ReflectedType.h"

namespace Lumina
{
    void FReflectedArrayProperty::AppendDefinition(eastl::string& Stream) const
    {
        eastl::string CustomData;

        // Add function pointers for array access
        CustomData += Outer + "::" + Name + "ArrayPushBack_WrapperImpl, ";
        CustomData += Outer + "::" + Name + "ArrayGetNum_WrapperImpl, ";
        CustomData += Outer + "::" + Name + "ArrayRemoveAt_WrapperImpl, ";
        CustomData += Outer + "::" + Name + "ArrayClear_WrapperImpl, ";
        CustomData += Outer + "::" + Name + "ArrayGetAt_WrapperImpl";

        AppendPropertyDef(Stream, "Lumina::EPropertyFlags::None", "Lumina::EPropertyTypeFlags::Vector", CustomData);
    }

    bool FReflectedArrayProperty::HasAccessors()
    {
        return true;
    }

    bool FReflectedArrayProperty::DeclareAccessors(eastl::string& Stream, const eastl::string& FileID)
    {
        FReflectedProperty::DeclareAccessors(Stream, FileID);

        // Array-specific accessors
        Stream += "static void " + Name + "ArrayGetter_WrapperImpl(const void* Object, void* OutValue); \\\n";
        Stream += "static void " + Name + "ArrayPushBack_WrapperImpl(void* Object, const void* InValue); \\\n";
        Stream += "static size_t " + Name + "ArrayGetNum_WrapperImpl(const void* Object); \\\n";
        Stream += "static void " + Name + "ArrayRemoveAt_WrapperImpl(void* Object, size_t Index); \\\n";
        Stream += "static void " + Name + "ArrayClear_WrapperImpl(void* Object); \\\n";
        Stream += "static void* " + Name + "ArrayGetAt_WrapperImpl(void* Object, size_t Index);\\\n";
        
        return true;
    }

    bool FReflectedArrayProperty::DefineAccessors(eastl::string& Stream, Reflection::FReflectedType* ReflectedType)
    {
        FReflectedProperty::DefineAccessors(Stream, ReflectedType);
        
        // Array Getter
        Stream += "void " + ReflectedType->QualifiedName + "::" + Name + "ArrayGetter_WrapperImpl(const void* Object, void* OutValue)\n";
        Stream += "{\n";
        Stream += "\tconst " + ReflectedType->DisplayName + "* Obj = (const " + ReflectedType->DisplayName + "*)Object;\n";
        Stream += "\tauto& Result = *(" + RawTypeName + "*)OutValue;\n";
        Stream += "\tResult = Obj->" + Name + ";\n";
        Stream += "}\n\n";
    
        // Array PushBack
        Stream += "void " + ReflectedType->QualifiedName + "::" + Name + "ArrayPushBack_WrapperImpl(void* Object, const void* InValue)\n";
        Stream += "{\n";
        Stream += "\t" + ReflectedType->DisplayName + "* Obj = (" + ReflectedType->DisplayName + "*)Object;\n";
        Stream += "\tif(InValue)\n";
        Stream += "\t{\n";
        Stream += "\t\tObj->" + Name + ".push_back(*(const " + ElementTypeName + "*)InValue);\n";
        Stream += "\t}\n";
        Stream += "\telse\n";
        Stream += "\t{\n";
        Stream += "\t\tObj->" + Name + ".emplace_back();\n";
        Stream += "\t}\n";
        Stream += "}\n\n";
    
        // Array GetNum
        Stream += "size_t " + ReflectedType->QualifiedName + "::" + Name + "ArrayGetNum_WrapperImpl(const void* Object)\n";
        Stream += "{\n";
        Stream += "\tconst " + ReflectedType->DisplayName + "* Obj = (const " + ReflectedType->DisplayName + "*)Object;\n";
        Stream += "\treturn Obj->" + Name + ".size();\n";
        Stream += "}\n\n";
    
        // Array RemoveAt
        Stream += "void " + ReflectedType->QualifiedName + "::" + Name + "ArrayRemoveAt_WrapperImpl(void* Object, size_t Index)\n";
        Stream += "{\n";
        Stream += "\t" + ReflectedType->DisplayName + "* Obj = (" + ReflectedType->DisplayName + "*)Object;\n";
        Stream += "\tObj->" + Name + ".erase(Obj->" + Name + ".begin() + Index);\n";
        Stream += "}\n\n";
    
        // Array Clear
        Stream += "void " + ReflectedType->QualifiedName + "::" + Name + "ArrayClear_WrapperImpl(void* Object)\n";
        Stream += "{\n";
        Stream += "\t" + ReflectedType->DisplayName + "* Obj = (" + ReflectedType->DisplayName + "*)Object;\n";
        Stream += "\tObj->" + Name + ".clear();\n";
        Stream += "}\n\n";
    
        // Array GetAt (mutable)
        Stream += "void* " + ReflectedType->QualifiedName + "::" + Name + "ArrayGetAt_WrapperImpl(void* Object, size_t Index)\n";
        Stream += "{\n";
        Stream += "\t" + ReflectedType->DisplayName + "* Obj = (" + ReflectedType->DisplayName + "*)Object;\n";
        Stream += "\treturn &Obj->" + Name + "[Index];\n";
        Stream += "}\n\n";
    
        return true;
    }
}
