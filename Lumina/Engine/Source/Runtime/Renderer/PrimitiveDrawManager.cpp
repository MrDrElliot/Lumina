#include "PrimitiveDrawManager.h"

#include "Renderer/RHIIncl.h"
#include "Core/Math/Math.h"
#include "Core/Math/Color.h"

namespace Lumina
{
    void FPrimitiveDrawManager::Initialize()
    {
        BatchedElements.Initialize();
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

    void FPrimitiveDrawManager::DrawQuad(const FVector3D& Center, float Size, const FColor& Color)
    {
        // Calculate half the size of the quad
        FVector3D HalfSize(Size * 0.5f, Size * 0.5f, 0.0f); // 2D quad (Z = 0)

        // Define the 4 vertices of the quad (in a plane)
        FVector3D TopLeft = Center + FVector3D(-HalfSize.X, HalfSize.Y, 0.0f);
        FVector3D TopRight = Center + FVector3D(HalfSize.X, HalfSize.Y, 0.0f);
        FVector3D BottomLeft = Center + FVector3D(-HalfSize.X, -HalfSize.Y, 0.0f);
        FVector3D BottomRight = Center + FVector3D(HalfSize.X, -HalfSize.Y, 0.0f);

        // Draw the 4 edges of the quad (forming the perimeter)
        DrawLine(TopLeft, TopRight, Color);
        DrawLine(TopRight, BottomRight, Color);
        DrawLine(BottomRight, BottomLeft, Color);
        DrawLine(BottomLeft, TopLeft, Color);
    }

    void FPrimitiveDrawManager::DrawCube(const FVector3D& Center, float Size, const FColor& Color)
    {
        // Calculate half the size of the cube
        FVector3D HalfSize(Size * 0.5f, Size * 0.5f, Size * 0.5f);

        // Define the 8 vertices of the cube
        FVector3D FrontTopLeft = Center + FVector3D(-HalfSize.X, HalfSize.Y, HalfSize.Z);
        FVector3D FrontTopRight = Center + FVector3D(HalfSize.X, HalfSize.Y, HalfSize.Z);
        FVector3D FrontBottomLeft = Center + FVector3D(-HalfSize.X, -HalfSize.Y, HalfSize.Z);
        FVector3D FrontBottomRight = Center + FVector3D(HalfSize.X, -HalfSize.Y, HalfSize.Z);

        FVector3D BackTopLeft = Center + FVector3D(-HalfSize.X, HalfSize.Y, -HalfSize.Z);
        FVector3D BackTopRight = Center + FVector3D(HalfSize.X, HalfSize.Y, -HalfSize.Z);
        FVector3D BackBottomLeft = Center + FVector3D(-HalfSize.X, -HalfSize.Y, -HalfSize.Z);
        FVector3D BackBottomRight = Center + FVector3D(HalfSize.X, -HalfSize.Y, -HalfSize.Z);

        // Draw the 12 edges of the cube
        // Front face
        DrawLine(FrontTopLeft, FrontTopRight, Color);
        DrawLine(FrontTopRight, FrontBottomRight, Color);
        DrawLine(FrontBottomRight, FrontBottomLeft, Color);
        DrawLine(FrontBottomLeft, FrontTopLeft, Color);

        // Back face
        DrawLine(BackTopLeft, BackTopRight, Color);
        DrawLine(BackTopRight, BackBottomRight, Color);
        DrawLine(BackBottomRight, BackBottomLeft, Color);
        DrawLine(BackBottomLeft, BackTopLeft, Color);

        // Connect the front and back faces
        DrawLine(FrontTopLeft, BackTopLeft, Color);
        DrawLine(FrontTopRight, BackTopRight, Color);
        DrawLine(FrontBottomLeft, BackBottomLeft, Color);
        DrawLine(FrontBottomRight, BackBottomRight, Color);
    }

    void FPrimitiveDrawManager::DrawPoint(const FVector3D& Point, const FColor& Color)
    {
        FSimpleElementVertex VertexStart;
        VertexStart.Position = Point;
        VertexStart.Color = Color;

        BatchedElements.SubmitElement(VertexStart);
    }

    void FPrimitiveDrawManager::DrawLine(const FVector3D& Start, const FVector3D& End, const FColor& Color)
    {
        FSimpleElementVertex VertexStart;
        VertexStart.Position = Start;
        VertexStart.Color = Color;

        BatchedElements.SubmitElement(VertexStart);
        
        FSimpleElementVertex VertexEnd;
        VertexEnd.Position = End;
        VertexEnd.Color = Color;

        BatchedElements.SubmitElement(VertexEnd);

    }

    void FPrimitiveDrawManager::Draw(const FSceneGlobalData& GlobalData)
    {
        BatchedElements.Draw(GlobalData);
    }
}
