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

    FString FFileHelper::FileFinder(const FString& FileName, const FString& IteratorPath, bool bRecursive)
    {
        std::filesystem::path Path = IteratorPath.c_str();

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
                if (Entry.is_regular_file() && Entry.path().filename() == FileName.c_str())
                {
                    return Entry.path().string().c_str();
                }
            }
        }
        else
        {
            for (const auto& Entry : std::filesystem::directory_iterator(Path, Options))
            {
                if (Entry.is_regular_file() && Entry.path().filename() == FileName.c_str())
                {
                    return Entry.path().string().c_str();
                }
            }
        }

        return "";
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

    bool FFileHelper::SaveStringToFile(const FStringView& String, const FString& Path, uint32 WriteFlags)
    {
        std::ofstream file(Path.c_str(), std::ios::out | std::ios::trunc);
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open file for writing: {0}", Path.c_str());
            return false;
        }

        file << String.data();
        file.close();

        return true;
    }

    bool FFileHelper::DoesFileExist(const FString& FilePath)
    {
        return std::filesystem::exists(FilePath.c_str());
    }

    bool FFileHelper::CreateNewFile(const FString& FilePath, bool bBinary, uint32 Flags)
    {
        if (std::filesystem::exists(FilePath.c_str())) 
        {
            LOG_ERROR("File already exists: {0}", FilePath.c_str());
            return false;
        }

        std::ios::openmode Mode = std::ios::out | std::ios::trunc;
        if (bBinary)
        {
            Mode |= std::ios::binary;
        }

        std::ofstream File(FilePath.c_str(), Mode);
        if (!File.is_open())
        {
            LOG_ERROR("Failed to create file: {0}", FilePath.c_str());
            return false;
        }

        File.close();
        return true;
    }

    uint64 FFileHelper::GetFileSize(const FString& FilePath)
    {
        if (std::filesystem::exists(FilePath.c_str()))
        {
            return std::filesystem::file_size(FilePath.c_str());
        }

        return 0;
    }
}
