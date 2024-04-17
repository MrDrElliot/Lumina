#pragma once

#include "Renderer.h"


namespace Lumina
{
    class FWindow;
    class FApplication;


    
    class FRenderContext
    {
    public:

        FRenderContext() = default;
        virtual ~FRenderContext() = default;

        static FRenderContext* Create(const FRenderConfig& InConfig);

        virtual void Destroy() = 0;
    
    private:
        
    };
}
