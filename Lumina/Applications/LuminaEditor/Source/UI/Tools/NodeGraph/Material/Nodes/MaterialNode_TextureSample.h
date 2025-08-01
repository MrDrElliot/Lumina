﻿#pragma once
#include "MaterialNodeExpression.h"
#include "Core/Object/ObjectHandleTyped.h"
#include "MaterialNode_TextureSample.generated.h"

namespace Lumina
{
    class CTexture;
}

namespace Lumina
{
    LUM_CLASS()
    class CMaterialExpression_TextureSample : public CMaterialExpression
    {
        GENERATED_BODY()
    public:

        void Serialize(FArchive& Ar) override;
        
        void BuildNode() override;
        void* GetNodeDefaultValue() override { return &Texture; }
        FString GetNodeDisplayName() const override { return "TextureSample"; }
        uint32 GenerateExpression(FMaterialCompiler* Compiler) override;
        void GenerateDefinition(FMaterialCompiler* Compiler) override;
        void SetNodeValue(void* Value) override;

        LUM_PROPERTY(Editable, Category = "Texture")
        TObjectHandle<CTexture> Texture;
    };
}
