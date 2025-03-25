#include "MaterialNodeExpression.h"

#include "UI/Tools/NodeGraph/Material/MaterialCompiler.h"


namespace Lumina
{
    void FMaterialExpression::BuildNode()
    {
        Output = CreatePin<FMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float);
        Output->SetShouldDrawEditor(false);
    }

    void FMaterialExpression_Math::BuildNode()
    {
        FMaterialExpression::BuildNode();
    }

    void FMaterialExpression_Constant::BuildNode()
    {
        switch (ValueType)
        {
        case EMaterialInputType::Float:
            {
                FMaterialOutput* ValuePin = CreatePin<FMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float);
                ValuePin->SetShouldDrawEditor(true);
                ValuePin->SetHideDuringConnection(false);
                ValuePin->SetPinName("X");
            }
            break;
        case EMaterialInputType::Float2:
            {
                FMaterialOutput* ValuePin = CreatePin<FMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float2);
                ValuePin->SetShouldDrawEditor(true);
                ValuePin->SetHideDuringConnection(false);
                ValuePin->SetPinName("XY");
                
                FMaterialOutput* R = CreatePin<FMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float);
                R->SetPinColor(IM_COL32(255, 10, 10, 255));
                R->SetHideDuringConnection(false);
                R->SetPinName("X");
                
                FMaterialOutput* G = CreatePin<FMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float);
                R->SetPinColor(IM_COL32(10, 255, 10, 255));
                G->SetHideDuringConnection(false);
                G->SetPinName("Y");
                
            }
            break;
        case EMaterialInputType::Float3:
            {
                FMaterialOutput* ValuePin = CreatePin<FMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float3);
                ValuePin->SetShouldDrawEditor(true);
                ValuePin->SetHideDuringConnection(false);
                ValuePin->SetPinName("RGB");
                
                FMaterialOutput* R = CreatePin<FMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float);
                R->SetPinColor(IM_COL32(255, 10, 10, 255));
                R->SetHideDuringConnection(false);
                R->SetPinName("R");
                
                FMaterialOutput* G = CreatePin<FMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float);
                G->SetPinColor(IM_COL32(10, 255, 10, 255));
                G->SetHideDuringConnection(false);
                G->SetPinName("G");

                FMaterialOutput* B = CreatePin<FMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float);
                B->SetPinColor(IM_COL32(10, 10, 255, 255));
                B->SetHideDuringConnection(false);
                B->SetPinName("B");

            }
            break;
        case EMaterialInputType::Float4:
            {
                FMaterialOutput* ValuePin = CreatePin<FMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float4);
                ValuePin->SetShouldDrawEditor(true);
                ValuePin->SetHideDuringConnection(false);
                ValuePin->SetPinName("RGBA");
                
                FMaterialOutput* R = CreatePin<FMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float);
                R->SetPinColor(IM_COL32(255, 10, 10, 255));
                R->SetHideDuringConnection(false);
                R->SetPinName("R");
                
                FMaterialOutput* G = CreatePin<FMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float);
                G->SetPinColor(IM_COL32(10, 255, 10, 255));
                G->SetHideDuringConnection(false);
                G->SetPinName("G");

                FMaterialOutput* B = CreatePin<FMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float);
                B->SetPinColor(IM_COL32(10, 10, 255, 255));
                B->SetHideDuringConnection(false);
                B->SetPinName("B");

                FMaterialOutput* A = CreatePin<FMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float);
                A->SetHideDuringConnection(false);
                A->SetPinName("A");
            }
            break;
        case EMaterialInputType::Wildcard:
            break;
        }
        
    }

    uint32 FMaterialExpression_ConstantFloat::GenerateExpression(FMaterialCompiler* Compiler)
    {
        return 0;
    }

    void FMaterialExpression_ConstantFloat::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->DefineConstantFloat(FullName, Value.R);
    }

    uint32 FMaterialExpression_ConstantFloat2::GenerateExpression(FMaterialCompiler* Compiler)
    {
        return 0;
    }

    void FMaterialExpression_ConstantFloat2::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->DefineConstantFloat2(FullName, &Value.R);
    }

    uint32 FMaterialExpression_ConstantFloat3::GenerateExpression(FMaterialCompiler* Compiler)
    {
        return 0;
    }

    void FMaterialExpression_ConstantFloat3::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->DefineConstantFloat3(FullName, &Value.R);
    }

    uint32 FMaterialExpression_ConstantFloat4::GenerateExpression(FMaterialCompiler* Compiler)
    {
        return 0;
    }
    
    void FMaterialExpression_ConstantFloat4::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->DefineConstantFloat4(FullName, &Value.R);
    }
    

    void FMaterialExpression_Addition::BuildNode()
    {
        FMaterialExpression_Math::BuildNode();

        A = CreatePin<FMaterialInput, ENodePinDirection::Input>(EMaterialInputType::Float);
        A->SetPinName("X");
        A->SetShouldDrawEditor(true);
        
        B = CreatePin<FMaterialInput, ENodePinDirection::Input>(EMaterialInputType::Float);
        B->SetPinName("Y");
        B->SetShouldDrawEditor(true);
    }

    void FMaterialExpression_Addition::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Add(A, B);
    }

    void FMaterialExpression_Subtraction::BuildNode()
    {
        FMaterialExpression_Math::BuildNode();

        A = CreatePin<FMaterialInput, ENodePinDirection::Input>(EMaterialInputType::Float);
        A->SetPinName("X");
        A->SetShouldDrawEditor(true);
        
        B = CreatePin<FMaterialInput, ENodePinDirection::Input>(EMaterialInputType::Float);
        B->SetPinName("Y");
        B->SetShouldDrawEditor(true);
    }

    void FMaterialExpression_Subtraction::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Subtract(A, B);
    }

    void FMaterialExpression_Multiplication::BuildNode()
    {
        FMaterialExpression_Math::BuildNode();
        
        A = CreatePin<FMaterialInput, ENodePinDirection::Input>(EMaterialInputType::Float);
        A->SetPinName("X");
        A->SetShouldDrawEditor(true);
        
        B = CreatePin<FMaterialInput, ENodePinDirection::Input>(EMaterialInputType::Float);
        B->SetPinName("Y");
        B->SetShouldDrawEditor(true);
    }

    void FMaterialExpression_Multiplication::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Multiply(A, B);
    }

    void FMaterialExpression_Division::BuildNode()
    {
        FMaterialExpression_Math::BuildNode();

        A = CreatePin<FMaterialInput, ENodePinDirection::Input>(EMaterialInputType::Float);
        A->SetPinName("X");
        A->SetShouldDrawEditor(true);
        
        B = CreatePin<FMaterialInput, ENodePinDirection::Input>(EMaterialInputType::Float);
        B->SetPinName("Y");
        B->SetShouldDrawEditor(true);
    }

    void FMaterialExpression_Division::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Divide(A, B);
    }
}
