#pragma once

#include <Core/Layer.h>
#include "Memory/RefCounted.h"

namespace Lumina
{
    class EditorImGuiWindow;
    class FEditorLayer : public FLayer
    {
    public:
        explicit FEditorLayer(const std::string& InName)
            : FLayer(InName)
        {
        }

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(double DeltaTime) override;
        void OnEvent(FEvent& InEvent) override;
        void ImGuiRender(double DeltaTime) override;
        TRefPtr<EditorImGuiWindow> GetEditorWindowByName(const LString& Name);
        
    private:
            
    };
}

