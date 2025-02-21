#include "FileHelper.h"

#include <fstream>

#include "Containers/Array.h"
#include "Log/Log.h"
#include "Paths/Paths.h"

namespace Lumina
{
    bool FFileHelper::SaveArrayToFile(const TVector<uint8>& Array, const FString& Path, uint32 WriteFlags)
    {
        std::filesystem::path FilePath = Path.c_str();

        
        std::ofstream outFile(FilePath, std::ios::binary | std::ios::trunc);
        if (!outFile)
        {
            LOG_ERROR("Failed to open file for writing: {0}", FilePath.string());
            return false;
        }

        if (!outFile.write(reinterpret_cast<const char*>(Array.data()), Array.size()))
        {
            LOG_ERROR("Failed to write data to file: {0}", FilePath.string());
            return false;
        }

        outFile.close();
        return true;
    }

    bool FFileHelper::LoadFileToArray(TVector<uint8>& Result, const FString& Path, uint32 ReadFlags)
    {
        std::filesystem::path FilePath = Path.c_str();
        
        std::ifstream inFile(FilePath, std::ios::binary | std::ios::ate);
        if (!inFile)
        {
            LOG_ERROR("Failed to open file for reading: {0}", FilePath.string());
            return false;
        }

        std::streamsize fileSize = inFile.tellg();
        if (fileSize == -1)
        {
            LOG_ERROR("Failed to get the file size: {0}", FilePath.string());
            return false;
        }

        inFile.seekg(0, std::ios::beg);

        Result.resize(static_cast<size_t>(fileSize));

        if (!inFile.read(reinterpret_cast<char*>(Result.data()), fileSize))
        {
            LOG_ERROR("Failed to read data from file: {0}", FilePath.string());
            return false;
        }

        inFile.close();
        return true;
    }

    bool FFileHelper::LoadFileIntoString(FString& OutString, const FString& Path, uint32 ReadFlags)
    {
        std::ifstream file(Path.c_str(), std::ios::in);
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open file: {0}", Path.c_str());
            return false;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string String = buffer.str();
        OutString.assign(String.c_str());
        
        return OutString.size();
    }
}
