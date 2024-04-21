#include "Shader.h"

#include "RHI/Vulkan/VulkanShader.h"


namespace Lumina
{
    std::shared_ptr<FShader> FShader::Create(std::vector<FShaderData> InData, const std::string& Tag)
    {
        return std::make_shared<FVulkanShader>(InData, Tag);
    }
}
