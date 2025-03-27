#include "MaterialNodeExpression.h"

#include "UI/Tools/NodeGraph/Material/MaterialCompiler.h"


namespace Lumina
{
    IMPLEMENT_CLASS(CMaterialExpression)
    IMPLEMENT_CLASS(CMaterialExpression_Math)
    IMPLEMENT_CLASS(CMaterialExpression_Constant)
    
    void CMaterialExpression::BuildNode()
    {
        Output = CreatePin<CMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float);
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
                CMaterialOutput* ValuePin = CreatePin<CMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float);
                ValuePin->SetShouldDrawEditor(true);
                ValuePin->SetHideDuringConnection(false);
                ValuePin->SetPinName("X");
            }
            break;
        case EMaterialInputType::Float2:
            {
                CMaterialOutput* ValuePin = CreatePin<CMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float2);
                ValuePin->SetShouldDrawEditor(true);
                ValuePin->SetHideDuringConnection(false);
                ValuePin->SetPinName("XY");
                
                CMaterialOutput* R = CreatePin<CMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float);
                R->SetPinColor(IM_COL32(255, 10, 10, 255));
                R->SetHideDuringConnection(false);
                R->SetPinName("X");
                
                CMaterialOutput* G = CreatePin<CMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float);
                R->SetPinColor(IM_COL32(10, 255, 10, 255));
                G->SetHideDuringConnection(false);
                G->SetPinName("Y");
                
            }
            break;
        case EMaterialInputType::Float3:
            {
                CMaterialOutput* ValuePin = CreatePin<CMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float3);
                ValuePin->SetShouldDrawEditor(true);
                ValuePin->SetHideDuringConnection(false);
                ValuePin->SetPinName("RGB");
                
                CMaterialOutput* R = CreatePin<CMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float);
                R->SetPinColor(IM_COL32(255, 10, 10, 255));
                R->SetHideDuringConnection(false);
                R->SetPinName("R");
                
                CMaterialOutput* G = CreatePin<CMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float);
                G->SetPinColor(IM_COL32(10, 255, 10, 255));
                G->SetHideDuringConnection(false);
                G->SetPinName("G");

                CMaterialOutput* B = CreatePin<CMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float);
                B->SetPinColor(IM_COL32(10, 10, 255, 255));
                B->SetHideDuringConnection(false);
                B->SetPinName("B");

            }
            break;
        case EMaterialInputType::Float4:
            {
                CMaterialOutput* ValuePin = CreatePin<CMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float4);
                ValuePin->SetShouldDrawEditor(true);
                ValuePin->SetHideDuringConnection(false);
                ValuePin->SetPinName("RGBA");
                
                CMaterialOutput* R = CreatePin<CMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float);
                R->SetPinColor(IM_COL32(255, 10, 10, 255));
                R->SetHideDuringConnection(false);
                R->SetPinName("R");
                
                CMaterialOutput* G = CreatePin<CMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float);
                G->SetPinColor(IM_COL32(10, 255, 10, 255));
                G->SetHideDuringConnection(false);
                G->SetPinName("G");

                CMaterialOutput* B = CreatePin<CMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float);
                B->SetPinColor(IM_COL32(10, 10, 255, 255));
                B->SetHideDuringConnection(false);
                B->SetPinName("B");

                CMaterialOutput* A = CreatePin<CMaterialOutput, ENodePinDirection::Output>(EMaterialInputType::Float);
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
    IMPLEMENT_CLASS(CMaterialExpression_ConstantFloat)


    uint32 CMaterialExpression_ConstantFloat2::GenerateExpression(FMaterialCompiler* Compiler)
    {
        return 0;
    }

    void CMaterialExpression_ConstantFloat2::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->DefineConstantFloat2(FullName, &Value.R);
    }
    IMPLEMENT_CLASS(CMaterialExpression_ConstantFloat2)


    uint32 CMaterialExpression_ConstantFloat3::GenerateExpression(FMaterialCompiler* Compiler)
    {
        return 0;
    }

    void CMaterialExpression_ConstantFloat3::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->DefineConstantFloat3(FullName, &Value.R);
    }
    IMPLEMENT_CLASS(CMaterialExpression_ConstantFloat3)


    uint32 CMaterialExpression_ConstantFloat4::GenerateExpression(FMaterialCompiler* Compiler)
    {
        return 0;
    }
    
    void CMaterialExpression_ConstantFloat4::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->DefineConstantFloat4(FullName, &Value.R);
    }
    IMPLEMENT_CLASS(CMaterialExpression_ConstantFloat4)


    void CMaterialExpression_Addition::BuildNode()
    {
        CMaterialExpression_Math::BuildNode();

        A = CreatePin<CMaterialInput, ENodePinDirection::Input>(EMaterialInputType::Float);
        A->SetPinName("X");
        A->SetShouldDrawEditor(true);
        
        B = CreatePin<CMaterialInput, ENodePinDirection::Input>(EMaterialInputType::Float);
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

        A = CreatePin<CMaterialInput, ENodePinDirection::Input>(EMaterialInputType::Float);
        A->SetPinName("X");
        A->SetShouldDrawEditor(true);
        
        B = CreatePin<CMaterialInput, ENodePinDirection::Input>(EMaterialInputType::Float);
        B->SetPinName("Y");
        B->SetShouldDrawEditor(true);
    }
    IMPLEMENT_CLASS(CMaterialExpression_Addition)


    void CMaterialExpression_Subtraction::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Subtract(A, B);
    }

    void CMaterialExpression_Multiplication::BuildNode()
    {
        CMaterialExpression_Math::BuildNode();
        
        A = CreatePin<CMaterialInput, ENodePinDirection::Input>(EMaterialInputType::Float);
        A->SetPinName("X");
        A->SetShouldDrawEditor(true);
        
        B = CreatePin<CMaterialInput, ENodePinDirection::Input>(EMaterialInputType::Float);
        B->SetPinName("Y");
        B->SetShouldDrawEditor(true);
    }

    IMPLEMENT_CLASS(CMaterialExpression_Subtraction)


    void CMaterialExpression_Multiplication::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Multiply(A, B);
    }

    IMPLEMENT_CLASS(CMaterialExpression_Multiplication)


    void CMaterialExpression_Division::BuildNode()
    {
        CMaterialExpression_Math::BuildNode();

        A = CreatePin<CMaterialInput, ENodePinDirection::Input>(EMaterialInputType::Float);
        A->SetPinName("X");
        A->SetShouldDrawEditor(true);
        
        B = CreatePin<CMaterialInput, ENodePinDirection::Input>(EMaterialInputType::Float);
        B->SetPinName("Y");
        B->SetShouldDrawEditor(true);
    }

    void CMaterialExpression_Division::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Divide(A, B);
    }

    IMPLEMENT_CLASS(CMaterialExpression_Division)

}
