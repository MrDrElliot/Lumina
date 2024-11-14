#pragma once

#include <string>
#include <filesystem>


namespace Lumina::Paths
{
    inline std::string GetProjectDirectory()
    {
        return std::filesystem::current_path().string();
    }

    inline std::string GetEngineDirectory()
    {
        return (std::filesystem::current_path().parent_path() / "Lumina" / "Engine" ).string();
    }
}