#pragma once

#include <filesystem>
#include <cstdlib>
#include "Log/Log.h"
#include "Project/Project.h"

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

    inline FString Combine(const FString& A, const FString& B)
    {
        std::filesystem::path PathA = A.c_str();
        std::filesystem::path PathB = B.c_str();

        std::filesystem::path Combined = PathA / PathB;

        return Combined.string().c_str();
    }
    

    // Function to resolve a virtual path to an absolute path
    inline FString ResolveVirtualPath(const FString& VirtualPath)
    {
        if (StringUtils::StartsWith(VirtualPath, "project://"))
        {
            // Remove the "project://" prefix and normalize the path to use forward slashes
            FString PathWithoutPrefix = VirtualPath.substr(10);  // Remove "project://"
            StringUtils::ReplaceAllOccurrencesInPlace(PathWithoutPrefix, "\\", "/");

            // Get the project content directory and append the relative path
            return FProject::Get()->GetProjectContentDirectory() + PathWithoutPrefix;
        }
        return VirtualPath;  // If the virtual path doesn't start with "project://", return it unchanged
    }

    // Function to convert an absolute path to a virtual path
    inline FString ConvertToVirtualPath(const FString& AbsolutePath)
    {
        FString ProjectDir = FProject::Get()->GetProjectContentDirectory();
    
        // Normalize the absolute path to use forward slashes
        FString NormalizedAbsolutePath = AbsolutePath;
        StringUtils::ReplaceAllOccurrencesInPlace(NormalizedAbsolutePath, "\\", "/");

        // If the absolute path starts with the project directory, convert it to a virtual path
        if (StringUtils::StartsWith(NormalizedAbsolutePath, ProjectDir.c_str()))
        {
            // Replace the project content directory prefix with "project://"
            return FString("project://") + NormalizedAbsolutePath.substr(ProjectDir.length());
        }
        return AbsolutePath;  // Return the path as-is if it's not part of the project
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

    inline void ReplaceFilename(FString& Path, const FString& NewFilename)
    {
        // Find the last occurrence of a path separator
        size_t LastSlash = Path.find_last_of("/\\");
    
        if (LastSlash != FString::npos)
        {
            // Extract directory and append the new filename
            Path = Path.substr(0, LastSlash + 1) + NewFilename;
            return;
        }

        // If no path separator is found, assume it's just a filename and replace it entirely
        Path = NewFilename;
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
