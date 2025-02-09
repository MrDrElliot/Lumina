#include "ContentBrowserWindow.h"
#include <string>
#include <filesystem>
#include "ImGuiAssetImporter.h"
#include "ImGui_MeshImporter.h"
#include "ImGui_TextureImporter.h"
#include "Assets/AssetTypes/Textures/Texture.h"
#include "Assets/Factories/TextureFactory/TextureFactory.h"
#include "ImGui/ImGuiRenderer.h"
#include "Paths/Paths.h"
#include "Project/Project.h"
#include "Renderer/Image.h"
#include "Renderer/RenderContext.h"
#include <Renderer/Material.h>

#include "Panels/Assets/Material/MaterialEditorPanel.h"

#if 0
// Convert from std::wstring to std::string
std::string WStringToString(const std::wstring& wstr)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &str[0], size_needed, NULL, NULL);
    return str;
}


// Function to open a native Windows file dialog and get the selected file path
std::string OpenFileDialog()
{
    OPENFILENAME ofn;           // Common dialog box structure
    wchar_t fileName[MAX_PATH] = L""; // Buffer for file name (wide character)

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;       // No owner window needed for console apps
    ofn.lpstrFile = fileName;   // File path output (wide char)
    ofn.nMaxFile = sizeof(fileName) / sizeof(wchar_t);
    ofn.lpstrFilter = L"All Files\0*.*\0Text Files\0*.TXT\0";
    ofn.nFilterIndex = 1;       // Start with the first filter
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn))
    {
        return WStringToString(fileName);  // Return wide string (wchar_t*)
    }
    return WStringToString(L"");  // Return empty if the dialog was cancelled
}
#endif

namespace Lumina
{
    void ContentBrowserWindow::OnAttach()
    {

        SelectedDirectory = Project::GetProjectContentDirectory().string() + "/Content/";
        
        FolderTexture = FTextureFactory::ImportFromSource(Paths::GetEngineInstallDirectory() / "Applications/LuminaEditor/Resources/Icons/ContentBrowser/Folder.png");
        FolderTexture->SetFriendlyName("Folder Texture");
        ImFolderTexture = FImGuiRenderer::CreateImGuiTexture(FolderTexture, FRenderer::GetLinearSampler(), {512, 512}, 0, true);
    
        AssetTexture = FTextureFactory::ImportFromSource(Paths::GetEngineInstallDirectory() / "Applications/LuminaEditor/Resources/Icons/ContentBrowser/StaticMeshIcon.png");
        AssetTexture->SetFriendlyName("Asset Texture");
        ImAssetTexture = FImGuiRenderer::CreateImGuiTexture(AssetTexture, FRenderer::GetLinearSampler(), {512, 512}, 0, true);

    
        fileDialog = ImGui::FileBrowser(ImGuiFileBrowserFlags_CloseOnEsc | ImGuiFileBrowserFlags_CreateNewDir);
    
        fileDialog.SetTitle("Select Asset to Import");
        fileDialog.SetTypeFilters({ ".png", ".gltf", ".jpg" });

        mImGuiImporterMap.try_emplace(EAssetType::Texture,      MakeSharedPtr<ImGui_TextureImporter>());
        mImGuiImporterMap.try_emplace(EAssetType::StaticMesh,   MakeSharedPtr<ImGui_MeshImporter>());
        
        RefreshItems(SelectedDirectory);
    }

    void ContentBrowserWindow::OnDetach()
    {
        FolderTexture->Release();
        AssetTexture->Release();
    }

    void ContentBrowserWindow::OnUpdate(double DeltaTime)
    {
        ImGui::Begin("Content Browser");

        // Import asset button with minimal gray style
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));

        if (ImGui::Button("Actions"))
        {
            ImGui::OpenPopup("ActionsPopup");
        }
        if (ImGui::BeginPopup("ActionsPopup"))
        {
            ImGui::SeparatorText("Content Actions");

            if (ImGui::Selectable("Import Asset"))
            {
                fileDialog.Open();
            }
            
            if (ImGui::Selectable("Refresh"))
            {
                RefreshItems(SelectedDirectory);
            }
                
            if (ImGui::Selectable("New Folder"))
            {
                std::filesystem::create_directory(SelectedDirectory + "/NewFolder");
                RefreshItems(SelectedDirectory);
            }
                
            ImGui::EndPopup();
        }
        
        ImGui::PopStyleColor(3);

        fileDialog.Display();
        if (fileDialog.HasSelected())
        {
            ImGui::OpenPopup("Asset Import Options");
            SelectedFile = fileDialog.GetSelected();
            fileDialog.ClearSelected();
        }

        // Popup modal for asset import options
        if (ImGui::BeginPopupModal("Asset Import Options", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            EAssetType AssetType = FileExtensionToAssetType(SelectedFile.extension().string().c_str());

            if (mImGuiImporterMap.find(AssetType) != mImGuiImporterMap.end())
            {
                mImGuiImporterMap.at(AssetType)->Render(this);
            }
            else
            {
                ImGui::Text("Unsupported file type.");

                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                {
                    ImGui::CloseCurrentPopup();
                    SelectedFile.clear();
                }
            }
            RefreshItems(SelectedDirectory);
            ImGui::EndPopup();
        }

        float leftPaneWidth = 200.0f;
        float rightPaneWidth = ImGui::GetContentRegionAvail().x - leftPaneWidth;

        // Left pane: Directory list
        ImGui::BeginChild("Directories", ImVec2(leftPaneWidth, 0), true);
        if (std::filesystem::exists(Project::GetProjectContentDirectory()))
        {
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
            ImGui::Text("Project Content");
            ImGui::PopFont();

            for (auto& p : std::filesystem::directory_iterator(Project::GetProjectContentDirectory()))
            {
                if (p.is_directory())
                {
                    if (ImGui::Selectable(p.path().filename().string().c_str(), SelectedDirectory == p.path()))
                    {
                        RefreshItems(p.path());
                    }
                }
            }
        }

        ImGui::Separator();

        if (std::filesystem::exists(Paths::GetEngineDirectory()))
        {
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
            ImGui::Text("Engine Content");
            ImGui::PopFont();

            for (auto& p : std::filesystem::directory_iterator(Paths::GetEngineDirectory()))
            {
                if (p.is_directory())
                {
                    if (ImGui::Selectable(p.path().filename().string().c_str(), SelectedDirectory == p.path()))
                    {
                        RefreshItems(p.path());
                    }
                }
            }
        }

        ImGui::EndChild();
        ImGui::SameLine();

        // Right pane: Contents of selected directory
        ImGui::BeginChild("Contents", ImVec2(rightPaneWidth, 0), true);
        
        // Top bar with current directory and refresh button
        if (!SelectedDirectory.empty())
        {
            // Minimal gray style for refresh button
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 4));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
            
            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar();

            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
            ImGui::TextUnformatted(std::filesystem::relative(SelectedDirectory, Project::GetProjectContentDirectory()).string().c_str());
            ImGui::PopStyleColor();
        }

        ImGui::Separator();

        if (!SelectedDirectory.empty())
        {
            RenderContentItems();
        }

        ImGui::EndChild();
        ImGui::End();
    }


    
    void ContentBrowserWindow::OnEvent(FEvent& InEvent)
    {
    }

    void ContentBrowserWindow::RefreshItems(const std::filesystem::path& InPath)
    {
        SelectedDirectory = InPath.empty() ? SelectedDirectory : InPath.string();
        ContentItemEntries.clear();
        
        for (auto& p : std::filesystem::directory_iterator(SelectedDirectory))
        {
            bool bIsLuminaFile = p.path().extension().string() == FILE_EXTENSION;
            if(bIsLuminaFile)
            {
                FAssetHeader Metadata = AssetRegistry::Get()->GetMetadataByPath(p.path());
                ContentItemEntries.push_back(MakeRefPtr<ContentBrowserItem>(p.path(), Metadata, false));
            }
            else if(p.is_directory())
            {
                ContentItemEntries.push_back(MakeRefPtr<ContentBrowserItem>(p.path(), FAssetHeader(), true));
            }
        }
    }

    void ContentBrowserWindow::RenderContentItems()
    {
        float paneWidth = ImGui::GetContentRegionAvail().x;
        float buttonSize = 100.0f;
        float cellSize = buttonSize + ImGui::GetStyle().ItemSpacing.x;
        int itemsPerRow = std::max(1, int(paneWidth / cellSize));
    
        int itemIndex = 0;
        for (TRefPtr<ContentBrowserItem>& Entry : ContentItemEntries)
        {
            std::filesystem::path Path = Entry->GetPath();
            if (itemIndex % itemsPerRow != 0)
            {
                ImGui::SameLine();
            }
        
            ImGui::PushID(itemIndex);
            ImGui::BeginGroup();
    
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

            Entry->OnRender(this);    
            
            ImGui::PopStyleColor(1);
            ImGui::PopStyleVar();
            
            ImGui::EndGroup();
            ImGui::PopID();
    
            ++itemIndex;
        }
    }
}
