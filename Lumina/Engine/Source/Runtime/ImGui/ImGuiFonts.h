#pragma once

#include "imgui.h"
#include "Containers/String.h"


namespace Lumina::Font
{
    
        struct FImGuiFont
        {
            FString Name;
            FString FilePath;
            ImFont* Font;
            float Size = 16.0f;
            
            operator ImFont*()
            {
                return Font;
            }
            
        };
    

        FImGuiFont Add(const FString& InFile, const FString& Name = "None", bool bDefault = false);
        FImGuiFont Get(const FString& Name);

        void PushFont(const FString& Name);
        void PopFont();
        
}
