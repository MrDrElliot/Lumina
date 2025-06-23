#pragma once

#include "Containers/Array.h"
#include "Containers/Name.h"
#include "Containers/String.h"
#include "Core/Assertions/Assert.h"
#include "Core/Math/Math.h"
#include "Log/Log.h"
#include "Memory/Memory.h"
#include "Platform/Platform.h"


#define LUMINA_VERSION "0.01.0"
#define LUMINA_VERSION_MAJOR 0
#define LUMINA_VERSION_MINOR 01
#define LUMINA_VERSION_PATCH 0
#define LUMINA_VERSION_NUM 0010

#define WITH_DEVELOPMENT_TOOLS 1

#ifdef  _MSC_VER
#pragma warning (push)
#pragma warning (disable: 26495)    // [Static Analyzer] Variable 'XXX' is uninitialized. Always initialize a member variable (type.6).
#endif


// Invalid Index
constexpr int8 INDEX_NONE = -1;

