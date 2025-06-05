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

    inline FString Parent(const FString& Path)
    {
        return std::filesystem::path(Path.c_str()).parent_path().string().c_str();
    }

    template <typename... Paths>
    inline FString Combine(Paths&&... InPaths)
    {
        std::filesystem::path Path = (std::filesystem::path(std::forward<Paths>(InPaths)) /= ...);
        
        return Path.string().c_str();
    }

    inline FString DirName(const FString& InPath)
    {
        size_t LastSlash = InPath.find_last_of("/\\");
        if (LastSlash != FString::npos)
        {
            return InPath.substr(0, LastSlash);
        }
        return InPath;
    }
    
    inline FString FileName(const FString& InPath)
    {
        size_t LastSlash = InPath.find_last_of("/\\");
        if (LastSlash != FString::npos)
        {
            return InPath.substr(LastSlash + 1);
        }
        return InPath;
    }

    inline FString RemoveExtension(const FString& InPath)
    {
        size_t Dot = InPath.find_last_of(".");
        if (Dot != FString::npos)
        {
            return InPath.substr(0, Dot);
        }

        return InPath;
    }
    
    inline bool Exists(const FString& Filename)
    {
        return std::filesystem::exists(Filename.c_str());
    }

    inline bool IsUnderDirectory(const FString& ParentDirectory, const FString& Directory)
    {
        if (Directory.length() < ParentDirectory.length())
        {
            return false;
        }

        if (FString::comparei(Directory.data(), Directory.data() + ParentDirectory.length(),
                              ParentDirectory.data(), ParentDirectory.data() + ParentDirectory.length()) != 0)
        {
            return false;
        }

        if (Directory.length() > ParentDirectory.length())
        {
            char nextChar = Directory[ParentDirectory.length()];
            if (nextChar != '/' && nextChar != '\\')
            {
                return false;
            }
        }

        return true;
    }
    
    // Function to resolve a virtual path to an absolute path
    LUMINA_API FString ResolveVirtualPath(const FString& VirtualPath);

    // Function to convert an absolute path to a virtual path
    LUMINA_API FString ConvertToVirtualPath(const FString& AbsolutePath);

    
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
