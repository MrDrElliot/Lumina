#pragma once
#include "imgui.h"
#include "ImGuiDesignIcons.h"
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

    LUMINA_API inline bool IconButton(char const* pIcon, char const* pLabel, const ImColor& iconColor = ImGui::ColorConvertFloat4ToU32( ImGui::GetStyle().Colors[ImGuiCol_Text] ), ImVec2 const& size = ImVec2(0, 0), bool shouldCenterContents = false )
    {
        return ButtonEx(pIcon, pLabel, size, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_Button]), iconColor, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_Text]), shouldCenterContents);
    }

    LUMINA_API TPair<bool, uint32> DirectoryTreeViewRecursive(const std::filesystem::path& Path, uint32* Count, int* SelectionMask);

    LUMINA_API void SameLineSeparator( float width = 0, const ImColor& color = ImColor(0));
    
    template<typename T>
    bool ObjectSelector(FARFilter& Filter, T*& OutSelected)
    {
        
        static ImGuiTextFilter SearchFilter;
        Filter.ClassNames.push_back(T::StaticClass()->GetName().ToString());

        ImGui::Dummy(ImVec2(0, 10));
        
        if (ImGui::Button("Clear", ImVec2(ImGui::GetContentRegionAvail().x / 2, 0)))
        {
            OutSelected = nullptr;
            return true;
        }

        ImGui::SameLine();
        
        if (ImGui::Button("Close", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
        {
            return true;
        }
        
        SearchFilter.Draw("##Search", ImGui::GetContentRegionAvail().x);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 10));

        
        TVector<FAssetData> FilteredAssets;
        GEngine->GetEngineSubsystem<FAssetRegistry>()->GetAssets(Filter, FilteredAssets);
        
        for (const FAssetData& Asset : FilteredAssets)
        {
            
            if (!SearchFilter.PassFilter(Asset.Name.c_str()))
            {
                continue;
            }
            
            const char* NameStr = Asset.Name.c_str();

            auto OnSelected = [&] (const FAssetData& Data)
            {
                FString VirtualPath = Paths::ConvertToVirtualPath(Data.Path);
                VirtualPath += "." + Data.Name.ToString();
                FName AssetName = VirtualPath.c_str();
                OutSelected = LoadObject<T>(AssetName);
                
                ImGui::PopID();
                ImGui::CloseCurrentPopup();
                ImGui::EndGroup();
            };
            
            ImGui::BeginGroup();
            ImGui::PushID(Asset.Path.c_str());
            if (ImGui::Button("Asset", ImVec2(64, 64)))
            {
                OnSelected(Asset);
                return true;
            }
            
            ImGui::SameLine();
            
            if (ImGui::Selectable(NameStr))
            {
                OnSelected(Asset);
                return true;
            }

            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("%s", Asset.Path.c_str());
            }

            ImGui::PopID();
            ImGui::EndGroup();
        }
        
        return false;
    }

    namespace Notifications
    {
        LUMINA_API void NotifyInfo( const char* format, ... );
        LUMINA_API void NotifySuccess( const char* format, ... );
        LUMINA_API void NotifyWarning( const char* format, ... );
        LUMINA_API void NotifyError( const char* format, ... );
    }

    namespace MessageBoxes
    {
        LUMINA_API bool ShowMessageBox(const char* Title, const char* Format, ...);
    }
    
    struct LUMINA_API ApplicationTitleBar
    {
        constexpr static float s_windowControlButtonWidth = 45;
        constexpr static float s_minimumDraggableGap = 24; // Minimum open gap left open to allow dragging
        constexpr static float s_sectionPadding = 8; // Padding between the window frame/window controls and the menu/control sections

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
