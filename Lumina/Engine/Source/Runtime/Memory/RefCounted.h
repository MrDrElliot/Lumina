#pragma once

#include <atomic>

#include "Core/Assertions/Assert.h"
#include "Log/Log.h"

enum class ERefCountedCastType
{
    None,
    AssertNull,
    LogNull,
};

    /** Intrusive Smart Pointer Implementation */
    class RefCounted
    {
    public:

        RefCounted(): RefCount() {}

        void AddRef() const
        {
            RefCount.fetch_add(1, std::memory_order_relaxed);
        }

        void Release() const
        {
            if(RefCount.fetch_sub(1, std::memory_order_acquire) == 1)
            {
                std::atomic_thread_fence(std::memory_order_acquire);
                RefCount = 0;

                delete this;
            }
        }


    protected:

        virtual ~RefCounted() = default;

    private:

        mutable std::atomic<int> RefCount = 0;
    
    };

template<typename T>
class TRefPtr
{
public:
    TRefPtr() : Ptr(nullptr) {}

    // Destructor
    ~TRefPtr() { Release(); }
    
    // Constructor from raw pointer
    TRefPtr(T* p): Ptr(p)
    {
        if(Ptr)
        {
            AddRef();
        }
    }
    
    // Copy constructor
    TRefPtr(const TRefPtr& other) : Ptr(other.Ptr)
    {
        AddRef();
    }
    
    // Move constructor
    TRefPtr(TRefPtr&& other) noexcept : Ptr(other.Ptr)
    {
        // Properly take ownership of the pointer and nullify the other
        other.Ptr = nullptr;  // Now `other` won't call Release() on the original pointer
    }

    // Templated conversion constructor for derived types (lvalue reference)
    template<typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value || std::is_base_of<U, T>::value>>
    TRefPtr(const TRefPtr<U>& other) : Ptr(other.Get())
    {
        static_assert(std::is_base_of<RefCounted, U>::value, "U does not inherit from RefCounted");
        static_assert(!std::is_same<T, U>::value, "Cannot convert TRefPtr of the same type.");
        AddRef();
    }

    // Templated conversion constructor for derived types (rvalue reference)
    template<typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value || std::is_base_of<U, T>::value>>
    TRefPtr(TRefPtr<U>&& other) noexcept : Ptr(other.Get())
    {
        static_assert(std::is_base_of<RefCounted, U>::value, "U does not inherit from RefCounted");
        static_assert(!std::is_same<T, U>::value, "Cannot convert TRefPtr of the same type.");
        AddRef();
        if(other.Get())
        {
            other.Release();  // Release other safely
        }
    }

    bool operator == (const TRefPtr& other) const noexcept
    {
        return Ptr == other.Ptr;
    }

    TRefPtr& operator = (const TRefPtr& other)
    {
        if (this != &other)
        {
            Release();
            Ptr = other.Ptr;
            AddRef();
        }
        return *this;
    }

    TRefPtr& operator = (TRefPtr&& other) noexcept
    {
        if (this != &other)
        {
            Release();
            Ptr = other.Ptr;
            other.Ptr = nullptr;  // Prevent double deletion
        }
        return *this;
    }

    T* operator ->() const { return Ptr; }
    T& operator *() const { return *Ptr; }
    explicit operator bool() const { return Ptr != nullptr; }

    T* Get() const { return Ptr; }

public:
    void AddRef()
    {
        if (Ptr) Ptr->AddRef();
    }

    void Release()
    {
        if (Ptr)
        {
            Ptr->Release();
            Ptr = nullptr; // Prevent dangling pointer after release
        }
    }

    T* Ptr; // Pointer to the managed object
};


    template<typename T, typename... Args>
    TRefPtr<T> MakeRefPtr(Args&&... args)
    {
        return TRefPtr<T>(new T(std::forward<Args>(args)...));
    }


    template<typename T, typename U>
    std::enable_if_t<std::is_base_of<U, T>::value || std::is_base_of<T, U>::value, TRefPtr<T>>
    RefPtrCast(const TRefPtr<U>& other, ERefCountedCastType Type = ERefCountedCastType::None)
    {
        // Check if the original pointer is nullptr
        if (other.Get() == nullptr)
        {
            if(Type == ERefCountedCastType::AssertNull)
            {
                AssertMsg(0, "Failed Cast Due to nullptr");
            }
            else if(Type == ERefCountedCastType::LogNull)
            {
                LOG_WARN("RefCounted: Casting to a nullptr!");
                return TRefPtr<T>();
            } 
            return TRefPtr<T>();               
        }

        if constexpr (std::is_polymorphic<U>::value && std::is_polymorphic<T>::value)
        {
            return TRefPtr<T>(dynamic_cast<T*>(other.Get()));
        }
        else
        {
            return TRefPtr<T>(static_cast<T*>(other.Get()));
        }
    }

