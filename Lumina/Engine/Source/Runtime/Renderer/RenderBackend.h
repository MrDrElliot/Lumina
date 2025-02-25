#pragma once

namespace Lumina
{
    class IRenderDevice;

    class IRenderBackend
    {
    public:
        virtual ~IRenderBackend() = default;

        virtual void Initialize() = 0;
        

    protected:
        
        IRenderDevice*      RenderDevice = nullptr;
        
    };
}
