#pragma once
#include "imgui.h"
#include "Core/Singleton/Singleton.h"
#include "Renderer/RHIFwd.h"

namespace Lumina
{
    class LUMINA_API FUITextureCache : public TSingleton<FUITextureCache>
    {
    public:

        FRHIImageRef GetImage(const FString& Path);
        ImTextureRef GetImTexture(const FString& Path);

        void Clear();
        
    private:

        TPair<FRHIImageRef, ImTextureRef> GetOrCreateGroup(const FName& PathName);
        
    private:

        THashMap<FName, FRHIImageRef> Images;
        THashMap<FName, ImTextureRef> ImTextures;
    };
    
}
