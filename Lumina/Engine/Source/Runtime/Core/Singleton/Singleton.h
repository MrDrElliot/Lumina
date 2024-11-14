#pragma once

/** Defines a simple singleton class */
template<typename T>
class TSingleton
{
public:

    TSingleton() = default;
    TSingleton(const TSingleton* obj) = delete;
    TSingleton* operator = (const TSingleton*) = delete;

    static T* Get() { static T Instance; return &Instance; }
    virtual void Shutdown() = 0;
    
};
