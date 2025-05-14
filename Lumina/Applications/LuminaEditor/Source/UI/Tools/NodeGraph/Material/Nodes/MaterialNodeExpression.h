#pragma once
#include "MaterialGraphNode.h"
#include "UI/Tools/NodeGraph/Material/MaterialInput.h"
#include "UI/Tools/NodeGraph/Material/MaterialOutput.h"
#include "MaterialNodeExpression.generated.h"

namespace Lumina
{
    LUM_CLASS()
    class CMaterialExpression : public CMaterialGraphNode
    {
        GENERATED_BODY()
        
    public:
        
        void BuildNode() override;
        
        CMaterialOutput* Output;
        
    };

    LUM_CLASS()
    class CMaterialExpression_Math : public CMaterialExpression
    {
        GENERATED_BODY()
        
    public:

        void BuildNode() override;
        
        CMaterialInput* A;
        CMaterialInput* B;
        
    };

    LUM_CLASS()
    class CMaterialExpression_Addition : public CMaterialExpression_Math
    {
        GENERATED_BODY()
        
    public:
        
        
        void BuildNode() override;

        FString GetNodeDisplayName() const override { return "Add"; }
        float* GetNodeDefaultValue() override { return &ConstA; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 0; }
        void GenerateDefinition(FMaterialCompiler* Compiler) override;
        
        CMaterialInput* A;
        CMaterialInput* B;

        float ConstA;
        float ConstB;
    };

    LUM_CLASS()
    class CMaterialExpression_Subtraction : public CMaterialExpression_Math
    {
        GENERATED_BODY()
        
    public:
        
        void BuildNode() override;


        FString GetNodeDisplayName() const override { return "Subtract"; }
        float* GetNodeDefaultValue() override { return &ConstA; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 0; }
        void GenerateDefinition(FMaterialCompiler* Compiler) override;
        
        CMaterialInput* A;
        CMaterialInput* B;

        float ConstA;
        float ConstB;
    };

    LUM_CLASS()
    class CMaterialExpression_Multiplication : public CMaterialExpression_Math
    {
        GENERATED_BODY()
    public:
        
        
        void BuildNode() override;

        FString GetNodeDisplayName() const override { return "Multiply"; }
        float* GetNodeDefaultValue() override { return &ConstA; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 0; }
        void GenerateDefinition(FMaterialCompiler* Compiler) override;

        CMaterialInput* A;
        CMaterialInput* B;

        float ConstA;
        float ConstB;
    };
    

    LUM_CLASS()
    class CMaterialExpression_Division : public CMaterialExpression_Math
    {
        GENERATED_BODY()
    public:
        
        void BuildNode() override;

        FString GetNodeDisplayName() const override { return "Divide"; }
        float* GetNodeDefaultValue() override { return &ConstA; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 0; }
        void GenerateDefinition(FMaterialCompiler* Compiler) override;

        CMaterialInput* A;
        CMaterialInput* B;

        float ConstA;
        float ConstB;
    };


    //---------------------------------------------------------------------------------------------------------

    LUM_CLASS()
    class CMaterialExpression_Constant : public CMaterialExpression
    {
        GENERATED_BODY()
    public:
        
        CMaterialExpression_Constant()
            :ValueType(EMaterialInputType::Wildcard)
        {}

        void BuildNode() override;
        
        float* GetNodeDefaultValue() override { return &Value.R; }

        FColor              Value;
        EMaterialInputType  ValueType;
        
    };

    LUM_CLASS()
    class CMaterialExpression_ConstantFloat : public CMaterialExpression_Constant
    {
        GENERATED_BODY()
    public:
        
        CMaterialExpression_ConstantFloat()
        {
            ValueType = EMaterialInputType::Float;
        }

        FString GetNodeDisplayName() const override { return "Float"; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override;
        void GenerateDefinition(FMaterialCompiler* Compiler) override;
        
    };

    LUM_CLASS()
    class CMaterialExpression_ConstantFloat2 : public CMaterialExpression_Constant
    {
        GENERATED_BODY()
    public:

        CMaterialExpression_ConstantFloat2()
        {
            ValueType = EMaterialInputType::Float2;
        }
        
        FString GetNodeDisplayName() const override { return "Vec2"; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override;
        void GenerateDefinition(FMaterialCompiler* Compiler) override;
        
    };

    LUM_CLASS()
    class CMaterialExpression_ConstantFloat3 : public CMaterialExpression_Constant
    {
        GENERATED_BODY()
    public:
        
        
        CMaterialExpression_ConstantFloat3()
        {
            ValueType = EMaterialInputType::Float3;
        }

        FString GetNodeDisplayName() const override { return "Vec3"; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override;
        void GenerateDefinition(FMaterialCompiler* Compiler) override;

    };

    LUM_CLASS()
    class CMaterialExpression_ConstantFloat4 : public CMaterialExpression_Constant
    {
        GENERATED_BODY()
    public:
        
        CMaterialExpression_ConstantFloat4()
        {
            ValueType = EMaterialInputType::Float4;
        }

        FString GetNodeDisplayName() const override { return "Vec4"; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override;
        void GenerateDefinition(FMaterialCompiler* Compiler) override;

    };
}
