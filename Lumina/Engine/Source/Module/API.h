#pragma once

#ifdef LUMINA_ENGINE
    #define LUMINA_API __declspec(dllexport)
#else
    #define LUMINA_API __declspec(dllimport)
#endif

