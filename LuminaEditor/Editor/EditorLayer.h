#pragma once
#include <memory>
#include <vector>
#include "Source/Runtime/ApplicationCore/Layer.h"



namespace Lumina
{
    class FEditorPanel;
    
    class FEditorLayer : public FLayer
    {
    public:

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(float DeltaTime) override;
        void OnImGuiRender(double DeltaTime) override;
        void OnEvent(FEvent& InEvent) override;

        void AddEditorLayer(FEditorPanel* NewPanel);


    private:

        std::vector<std::shared_ptr<FEditorPanel>> EditorPanels;
    
    };
}

