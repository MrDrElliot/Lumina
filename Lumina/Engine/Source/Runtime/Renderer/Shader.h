#pragma once

#include "RenderResource.h"
#include "RenderTypes.h"
#include "ShaderParameters.h"
#include "ShaderTypes.h"
#include "spirv_reflect.h"
#include "Containers/String.h"
#include "Containers/Name.h"

namespace Lumina
{
    class FBuffer;
    class FDescriptorSet;

    enum class EShaderStage : uint8
    {
        NONE                    = 0,           // No shader stage
        VERTEX                  = 1 << 0,      // 1
        FRAGMENT                = 1 << 1,      // 2
        COMPUTE                 = 1 << 2,      // 4
        GEOMETRY                = 1 << 3,      // 8
        TESSELLATION_CONTROL    = 1 << 4,      // 16
        TESSELLATION_EVALUATION = 1 << 5,      // 32
        ALL_GRAPHICS            = VERTEX | FRAGMENT | GEOMETRY | TESSELLATION_CONTROL | TESSELLATION_EVALUATION,
        ALL                     = ALL_GRAPHICS | COMPUTE,
        UNKNOWN                 = 1 << 7       // 128 (for any undefined or unknown stage)
    };

    // Enable bitwise operations for EShaderStage
    inline EShaderStage operator|(EShaderStage lhs, EShaderStage rhs)
    {
        return static_cast<EShaderStage>(static_cast<uint8>(lhs) | static_cast<uint8>(rhs));
    }

    inline EShaderStage operator&(EShaderStage lhs, EShaderStage rhs)
    {
        return static_cast<EShaderStage>(static_cast<uint8>(lhs) & static_cast<uint8>(rhs));
    }

    inline bool operator!(EShaderStage stage)
    {
        return static_cast<uint8>(stage) == 0;
    }
    
    inline FString StageToString(const EShaderStage& Stage)
    {
        switch (Stage)
        {
            case EShaderStage::VERTEX:		    return "vertex";
            case EShaderStage::FRAGMENT:		return "fragment";
            case EShaderStage::COMPUTE:		    return "compute";
            case EShaderStage::UNKNOWN:		    return "unknown";
        }
        return "error";
    }
    
    struct FPushConstantMember
    {
        FName Name;
        uint32 Size;
        uint32 Offset;
    };

    struct FPushConstantRange
    {
        FName Name;
        TVector<FPushConstantMember>    Members;
        uint32                          StageFlags = 0;
        
        // This is the lowest offset of all memebers
        uint32                          Offset = 0;
        uint32                          AbsoluteOffset = 0;
        uint32                          Size = 0;
        uint32                          PaddedSize = 0;
    };

    struct FInputVariableBinding
    {
        FName Name;                  // The name of the input variable
        uint32 Location = 0;         // The location (or binding) of the input variable in the shader
        uint32 Size = 0;             // Size of the input variable in bytes
        uint32 Count = 0;            // Number of elements (for arrays or vectors)
    };
    
    struct FShaderReflectionData
    {
        TVector<THashMap<FName, FDescriptorBinding>>    DescriptorBindings;
        THashMap<FName, FPushConstantRange>             PushConstantRanges;
        TVector<FInputVariableBinding>                  InputVariableBindings;
    };

    class FShader : public IRenderResource
    {
    public:
        
        static TRefCountPtr<FShader> Create();
        virtual ~FShader() {}

        virtual void CreateStage(const FShaderStage& StageData) = 0;
        
        /** Generate platform-agonstic shader reflection data. */
        void GenerateShaderStageReflectionData(const FShaderStage& StageData, SpvReflectShaderModule* ReflectionModule);

        /** Give the platform specific shader a chance to generate reflection data. Called after all stages have been reflected. */
        virtual void GeneratePlatformShaderStageReflectionData(const FShaderReflectionData& ReflectionData) = 0;

        /** Returns platform agonistic shader reflection data */
        FORCEINLINE const FShaderReflectionData& GetShaderReflectionData() const { return ShaderReflectionData; }

        /** Do not store this pointer, it is transient and tied to the lifetime of this shader, you will end up with stale or invalid memory */
        FORCEINLINE virtual const void* GetPlatformReflectionData() const = 0;
        
        
        void PrintShaderReflectionData();
        
    protected:

        FShaderReflectionData                       ShaderReflectionData;
    };
}
