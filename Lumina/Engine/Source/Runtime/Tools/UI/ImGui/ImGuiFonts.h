#pragma once

#include "imgui.h"
#include "Containers/String.h"


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
    
    struct FSystemFonts
    {
        static ImFont* s_fonts[static_cast<int32>(EFont::NumFonts)];
    };

    inline void PushFont(EFont font ) 
    {
        ImGui::PushFont( FSystemFonts::s_fonts[static_cast<int8_t>(font)] ); 
    }

    inline void PopFont() { ImGui::PopFont(); }

    inline void PushFontAndColor(EFont font, ImColor& color)
    {
        ImGui::PushFont(FSystemFonts::s_fonts[(int8_t) font] );
        //ImGui::PushStyleColor(ImGuiCol_Text, color);
    }
    
}

