#pragma once

#include "Platform/GenericPlatform.h"
#include "Containers/Array.h"

namespace Lumina
{
    enum class EShaderStage : uint8;

    struct FShaderStage
    {
        EShaderStage Stage;
        TVector<uint32> Binaries;
        FString RawPath;
    };
}
