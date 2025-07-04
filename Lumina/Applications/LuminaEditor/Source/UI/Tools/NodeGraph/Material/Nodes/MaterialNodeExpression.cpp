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

    void CMaterialExpression_Math::BuildNode()
    {
        CMaterialExpression::BuildNode();
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
                
                CMaterialOutput* G = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "Y", ENodePinDirection::Output, EMaterialInputType::Float));
                R->SetPinColor(IM_COL32(10, 255, 10, 255));
                G->SetHideDuringConnection(false);
                G->SetPinName("Y");
                
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
                
                CMaterialOutput* G = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "G", ENodePinDirection::Output, EMaterialInputType::Float));
                G->SetPinColor(IM_COL32(10, 255, 10, 255));
                G->SetHideDuringConnection(false);
                G->SetPinName("G");

                CMaterialOutput* B = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "B", ENodePinDirection::Output, EMaterialInputType::Float2));
                B->SetPinColor(IM_COL32(10, 10, 255, 255));
                B->SetHideDuringConnection(false);
                B->SetPinName("B");

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
                
                CMaterialOutput* G = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "G", ENodePinDirection::Output, EMaterialInputType::Float));
                G->SetPinColor(IM_COL32(10, 255, 10, 255));
                G->SetHideDuringConnection(false);
                G->SetPinName("G");

                CMaterialOutput* B = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "B", ENodePinDirection::Output, EMaterialInputType::Float));
                B->SetPinColor(IM_COL32(10, 10, 255, 255));
                B->SetHideDuringConnection(false);
                B->SetPinName("B");

                CMaterialOutput* A = Cast<CMaterialOutput>(CreatePin(CMaterialOutput::StaticClass(), "A", ENodePinDirection::Output, EMaterialInputType::Float));
                A->SetHideDuringConnection(false);
                A->SetPinName("A");
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
        
        B = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "Y", ENodePinDirection::Input, EMaterialInputType::Float));
        B->SetPinName("Y");
        B->SetShouldDrawEditor(true);
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
        
        B = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "Y", ENodePinDirection::Input, EMaterialInputType::Float));
        B->SetPinName("Y");
        B->SetShouldDrawEditor(true);
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
        
        B = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "Y", ENodePinDirection::Input, EMaterialInputType::Float));
        B->SetPinName("Y");
        B->SetShouldDrawEditor(true);
    }
    
    void CMaterialExpression_Multiplication::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Multiply(A, B);
    }

    void CMaterialExpression_Division::BuildNode()
    {
        CMaterialExpression_Math::BuildNode();
        
        A = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "X", ENodePinDirection::Input, EMaterialInputType::Float));
        A->SetPinName("X");
        A->SetShouldDrawEditor(true);
        
        B = Cast<CMaterialInput>(CreatePin(CMaterialInput::StaticClass(), "Y", ENodePinDirection::Input, EMaterialInputType::Float));
        B->SetPinName("Y");
        B->SetShouldDrawEditor(true);
    }

    void CMaterialExpression_Division::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Divide(A, B);
    }
}
