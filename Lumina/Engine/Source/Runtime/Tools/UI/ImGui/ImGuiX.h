#pragma once
#include "imgui.h"
#include "Assets/AssetPath.h"
#include "Core/Functional/Function.h"
#include "glm/glm.hpp"
#include "Platform/GenericPlatform.h"


#include "Containers/Array.h"
#include "Module/API.h"
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

    LUMINA_API void ObjectSelector(const FARFilter& Filter, CObject*& OutSelected);
    
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
