#pragma once
#include "Containers/Array.h"
#include "Platform/GenericPlatform.h"
#include "World/Entity/Entity.h"


namespace Lumina
{
    struct SRelationshipComponent : SEntityComponent
    {
        constexpr static SIZE_T MaxChildren = 32;
        
        SIZE_T Size {};
        TArray<Entity, MaxChildren> Children {};
        Entity Parent {};
    };
}
