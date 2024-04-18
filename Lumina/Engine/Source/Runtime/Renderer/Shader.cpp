#include "Shader.h"

#include "RHI/Vulkan/VulkanShader.h"

namespace Lumina
{
    std::shared_ptr<FShader> FShader::Create(std::map<EShaderStage, std::vector<glm::uint32>> Binaries, std::filesystem::path Path)
    {
        return std::make_shared<FVulkanShader>(Binaries, Path);
    }
}
