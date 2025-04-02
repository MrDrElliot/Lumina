#pragma once
#include "Containers/Array.h"
#include "Containers/Name.h"
#include "Containers/String.h"

namespace Lumina::Reflection
{
    class FReflectedHeader
    {
    public:

        FReflectedHeader() = default;
        FReflectedHeader(const FString& Path);

        bool Parse();
        

        FName HeaderID;
        FString HeaderPath;
        TVector<FString> Contents;
        
    };
}
