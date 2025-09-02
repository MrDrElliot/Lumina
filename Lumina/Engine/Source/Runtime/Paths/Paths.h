#pragma once

#include <filesystem>

#include "Containers/Name.h"
#include "Log/Log.h"
#include "Project/Project.h"

namespace Lumina::Paths
{

    /** Mounts a virtual file system to a specified path */
    LUMINA_API void Mount(const FName& VirtualPrefix, const FString& PhysicalPath);

    /** Unmounts a virtual file system */
    LUMINA_API void Unmount(const FName& VirtualPrefix);

    LUMINA_API const THashMap<FName, FString>& GetMountedPaths();
    
    /** Gets the directory where the Lumina engine is installed. */
    LUMINA_API FString GetEngineDirectory();
    
    /** 
     * Returns the filename from the given path.
     * @param InPath The full path.
     * @param bRemoveExtension If true, removes the file extension from the result.
     */
    LUMINA_API FString FileName(const FString& InPath, bool bRemoveExtension = false);

    LUMINA_API FString GetExtension(const FString& InPath);

    LUMINA_API bool HasExtension(const FString& Path, const FString& Ext);
    
    /** Removes and returns the path without its file extension. */
    LUMINA_API FString RemoveExtension(const FString& InPath);
    
    /** Checks whether the given file or directory exists. */
    LUMINA_API bool Exists(FStringView Filename);

    /** 
     * Checks if the given directory is under a specific parent directory.
     * This does a path-based comparison, not a file system query.
     */
    LUMINA_API bool IsUnderDirectory(const FString& ParentDirectory, const FString& Directory);
    
    /** 
     * Resolves a virtual path (e.g., starting with "/Lumina/") to an absolute path on the file system.
     */
    LUMINA_API FString ResolveVirtualPath(const FString& VirtualPath);

    /** 
     * Converts an absolute path on the file system to a virtual path understood by the Lumina engine.
     */
    LUMINA_API FString ConvertToVirtualPath(const FString& AbsolutePath);

    /**
     * Makes a path relative to a base path.
     * For example, MakeRelativeTo("/a/b/c.txt", "/a/") would return "b/c.txt".
     */
    LUMINA_API FString MakeRelativeTo(const FString& Path, const FString& BasePath);

    /**
     * Replaces the filename portion of a given path.
     * @param Path The full path whose filename will be replaced.
     * @param NewFilename The new filename to insert.
     */
    LUMINA_API void ReplaceFilename(FString& Path, const FString& NewFilename);
    
    /** Gets the path to the engine's resource directory. */
    LUMINA_API const FString& GetEngineResourceDirectory();

    /** Gets the path to the engine's content directory */
    LUMINA_API const FString& GetEngineContentDirectory();

    /** Gets the path to the engine's shaders */
    LUMINA_API const FString& GetEngineShadersDirectory();
    
    /** Gets the engine installation directory (one level above the engine binary). */
    LUMINA_API FString GetEngineInstallDirectory();
    
    /**
     * Sets an environment variable in a cross-platform way.
     * @param name The name of the environment variable.
     * @param value The value to set.
     * @return True if the operation succeeded, false otherwise.
     */
    LUMINA_API bool SetEnvVariable(const FString& name, const FString& value);


    LUMINA_API void AddPackageExtension(FString& FileName);
    
    // -------------------------------------------------------------------

    template<typename T>
    concept ValidStringType = requires(T s)
    {
        typename            T::value_type;
        { s.c_str() }       -> std::convertible_to<const typename T::value_type*>;
    };
    
    /**
     * Combines multiple path segments into a single normalized path string.
     * @param InPaths One or more path fragments to join.
     * @return The combined path as an FString.
     */
    template <typename... Paths>
    inline FString Combine(Paths&&... InPaths)
    {
        std::filesystem::path Path = (std::filesystem::path(std::forward<Paths>(InPaths)) /= ...);
        return Path.string().c_str();
    }

    
    template<ValidStringType StringType>
    StringType Parent(const StringType& path)
    {
        return std::filesystem::path::path(path.c_str()).parent_path().generic_string().c_str();
    }
    

    template<ValidStringType StringType>
    StringType DirName(const StringType& String)
    {
        size_t LastSlash = String.find_last_of("/\\");
        if (LastSlash != FString::npos)
        {
            return String.substr(0, LastSlash);
        }
        return String;
    }
}
