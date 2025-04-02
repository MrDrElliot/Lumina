#pragma once

#include "Log/Log.h"
#include "Core/Math/Math.h"
#include "Platform/Platform.h"
#include "Containers/Name.h"
#include "Containers/String.h"
#include "Containers/Array.h"
#include "Core/Assertions/Assert.h"


#if !LE_SHIP
#define WITH_DEVELOPMENT_TOOLS 1
#endif

constexpr int32 INDEX_NONE = -1;
constexpr uint32 INVALID_HANDLE = 0xFFFFFFFF;
