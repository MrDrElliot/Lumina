#pragma once

#include <string>
#include <filesystem>
#include <cstdlib>


namespace Lumina::Paths
{
    inline std::filesystem::path GetEngineDirectory()
    {
        std::string luminaDirEnv(std::getenv("LUMINA_DIR"));

        // Check if the environment variable is set
        if (luminaDirEnv.size() > 0)
        {
            // Return the directory path from the environment variable
            std::filesystem::path finalDir = std::filesystem::path(luminaDirEnv) / "Lumina" / "Engine";
            return finalDir;
        }
        else
        {
            // Fall back to the default path if the environment variable is not set
            return (std::filesystem::current_path().parent_path() / "Lumina" / "Engine").string();
        }
    }
    
    inline std::filesystem::path GetEngineInstallDirectory()
    {
        return GetEngineDirectory().parent_path().parent_path();
    }

    inline bool SetEnvVariable(const std::string& name, const std::string& value)
    {
        // Platform-specific environment variable setting
        #ifdef _WIN32
        // Windows uses _putenv_s
        int result = _putenv_s(name.c_str(), value.c_str());
        if (result == 0)
        {
            std::cout << "Environment variable " << name << " set to " << value << std::endl;
            return true;
        }
        else
        {
            std::cerr << "Failed to set environment variable " << name << std::endl;
            return false;
        }
#else
        // POSIX (Linux/macOS) systems use setenv
        int result = setenv(name.c_str(), value.c_str(), 1);  // 1 to overwrite existing variable
        if (result == 0)
        {
            std::cout << "Environment variable " << name << " set to " << value << std::endl;
            return true;
        }
        else
        {
            std::cerr << "Failed to set environment variable " << name << std::endl;
            return false;
        }
#endif
    }


}