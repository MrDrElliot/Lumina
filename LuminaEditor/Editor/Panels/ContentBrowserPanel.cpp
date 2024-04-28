#include "ContentBrowserPanel.h"

#include "imgui_internal.h"
#include <filesystem>

#include "Source/Runtime/Assets/AssetRegistry/AssetRegistry.h"
#include "Source/Runtime/Assets/Factories/TextureFactory/TextureFactory.h"
#include "Source/Runtime/Events/ApplicationEvent.h"
#include "Source/Runtime/Events/Event.h"
#include "Source/Runtime/ImGui/ImGuiRenderAPI.h"
#include "Source/Runtime/ImGui/ImGuiRenderer.h"
#include "Source/Runtime/Input/Input.h"
#include "Source/Runtime/Log/Log.h"
#include "Source/Runtime/Renderer/Image.h"
#include "Source/Runtime/Renderer/RHI/Vulkan/VulkanRenderContext.h"

namespace fs = std::filesystem;

namespace Lumina
{
    FContentBrowserPanel::FContentBrowserPanel()
    : FolderTexture(nullptr)
    {
        Name = "Content Browser";
    }

    FContentBrowserPanel::~FContentBrowserPanel()
    {
        
    }

    void FContentBrowserPanel::OnAdded()
    {
        FImageSpecification Specs = FImageSpecification::Default();
        Specs.Extent = {512, 512, 1};
        Specs.Format = EImageFormat::RGBA32_UNORM;
        Specs.Usage = EImageUsage::TEXTURE;
        Specs.Pixels = FTextureFactory::ImportFromSource("Resources/Icons/ContentBrowser/Folder.png");
        Specs.MipLevels = 1;
        Specs.ArrayLayers = 1;

        FolderImage = FImage::Create(Specs);
        
        FolderTexture = FImGuiRenderer::CreateImGuiTexture(FolderImage, FVulkanRenderContext::GetLinearSampler(), {512, 512}, 0, true);

    }

    void FContentBrowserPanel::OnRemoved()
    {
    }

    void FContentBrowserPanel::OnEvent(FEvent& Event)
    {
        FEventDispatcher Dispatcher(Event);
        Dispatcher.Dispatch<WindowDropEvent>(BIND_EVENT_FN(OnWindowDrop));
    }

    bool FContentBrowserPanel::OnWindowDrop(WindowDropEvent& Event)
    {
        glm::vec2 MousePos = Input::GetMousePos();
        ImVec2 MousePosGui;
        MousePosGui.x = MousePos.x;
        MousePosGui.y = MousePos.y;
        if(WindowSize.Contains(MousePosGui))
        {
            bHasBeenDropped = true;
        }
        return false;
    }

    void FContentBrowserPanel::OnNewScene()
    {
    }

    void FContentBrowserPanel::OnRender(double DeltaTime)
    {
        ImGui::Begin(Name.c_str());

                
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.8f, 0.2f, 1.0f));
        if (ImGui::Button("Import Asset"))
        {
            ImGui::OpenPopup("Import Asset");  
        }

        // Popup modal
        char filePath[1024] = "";  // Buffer to store the file path
        char name[1024] = ""; // New Asset name.
        if (ImGui::BeginPopupModal("Import Asset", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Enter the asset name:");
            ImGui::InputText("##name", name, IM_ARRAYSIZE(name));
            ImGui::Text("Enter the file path to import:");
            ImGui::InputText("##filepath", filePath, IM_ARRAYSIZE(filePath));

            if (ImGui::Button("OK"))
            {
                ImGui::CloseCurrentPopup();
            }
            
            ImGui::SameLine();
            
            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::PopStyleColor();

        
        float leftPaneWidth = 200.0f;
        float rightPaneWidth = ImGui::GetContentRegionAvail().x - leftPaneWidth;
        
        ImGui::BeginChild("Directories", ImVec2(leftPaneWidth, 0), true);
        for (auto& p : fs::directory_iterator(CurrentPath))
        {
            if (p.is_directory()) {
                if (ImGui::Selectable(p.path().filename().string().c_str(), SelectedDirectory == p.path()))
                {
                    SelectedDirectory = p.path().string();
                }
            }
        }
        ImGui::EndChild();
        
        ImGui::SameLine();
        
        ImGui::BeginChild("Contents", ImVec2(rightPaneWidth, 0), true);
        if (!SelectedDirectory.empty())
        {
            float paneWidth = ImGui::GetContentRegionAvail().x;
            float buttonSize = 100.0f;
            float cellSize = buttonSize + ImGui::GetStyle().ItemSpacing.x;
            int itemsPerRow = std::max(1, int(paneWidth / cellSize));
        
            int itemIndex = 0;
            for (auto& p : std::filesystem::directory_iterator(SelectedDirectory))
            {
                if (itemIndex % itemsPerRow != 0)
                {
                    ImGui::SameLine();
                }
        
                ImGui::PushID(itemIndex);
                ImGui::BeginGroup();

                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f); // Remove border
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0)); // Transparent background for button
                if (p.is_directory())
                {
                    if (ImGui::ImageButton(FolderTexture, ImVec2(buttonSize, buttonSize), ImVec2(0, 1), ImVec2(1, 0)))
                    {
                        SelectedDirectory = p.path().string();
                    }
                    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + buttonSize);
                    ImGui::TextWrapped("%s", p.path().filename().string().c_str());
                    ImGui::PopTextWrapPos();
                }
                else if(p.path().extension().string() == ".lum")
                {
                    ImGui::ImageButton(FolderTexture, ImVec2(buttonSize, buttonSize), ImVec2(0, 1), ImVec2(1, 0));
                    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + buttonSize);
                    ImGui::TextWrapped("%s", p.path().filename().string().c_str());
                    ImGui::PopTextWrapPos();
                }
                
                ImGui::PopStyleColor(1);
                ImGui::PopStyleVar();
                
                ImGui::EndGroup();
                ImGui::PopID();
        
                ++itemIndex;
            }
        }
        ImGui::EndChild();
        
        ImGui::End();
    }
}
