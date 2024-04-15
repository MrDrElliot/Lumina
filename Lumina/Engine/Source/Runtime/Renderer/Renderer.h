#pragma once
#include <memory>


namespace Lumina
{
    class FRenderContext;
    
    class Renderer
    {
    public:

    private:

        std::unique_ptr<FRenderContext> RenderContext;
    };
}
