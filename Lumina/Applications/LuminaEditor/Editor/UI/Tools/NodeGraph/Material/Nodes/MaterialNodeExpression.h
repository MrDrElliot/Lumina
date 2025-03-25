#pragma once
#include "MaterialGraphNode.h"
#include "UI/Tools/NodeGraph/Material/MaterialInput.h"
#include "UI/Tools/NodeGraph/Material/MaterialOutput.h"

namespace Lumina
{
    class FMaterialExpression : public FMaterialGraphNode
    {
    public:

        void BuildNode() override;
        
        FMaterialOutput* Output;
        
    };

    class FMaterialExpression_Math : public FMaterialExpression
    {
    public:

        void BuildNode() override;
        
        FMaterialInput* A;
        FMaterialInput* B;
        
    };
    
    class FMaterialExpression_Addition : public FMaterialExpression_Math
    {
    public:

        LUMINA_ED_GRAPH_NODE(FMaterialExpression_Addition, "Addition", "X + Y")
        
        void BuildNode() override;

        FString GetNodeDisplayName() const override { return "Add"; }
        float* GetNodeDefaultValue() override { return &ConstA; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 0; }
        void GenerateDefinition(FMaterialCompiler* Compiler) override;
        
        FMaterialInput* A;
        FMaterialInput* B;

        float ConstA;
        float ConstB;
    };

    class FMaterialExpression_Subtraction : public FMaterialExpression_Math
    {
    public:
        
        LUMINA_ED_GRAPH_NODE(FMaterialExpression_Subtraction, "Subtraction", "X - Y")

        void BuildNode() override;


        FString GetNodeDisplayName() const override { return "Subtract"; }
        float* GetNodeDefaultValue() override { return &ConstA; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 0; }
        void GenerateDefinition(FMaterialCompiler* Compiler) override;
        
        FMaterialInput* A;
        FMaterialInput* B;

        float ConstA;
        float ConstB;
    };

    class FMaterialExpression_Multiplication : public FMaterialExpression_Math
    {
    public:

        LUMINA_ED_GRAPH_NODE(FMaterialExpression_Multiplication, "Multiplication", "X * Y")

        
        void BuildNode() override;

        FString GetNodeDisplayName() const override { return "Multiply"; }
        float* GetNodeDefaultValue() override { return &ConstA; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 0; }
        void GenerateDefinition(FMaterialCompiler* Compiler) override;

        FMaterialInput* A;
        FMaterialInput* B;

        float ConstA;
        float ConstB;
    };

    class FMaterialExpression_Division : public FMaterialExpression_Math
    {
    public:

        LUMINA_ED_GRAPH_NODE(FMaterialExpression_Division, "Division", "X / Y")

        void BuildNode() override;

        FString GetNodeDisplayName() const override { return "Divide"; }
        float* GetNodeDefaultValue() override { return &ConstA; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 0; }
        void GenerateDefinition(FMaterialCompiler* Compiler) override;

        FMaterialInput* A;
        FMaterialInput* B;

        float ConstA;
        float ConstB;
    };


    //---------------------------------------------------------------------------------------------------------


    class FMaterialExpression_Constant : public FMaterialExpression
    {
    public:

        FMaterialExpression_Constant(EMaterialInputType Input)
            :ValueType(Input)
        {}

        void BuildNode() override;
        
        float* GetNodeDefaultValue() override { return &Value.R; }

        FColor              Value;
        EMaterialInputType  ValueType;
        
    };

    class FMaterialExpression_ConstantFloat : public FMaterialExpression_Constant
    {
    public:

        LUMINA_ED_GRAPH_NODE(FMaterialExpression_ConstantFloat, "Float", "Static float value")
        
        FMaterialExpression_ConstantFloat()
            :FMaterialExpression_Constant(EMaterialInputType::Float)
        {}

        FString GetNodeDisplayName() const override { return "Float"; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override;
        void GenerateDefinition(FMaterialCompiler* Compiler) override;
        
        
    };

    class FMaterialExpression_ConstantFloat2 : public FMaterialExpression_Constant
    {
    public:
        
        LUMINA_ED_GRAPH_NODE(FMaterialExpression_ConstantFloat2, "Vector 2", "Static vec2 value")


        FMaterialExpression_ConstantFloat2()
            :FMaterialExpression_Constant(EMaterialInputType::Float2)
        {}
        
        FString GetNodeDisplayName() const override { return "Vec2"; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override;
        void GenerateDefinition(FMaterialCompiler* Compiler) override;

        
    };

    class FMaterialExpression_ConstantFloat3 : public FMaterialExpression_Constant
    {
    public:

        LUMINA_ED_GRAPH_NODE(FMaterialExpression_ConstantFloat3, "Vector 3", "Static vec3 value")

        
        FMaterialExpression_ConstantFloat3()
            :FMaterialExpression_Constant(EMaterialInputType::Float3)
        {}

        FString GetNodeDisplayName() const override { return "Vec3"; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override;
        void GenerateDefinition(FMaterialCompiler* Compiler) override;

    };
    

    class FMaterialExpression_ConstantFloat4 : public FMaterialExpression_Constant
    {
    public:

        LUMINA_ED_GRAPH_NODE(FMaterialExpression_ConstantFloat4, "Vector 4", "Static vec4 value")

        
        FMaterialExpression_ConstantFloat4()
            :FMaterialExpression_Constant(EMaterialInputType::Float4)
        {}

        FString GetNodeDisplayName() const override { return "Vec4"; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override;
        void GenerateDefinition(FMaterialCompiler* Compiler) override;

    };
}
