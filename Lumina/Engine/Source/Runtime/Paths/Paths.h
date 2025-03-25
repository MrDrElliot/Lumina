#pragma once

#include <filesystem>
#include <cstdlib>

namespace Lumina::Paths
{
    // Gets the directory where the Lumina engine is installed.
    inline std::filesystem::path GetEngineDirectory()
    {
        const char* luminaDirEnv = std::getenv("LUMINA_DIR");

        if (luminaDirEnv && std::strlen(luminaDirEnv) > 0)
        {
            return std::filesystem::path(luminaDirEnv) / "Lumina" / "Engine";
        }

        // Fallback: Assume the engine is two levels up from the executable
        return std::filesystem::canonical(std::filesystem::current_path().parent_path().parent_path() / "Lumina" / "Engine");
    }

    inline FString CombinePaths(const FString& A, const FString& B)
    {
        std::filesystem::path PathA = A.c_str();
        std::filesystem::path PathB = B.c_str();

        std::filesystem::path Combined = PathA / PathB;

        return Combined.string().c_str();
    }

    
    inline FString MakeRelativeTo(const FString& Path, const FString& BasePath)
    {
        std::filesystem::path FullPath = std::filesystem::weakly_canonical(Path.c_str());
        std::filesystem::path Base = std::filesystem::weakly_canonical(BasePath.c_str());

        if (!std::filesystem::exists(Base) || !std::filesystem::is_directory(Base))
        {
            return Path;
        }

        if (!std::filesystem::equivalent(FullPath.root_path(), Base.root_path()))
        {
            return Path;
        }

        std::error_code ec;
        std::filesystem::path RelativePath = std::filesystem::relative(FullPath, Base, ec);
    
        if (ec)
        {
            LOG_ERROR("Path Error: {0}", ec.message());
            return Path;
        }

        return RelativePath.string().c_str();
    }

    
    inline std::filesystem::path GetEngineResourceDirectory()
    {
        const char* luminaDirEnv = std::getenv("LUMINA_DIR");

        if (luminaDirEnv && std::strlen(luminaDirEnv) > 0)
        {
            return std::filesystem::path(luminaDirEnv) / "Lumina" / "Engine" / "Resources";
        }

        // Fallback: Assume the engine is two levels up from the executable
        return std::filesystem::canonical(std::filesystem::current_path().parent_path().parent_path() / "Lumina" / "Engine " / "Resources");
    }
    
    // Gets the directory where the Lumina engine is installed (one level up from the engine).
    inline std::filesystem::path GetEngineInstallDirectory()
    {
        return GetEngineDirectory().parent_path();
    }

    // Resolves a relative path from the engine's install directory.
    inline std::filesystem::path ResolveFromEngine(const std::filesystem::path& relativePath)
    {
        return GetEngineInstallDirectory() / relativePath;
    }
    
    // Sets an environment variable (cross-platform).
    inline bool SetEnvVariable(const FString& name, const FString& value)
    {
    #ifdef _WIN32
        if (_putenv_s(name.c_str(), value.c_str()) == 0)
        {
            LOG_TRACE("Environment variable {} set to {}", name, value);
            return true;
        }
        LOG_WARN("Failed to set environment variable {}", name);
        return false;
    #else
        if (setenv(name.c_str(), value.c_str(), 1) == 0)
        {
            LOG_TRACE("Environment variable {} set to {}", name, value);
            return true;
        }
        LOG_WARN("Failed to set environment variable {}", name);
        return false;
    #endif
    }
}
