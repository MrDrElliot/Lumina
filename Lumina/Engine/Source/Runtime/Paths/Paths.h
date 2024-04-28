#pragma once
#include <filesystem>

namespace Lumina::Paths
{

    std::filesystem::path GetProjectPath();
    std::filesystem::path GetEnginePath();

    void CreateFilePath(std::filesystem::path NewPath);
    
}
