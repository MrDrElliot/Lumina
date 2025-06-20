#include "EditorToolModal.h"

namespace Lumina
{
    FEditorModalManager::~FEditorModalManager()
    {
        if (ActiveModal != nullptr)
        {
            Memory::Delete(ActiveModal);
        }
    }

    void FEditorModalManager::CreateModalDialogue(const FString& Title, ImVec2 Size, TFunction<bool(const FUpdateContext&)> DrawFunction)
    {
        if (ActiveModal != nullptr)
        {
            return;
        }
        
        ActiveModal = Memory::New<FEditorToolModal>(Title, Size);
        ActiveModal->DrawFunction = DrawFunction;
        
    }

    void FEditorModalManager::CreateModalDialogue(const FString& Title, ImVec2 Size, FEditorToolModal* Modal)
    {
        if (ActiveModal != nullptr)
        {
            return;
        }
        
        ActiveModal = Modal;
        
    }

    void FEditorModalManager::DrawDialogue(const FUpdateContext& UpdateContext)
    {
        if (ActiveModal == nullptr)
        {
            return;
        }
        
        ImGui::OpenPopup(ActiveModal->Title.c_str());
        
        ImGui::SetNextWindowSize(ActiveModal->Size);

        if (ImGui::BeginPopupModal(ActiveModal->Title.c_str(), nullptr, ImGuiWindowFlags_NoResize))
        {
            if (ActiveModal->DrawModal(UpdateContext))
            {
                ImGui::CloseCurrentPopup();
                Memory::Delete(ActiveModal);
                ActiveModal = nullptr;
            }

            ImGui::EndPopup();
        }
    }
}
