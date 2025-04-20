#pragma once
#include "MaterialGraphNode.h"
#include "UI/Tools/NodeGraph/Material/MaterialInput.h"
#include "UI/Tools/NodeGraph/Material/MaterialOutput.h"

namespace Lumina
{
    class CMaterialExpression : public CMaterialGraphNode
    {
    public:
        
        void BuildNode() override;
        
        CMaterialOutput* Output;
        
    };

    class CMaterialExpression_Math : public CMaterialExpression
    {
    public:

        void BuildNode() override;
        
        CMaterialInput* A;
        CMaterialInput* B;
        
    };
    
    class CMaterialExpression_Addition : public CMaterialExpression_Math
    {
    public:
        
        LUMINA_ED_GRAPH_NODE(CMaterialExpression_Addition, "Addition", "X + Y")

        
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
    
    class CMaterialExpression_Subtraction : public CMaterialExpression_Math
    {
    public:
        
        LUMINA_ED_GRAPH_NODE(CMaterialExpression_Subtraction, "Subtraction", "X - Y")

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
    
    class CMaterialExpression_Multiplication : public CMaterialExpression_Math
    {
    public:
        
        LUMINA_ED_GRAPH_NODE(CMaterialExpression_Multiplication, "Multiplication", "X * Y")

        
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
    

    
    class CMaterialExpression_Division : public CMaterialExpression_Math
    {
    public:

        LUMINA_ED_GRAPH_NODE(CMaterialExpression_Division, "Division", "X / Y")

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


    class CMaterialExpression_Constant : public CMaterialExpression
    {
    public:
        
        CMaterialExpression_Constant(EMaterialInputType Input)
            :ValueType(Input)
        {}

        void BuildNode() override;
        
        float* GetNodeDefaultValue() override { return &Value.R; }

        FColor              Value;
        EMaterialInputType  ValueType;
        
    };

    class CMaterialExpression_ConstantFloat : public CMaterialExpression_Constant
    {
    public:
        
        LUMINA_ED_GRAPH_NODE(CMaterialExpression_ConstantFloat, "Float", "Static float value")
        
        CMaterialExpression_ConstantFloat()
            :CMaterialExpression_Constant(EMaterialInputType::Float)
        {}

        FString GetNodeDisplayName() const override { return "Float"; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override;
        void GenerateDefinition(FMaterialCompiler* Compiler) override;
        
    };

    class CMaterialExpression_ConstantFloat2 : public CMaterialExpression_Constant
    {
    public:
        LUMINA_ED_GRAPH_NODE(CMaterialExpression_ConstantFloat2, "Vector 2", "Static vec2 value")


        CMaterialExpression_ConstantFloat2()
            :CMaterialExpression_Constant(EMaterialInputType::Float2)
        {}
        
        FString GetNodeDisplayName() const override { return "Vec2"; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override;
        void GenerateDefinition(FMaterialCompiler* Compiler) override;
        
    };

    
    class CMaterialExpression_ConstantFloat3 : public CMaterialExpression_Constant
    {
    public:
        
        LUMINA_ED_GRAPH_NODE(CMaterialExpression_ConstantFloat3, "Vector 3", "Static vec3 value")

        
        CMaterialExpression_ConstantFloat3()
            :CMaterialExpression_Constant(EMaterialInputType::Float3)
        {}

        FString GetNodeDisplayName() const override { return "Vec3"; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override;
        void GenerateDefinition(FMaterialCompiler* Compiler) override;

    };

    class CMaterialExpression_ConstantFloat4 : public CMaterialExpression_Constant
    {
    public:
        LUMINA_ED_GRAPH_NODE(CMaterialExpression_ConstantFloat4, "Vector 4", "Static vec4 value")
        
        CMaterialExpression_ConstantFloat4()
            :CMaterialExpression_Constant(EMaterialInputType::Float4)
        {}

        FString GetNodeDisplayName() const override { return "Vec4"; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override;
        void GenerateDefinition(FMaterialCompiler* Compiler) override;

    };
}
