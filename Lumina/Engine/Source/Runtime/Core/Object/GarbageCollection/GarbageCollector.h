#pragma once
#include "Module/API.h"

namespace Lumina
{
    class CObjectBase;
}

namespace Lumina::GarbageCollection
{
    LUMINA_API void AddGarbage(CObjectBase* Obj);

    LUMINA_API void CollectGarbage();
}
