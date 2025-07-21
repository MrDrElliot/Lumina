#pragma once
#include "imgui.h"
#include "Core/Object/Class.h"
#include "Assets/AssetPath.h"
#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Core/Functional/Function.h"
#include "glm/glm.hpp"
#include "Platform/GenericPlatform.h"


#include "Containers/Array.h"
#include "Core/Engine/Engine.h"
#include "Core/Object/ObjectCore.h"
#include "Module/API.h"
#include "Paths/Paths.h"
#include "Platform/WindowsPlatform.h"

namespace Lumina
{
    struct FARFilter;
    class CClass;
}

namespace Lumina::ImGuiX
{
    struct LUMINA_API FImGuiImageInfo
    {
        FORCEINLINE bool IsValid() const { return ID != 0; }
        
        ImTextureID     ID = 0;
        ImVec2          Size = ImVec2(0, 0);
    };
    
    //--------------------------------------------------------------
    // Generic draw helpers...
    //--------------------------------------------------------------

    LUMINA_API void ItemTooltip(const char* fmt, ...);

    LUMINA_API void TextTooltip(const char* fmt, ...);

    LUMINA_API bool ButtonEx(char const* pIcon, char const* pLabel, ImVec2 const& size = ImVec2( 0, 0 ), const ImColor& backgroundColor = ImGui::ColorConvertFloat4ToU32( ImGui::GetStyle().Colors[ImGuiCol_Button] ), const ImColor& iconColor = ImGui::ColorConvertFloat4ToU32( ImGui::GetStyle().Colors[ImGuiCol_Text] ), const ImColor& foregroundColor = ImGui::ColorConvertFloat4ToU32( ImGui::GetStyle().Colors[ImGuiCol_Text] ), bool shouldCenterContents = false );

    LUMINA_API inline bool FlatButton( char const* pLabel, ImVec2 const& size = ImVec2( 0, 0 ), const ImColor& foregroundColor = ImGui::ColorConvertFloat4ToU32( ImGui::GetStyle().Colors[ImGuiCol_Text] ) )
    {
        return ButtonEx( nullptr, pLabel, size, ImColor(0), ImColor(0), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_Text]));
    }

    LUMINA_API TPair<bool, uint32> DirectoryTreeViewRecursive(const std::filesystem::path& Path, uint32* Count, int* SelectionMask);

    LUMINA_API void SameLineSeparator( float width = 0, const ImColor& color = ImColor(0));

    LUMINA_API void DrawObjectProperties(CObject* Object);

    template<typename T>
    bool ObjectSelector(FARFilter& Filter, T*& OutSelected)
    {
        static ImGuiTextFilter SearchFilter;
        Filter.ClassNames.push_back(T::StaticClass()->GetName().ToString());
        
        const char* Text = "Select Object";
        float WindowWidth = ImGui::GetContentRegionAvail().x;
        float TextWidth = ImGui::CalcTextSize(Text).x;

        if (ImGui::Button("Clear", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
        {
            OutSelected = nullptr;
            ImGui::EndPopup();
            return false;
        }
        
        ImGui::SetCursorPosX((WindowWidth - TextWidth) * 0.5f);
        ImGui::Text("%s", Text);        ImGui::Separator();

        SearchFilter.Draw("##", ImGui::GetContentRegionAvail().x);
        ImGui::Separator();

        TVector<FAssetData> FilteredAssets;
        GEngine->GetEngineSubsystem<FAssetRegistry>()->GetAssets(Filter, FilteredAssets);

        if (ImGui::BeginTable("AssetTable", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollY))
        {
            ImGui::TableSetupColumn("Asset", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            
            for (const FAssetData& Asset : FilteredAssets)
            {
                if (!SearchFilter.PassFilter(Asset.Name.c_str()))
                {
                    continue;
                }

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                const char* NameStr = Asset.Name.c_str();
                float TextWidth = ImGui::CalcTextSize(NameStr).x;
                float ColumnWidth = ImGui::GetColumnWidth();
                float SelectableWidth = TextWidth + ImGui::GetStyle().FramePadding.x * 2.0f;

                float Padding = (ColumnWidth - SelectableWidth) * 0.5f;
                if (Padding > 0.0f)
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + Padding);

                // Limit width of the selectable to just fit the text
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, ImGui::GetStyle().FramePadding.y));
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, ImGui::GetStyle().ItemSpacing.y));

                ImGui::PushID(Asset.Path.c_str());

                if (ImGui::Selectable(NameStr, false, 0, ImVec2(SelectableWidth, 0)))
                {
                    FString VirtualPath = Paths::ConvertToVirtualPath(Asset.Path);
                    VirtualPath += "." + Asset.Name.ToString();
                    FName AssetName = VirtualPath.c_str();
                    OutSelected = LoadObject<T>(AssetName);
                    ImGui::CloseCurrentPopup();
                    ImGui::PopStyleVar(2);
                    ImGui::PopID();
                    break;
                }

                if (ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip("%s", Asset.Path.c_str());
                }

                ImGui::PopID();

                ImGui::PopStyleVar(2);
            }

            ImGui::EndTable();
        }
        
        return true;
    }
    
    struct LUMINA_API ApplicationTitleBar
    {
        constexpr static float const s_windowControlButtonWidth = 45;
        constexpr static float const s_minimumDraggableGap = 24; // Minimum open gap left open to allow dragging
        constexpr static float const s_sectionPadding = 8; // Padding between the window frame/window controls and the menu/control sections

        static inline float GetWindowsControlsWidth() { return s_windowControlButtonWidth * 3; }
        static void DrawWindowControls();

    public:

        // This function takes two delegates and sizes each representing the title bar menu and an extra optional controls section
        void Draw(TFunction<void()>&& menuSectionDrawFunction = TFunction<void()>(), float menuSectionWidth = 0, TFunction<void()>&& controlsSectionDrawFunction = TFunction<void()>(), float controlsSectionWidth = 0);

        // Get the screen space rectangle for this title bar
        glm::vec4 const& GetScreenRectangle() const { return Rect; }

    private:

        glm::vec4 Rect = glm::vec4(0.0f);
    };
    
}
