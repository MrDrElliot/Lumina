#include "MaterialNodeExpression.h"

#include "UI/Tools/NodeGraph/Material/MaterialCompiler.h"


namespace Lumina
{
    
    void CMaterialExpression::BuildNode()
    {

    }

    void CMaterialExpression_Math::BuildNode()
    {
        CMaterialExpression::BuildNode();
    }

    void CMaterialExpression_Constant::BuildNode()
    {

        
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
    }

    void CMaterialExpression_Addition::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Add(A, B);
    }

    void CMaterialExpression_Subtraction::BuildNode()
    {
        CMaterialExpression_Math::BuildNode();
    }


    void CMaterialExpression_Subtraction::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Subtract(A, B);
    }

    void CMaterialExpression_Multiplication::BuildNode()
    {
        CMaterialExpression_Math::BuildNode();
    }



    void CMaterialExpression_Multiplication::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Multiply(A, B);
    }

    void CMaterialExpression_Division::BuildNode()
    {
        CMaterialExpression_Math::BuildNode();
    }

    void CMaterialExpression_Division::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Divide(A, B);
    }
}
