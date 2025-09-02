#include "UITextureCache.h"

#include "Core/Engine/Engine.h"
#include "ImGui/ImGuiRenderer.h"
#include "Renderer/RenderManager.h"
#include "Renderer/RHIGlobals.h"
#include "TaskSystem/TaskSystem.h"
#include "Tools/Import/ImportHelpers.h"

namespace Lumina
{
    FUITextureCache::FUITextureCache()
    {
        if (IImGuiRenderer* ImGuiRenderer = GEngine->GetEngineSubsystem<FRenderManager>()->GetImGuiRenderer())
        {
            FName SquareTexturePath = Paths::GetEngineResourceDirectory() + "/Textures/WhiteSquareTexture.png";
            FRHIImageRef RHI = Import::Textures::CreateTextureFromImport(GRenderContext,
                SquareTexturePath.ToString(), false);

            ImTextureRef ImTex = ImGuiRenderer->GetOrCreateImTexture(RHI);

            SquareWhiteTexture = Memory::New<FEntry>();
            SquareWhiteTexture->RHIImage = RHI;
            SquareWhiteTexture->ImTexture = ImTex;
            SquareWhiteTexture->State.store(ETextureState::Ready, std::memory_order_release);
            Images.emplace(SquareTexturePath, SquareWhiteTexture);
        }
    }

    FRHIImageRef FUITextureCache::GetImage(const FName& Path)
    {
        FEntry* Entry = GetOrCreateGroup(Path);
        return Entry ? Entry->RHIImage : nullptr;
    }

    ImTextureRef FUITextureCache::GetImTexture(const FName& Path)
    {
        FEntry* Entry = GetOrCreateGroup(Path);
        return Entry ? Entry->ImTexture : ImTextureRef();
    }

    void FUITextureCache::Clear()
    {
        for (auto& Pair : Images)
        {
            Memory::Delete(Pair.second);
        }
        
        Images.clear();

        if (SquareWhiteTexture)
        {
            SquareWhiteTexture->RHIImage.SafeRelease();
            SquareWhiteTexture->ImTexture = ImTextureRef();
            SquareWhiteTexture->State.store(ETextureState::Empty, std::memory_order_release);
            Memory::Delete(SquareWhiteTexture);
        }
        
        bCleared = true;
    }

    FUITextureCache::FEntry* FUITextureCache::GetOrCreateGroup(const FName& PathName)
    {
        if (bCleared)
        {
            return nullptr;
        }
        
        if (!Paths::Exists(PathName.ToString()))
        {
            return nullptr;
        }
        
        auto Iter = Images.find(PathName);
        if (Iter != Images.end())
        {
            FEntry* Entry = Iter->second;
            if (Entry->State.load(std::memory_order_acquire) == ETextureState::Ready)
            {
                return Entry;
            }

            return SquareWhiteTexture;
        }

        FEntry* NewEntry = Memory::New<FEntry>();
        NewEntry->State.store(ETextureState::Empty, std::memory_order_release);

        auto [InsertedIter, Inserted] = Images.try_emplace(PathName, NewEntry);
        FEntry* Entry = InsertedIter->second;

        if (!Inserted) // Another thread inserted it first
        {
            Memory::Delete(NewEntry);
            
            if (Entry->State.load(std::memory_order_acquire) == ETextureState::Ready)
            {
                return Entry;
            }
            
            return SquareWhiteTexture;
        }

        ETextureState Expected = ETextureState::Empty;
        if (Entry->State.compare_exchange_strong(Expected, ETextureState::Loading, std::memory_order_acq_rel))
        {
            FTaskSystem::Get().ScheduleLambda(1, [Entry, PathName](uint32, uint32, uint32)
            {
                FString PathString = PathName.ToString();
                Entry->RHIImage = Import::Textures::CreateTextureFromImport(GRenderContext, PathString, false);
                Entry->ImTexture = ImGuiX::ToImTextureRef(Entry->RHIImage);

                Entry->State.store(ETextureState::Ready, std::memory_order_release);
            });
        }

        return SquareWhiteTexture;
    }
}
