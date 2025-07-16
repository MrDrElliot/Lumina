#pragma once

#include "RenderTypes.h"
#include "RHIFwd.h"
#include "Memory/SmartPtr.h"
#include "Types/BitFlags.h"


namespace Lumina
{
    struct FTextureSubresourceSet;
    struct FRHIImageDesc;
    struct FRHIBufferDesc;
}

namespace Lumina
{
    struct FBufferStateExtension
    {
        const FRHIBufferDesc& DescRef;
        EResourceStates permanentState = EResourceStates::Unknown;

        explicit FBufferStateExtension(const FRHIBufferDesc& desc)
            : DescRef(desc)
        { }
    };

    struct FTextureStateExtension
    {
        explicit FTextureStateExtension(const FRHIImageDesc& desc)
            : DescRef(desc)
        { }
        
        const FRHIImageDesc& DescRef;
        EResourceStates permanentState = EResourceStates::Unknown;
        bool stateInitialized = false;
        bool isSamplerFeedback = false;

    };

    struct FTextureState
    {
        TVector<EResourceStates> SubresourceStates;
        EResourceStates State = EResourceStates::Unknown;
        bool bEnableUavBarriers = true;
        bool bFirstUavBarrierPlaced = false;
        bool bPermanentTransition = false;
    };

    struct FBufferState
    {
        EResourceStates state = EResourceStates::Unknown;
        bool enableUavBarriers = true;
        bool firstUavBarrierPlaced = false;
        bool permanentTransition = false;
    };

    struct FTextureBarrier
    {
        FTextureStateExtension* Texture = nullptr;
        uint32 MipLevel = 0;
        uint32 ArraySlice = 0;
        bool bEntireTexture = false;
        EResourceStates StateBefore = EResourceStates::Unknown;
        EResourceStates StateAfter = EResourceStates::Unknown;
    };

    struct FBufferBarrier
    {
        FBufferStateExtension* Buffer = nullptr;
        EResourceStates StateBefore = EResourceStates::Unknown;
        EResourceStates StateAfter = EResourceStates::Unknown;
    };

    class LUMINA_API FCommandListResourceStateTracker
    {
    public:

        void SetEnableUavBarriersForTexture(FTextureStateExtension* Texture, bool bEnableBarriers);
        void SetEnableUavBarriersForBuffer(FBufferStateExtension* Buffer, bool bEnableBarriers);

        void BeginTrackingTextureState(FTextureStateExtension* texture, FTextureSubresourceSet subresources, EResourceStates stateBits);
        void BeginTrackingBufferState(FBufferStateExtension* buffer, EResourceStates stateBits);

        void SetPermanentTextureState(FTextureStateExtension* texture, FTextureSubresourceSet subresources, EResourceStates stateBits);
        void SetPermanentBufferState(FBufferStateExtension* buffer, EResourceStates stateBits);

        EResourceStates GetTextureSubresourceState(FTextureStateExtension* texture, uint32 arraySlice, uint32 mipLevel);
        EResourceStates GetBufferState(FBufferStateExtension* buffer);

        // Internal interface
        
        void RequireTextureState(FTextureStateExtension* texture, FTextureSubresourceSet subresources, EResourceStates state);
        void RequireBufferState(FBufferStateExtension* buffer, EResourceStates state);

        void KeepBufferInitialStates();
        void KeepTextureInitialStates();
        void CommandListSubmitted();

        NODISCARD const TVector<FTextureBarrier>& GetTextureBarriers() const { return TextureBarriers; }
        NODISCARD const TVector<FBufferBarrier>& GetBufferBarriers() const { return BufferBarriers; }
        void ClearBarriers() { TextureBarriers.clear(); BufferBarriers.clear(); }

    private:
        THashMap<FTextureStateExtension*, std::shared_ptr<FTextureState>> TextureStates;
        THashMap<FBufferStateExtension*, std::shared_ptr<FBufferState>> BufferStates;

        // Deferred transitions of textures and buffers to permanent states.
        // They are executed only when the command list is executed, not when the app calls setPermanentTextureState or setPermanentBufferState.
        TVector<TPair<FTextureStateExtension*, EResourceStates>> PermanentTextureStates;
        TVector<TPair<FBufferStateExtension*, EResourceStates>> PermanentBufferStates;

        TVector<FTextureBarrier> TextureBarriers;
        TVector<FBufferBarrier> BufferBarriers;

        FTextureState* GetTextureStateTracking(FTextureStateExtension* texture, bool allowCreate);
        FBufferState* GetBufferStateTracking(FBufferStateExtension* buffer, bool allowCreate);
    };
    
}
