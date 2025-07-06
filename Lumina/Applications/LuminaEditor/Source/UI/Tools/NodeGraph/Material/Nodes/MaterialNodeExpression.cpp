#include "MaterialNodeExpression.h"

#include "Core/Object/Class.h"
#include "Core/Object/Cast.h"
#include "UI/Tools/NodeGraph/Material/MaterialOutput.h"
#include "UI/Tools/NodeGraph/Material/MaterialCompiler.h"


namespace Lumina
{
    
    void CMaterialExpression::BuildNode()
    {
        Output = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "Material Output", ENodePinDirection::Output, EMaterialInputType::Float));
        Output->SetShouldDrawEditor(false);
    }

    void CMaterialExpression_Math::Serialize(FArchive& Ar)
    {
        CMaterialExpression::Serialize(Ar);

        Ar << ConstA;
        Ar << ConstB;
    }

    void CMaterialExpression_Math::BuildNode()
    {
        CMaterialExpression::BuildNode();
    }

    void CMaterialExpression_WorldPos::BuildNode()
    {
        CMaterialExpression::BuildNode();
    }

    void CMaterialExpression_WorldPos::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->WorldPos(FullName);
    }

    void CMaterialExpression_CameraPos::BuildNode()
    {
        CMaterialExpression::BuildNode();
    }

    void CMaterialExpression_CameraPos::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->CameraPos(FullName);
    }

    void CMaterialExpression_Constant::Serialize(FArchive& Ar)
    {
        CMaterialExpression::Serialize(Ar);

        Ar << Value;
    }

    void CMaterialExpression_Constant::BuildNode()
    {
        switch (ValueType)
        {
        case EMaterialInputType::Float:
            {
                CMaterialOutput* ValuePin = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "X", ENodePinDirection::Output, EMaterialInputType::Float));
                ValuePin->SetShouldDrawEditor(true);
                ValuePin->SetHideDuringConnection(false);
                ValuePin->SetPinName("X");
                ValuePin->InputType = EMaterialInputType::Float;
            }
            break;
        case EMaterialInputType::Float2:
            {
                CMaterialOutput* ValuePin = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "XY", ENodePinDirection::Output, EMaterialInputType::Float2));
                ValuePin->SetShouldDrawEditor(true);
                ValuePin->SetHideDuringConnection(false);
                ValuePin->SetPinName("XY");
                ValuePin->InputType = EMaterialInputType::Float2;
                
                CMaterialOutput* R = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "X", ENodePinDirection::Output, EMaterialInputType::Float));
                R->SetPinColor(IM_COL32(255, 10, 10, 255));
                R->SetHideDuringConnection(false);
                R->SetPinName("X");
                R->SetComponentMask(EComponentMask::R);

                CMaterialOutput* G = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "Y", ENodePinDirection::Output, EMaterialInputType::Float));
                R->SetPinColor(IM_COL32(10, 255, 10, 255));
                G->SetHideDuringConnection(false);
                G->SetPinName("Y");
                G->SetComponentMask(EComponentMask::G);

            }
            break;
        case EMaterialInputType::Float3:
            {
                CMaterialOutput* ValuePin = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "RGB", ENodePinDirection::Output, EMaterialInputType::Float3));
                ValuePin->SetShouldDrawEditor(true);
                ValuePin->SetHideDuringConnection(false);
                ValuePin->SetPinName("RGB");
                ValuePin->InputType = EMaterialInputType::Float3;
                
                CMaterialOutput* R = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "R", ENodePinDirection::Output, EMaterialInputType::Float));
                R->SetPinColor(IM_COL32(255, 10, 10, 255));
                R->SetHideDuringConnection(false);
                R->SetPinName("R");
                R->SetComponentMask(EComponentMask::R);

                CMaterialOutput* G = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "G", ENodePinDirection::Output, EMaterialInputType::Float));
                G->SetPinColor(IM_COL32(10, 255, 10, 255));
                G->SetHideDuringConnection(false);
                G->SetPinName("G");
                G->SetComponentMask(EComponentMask::G);

                CMaterialOutput* B = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "B", ENodePinDirection::Output, EMaterialInputType::Float2));
                B->SetPinColor(IM_COL32(10, 10, 255, 255));
                B->SetHideDuringConnection(false);
                B->SetPinName("B");
                B->SetComponentMask(EComponentMask::B);

            }
            break;
        case EMaterialInputType::Float4:
            {
                CMaterialOutput* ValuePin = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "RGBA", ENodePinDirection::Output, EMaterialInputType::Float4));
                ValuePin->SetShouldDrawEditor(true);
                ValuePin->SetHideDuringConnection(false);
                ValuePin->SetPinName("RGBA");
                ValuePin->InputType = EMaterialInputType::Float4;

                
                CMaterialOutput* R = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "R", ENodePinDirection::Output, EMaterialInputType::Float));
                R->SetPinColor(IM_COL32(255, 10, 10, 255));
                R->SetHideDuringConnection(false);
                R->SetPinName("R");
                R->SetComponentMask(EComponentMask::R);

                CMaterialOutput* G = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "G", ENodePinDirection::Output, EMaterialInputType::Float));
                G->SetPinColor(IM_COL32(10, 255, 10, 255));
                G->SetHideDuringConnection(false);
                G->SetPinName("G");
                G->SetComponentMask(EComponentMask::G);

                CMaterialOutput* B = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "B", ENodePinDirection::Output, EMaterialInputType::Float));
                B->SetPinColor(IM_COL32(10, 10, 255, 255));
                B->SetHideDuringConnection(false);
                B->SetPinName("B");
                B->SetComponentMask(EComponentMask::B);

                CMaterialOutput* A = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "A", ENodePinDirection::Output, EMaterialInputType::Float));
                A->SetHideDuringConnection(false);
                A->SetPinName("A");
                A->SetComponentMask(EComponentMask::A);

            }
            break;
        case EMaterialInputType::Wildcard:
            break;
        }
    }

    uint32 CMaterialExpression_ConstantFloat::GenerateExpression(FMaterialCompiler* Compiler)
    {
        return 0;
    }

    void CMaterialExpression_ConstantFloat::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->DefineConstantFloat(FullName, Value.R);
    }

    uint32 CMaterialExpression_ConstantFloat2::GenerateExpression(FMaterialCompiler* Compiler)
    {
        return 0;
    }

    void CMaterialExpression_ConstantFloat2::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->DefineConstantFloat2(FullName, &Value.R);
    }


    uint32 CMaterialExpression_ConstantFloat3::GenerateExpression(FMaterialCompiler* Compiler)
    {
        return 0;
    }

    void CMaterialExpression_ConstantFloat3::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->DefineConstantFloat3(FullName, &Value.R);
    }


    uint32 CMaterialExpression_ConstantFloat4::GenerateExpression(FMaterialCompiler* Compiler)
    {
        return 0;
    }
    
    void CMaterialExpression_ConstantFloat4::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->DefineConstantFloat4(FullName, &Value.R);
    }


    void CMaterialExpression_Addition::BuildNode()
    {
        CMaterialExpression_Math::BuildNode();

        A = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "X", ENodePinDirection::Input, EMaterialInputType::Float));
        A->SetPinName("X");
        A->SetShouldDrawEditor(true);
        A->SetIndex(0);
        
        B = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "Y", ENodePinDirection::Input, EMaterialInputType::Float));
        B->SetPinName("Y");
        B->SetShouldDrawEditor(true);
        B->SetIndex(1);

    }

    void CMaterialExpression_Addition::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Add(A, B);
    }

    void CMaterialExpression_Subtraction::BuildNode()
    {
        CMaterialExpression_Math::BuildNode();

        A = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "X", ENodePinDirection::Input, EMaterialInputType::Float));
        A->SetPinName("X");
        A->SetShouldDrawEditor(true);
        A->SetIndex(0);
        
        B = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "Y", ENodePinDirection::Input, EMaterialInputType::Float));
        B->SetPinName("Y");
        B->SetShouldDrawEditor(true);
        B->SetIndex(1);
    }


    void CMaterialExpression_Subtraction::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Subtract(A, B);
    }

    void CMaterialExpression_Multiplication::BuildNode()
    {
        CMaterialExpression_Math::BuildNode();

        A = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "X", ENodePinDirection::Input, EMaterialInputType::Float));
        A->SetPinName("X");
        A->SetShouldDrawEditor(true);
        A->SetIndex(0);
        
        B = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "Y", ENodePinDirection::Input, EMaterialInputType::Float));
        B->SetPinName("Y");
        B->SetShouldDrawEditor(true);
        B->SetIndex(1);
    }
    
    void CMaterialExpression_Multiplication::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Multiply(A, B);
    }

    void CMaterialExpression_Sin::BuildNode()
    {
        CMaterialExpression_Math::BuildNode();

        A = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "X", ENodePinDirection::Input, EMaterialInputType::Float));
        A->SetPinName("X");
        A->SetShouldDrawEditor(true);
    }

    void CMaterialExpression_Sin::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Sin(A, B);
    }

    void CMaterialExpression_Cosin::BuildNode()
    {
        CMaterialExpression_Math::BuildNode();

        A = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "X", ENodePinDirection::Input, EMaterialInputType::Float));
        A->SetPinName("X");
        A->SetShouldDrawEditor(true);
    }

    void CMaterialExpression_Cosin::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Cos(A, B);
    }

    void CMaterialExpression_Floor::BuildNode()
    {
        CMaterialExpression_Math::BuildNode();

        A = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "X", ENodePinDirection::Input, EMaterialInputType::Float));
        A->SetPinName("X");
        A->SetShouldDrawEditor(true);
    }

    void CMaterialExpression_Floor::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Floor(A, B);
    }

    void CMaterialExpression_Ceil::BuildNode()
    {
        CMaterialExpression_Math::BuildNode();

        A = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "X", ENodePinDirection::Input, EMaterialInputType::Float));
        A->SetPinName("X");
        A->SetShouldDrawEditor(true);
        
    }

    void CMaterialExpression_Ceil::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Ceil(A, B);
    }

    void CMaterialExpression_Power::BuildNode()
    {
        CMaterialExpression_Math::BuildNode();

        A = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "X", ENodePinDirection::Input, EMaterialInputType::Float));
        A->SetPinName("X");
        A->SetShouldDrawEditor(true);
        A->SetIndex(0);
        
        B = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "Y", ENodePinDirection::Input, EMaterialInputType::Float));
        B->SetPinName("Y");
        B->SetShouldDrawEditor(true);
        B->SetIndex(3);
        
    }

    void CMaterialExpression_Power::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Power(A, B);
    }

    void CMaterialExpression_Mod::BuildNode()
    {
        CMaterialExpression_Math::BuildNode();
        
        A = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "X", ENodePinDirection::Input, EMaterialInputType::Float));
        A->SetPinName("X");
        A->SetShouldDrawEditor(true);
        A->SetIndex(0);
        
        B = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "Y", ENodePinDirection::Input, EMaterialInputType::Float));
        B->SetPinName("Y");
        B->SetShouldDrawEditor(true);
        B->SetIndex(1);
    }

    void CMaterialExpression_Min::BuildNode()
    {
        CMaterialExpression_Math::BuildNode();

        A = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "X", ENodePinDirection::Input, EMaterialInputType::Float));
        A->SetPinName("X");
        A->SetShouldDrawEditor(true);
        A->SetIndex(0);
        
        B = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "Y", ENodePinDirection::Input, EMaterialInputType::Float));
        B->SetPinName("Y");
        B->SetShouldDrawEditor(true);
        B->SetIndex(1);
    }

    void CMaterialExpression_Max::BuildNode()
    {
        CMaterialExpression_Math::BuildNode();

        A = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "X", ENodePinDirection::Input, EMaterialInputType::Float));
        A->SetPinName("X");
        A->SetShouldDrawEditor(true);
        A->SetIndex(0);
        
        B = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "Y", ENodePinDirection::Input, EMaterialInputType::Float));
        B->SetPinName("Y");
        B->SetShouldDrawEditor(true);
        B->SetIndex(1);
    }

    void CMaterialExpression_Step::BuildNode()
    {
        CMaterialExpression_Math::BuildNode();
        
        A = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "X", ENodePinDirection::Input, EMaterialInputType::Float));
        A->SetPinName("X");
        A->SetShouldDrawEditor(true);
        A->SetIndex(0);

        B = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "Y", ENodePinDirection::Input, EMaterialInputType::Float));
        B->SetPinName("Y");
        B->SetShouldDrawEditor(true);
        B->SetIndex(1);

    }

    void CMaterialExpression_Step::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Step(A, B);
    }

    void CMaterialExpression_Lerp::Serialize(FArchive& Ar)
    {
        CMaterialExpression_Math::Serialize(Ar);

        Ar << ConstC;
    }

    void CMaterialExpression_Lerp::BuildNode()
    {
        CMaterialExpression_Math::BuildNode();

        A = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "X", ENodePinDirection::Input, EMaterialInputType::Float));
        A->SetPinName("X");
        A->SetShouldDrawEditor(true);
        A->SetIndex(0);
        
        B = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "Y", ENodePinDirection::Input, EMaterialInputType::Float));
        B->SetPinName("Y");
        B->SetShouldDrawEditor(true);
        B->SetIndex(1);

        C = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "A", ENodePinDirection::Input, EMaterialInputType::Float));
        C->SetPinName("A");
        C->SetShouldDrawEditor(true);
        C->SetIndex(2);
    }

    void CMaterialExpression_Lerp::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Lerp(A, B, C);
    }

    void CMaterialExpression_Max::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Max(A, B);
    }

    void CMaterialExpression_Min::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Min(A, B);
    }

    void CMaterialExpression_Mod::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Mod(A, B);
    }

    void CMaterialExpression_Division::BuildNode()
    {
        CMaterialExpression_Math::BuildNode();
        
        A = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "X", ENodePinDirection::Input, EMaterialInputType::Float));
        A->SetPinName("X");
        A->SetShouldDrawEditor(true);
        A->SetIndex(0);
        
        B = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "Y", ENodePinDirection::Input, EMaterialInputType::Float));
        B->SetPinName("Y");
        B->SetShouldDrawEditor(true);
        B->SetIndex(1);
    }

    void CMaterialExpression_Division::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Divide(A, B);
    }
}
