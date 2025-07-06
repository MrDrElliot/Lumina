#include "MaterialNode_TextureSample.h"
#include "Assets/AssetTypes/Textures/Texture.h"
#include "Core/Object/Cast.h"
#include "UI/Tools/NodeGraph/Material/MaterialCompiler.h"

namespace Lumina
{
    void CMaterialExpression_TextureSample::Serialize(FArchive& Ar)
    {
        CMaterialExpression::Serialize(Ar);

        Ar << Texture;
    }

    void CMaterialExpression_TextureSample::BuildNode()
    {
        CMaterialOutput* ValuePin = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "RGBA", ENodePinDirection::Output, EMaterialInputType::Texture));
        ValuePin->SetShouldDrawEditor(true);
        ValuePin->SetHideDuringConnection(false);
        ValuePin->SetPinName("RGBA");
        ValuePin->InputType = EMaterialInputType::Texture;

        //@ TODO Fixup ugly fat nodes.
        //CMaterialOutput* R = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "R", ENodePinDirection::Output, EMaterialInputType::Float));
        //R->SetPinColor(IM_COL32(255, 10, 10, 255));
        //R->SetHideDuringConnection(false);
        //R->SetPinName("R");
        //R->SetComponentMask(EComponentMask::R);
        //
        //CMaterialOutput* G = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "G", ENodePinDirection::Output, EMaterialInputType::Float));
        //G->SetPinColor(IM_COL32(10, 255, 10, 255));
        //G->SetHideDuringConnection(false);
        //G->SetPinName("G");
        //G->SetComponentMask(EComponentMask::G);
        //
        //CMaterialOutput* B = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "B", ENodePinDirection::Output, EMaterialInputType::Float));
        //B->SetPinColor(IM_COL32(10, 10, 255, 255));
        //B->SetHideDuringConnection(false);
        //B->SetPinName("B");
        //B->SetComponentMask(EComponentMask::B);
        //
        //CMaterialOutput* A = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "A", ENodePinDirection::Output, EMaterialInputType::Float));
        //A->SetHideDuringConnection(false);
        //A->SetPinName("A");
        //A->SetComponentMask(EComponentMask::A);
    }

    uint32 CMaterialExpression_TextureSample::GenerateExpression(FMaterialCompiler* Compiler)
    {
        Compiler->DefineTextureSample(FullName);
        return 0;
    }

    void CMaterialExpression_TextureSample::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->TextureSample(FullName, Texture);
    }
}
