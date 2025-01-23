#pragma once

#include <string>

#include "RenderResource.h"
#include "ShaderTypes.h"
#include "Containers/String.h"

namespace Lumina
{
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
    
    inline constexpr std::string StageToString(const EShaderStage& Stage)
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

    
    enum class EShaderDataType : uint8
    {
        INT,
        INT2,
        INT3,
        INT4,

        FLOAT,
        FLOAT2,
        FLOAT3,
        FLOAT4,

        IMAT3,
        IMAT4,

        MAT3,
        MAT4
    };

    constexpr uint32 DeviceDataTypeSize(const EShaderDataType& Type)
    {
        switch (Type)
        {
            case EShaderDataType::INT:			return 4;
            case EShaderDataType::INT2:			return 4 * 2;
            case EShaderDataType::INT3:			return 4 * 3;
            case EShaderDataType::INT4:			return 4 * 4;
            case EShaderDataType::FLOAT:		return 4;
            case EShaderDataType::FLOAT2:		return 4 * 2;
            case EShaderDataType::FLOAT3:		return 4 * 3;
            case EShaderDataType::FLOAT4:		return 4 * 4;
            case EShaderDataType::IMAT3:		return 4 * 3 * 3;
            case EShaderDataType::IMAT4:		return 4 * 4 * 4;
            case EShaderDataType::MAT3:			return 4 * 3 * 3;
            case EShaderDataType::MAT4:			return 4 * 4 * 4;
            default:							std::unreachable();
        }
    }

    class FShader : public FRenderResource
    {
    public:

        static TRefPtr<FShader> Create(const TFastVector<FShaderData>& InData, const LString& Tag);
        virtual ~FShader() {}

        virtual bool IsDirty() const = 0;
        virtual void SetDirty(bool dirty) = 0;

        virtual void RestoreShaderModule(std::filesystem::path path) = 0;


        
    };
}
