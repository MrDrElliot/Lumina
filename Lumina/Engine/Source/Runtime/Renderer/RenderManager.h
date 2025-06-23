#pragma once
#include "Subsystems/Subsystem.h"
#include "Lumina.h"
#include "RHIFwd.h"

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

        void Initialize(FSubsystemManager& Manager) override;
        void Deinitialize() override;

        void FrameStart(const FUpdateContext& UpdateContext);
        void FrameEnd(const FUpdateContext& UpdateContext);

        
        template<typename T>
        requires (std::is_base_of_v<IRenderContext, T>)
        T* GetRenderContext() const;


        #if WITH_DEVELOPMENT_TOOLS
        INLINE IImGuiRenderer* GetImGuiRenderer() const { return ImGuiRenderer; }
        #endif
        
        
        FORCEINLINE IRenderContext* GetRenderContext() const { return RenderContext; }

        INLINE FRHISamplerRef GetNearestSampler() const { return NearestSampler; }
        INLINE FRHISamplerRef GetLinearSampler() const { return LinearSampler; }
        
    private:

        #if WITH_DEVELOPMENT_TOOLS
        IImGuiRenderer*     ImGuiRenderer =         nullptr;
        #endif
        
        IRenderContext*     RenderContext = nullptr;
        uint8               CurrentFrameIndex = 1;
        
        FRHISamplerRef      NearestSampler;
        FRHISamplerRef      LinearSampler;
        
    };

    template <typename T>
    requires (std::is_base_of_v<IRenderContext, T>)
    T* FRenderManager::GetRenderContext() const
    {
        return static_cast<T*>(RenderContext);
    }
}
