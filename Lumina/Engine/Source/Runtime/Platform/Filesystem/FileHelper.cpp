#include "FileHelper.h"

#include <fstream>

#include "Containers/Array.h"
#include "Log/Log.h"
#include "Paths/Paths.h"

namespace Lumina
{
    bool FFileHelper::SaveArrayToFile(const TArray<uint8>& Array, const std::filesystem::path& Path, uint32 WriteFlags)
    {
        std::ofstream outFile(Path, std::ios::binary | std::ios::trunc);
        if (!outFile)
        {
            LOG_ERROR("Failed to open file for writing: {0}", Path.string());
            return false;
        }

        // Write the array data
        if (!outFile.write(reinterpret_cast<const char*>(Array.data()), Array.size()))
        {
            LOG_ERROR("Failed to write data to file: {0}", Path.string());
            return false;
        }

        outFile.close();
        return true;
    }

    bool FFileHelper::LoadFileToArray(TArray<uint8>& Result, const std::filesystem::path& Path, uint32 ReadFlags)
    {
        std::ifstream inFile(Path, std::ios::binary | std::ios::ate);
        if (!inFile)
        {
            LOG_ERROR("Failed to open file for reading: {0}", Path.string());
            return false;
        }

        std::streamsize fileSize = inFile.tellg();
        if (fileSize == -1)
        {
            LOG_ERROR("Failed to get the file size: {0}", Path.string());
            return false;
        }

        inFile.seekg(0, std::ios::beg);

        // Resize the result vector to hold the file data
        Result.resize(static_cast<size_t>(fileSize));

        // Read the file data into the result vector
        if (!inFile.read(reinterpret_cast<char*>(Result.data()), fileSize))
        {
            LOG_ERROR("Failed to read data from file: {0}", Path.string());
            return false;
        }

        inFile.close();
        return true;
    }
}
