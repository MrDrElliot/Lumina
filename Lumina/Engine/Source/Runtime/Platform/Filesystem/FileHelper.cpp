#include "FileHelper.h"

#include <fstream>

#include "Containers/Array.h"
#include "Log/Log.h"
#include "Paths/Paths.h"

namespace Lumina::FileHelper
{
    bool SaveArrayToFile(const TVector<uint8>& Array, FStringView Path, uint32 WriteFlags)
    {
        std::filesystem::path FilePath = Path.data();

        
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

    bool LoadFileToArray(TVector<uint8>& Result, FStringView Path, uint32 ReadFlags)
    {
        Result.clear();
        
        std::ifstream InFile(Path.data(), std::ios::binary | std::ios::ate);
        if (!InFile)
        {
            Result.clear();
            LOG_ERROR("Failed to open file for reading: {0}", Path);
            return false;
        }

        std::streamsize FileSize = InFile.tellg();
        if (FileSize == -1)
        {
            Result.clear();
            LOG_ERROR("Failed to get the file size: {0}", Path);
            return false;
        }

        InFile.seekg(0, std::ios::beg);

        Result.resize(static_cast<size_t>(FileSize));

        if (!InFile.read(reinterpret_cast<char*>(Result.data()), FileSize))
        {
            Result.clear();
            LOG_ERROR("Failed to read data from file: {0}", Path);
            return false;
        }

        InFile.close();
        return true;
    }

    bool LoadFileToArray(TVector<uint8>& Result, FStringView Path, uint32 Seek, uint32 ReadSize, uint32 ReadFlags)
    {
        Result.clear();
        
        std::ifstream InFile(Path.data(), std::ios::binary | std::ios::ate);
        if (!InFile)
        {
            Result.clear();
            LOG_ERROR("Failed to open file for reading: {0}", Path);
            return false;
        }

        std::streamsize FileSize = InFile.tellg();
        if (FileSize == -1)
        {
            Result.clear();
            LOG_ERROR("Failed to get the file size: {0}", Path);
            return false;
        }

        InFile.seekg(Seek, std::ios::beg);

        size_t ActualRead = std::max<size_t>(ReadSize, FileSize);
        
        Result.resize(ActualRead);
        if (!InFile.read(reinterpret_cast<char*>(Result.data()), ActualRead))
        {
            Result.clear();
            LOG_ERROR("Failed to read data from file: {0}", Path);
            return false;
        }

        InFile.close();
        return true;
    }

    FString FileFinder(FStringView FileName, FStringView IteratorPath, bool bRecursive)
    {
        std::filesystem::path Path = IteratorPath.data();

        if (!std::filesystem::exists(Path) || !std::filesystem::is_directory(Path))
        {
            return "";
        }

        std::filesystem::directory_options Options = bRecursive 
            ? std::filesystem::directory_options::follow_directory_symlink
            : std::filesystem::directory_options::none;

        if (bRecursive)
        {
            for (const auto& Entry : std::filesystem::recursive_directory_iterator(Path, Options))
            {
                if (Entry.is_regular_file() && Entry.path().filename() == FileName.data())
                {
                    return Entry.path().string().c_str();
                }
            }
        }
        else
        {
            for (const auto& Entry : std::filesystem::directory_iterator(Path, Options))
            {
                if (Entry.is_regular_file() && Entry.path().filename() == FileName.data())
                {
                    return Entry.path().string().c_str();
                }
            }
        }

        return "";
    }
    
    bool LoadFileIntoString(FString& OutString, FStringView Path, uint32 ReadFlags)
    {
        std::ifstream file(Path.data(), std::ios::in);
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open file: {0}", Path.data());
            return false;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string String = buffer.str();
        OutString.assign(String.c_str());
        
        return OutString.size();
    }

    bool SaveStringToFile(FStringView String, FStringView Path, uint32 WriteFlags)
    {
        std::ofstream file(Path.data(), std::ios::out | std::ios::trunc);
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open file for writing: {0}", Path.data());
            return false;
        }

        file << String.data();
        file.close();

        return true;
    }

    bool DoesFileExist(FStringView FilePath)
    {
        return std::filesystem::exists(FilePath.data());
    }

    bool CreateNewFile(FStringView FilePath, bool bBinary, uint32 Flags)
    {
        if (std::filesystem::exists(FilePath.data())) 
        {
            LOG_ERROR("File already exists: {0}", FilePath.data());
            return false;
        }

        std::ios::openmode Mode = std::ios::out | std::ios::trunc;
        if (bBinary)
        {
            Mode |= std::ios::binary;
        }

        std::ofstream File(FilePath.data(), Mode);
        if (!File.is_open())
        {
            LOG_ERROR("Failed to create file: {0}", FilePath.data());
            return false;
        }

        File.close();
        return true;
    }

    uint64 GetFileSize(FStringView FilePath)
    {
        if (std::filesystem::exists(FilePath.data()))
        {
            return std::filesystem::file_size(FilePath.data());
        }

        return 0;
    }
}
