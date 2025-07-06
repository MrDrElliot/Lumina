#pragma once
#include "MaterialNodeExpression.h"
#include "Core/Object/ObjectMacros.h"
#include "MaterialNodeGetTime.generated.h"



namespace Lumina
{
    LUM_CLASS()
    class CMaterialNodeGetTime : public CMaterialExpression
    {
        GENERATED_BODY()
    public:

        FString GetNodeDisplayName() const override { return "Time"; }
        void* GetNodeDefaultValue() override { return nullptr; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override;
        void GenerateDefinition(FMaterialCompiler* Compiler) override;
    
    };
}
