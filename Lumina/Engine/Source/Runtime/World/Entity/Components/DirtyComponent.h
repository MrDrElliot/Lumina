#pragma once
#include <type_traits>

#include "Component.h"


namespace Lumina
{
    template<typename T> struct FNeedsRenderState            : SEntityComponent {};
    template<typename T> struct FNeedsRenderStateUpdated     : SEntityComponent {};
    template<typename T> struct FNeedsRenderStateDestroyed   : SEntityComponent {};
    template<typename T> struct FNeedsRenderTransformUpdated : SEntityComponent {};
}
