#include "RendererInfoEditorTool.h"

#include "Renderer/RHIIncl.h"

namespace Lumina
{
void FRendererInfoEditorTool::OnInitialize()
{
        CreateToolWindow("Vulkan Device Info", [this](const FUpdateContext& Context, bool bFocused)
        {
            
        });
    }



    void FRendererInfoEditorTool::OnDeinitialize(const FUpdateContext& UpdateContext)
    {
    }
}
