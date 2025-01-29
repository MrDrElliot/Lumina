#include "Shader.h"

#include "RHI/Vulkan/VulkanShader.h"


namespace Lumina
{
    TRefPtr<FShader> FShader::Create(const TVector<FShaderData>& InData, const FString& Tag)
    {
        return MakeRefPtr<FVulkanShader>(InData, Tag);
    }
}
