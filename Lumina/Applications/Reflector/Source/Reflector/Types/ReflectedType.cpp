#include "ReflectedType.h"

namespace Lumina::Reflection
{
    FString FReflectedEnum::GetTypeAsString() const
    {
        FString ReturnValue;
        for (const FConstant& Constant : Constants)
        {
            ReturnValue += Constant.Label + " " + eastl::to_string(Constant.Value) + "\n";            
        }

        return ReturnValue;
    }
}
