#pragma once

#include "Source/Runtime/CoreObject/Object.h"
#include <stdint.h>

namespace Lumina
{
    template<typename T>
    class TSharedPtr
    {
    public:
        // Constructors
        TSharedPtr() : ptr(nullptr), refCount(nullptr) {}

        explicit TSharedPtr(T* ptr) : ptr(ptr), refCount(new size_t(1)) {}

        TSharedPtr(const TSharedPtr<T>& other) : ptr(other.ptr), refCount(other.refCount)
        {
            if (refCount)
            {
                (*refCount)++;
            }
        }

        // Destructor
        ~TSharedPtr()
        {
            Release();
        }

        // Assignment operator
        TSharedPtr<T>& operator=(const TSharedPtr<T>& other)
        {
            if (this != &other)
            {
                Release();
                ptr = other.ptr;
                refCount = other.refCount;
                if (refCount)
                {
                    (*refCount)++;
                }
            }
            return *this;
        }

        // Operators
        T& operator*() const
        {
            return *ptr;
        }

        T* operator->() const
        {
            return ptr;
        }

        explicit operator bool() const
        {
            return ptr != nullptr;
        }

        template<typename... Args>
        static TSharedPtr<T> Create(Args&&... args)
        {
            return TSharedPtr<T>(new T(std::forward<Args>(args)...));
        }

        // Methods
        T* Get() const
        {
            return ptr;
        }

        template<typename T2>
        TSharedPtr<T> GetAs()
        {
            return TSharedptr<T2>(*this);
        }
        
        void Reset()
        {
            Release();
            ptr = nullptr;
            refCount = nullptr;
        }

    private:
        T* ptr;
        size_t* refCount;

        // Private methods
        void Release() const
        {
            if (refCount)
            {
                (*refCount)--;
                
                if (*refCount == 0)
                {
                    delete ptr;
                    delete refCount;
                }
            }
        }
    };
}