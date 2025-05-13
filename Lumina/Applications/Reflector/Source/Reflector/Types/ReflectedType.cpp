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
            delete Prop;
        }
    }

    void FReflectedStruct::DefineConstructionStatics(std::stringstream& SS)
    {
        SS << "struct Construct_CStruct_" << Namespace.c_str() << "_" << DisplayName.c_str() << "_Statics\n{\n\n";
    }

    void FReflectedClass::DefineConstructionStatics(std::stringstream& SS)
    {
        SS << "struct Construct_CClass_" << Namespace.c_str() << "_" << DisplayName.c_str() << "_Statics\n{\n";
    }
}
