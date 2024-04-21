#pragma once
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <glm/glm.hpp>

#include "ShaderTypes.h"

namespace Lumina
{
    enum class EShaderStage : glm::uint32
    {
        VERTEX,
        FRAGMENT,
        COMPUTE,
        UNKNOWN
    };
    
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
    
    enum class EShaderDataType : glm::uint32
    {
        INT, INT2, INT3, INT4,
        FLOAT, FLOAT2, FLOAT3, FLOAT4,
        IMAT3, IMAT4, MAT3, MAT4
    };

    constexpr glm::uint32 DeviceDataTypeSize(const EShaderDataType& Type)
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

    class FShader
    {
    public:

        static std::shared_ptr<FShader> Create(std::vector<FShaderData> InData, const std::string& Tag);
        virtual ~FShader() {}
        virtual void Destroy() = 0;

        virtual bool IsDirty() const = 0;
        virtual void SetDirty(bool dirty) = 0;

        virtual void RestoreShaderModule(std::filesystem::path path) = 0;
        
    };
}
