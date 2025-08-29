#pragma once
#include <type_traits>

#include "Component.h"


namespace Lumina
{
    template<typename T> struct FNeedsRenderState            {};
    template<typename T> struct FNeedsRenderStateUpdated     {};
    template<typename T> struct FNeedsRenderStateDestroyed   {};
    template<typename T> struct FNeedsRenderTransformUpdated {};
}
