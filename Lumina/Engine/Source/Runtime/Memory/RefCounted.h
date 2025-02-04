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
    class FRefCounted
    {
    public:

        FRefCounted(): RefCount() {}

        FORCEINLINE void AddRef() const
        {
            /** Add 1 to the reference count */
            RefCount.fetch_add(1, std::memory_order_relaxed);
        }

        FORCEINLINE void Release() const
        {
            /** Returns the previous value (if previous value is 1, our new value is 0). */
            if(RefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
            {
                std::atomic_thread_fence(std::memory_order_acquire);
                delete this;
            }
        }

        FORCEINLINE uint32 GetRefCount() const { return RefCount; }

    protected:

        virtual ~FRefCounted() = default;

    private:

        /** Atomic for safety across threads */
        mutable std::atomic<int> RefCount = 0;
    
    };

    template<typename T>
    class TRefPtr
    {
    public:
        
        TRefPtr() : Ptr(nullptr) {}
    
        // Destructor
        ~TRefPtr()
        {
            Release();
        }
        
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
            static_assert(std::is_base_of<FRefCounted, U>::value, "U does not inherit from RefCounted");
            static_assert(!std::is_same<T, U>::value, "Cannot convert TRefPtr of the same type.");
            AddRef();
        }
    
        // Templated conversion constructor for derived types (rvalue reference)
        template<typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value || std::is_base_of<U, T>::value>>
        TRefPtr(TRefPtr<U>&& other) noexcept : Ptr(eastl::move(std::exchange(other.Ptr, nullptr)))
        {
            static_assert(std::is_base_of<FRefCounted, U>::value, "U does not inherit from RefCounted");
            static_assert(!std::is_same<T, U>::value, "Cannot convert TRefPtr of the same type.");
        }

        template<typename U>
        TRefPtr<U> As()
        {
            return TRefPtr<U>(static_cast<U*>(Ptr));
        }

        template<typename U>
        TRefPtr<U> As() const
        {
            return TRefPtr<U>(static_cast<U*>(Ptr));
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
                other.Ptr = nullptr;
            }
            return *this;
        }
    
        T* operator ->() const { Assert(Ptr != nullptr); return Get(); }
        T& operator *() const { return *Get(); }
        explicit operator bool() const { return Ptr != nullptr; }
    
        T* Get() const { return Ptr; }
    
    public:
        
        
        FORCEINLINE void AddRef()
        {
            if (Ptr) Ptr->AddRef();
        }
    
        FORCEINLINE bool IsValid()
        {
            return Ptr != nullptr;
        }
        
        FORCEINLINE void Release()
        {
            if (Ptr)
            {
                Ptr->Release();
                Ptr = nullptr;
            }
        }
    
        T* Ptr = nullptr;
    };


    template<typename T, typename... Args>
    std::enable_if_t<std::is_constructible_v<T, Args...>, TRefPtr<T>>
    MakeRefPtr(Args&&... args)
    {
        return TRefPtr<T>(new T(std::forward<Args>(args)...));
    }


    template<typename T, typename U>
    TRefPtr<T> RefPtrCast(const TRefPtr<U>& other, ERefCountedCastType Type = ERefCountedCastType::None)
    {
        static_assert(std::is_base_of_v<U, T> || std::is_base_of_v<T, U> || std::is_same_v<T, U>, "RefPtrCast: Invalid cast between unrelated types.");
    
        if (!other.Get())
        {
            // Handle null pointer cases
            switch (Type)
            {
            case ERefCountedCastType::AssertNull:
                AssertMsg(0, "Failed Cast Due to nullptr");
                break;
            case ERefCountedCastType::LogNull:
                LOG_WARN("RefCounted: Casting to a nullptr!");
                break;
            default:
                break;
            }
            return TRefPtr<T>();
        }
    
        // Use dynamic_cast for polymorphic types
        if constexpr (std::is_polymorphic_v<U> && std::is_polymorphic_v<T>)
        {
            if (auto casted = dynamic_cast<T*>(other.Get()))
            {
                return TRefPtr<T>(casted);
            }
            else
            {
                if (Type == ERefCountedCastType::LogNull)
                {
                    LOG_WARN("RefCounted: Failed dynamic_cast between polymorphic types.");
                }
                return TRefPtr<T>();
            }
        }
        else
        {
            // Use static_cast for non-polymorphic types or trivial casts
            return TRefPtr<T>(static_cast<T*>(other.Get()));
        }
    }



