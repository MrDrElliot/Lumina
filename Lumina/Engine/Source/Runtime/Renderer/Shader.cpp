#include "Shader.h"

#include "RHI/Vulkan/VulkanShader.h"


namespace Lumina
{
    TRefPtr<FShader> FShader::Create(const TArray<FShaderData>& InData, const LString& Tag)
    {
        return MakeRefPtr<FVulkanShader>(InData, Tag);
    }
}
