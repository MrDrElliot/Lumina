#include "ImGuiFonts.h"

#include <filesystem>

#include "Source/Runtime/Log/Log.h"

namespace Lumina::Font
{

#define FONT_ROBOTO_LIGHT   "Roboto-Light"
#define FONT_ROBOTO_MEDDIUM "Roboto-Medium"
#define FONT_ROBOTO_BOLD    "Roboto-Bold"
#define FONT_ROBOTO_ITALIC  "Roboto-Italic"

    
    static std::vector<FImGuiFont> Fonts;

    
    FImGuiFont Add(const std::string& InFile, const std::string& Name, bool bDefault)
    {
        FImGuiFont NewFont;
        ImGuiIO& IO = ImGui::GetIO();

        if(std::filesystem::exists(InFile))
        {
                ImFontConfig imguiFontConfig;
                imguiFontConfig.MergeMode = false;
            
                NewFont.Name = Name;
                NewFont.FilePath = InFile;
                NewFont.Font = IO.Fonts->AddFontFromFileTTF(InFile.c_str(), 16.0f, &imguiFontConfig, IO.Fonts->GetGlyphRangesDefault());
                if(NewFont.Font == nullptr)
                {
                    LE_LOG_CRITICAL("Failed to initialize font!");
                }
                LE_LOG_INFO("New Font Added: {0}", Name);

                Fonts.push_back(NewFont);
                return NewFont;
        }

        LE_LOG_ERROR("[.ttf] file not found!");
    }
    
    FImGuiFont Get(const std::string& Name)
    {
        for (auto Font : Fonts)
        {
            if(Font.Name == Name)
            {
                return Font;
            }
        }
    }

    void PushFont(const std::string& Name)
    {
        FImGuiFont Font = Get(Name);
        ImGui::PushFont(Font);
    }

    void PopFont()
    {
        ImGui::PopFont();
    }
}
