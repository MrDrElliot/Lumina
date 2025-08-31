#include "Paths.h"
#include <cstdlib>

#include "Core/Assertions/Assert.h"

namespace Lumina::Paths
{

    static THashMap<FName, FString> VirtualPathMap;
    static THashMap<FName, FString> CachedDirectories;

    namespace
    {
        const char* EngineResourceDirectoryName = "EngineResourceDirectory";
        const char* EngineContentDirectoryName = "EngineContentDirectory";
        const char* EngineShadersDirectoryName = "EngineShadersDirectory";
        const char* EngineDirectoryName = "EngineDirectory";

    }
    
    void Mount(const FName& VirtualPrefix, const FString& PhysicalPath)
    {
        Assert(VirtualPathMap.find(VirtualPrefix) == VirtualPathMap.end())

        LOG_DEBUG("Virtual file path mounted: {} - {}", VirtualPrefix, PhysicalPath);
        VirtualPathMap[VirtualPrefix] = PhysicalPath;
    }

    void Unmount(const FName& VirtualPrefix)
    {
        auto Itr = VirtualPathMap.find(VirtualPrefix);
        Assert(Itr != VirtualPathMap.end())

        LOG_DEBUG("Virtual file path unmounted: {} - {}", VirtualPrefix, Itr->second);
        VirtualPathMap.erase(Itr);
    }

    const THashMap<FName, FString>& GetMountedPaths()
    {
        return VirtualPathMap;
    }

    FString GetEngineDirectory()
    {
        if (CachedDirectories.find(EngineDirectoryName) == CachedDirectories.end())
        {
            const char* LuminaDir = std::getenv("LUMINA_DIR");
            
            CachedDirectories[EngineDirectoryName] = FString(LuminaDir) + "/Lumina/Engine";
        }

        return CachedDirectories[EngineDirectoryName];
    }
    

    FString Parent(const FString& Path)
    {
        return std::filesystem::path(Path.c_str()).parent_path().generic_string().c_str();
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
        size_t Dot = InPath.find_last_of('.');
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
        FString NormalizedPath = RemoveExtension(VirtualPath);
        StringUtils::ReplaceAllOccurrencesInPlace(NormalizedPath, "\\", "/");

        for (const auto& [VirtualPrefix, PhysicalPath] : VirtualPathMap)
        {
            const FString& PrefixStr = VirtualPrefix.ToString();
            if (StringUtils::StartsWith(NormalizedPath, PrefixStr.c_str()))
            {
                FString Remaining = NormalizedPath.substr(PrefixStr.length());
                return PhysicalPath + "/" + Remaining;
            }
        }

        return NormalizedPath;
    }

    FString ConvertToVirtualPath(const FString& AbsolutePath)
    {
        FString NormalizedAbsolutePath = AbsolutePath;
        StringUtils::ReplaceAllOccurrencesInPlace(NormalizedAbsolutePath, "\\", "/");

        for (const auto& [VirtualPrefix, PhysicalPath] : VirtualPathMap)
        {
            FString NormalizedPhysical = PhysicalPath;
            StringUtils::ReplaceAllOccurrencesInPlace(NormalizedPhysical, "\\", "/");

            if (StringUtils::StartsWith(NormalizedAbsolutePath, NormalizedPhysical.c_str()))
            {
                FString Remaining = NormalizedAbsolutePath.substr(NormalizedPhysical.length());
                if (StringUtils::StartsWith(Remaining, "/"))
                {
                    Remaining.erase(0, 1);
                }

                Remaining = RemoveExtension(Remaining);
                return VirtualPrefix.ToString() + Remaining;
            }
        }

        return RemoveExtension(NormalizedAbsolutePath);
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

    FString GetEngineResourceDirectory()
    {
        if (CachedDirectories.find(EngineResourceDirectoryName) == CachedDirectories.end())
        {
            const char* LuminaDir = std::getenv("LUMINA_DIR");
            return CachedDirectories[EngineResourceDirectoryName] = FString(LuminaDir) + "/Lumina/Engine/Resources";
        }

        return CachedDirectories[EngineResourceDirectoryName];
    }

    FString GetEngineContentDirectory()
    {
        if (CachedDirectories.find(EngineContentDirectoryName) == CachedDirectories.end())
        {
            return CachedDirectories[EngineContentDirectoryName] = FString(GetEngineResourceDirectory() + "/Content");
        }

        return CachedDirectories[EngineContentDirectoryName];
    }

    FString GetEngineShadersDirectory()
    {
        if (CachedDirectories.find(EngineShadersDirectoryName) == CachedDirectories.end())
        {
            return CachedDirectories[EngineShadersDirectoryName] = FString(GetEngineResourceDirectory() + "/Shaders");
        }

        return CachedDirectories[EngineShadersDirectoryName];
    }

    FString GetEngineInstallDirectory()
    {
        return Parent(GetEngineDirectory());
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
