#pragma once
#include "Containers/Array.h"
#include "Core/Functional/Function.h"
#include "Memory/Memory.h"
#include "Platform/GenericPlatform.h"
#include "Platform/WindowsPlatform.h"

namespace Lumina
{

    enum class ERenderHandleType : uint8
    {
        Buffer,
        Image,
        Shader,
        CommandBuffer,
    };

    
    class FRenderHandle
    {
    public:

        FRenderHandle() = default;
        
        FORCEINLINE uint32 GetHandle() const { return Handle; }
        FORCEINLINE uint32 GetGeneration() const { return Generation; }

        FORCEINLINE bool operator == (const FRenderHandle& Other)
        {
            return Handle == Other.Handle && Generation == Other.Generation;
        }
        
    private:
        
        uint32 Handle = 0;
        uint32 Generation = 0;
    
    };

    //-----------------------------------------------------------------------------------------------------

    
    template <ERenderHandleType Type>
    class TRenderResourceHandle : public FRenderHandle
    {
    public:
        
        static constexpr ERenderHandleType HandleType = Type;
        constexpr ERenderHandleType GetHandleType() { return HandleType; }

        

        FORCEINLINE bool operator==(const TRenderResourceHandle& Other) const
        {
            return static_cast<const FRenderHandle&>(*this) == static_cast<const FRenderHandle&>(Other);
        }
    };

    //-----------------------------------------------------------------------------------------------------

    template <ERenderHandleType THandleType, typename TResource>
    class TRenderResourcePool
    {
    public:

        TRenderResourcePool(const TRenderResourcePool&) = delete;
        TRenderResourcePool& operator = (const TRenderResourcePool&) = delete;
        
        using HandleType = TRenderResourceHandle<THandleType>;

        void SetFreeCallback(TFunction<void(TResource*)>&& Functor)
        {
            FreeCallback = FMemory::Move(Functor);
        }

        HandleType Allocate()
        {
            uint32 index;
            if (!FreeList.empty())
            {
                index = FreeList.back();
                FreeList.pop_back();
            }
            else
            {
                index = static_cast<uint32>(Resources.size());
                Resources.emplace_back();
                Generations.push_back(0);
            }

            return HandleType(index, Generations[index]);
        }

        TResource* GetResource(const HandleType& Handle)
        {
            uint32 index = Handle.GetHandle();
            if (index >= Resources.size() || Generations[index] != Handle.GetGeneration())
            {
                return nullptr;
            }
            return &Resources[index];
        }

        void Free(const HandleType& Handle)
        {
            uint32 index = Handle.GetHandle();
            if (index >= Resources.size() || Generations[index] != Handle.GetGeneration())
            {
                return;
            }

            FreeCallback(GetResource(Handle));

            Generations[index]++;
            FreeList.push_back(index);
        }
        
    private:

        TFunction<void(TResource*)> FreeCallback;
        TVector<TResource> Resources;
        TVector<uint32> Generations;
        TVector<uint32> FreeList;
    };

    
}
