#include "ReflectedType.h"
#include "Properties/ReflectedProperty.h"


namespace Lumina::Reflection
{
    void FReflectedEnum::DefineConstructionStatics(std::stringstream& SS)
    {
    }

    FReflectedStruct::~FReflectedStruct()
    {
        for (const FReflectedProperty* Prop : Props)
        {
            FMemory::Delete(Prop);    
        }
    }

    void FReflectedStruct::DefineConstructionStatics(std::stringstream& SS)
    {
        SS << "struct Construct_CStruct_" << DisplayName.c_str() << "_Statics\n{\n\n";
    }

    void FReflectedClass::DefineConstructionStatics(std::stringstream& SS)
    {
        SS << "struct Construct_CClass_" << DisplayName.c_str() << "_Statics\n{\n";
    }
}
