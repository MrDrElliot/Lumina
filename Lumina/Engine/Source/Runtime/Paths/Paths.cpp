#include "Paths.h"
#include <cstdlib>

namespace Lumina::Paths
{
    std::filesystem::path GetEngineDirectory()
    {
        const char* luminaDirEnv = std::getenv("LUMINA_DIR");

        if (luminaDirEnv && std::strlen(luminaDirEnv) > 0)
        {
            return std::filesystem::path(luminaDirEnv) / "Lumina" / "Engine";
        }

        // Fallback: Assume the engine is two levels up from the executable
        return std::filesystem::canonical(std::filesystem::current_path().parent_path().parent_path() / "Lumina" / "Engine");
    }

    std::filesystem::path GetEngineBinariesDirectory()
    {
        const char* luminaDirEnv = std::getenv("LUMINA_DIR");

        if (luminaDirEnv && std::strlen(luminaDirEnv) > 0)
        {
#if defined (_DEBUG)
            return std::filesystem::path(luminaDirEnv) / "Binaries" / "Debug-windows-x86_64";
#endif
        }

        // Fallback: Assume the engine is two levels up from the executable
        return std::filesystem::canonical(std::filesystem::current_path().parent_path().parent_path() / "Lumina" / "Engine");
    }

    FString Parent(const FString& Path)
    {
        return std::filesystem::path(Path.c_str()).parent_path().string().c_str();
    }

    FString DirName(const FString& InPath)
    {
        size_t LastSlash = InPath.find_last_of("/\\");
        if (LastSlash != FString::npos)
        {
            return InPath.substr(0, LastSlash);
        }
        return InPath;
    }

    FString FileName(const FString& InPath, bool bRemoveExtension)
    {
        size_t LastSlash = InPath.find_last_of("/\\");
        FString FilePart = (LastSlash != FString::npos) ? InPath.substr(LastSlash + 1) : InPath;

        if (bRemoveExtension)
        {
            size_t DotPos = FilePart.find_last_of('.');
            if (DotPos != FString::npos)
            {
                return FilePart.substr(0, DotPos);
            }
        }

        return FilePart;
    }

    FString GetExtension(const FString& InPath)
    {
        size_t Dot = InPath.find_last_of(".");
        if (Dot != FString::npos && Dot + 1 < InPath.length())
        {
            return InPath.substr(Dot + 1);
        }

        return InPath;
    }

    bool HasExtension(const FString& Path, const FString& Ext)
    {
        size_t Dot = Path.find_last_of('.');
        if (Dot == FString::npos || Dot + 1 >= Path.length())
            return false;

        FString ActualExt = Path.substr(Dot + 1);
        return StringUtils::ToLower(ActualExt) == StringUtils::ToLower(Ext);
    }

    FString RemoveExtension(const FString& InPath)
    {
        size_t Dot = InPath.find_last_of(".");
        if (Dot != FString::npos)
        {
            return InPath.substr(0, Dot);
        }

        return InPath;
    }

    bool Exists(const FString& Filename)
    {
        return std::filesystem::exists(Filename.c_str());
    }

    bool IsUnderDirectory(const FString& ParentDirectory, const FString& Directory)
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
    
    FString ResolveVirtualPath(const FString& VirtualPath)
    {
        if (StringUtils::StartsWith(VirtualPath, "project://"))
        {
            FString PathWithoutPrefix = VirtualPath.substr(10);
        
            return FProject::Get()->GetProjectContentDirectory() + "/" + PathWithoutPrefix;
        }
    
        return VirtualPath;
    }

    FString ConvertToVirtualPath(const FString& AbsolutePath)
    {
        FString VirtualPath = FString();
        FString ProjectDir = FProject::Get()->GetProjectContentDirectory();
    
        FString NormalizedAbsolutePath = AbsolutePath;
        StringUtils::ReplaceAllOccurrencesInPlace(NormalizedAbsolutePath, "\\", "/");
    

        if (StringUtils::StartsWith(NormalizedAbsolutePath, ProjectDir.c_str()))
        {
            FString AdditionalString = NormalizedAbsolutePath.substr(ProjectDir.length());

            if (StringUtils::StartsWith(AdditionalString, "/"))
            {
                AdditionalString.erase(0, 1);
            }
        
            VirtualPath = FString("project://") + AdditionalString;
        }
    
        return Paths::RemoveExtension(VirtualPath);
    }

    FString MakeRelativeTo(const FString& Path, const FString& BasePath)
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

    void ReplaceFilename(FString& Path, const FString& NewFilename)
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

    std::filesystem::path GetEngineResourceDirectory()
    {
        const char* luminaDirEnv = std::getenv("LUMINA_DIR");

        if (luminaDirEnv && std::strlen(luminaDirEnv) > 0)
        {
            return std::filesystem::path(luminaDirEnv) / "Lumina" / "Engine" / "Resources";
        }

        // Fallback: Assume the engine is two levels up from the executable
        return std::filesystem::canonical(std::filesystem::current_path().parent_path().parent_path() / "Lumina" / "Engine " / "Resources");
    }

    std::filesystem::path GetEngineInstallDirectory()
    {
        return GetEngineDirectory().parent_path();
    }

    std::filesystem::path ResolveFromEngine(const std::filesystem::path& relativePath)
    {
        return GetEngineInstallDirectory() / relativePath;
    }

    
    void AddPackageExtension(FString& FileName)
    {
        FileName += ".lasset";
    }
    
    bool SetEnvVariable(const FString& name, const FString& value)
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
