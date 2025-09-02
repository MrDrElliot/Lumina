#pragma once
#include "imgui.h"
#include "Core/Singleton/Singleton.h"
#include "Core/Threading/Thread.h"
#include "Renderer/RHIFwd.h"

namespace Lumina
{
    class LUMINA_API FUITextureCache : public TSingleton<FUITextureCache>
    {
    public:

        enum class ETextureState : uint8
        {
            Empty,
            Loading,
            Ready,
        };
        
        struct FEntry
        {
            std::atomic<ETextureState> State = ETextureState::Empty;
            FRHIImageRef RHIImage;
            ImTextureRef ImTexture;
        };

        FUITextureCache();

        FRHIImageRef GetImage(const FName& Path);
        ImTextureRef GetImTexture(const FName& Path);

        void Clear();
        
    private:

        FEntry* GetOrCreateGroup(const FName& PathName);
        
    private:
        
        THashMap<FName, FEntry*>    Images;
        FEntry*                     SquareWhiteTexture = nullptr;

        uint32                      bCleared:1=0;
    };
    
}
