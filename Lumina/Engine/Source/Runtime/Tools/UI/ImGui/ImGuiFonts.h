#pragma once

#include "imgui.h"
#include "Containers/String.h"
#include "Module/API.h"


namespace Lumina::ImGuiX::Font
{

#define ROBOTO_LIGHT   "Roboto-Light"
#define ROBOTO_MEDDIUM "Roboto-Medium"
#define ROBOTO_BOLD    "Roboto-Bold"
#define ROBOTO_ITALIC  "Roboto-Italic"


    enum class EFont : uint8
    {
        Tiny,
        TinyBold,
        Small,
        SmallBold,
        Medium,
        MediumBold,
        Large,
        LargeBold,

        NumFonts,
        Default = Medium,
    };
    
    LUMINA_API extern ImFont* GFonts[static_cast<int32>(EFont::NumFonts)];

    LUMINA_API INLINE void PushFont(EFont font) 
    {
        ImFont* Font = GFonts[static_cast<int8>(font)];
        ImGui::PushFont(Font); 
    }

    LUMINA_API INLINE void PopFont() { ImGui::PopFont(); }

    LUMINA_API INLINE void PushFontAndColor(EFont font, ImColor& color)
    {
        ImGui::PushFont(GFonts[(int8) font] );
        //ImGui::PushStyleColor(ImGuiCol_Text, color);
    }
    
}

