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
        void Serialize(FArchive& Ar) override;
        void BuildNode() override;
        
        CMaterialInput* A;
        CMaterialInput* B;

        float ConstA = 0;
        float ConstB = 0;
    };

    LUM_CLASS()
    class CMaterialExpression_Addition : public CMaterialExpression_Math
    {
        GENERATED_BODY()
        
    public:
        
        
        void BuildNode() override;

        FString GetNodeDisplayName() const override { return "Add"; }
        void* GetNodeDefaultValue() override { return &ConstA; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 0; }
        void GenerateDefinition(FMaterialCompiler* Compiler) override;
        

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
        void* GetNodeDefaultValue() override { return &ConstA; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 0; }
        void GenerateDefinition(FMaterialCompiler* Compiler) override;
    };

    LUM_CLASS()
    class CMaterialExpression_Multiplication : public CMaterialExpression_Math
    {
        GENERATED_BODY()
    public:
        
        
        void BuildNode() override;

        FString GetNodeDisplayName() const override { return "Multiply"; }
        void* GetNodeDefaultValue() override { return &ConstA; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 0; }
        void GenerateDefinition(FMaterialCompiler* Compiler) override;
    };
    

    LUM_CLASS()
    class CMaterialExpression_Sin : public CMaterialExpression_Math
    {
        GENERATED_BODY()
    public:
        
        
        void BuildNode() override;

        FString GetNodeDisplayName() const override { return "Sin"; }
        void* GetNodeDefaultValue() override { return &ConstA; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 0; }
        void GenerateDefinition(FMaterialCompiler* Compiler) override;

    };

    LUM_CLASS()
    class CMaterialExpression_Cosin : public CMaterialExpression_Math
    {
        GENERATED_BODY()
    public:
        
        
        void BuildNode() override;

        FString GetNodeDisplayName() const override { return "Cosin"; }
        void* GetNodeDefaultValue() override { return &ConstA; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 0; }
        void GenerateDefinition(FMaterialCompiler* Compiler) override;

    };

    LUM_CLASS()
    class CMaterialExpression_Floor : public CMaterialExpression_Math
    {
        GENERATED_BODY()
    public:
        
        
        void BuildNode() override;

        FString GetNodeDisplayName() const override { return "Floor"; }
        void* GetNodeDefaultValue() override { return &ConstA; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 0; }
        void GenerateDefinition(FMaterialCompiler* Compiler) override;

    };

    LUM_CLASS()
    class CMaterialExpression_Ceil : public CMaterialExpression_Math
    {
        GENERATED_BODY()
    public:
        
        
        void BuildNode() override;

        FString GetNodeDisplayName() const override { return "Ceil"; }
        void* GetNodeDefaultValue() override { return &ConstA; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 0; }
        void GenerateDefinition(FMaterialCompiler* Compiler) override;

    };

    LUM_CLASS()
    class CMaterialExpression_Power : public CMaterialExpression_Math
    {
        GENERATED_BODY()
    public:
        
        
        void BuildNode() override;

        FString GetNodeDisplayName() const override { return "Power"; }
        void* GetNodeDefaultValue() override { return &ConstA; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 0; }
        void GenerateDefinition(FMaterialCompiler* Compiler) override;

    };

    LUM_CLASS()
    class CMaterialExpression_Mod : public CMaterialExpression_Math
    {
        GENERATED_BODY()
    public:
        
        
        void BuildNode() override;

        FString GetNodeDisplayName() const override { return "Mod"; }
        void* GetNodeDefaultValue() override { return &ConstA; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 0; }
        void GenerateDefinition(FMaterialCompiler* Compiler) override;

    };

    LUM_CLASS()
    class CMaterialExpression_Min : public CMaterialExpression_Math
    {
        GENERATED_BODY()
    public:
        
        
        void BuildNode() override;

        FString GetNodeDisplayName() const override { return "Min"; }
        void* GetNodeDefaultValue() override { return &ConstA; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 0; }
        void GenerateDefinition(FMaterialCompiler* Compiler) override;

    };

    LUM_CLASS()
    class CMaterialExpression_Max : public CMaterialExpression_Math
    {
        GENERATED_BODY()
    public:
        
        
        void BuildNode() override;

        FString GetNodeDisplayName() const override { return "Max"; }
        void* GetNodeDefaultValue() override { return &ConstA; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 0; }
        void GenerateDefinition(FMaterialCompiler* Compiler) override;

    };

    LUM_CLASS()
    class CMaterialExpression_Step : public CMaterialExpression_Math
    {
        GENERATED_BODY()
    public:
        
        
        void BuildNode() override;

        FString GetNodeDisplayName() const override { return "Step"; }
        void* GetNodeDefaultValue() override { return &ConstA; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 0; }
        void GenerateDefinition(FMaterialCompiler* Compiler) override;
        
    };
    
    LUM_CLASS()
    class CMaterialExpression_Lerp : public CMaterialExpression_Math
    {
        GENERATED_BODY()
    public:
        
        void Serialize(FArchive& Ar) override;
        void BuildNode() override;

        FString GetNodeDisplayName() const override { return "Lerp"; }
        void* GetNodeDefaultValue() override { return &ConstA; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 0; }
        void GenerateDefinition(FMaterialCompiler* Compiler) override;
        
        float ConstC = 0;
        CMaterialInput* C = nullptr;

    };
    
    
    LUM_CLASS()
    class CMaterialExpression_Division : public CMaterialExpression_Math
    {
        GENERATED_BODY()
    public:
        
        void BuildNode() override;

        FString GetNodeDisplayName() const override { return "Divide"; }
        void* GetNodeDefaultValue() override { return &ConstA; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 0; }
        void GenerateDefinition(FMaterialCompiler* Compiler) override;
        
    };


    //============================================================================================

    LUM_CLASS()
    class CMaterialExpression_WorldPos : public CMaterialExpression
    {
        GENERATED_BODY()
    public:
        
        void BuildNode() override;

        FString GetNodeDisplayName() const override { return "WorldPosition"; }
        void* GetNodeDefaultValue() override { return nullptr; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 0; }
        void GenerateDefinition(FMaterialCompiler* Compiler) override;
    };

    LUM_CLASS()
    class CMaterialExpression_CameraPos : public CMaterialExpression
    {
        GENERATED_BODY()
    public:
        
        void BuildNode() override;

        FString GetNodeDisplayName() const override { return "CameraPosition"; }
        void* GetNodeDefaultValue() override { return nullptr; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override { return 0; }
        void GenerateDefinition(FMaterialCompiler* Compiler) override;
    };

    //============================================================================================


    LUM_CLASS()
    class CMaterialExpression_Constant : public CMaterialExpression
    {
        GENERATED_BODY()
    public:
        

        void Serialize(FArchive& Ar) override;

        
        void BuildNode() override;
        
        void* GetNodeDefaultValue() override { return &Value.R; }

        FColor              Value;
        EMaterialInputType  ValueType = EMaterialInputType::Wildcard;
        
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
