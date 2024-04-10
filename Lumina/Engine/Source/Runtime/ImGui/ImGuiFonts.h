#pragma once


#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "imgui.h"


namespace Lumina::Font
{
    
        struct FImGuiFont
        {
            std::string Name;
            std::string FilePath;
            ImFont* Font;
            float Size = 16.0f;
            
            operator ImFont*()
            {
                return Font;
            }
            
        };
    

        FImGuiFont Add(const std::string& InFile, const std::string& Name = "None", bool bDefault = false);
        FImGuiFont Get(const std::string& Name);

        void PushFont(const std::string& Name);
        void PopFont();
        
}
