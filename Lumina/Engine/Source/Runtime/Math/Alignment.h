#pragma once
#include "Platform/GenericPlatform.h"

namespace Lumina::Math
{
    inline uint64 GetAligned(uint64 Operand, uint64 Granularity)
    {
        return ((Operand + (Granularity - 1)) & ~(Granularity - 1));
    }
}
