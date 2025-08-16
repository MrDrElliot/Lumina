#pragma once
#include "imgui.h"
#include "Core/UpdateContext.h"
#include "Core/Functional/Function.h"
#include "Platform/WindowsPlatform.h"

namespace Lumina
{
    class CClass;
    class FSubsystemManager;
    class FAssetRegistry;
}

namespace Lumina
{
    class IEditorToolContext
    {
    public:

        IEditorToolContext() = default;
        virtual ~IEditorToolContext() = default;

        FSubsystemManager* GetSubsystemManager() const { return SubsystemManager; }

        virtual void PushModal(const FString& Title, ImVec2 Size, TFunction<bool(const FUpdateContext&)> DrawFunction) = 0;

        virtual void OpenAssetEditor(CObject* InAsset) = 0;

        /** Called just before an asset is marked for destroy, mostly to close any asset editors that may be using it */
        virtual void OnDestroyAsset(CObject* InAsset) = 0;

        

    protected:
    
    public:

        FAssetRegistry*               AssetRegistry = nullptr;
        FSubsystemManager*            SubsystemManager = nullptr;

    };
}
