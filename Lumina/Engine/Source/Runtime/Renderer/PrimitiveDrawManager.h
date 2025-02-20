#pragma once

#include "BatchedElements.h"
#include "Viewport.h"
#include "Core/Math/Math.h"
#include "Core/Math/Color.h"


namespace Lumina
{
    class FPrimitiveDrawManager
    {
    public:

        void Initialize();
        void Deinitialize();

        void StartDraw();
        void EndDraw();
        
        void DrawLine(const FVector3D& Start, const FVector3D& End, const FColor& Color);
        
        void Draw(const FViewport& Viewport);

    private:

        FBatchedElements BatchedElements;
        
    };
}
