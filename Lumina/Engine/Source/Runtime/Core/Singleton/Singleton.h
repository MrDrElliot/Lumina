#pragma once
#include "Platform/Platform.h"

/** Defines a simple singleton class */
template<typename T>
class TSingleton
{
public:
    virtual ~TSingleton() = default;

    TSingleton() = default;
    TSingleton(const TSingleton* obj) = delete;
    TSingleton* operator = (const TSingleton*) = delete;

    NODISCARD static T* Get() { static T Instance; return &Instance; }
    
};
