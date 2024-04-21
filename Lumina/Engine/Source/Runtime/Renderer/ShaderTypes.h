#pragma once

#include <filesystem>
#include <glm/glm.hpp>

namespace Lumina
{
    enum class EShaderStage : glm::uint32;

    struct FShaderData
    {
        EShaderStage Stage;
        std::vector<glm::uint32> Binaries;
        std::filesystem::path RawPath;
    };
}
