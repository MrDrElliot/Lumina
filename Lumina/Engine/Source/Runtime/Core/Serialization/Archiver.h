#pragma once

#include <string>
#include <vector>
#include <type_traits>
#include <stdexcept>
#include <fstream>
#include <filesystem>
#include "Core/Assertions/Assert.h"

#include "Containers/Array.h"
#include "Log/Log.h"

enum class EArchiverFlags : uint8
{
    None      = 0,
    Reading   = 1 << 0,
    Writing   = 1 << 1,
    Compress  = 1 << 2,
    Encrypt   = 1 << 3
};

// Bitwise operators for EArchiverFlags
inline EArchiverFlags operator|(EArchiverFlags a, EArchiverFlags b)
{
    return static_cast<EArchiverFlags>(static_cast<uint8>(a) | static_cast<uint8>(b));
}

inline EArchiverFlags operator&(EArchiverFlags a, EArchiverFlags b)
{
    return static_cast<EArchiverFlags>(static_cast<uint8>(a) & static_cast<uint8>(b));
}

inline EArchiverFlags operator~(EArchiverFlags flag)
{
    return static_cast<EArchiverFlags>(~static_cast<uint8>(flag));
}

class FArchive
{
public:
    
    FArchive(EArchiverFlags flags) : Flags(flags) {}

    // Set, remove, and check flags
    void SetFlag(EArchiverFlags flag) { Flags = Flags | flag; }
    void RemoveFlag(EArchiverFlags flag) { Flags = Flags & ~flag; }
    bool HasFlag(EArchiverFlags flag) const { return (Flags & flag) != EArchiverFlags::None; }

    bool IsWriting() const { return HasFlag(EArchiverFlags::Writing); }
    bool IsReading() const { return HasFlag(EArchiverFlags::Reading); }

    // Combined << operator for reading/writing data based on mode
    template<typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value || std::is_enum<T>::value>>
    FArchive& operator<<(T& value)
    {
        if (HasFlag(EArchiverFlags::Writing))
        {
            WriteToBuffer(reinterpret_cast<const uint8*>(&value), sizeof(T));
        }
        else if (HasFlag(EArchiverFlags::Reading))
        {
            ReadFromBuffer(reinterpret_cast<uint8*>(&value), sizeof(T));
        }
        else
        {
            AssertMsg(false, "Archive mode not set to reading or writing.");
        }

        return *this;
    }

    template<typename T>
    FArchive& operator << (Lumina::TFastVector<T>& data)
    {
        if(IsWriting())
        {
            size_t size = data.size();
            *this << size;

            if constexpr (std::is_arithmetic<T>::value)
            {
                WriteToBuffer(reinterpret_cast<const uint8*>(data.data()), size * sizeof(T));
            }
            else
            {
                for (size_t i = 0; i < size; ++i)
                {
                    *this << data[i];
                }
            }
        }
        else if(IsReading())
        {
            size_t size;
            *this << size;
            data.resize(size);

            if constexpr (std::is_arithmetic<T>::value)
            {
                ReadFromBuffer(reinterpret_cast<uint8*>(data.data()), size * sizeof(T));
            }
            else
            {
                size_t totalSize = size * sizeof(T);
                uint8* buffer = reinterpret_cast<uint8*>(data.data());
                ReadFromBuffer(buffer, totalSize);
                /*for (size_t i = 0; i < size; ++i)
                {
                    *this << data[i];
                }*/
            }
        }

        return *this;
    }
    
    // Combined << operator overload for std::string
    FArchive& operator<<(std::string& str)
    {
        if (IsWriting())
        {
            size_t size = str.size();
            *this << size;  // Write the size of the string
            WriteToBuffer(reinterpret_cast<const uint8*>(str.data()), size);
        }
        else if (IsReading())
        {
            size_t size;
            *this << size;  // Read the size of the string
            str.resize(size);
            ReadFromBuffer(reinterpret_cast<uint8*>(&str[0]), size);
        }
        else
        {
             AssertMsg(false, "Archive mode not set to reading or writing.");
        }

        return *this;
    }

    
    // Write the entire buffer to a file
    void WriteToFile(const std::filesystem::path& filePath)
    {
        std::ofstream outFile(filePath, std::ios::binary | std::ios::trunc);
        if (!outFile)
        {
             LOG_ERROR("Failed to open file for writing: {0}", filePath.string());
        }
        outFile.write(reinterpret_cast<const char*>(Buffer.data()), Buffer.size());
        outFile.close();
    }

    // Load the entire buffer from a file
    void ReadFromFile(const std::string& filePath)
    {
        std::ifstream inFile(filePath, std::ios::binary | std::ios::ate);
        if (!inFile)
        {
            LOG_ERROR("Failed to open file for reading: {0}", filePath);
            return;
        }

        std::streamsize fileSize = inFile.tellg();
        inFile.seekg(0, std::ios::beg);

        Buffer.resize(fileSize);
        inFile.read(reinterpret_cast<char*>(Buffer.data()), fileSize);
        inFile.close();

        // Reset read position for future reads from buffer
        ReadPosition = 0;
    }

private:
    
    EArchiverFlags Flags;
    std::vector<uint8> Buffer;
    size_t ReadPosition = 0;

    // Helper functions to write/read raw bytes to/from the buffer
    void WriteToBuffer(const uint8* data, size_t size)
    {
        Buffer.insert(Buffer.end(), data, data + size);
    }

    void ReadFromBuffer(uint8* data, size_t size)
    {
        if (ReadPosition + size > Buffer.size())
        {
            throw std::runtime_error("Read out of buffer bounds.");
        }
        std::copy(Buffer.begin() + ReadPosition, Buffer.begin() + ReadPosition + size, data);
        ReadPosition += size;
    }
};

// Example usage for a custom struct
#if 0
struct MyStruct
{
    int x;
    float y;
    std::string name;

    friend FArchive& operator<<(FArchive& archive, MyStruct& s)
    {
        archive << s.x << s.y << s.name;
        return archive;
    }
};
#endif