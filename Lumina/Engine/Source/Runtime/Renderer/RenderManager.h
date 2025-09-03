#pragma once
#include "Subsystems/Subsystem.h"
#include "Lumina.h"
#include "RHIFwd.h"

namespace Lumina
{
    class FUITextureCache;
}

namespace Lumina
{
    class IImGuiRenderer;
    class IRenderContext;
}

namespace Lumina
{
    class FRenderManager : public ISubsystem
    {
    public:

        void Initialize() override;
        void Deinitialize() override;

        void FrameStart(const FUpdateContext& UpdateContext);
        void FrameEnd(const FUpdateContext& UpdateContext);


        #if WITH_DEVELOPMENT_TOOLS
        IImGuiRenderer* GetImGuiRenderer() const { return ImGuiRenderer; }
        FUITextureCache* GetTextureCache() const { return TextureCache; }
        #endif
        
        uint32 GetCurrentFrameIndex() const { return CurrentFrameIndex; }
        
    private:

        #if WITH_DEVELOPMENT_TOOLS
        IImGuiRenderer*     ImGuiRenderer = nullptr;
        FUITextureCache*    TextureCache = nullptr;
        #endif
        
        uint8               CurrentFrameIndex = 0;
        
    };
}
