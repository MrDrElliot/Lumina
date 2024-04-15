#pragma once
#include <memory>

#include "RenderAPI.h"


namespace Lumina
{
    class FWindow;
    class FRenderContext;

    struct FRenderConfig
    {
        int FramesInFlight;
        FWindow* Window;
    };
    
    class FRenderer
    {
    public:


        static void Init(const FRenderConfig& InConfig);
        static void Shutdown();
        
        static void BeginFrame();
        static void EndFrame();
        static void BeginRender();
        
    private:

        static FRenderAPI* RenderAPI;
    };
}
