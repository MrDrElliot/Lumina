#include "PrimitiveDrawManager.h"

#include "Renderer/RHIIncl.h"
#include "Core/Math/Color.h"

namespace Lumina
{
    void FPrimitiveDrawManager::Initialize()
    {
    }

    void FPrimitiveDrawManager::Deinitialize()
    {
    }

    void FPrimitiveDrawManager::StartDraw()
    {
    }

    void FPrimitiveDrawManager::EndDraw()
    {
    }

    void FPrimitiveDrawManager::DrawLine(const FVector3D& Start, const FVector3D& End, const FColor& Color)
    {
    }

    void FPrimitiveDrawManager::Draw(const FViewport& Viewport)
    {
        BatchedElements.Draw(Viewport);
    }
}
