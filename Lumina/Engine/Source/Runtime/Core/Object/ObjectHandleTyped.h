#pragma once
#include "ObjectArray.h"
#include "Core/Templates/LuminaTemplate.h"


namespace Lumina
{

    template <typename T, int = sizeof(T)>
    char (&ResolveTypeIsComplete(int))[2];

    template <typename T>
    char (&ResolveTypeIsComplete(...))[1];
    
    template<typename ReferencedType>
    class TObjectHandle
    {
    public:

        static_assert(std::disjunction_v<std::bool_constant<sizeof(ResolveTypeIsComplete<ReferencedType>(1)) != 2>, std::is_base_of<CObject, ReferencedType>>, "TObjectHandle<T> can only be used with types derived from UObject");
        
        TObjectHandle() = default;

        NODISCARD TObjectHandle(FObjectHandle InHandle)
            :Handle(Memory::Move(InHandle))
        {}

        NODISCARD TObjectHandle(TObjectHandle&& Other) noexcept
            :Handle(Memory::Move(Other.Handle))
        {}

        NODISCARD TObjectHandle(const TObjectHandle& Other) noexcept
            :Handle(Other.Handle)
        {}

        template<typename U>
        requires std::is_base_of_v<ReferencedType, U>
        NODISCARD TObjectHandle(const TObjectHandle<U>& Other)
            : Handle(Other.GetRawHandle())
        {}

        template<typename U>
        requires std::is_base_of_v<ReferencedType, U>
        NODISCARD TObjectHandle& operator=(const TObjectHandle<U>& Other)
        {
            Handle = Other.GetRawHandle();
            return *this;
        }

        template<typename U>
        requires std::is_base_of_v<ReferencedType, std::remove_cv_t<U>>
        NODISCARD TObjectHandle(U* Object)
        {
            if (Object)
            {
                Handle = GObjectArray.ToHandle(Object);
            }
        }

        NODISCARD TObjectHandle(nullptr_t)
            :Handle(FObjectHandle())
        {
        }

        NODISCARD operator ReferencedType*() const
        {
            return Get();
        }

        NODISCARD ReferencedType& operator*() const
        {
            return *Get();
        }

        TObjectHandle& operator = (nullptr_t)
        {
            Handle = FObjectHandle();
            return *this;
        }

        TObjectHandle& operator = (TObjectHandle&& Other) noexcept
        {
            Handle = Memory::Move(Other.Handle);
            return *this;
        }

        TObjectHandle& operator = (const TObjectHandle& Other) noexcept
        {
            Handle = Other.Handle;
            return *this;
        }
        
        template<typename U>
        requires std::is_convertible_v<U*, ReferencedType*>
        TObjectHandle& operator = (const TObjectHandle<U>& Other)
        {
            Handle = Other.Handle;
            return *this;
        }

        template<typename U>
        requires (std::is_convertible_v<U, ReferencedType*>)
        TObjectHandle& operator=(U&& Other)
        {
            Handle = const_cast<std::remove_const_t<ReferencedType>*>(ImplicitConv<ReferencedType*>(Other));
            return *this;
        }

        
        friend bool operator==(const TObjectHandle& Handle, nullptr_t)
        {
            return !Handle.IsValid();
        }

        friend bool operator!=(const TObjectHandle& Handle, nullptr_t)
        {
            return Handle.IsValid();
        }

        bool operator==(ReferencedType* Other) const { return Get() == Other; }
        bool operator==(const TObjectHandle& Other) const { return Handle == Other.Handle; }
        bool operator!=(const TObjectHandle& Other) const { return !(*this == Other); }

        ReferencedType* operator->() { return Get(); }
        const ReferencedType* operator->() const { return Get(); }
        ReferencedType& operator*() { return *Get(); }

        explicit operator bool() const noexcept { return IsValid(); }
        
        ReferencedType* Get() const
        {
            return (ReferencedType*)(GObjectArray.Resolve(Handle));
        }

        void MarkGarbage()
        {
            Get()->MarkGarbage();
            Handle = FObjectHandle();
        }
        
        bool IsValid() const
        {
            return Get() != nullptr;
        }

        FObjectHandle GetHandle() const { return Handle; }
    
    private:
        
        static bool IsObjectPtrNull(const FObjectHandle& ObjectHandle)
        {
            return !ObjectHandle.operator bool();
        }
    
    private:
		
        FObjectHandle Handle;
    };

}
