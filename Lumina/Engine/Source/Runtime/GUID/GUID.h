#pragma once

#include <random>
#include <string>
#include <sstream>
#include <iomanip>
#include <array>
#include <functional>
#include "Core/Serialization/Archiver.h"


class FArchive;

namespace Lumina
{
    // 128-byte globally-unique-identifier (GUID).
    class FGuid
    {
    public:
        
        // Default constructor: Initializes GUID to zero
        FGuid() : Guid{} {}

        // Constructor for initializing GUID with a specific value (array of bytes)
        explicit FGuid(const std::array<uint8, 16>& New) : Guid(New) {}
        
        FGuid(const FGuid& Other)
        {
            std::memcpy(Guid.data(), Other.Guid.data(), sizeof(Guid));  // Copy data properly
        }

        FGuid(FGuid&& other) noexcept
        {
            Guid = std::move(other.Guid);
        }

        
        // Constructor for initializing GUID from a string (e.g., "60DBF646-773C-4B6D-AAD9-E9D51BE7E957")
        explicit FGuid(const std::string& GuidString)
        {
            // Validate and parse the string into Guid
            if (GuidString.length() != 36)
            {
                throw std::invalid_argument("Invalid GUID string length.");
            }

            std::string cleanedGuid = GuidString;
            cleanedGuid.erase(std::remove(cleanedGuid.begin(), cleanedGuid.end(), '-'), cleanedGuid.end()); // Remove hyphens

            if (cleanedGuid.length() != 32)
            {
                throw std::invalid_argument("Invalid GUID string format.");
            }

            // Parse the 32-character string into the Guid array (16 bytes)
            std::istringstream stream(cleanedGuid);
            for (size_t i = 0; i < 16; ++i)
            {
                uint8_t byte;
                stream >> std::setw(2) >> std::hex >> byte;
                Guid[i] = byte;
            }
        }

        // Generate a random 128-bit GUID
        static FGuid Generate()
        {
            std::random_device rd;
            std::uniform_int_distribution<uint32> dist;
            std::array<uint8, 16> randomBytes;

            // Generate random 128 bits (16 bytes)
            for (int i = 0; i < 16; ++i)
            {
                randomBytes[i] = static_cast<uint8>(dist(rd) & 0xFF);
            }

            return FGuid(randomBytes);
        }

        // Getter for the internal GUID value (as a byte array)
        const std::array<uint8, 16>& Get() const { return Guid; }

        // Check if GUID is valid (non-zero)
        bool IsValid() const
        {
            return std::any_of(Guid.begin(), Guid.end(), [](uint8_t byte) { return byte != 0; });
        }

        // Get the GUID as a string in the format: "60DBF646-773C-4B6D-AAD9-E9D51BE7E957"
        std::string ToString() const
        {
            std::ostringstream stream;
            stream << std::hex << std::uppercase << std::setfill('0');

            // Print the first 8 hex digits
            stream << std::setw(8) << (Guid[0] << 24 | Guid[1] << 16 | Guid[2] << 8 | Guid[3]);
            stream << '-';
            // Next 4 hex digits
            stream << std::setw(4) << (Guid[4] << 8 | Guid[5]);
            stream << '-';
            // Next 4 hex digits
            stream << std::setw(4) << (Guid[6] << 8 | Guid[7]);
            stream << '-';
            // Next 4 hex digits
            stream << std::setw(4) << (Guid[8] << 8 | Guid[9]);
            stream << '-';
            // Last 12 hex digits
            for (size_t i = 10; i < 16; ++i)
            {
                stream << std::setw(2) << static_cast<int>(Guid[i]);
            }

            return stream.str();
        }

        // Convert a string representation back to a GUID (if valid)
        static FGuid FromString(const std::string& str)
        {
            std::array<uint8, 16> guidBytes = {};
            for (size_t i = 0, j = 0; i < str.size() && j < 16; i += 2, ++j)
            {
                uint8 byte = static_cast<uint8_t>(std::stoi(str.substr(i, 2), nullptr, 16));
                guidBytes[j] = byte;
            }
            return FGuid(guidBytes);
        }

        FGuid& operator=(FGuid&& other) noexcept
        {
            if (this != &other)
            {
                Guid = std::move(other.Guid);  // Move the array
            }
            return *this;
        }
        
        FGuid& operator=(const FGuid& other)
        {
            if (this != &other)
            {
                Guid = other.Guid;  // Copy the array
            }
            return *this;
        }

        
        // Comparison operators for ease of use
        bool operator==(const FGuid& Other) const
        {
            return Guid == Other.Guid;
        }

        bool operator!=(const FGuid& Other) const
        {
            return Guid != Other.Guid;
        }

        bool operator<(const FGuid& Other) const
        {
            return Guid < Other.Guid;
        }

        bool operator>(const FGuid& Other) const
        {
            return Guid > Other.Guid;
        }

        // Allows casting FGuid to uint64 (for convenience, but truncated)
        operator uint64() const
        {
            // Only return the first 64 bits (truncated)
            uint64 result = 0;
            for (size_t i = 0; i < 8; ++i)
            {
                result |= static_cast<uint64>(Guid[i]) << (8 * (7 - i));
            }
            return result;
        }

        // Serialization operator
        friend FArchive& operator<<(FArchive& Ar, FGuid& Data)
        {
            for (auto& byte : Data.Guid)
            {
                Ar << byte;
            }
            return Ar;
        }

    private:
        // 128-bit GUID stored as 16 bytes (128 bits)
        std::array<uint8, 16> Guid;
    };
    
}


namespace std
{
    template <>
   struct std::hash<Lumina::FGuid>
    {
        std::size_t operator()(const Lumina::FGuid& Guid) const noexcept
        {
            // Combine the first 64 bits (or other suitable portion) to create a hash
            uint64 hashValue = *reinterpret_cast<const uint64*>(&Guid.Get()[0]);
            return std::hash<uint64>{}(hashValue);
        }
    };
}
