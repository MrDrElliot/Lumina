#include "UITextureCache.h"

#include "Core/Engine/Engine.h"
#include "ImGui/ImGuiRenderer.h"
#include "Renderer/RenderManager.h"
#include "Renderer/RHIGlobals.h"
#include "Tools/Import/ImportHelpers.h"

namespace Lumina
{
    FRHIImageRef FUITextureCache::GetImage(const FString& Path)
    {
        FName PathName = Path;

        return GetOrCreateGroup(PathName).first;
    }

    ImTextureRef FUITextureCache::GetImTexture(const FString& Path)
    {
        FName PathName = Path;

        return GetOrCreateGroup(PathName).second;
    }

    void FUITextureCache::Clear()
    {
        Images.clear();
        ImTextures.clear();
    }

    TPair<FRHIImageRef, ImTextureRef> FUITextureCache::GetOrCreateGroup(const FName& PathName)
    {
        TPair<FRHIImageRef, ImTextureRef> ReturnValue;
        
        if (Images.find(PathName) != Images.end())
        {
            ReturnValue.first = Images.at(PathName);
            ReturnValue.second = ImTextures.at(PathName);
            return ReturnValue;
        }

        FString PathString = PathName.ToString();
        ReturnValue.first = Import::Textures::CreateTextureFromImport(GRenderContext, PathString, false);
        ReturnValue.second = GEngine->GetEngineSubsystem<FRenderManager>()->GetImGuiRenderer()->GetOrCreateImTexture(ReturnValue.first);

        Images.try_emplace(PathName, ReturnValue.first );
        ImTextures.try_emplace(PathName, ReturnValue.second);
        
        return ReturnValue;
    }
}
