#pragma once
#include <memory>
#include <string>
#include "imgui.h"

namespace Lumina
{
    class ContentBrowserWindow;

    class ImGuiAssetImporter : public std::enable_shared_from_this<ImGuiAssetImporter>
    {
    public:

        virtual void Render(ContentBrowserWindow* ContentBrowser) = 0;


    protected:
    
        char Name [256] = "NONE";

    
    };
}
