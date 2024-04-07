#pragma once
#include <memory>
#include <vulkan/vulkan_core.h>

#include "Source/Runtime/ApplicationCore/Application.h"


/*
 *  RenderContext is the "manager" class for the renderer to interface.
 *  There is only one per application.
 *
 */

namespace Lumina
{
    class FWindow;
    class FApplication;


    
    class FRenderContext
    {
    public:

        FRenderContext() = default;
        virtual ~FRenderContext() = default;
        
        FRenderContext(const FRenderContext &) = delete;
        FRenderContext(FRenderContext &&) = delete;
        FRenderContext &operator=(const FRenderContext &) = delete;
        FRenderContext &operator=(FRenderContext &&) = delete;

        virtual void ImGuiDraw(float DeltaTime) = 0;
        virtual void Draw(float DeltaTime) = 0;

        static FRenderContext* Create();


        template<typename T>
        static T* Get()
        {
            //@ TODO Not allowed?
            //static_assert(std::is_base_of<FRenderContext, T>::Value, "T Must be derived from FRenderContext");
            return dynamic_cast<T*>(FApplication::Get().GetRenderContext<T>());
        }
        
    private:
        
        virtual void InternalInit() = 0;
    
    private:
        
    };
}
