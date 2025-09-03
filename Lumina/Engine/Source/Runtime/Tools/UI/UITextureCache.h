#pragma once
#include "imgui.h"
#include "Renderer/RHIFwd.h"

namespace Lumina
{
    class FUITextureCache
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
        ~FUITextureCache();
        
        LUMINA_API FRHIImageRef GetImage(const FName& Path);
        LUMINA_API ImTextureRef GetImTexture(const FName& Path);

        bool HasImagesPendingLoad() const;
    
    private:

        FEntry* GetOrCreateGroup(const FName& PathName);
        
    private:
        
        THashMap<FName, FEntry*>    Images;

        TPair<FName, FEntry*>       SquareWhiteTexture;
    };
    
}
