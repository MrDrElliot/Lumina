#pragma once
#include "imgui.h"
#include "Platform/WindowsPlatform.h"

namespace Lumina
{
    class CClass;
}

namespace Lumina
{
    class FSubsystemManager;
}

namespace Lumina
{
    class IEditorToolContext
    {
    public:

        IEditorToolContext() = default;
        virtual ~IEditorToolContext() = default;

        FORCEINLINE FSubsystemManager* GetSubsystemManager() const { return SubsystemManager; }

        virtual void PushModal(const FString& Title, ImVec2 Size, TFunction<bool(const FUpdateContext&)> DrawFunction) = 0;

        virtual void OpenAssetEditor(const FString& InPath) = 0;

    protected:
    
    public:

        FAssetRegistry*               AssetRegistry = nullptr;
        FSubsystemManager*            SubsystemManager = nullptr;
    };
}
