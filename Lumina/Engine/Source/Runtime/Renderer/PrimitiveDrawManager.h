#pragma once

#include "BatchedElements.h"
#include "Viewport.h"
#include "Core/Math/Math.h"
#include "Core/Math/Color.h"
#include "Scene/SceneRenderTypes.h"


namespace Lumina
{
    class FScene;
}

namespace Lumina
{
    class FPrimitiveDrawManager
    {
    public:

        FPrimitiveDrawManager() = default;
        
        FPrimitiveDrawManager(const FPrimitiveDrawManager&) = delete;
        FPrimitiveDrawManager& operator = (const FPrimitiveDrawManager&) = delete;

        void Initialize();
        void Deinitialize();

        void StartDraw();
        void EndDraw();


        void DrawQuad(const FVector3D& Center, float Size, const FColor& Color);
        void DrawCube(const FVector3D& Center, float Size, const FColor& Color);
        void DrawPoint(const FVector3D& Point, const FColor& Color);
        void DrawLine(const FVector3D& Start, const FVector3D& End, const FColor& Color);
        
        void Draw(const FSceneGlobalData& GlobalData);

    private:

        FBatchedElements BatchedElements;
        
    };
}
