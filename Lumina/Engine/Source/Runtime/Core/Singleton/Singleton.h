#pragma once
#include "Module/API.h"
#include "Platform/Platform.h"

/** Defines a simple singleton class */
template<typename T>
class TSingleton
{
public:
    friend T;
    virtual ~TSingleton() = default;

    NODISCARD static T& Get() { static T Instance; return Instance; }
private:
    TSingleton() = default;
    explicit TSingleton(const TSingleton* obj) = delete;
    TSingleton* operator = (const TSingleton*) = delete;

    
};
