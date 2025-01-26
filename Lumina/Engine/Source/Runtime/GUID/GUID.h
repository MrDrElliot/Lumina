#pragma once

#include <string>
#include <array>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <objbase.h> // For CoCreateGuid and related functions
#include "Core/Serialization/Archiver.h"
#include "Core/Templates/CanBulkSerialize.h"

namespace Lumina
{
    // 128-bit globally unique identifier (GUID).
    class FGuid
    {
    public:
        // Default constructor: Initializes GUID to zero
        FGuid() : Guid{} {}

        // Constructor for initializing GUID with a specific value (array of bytes)
        explicit FGuid(const std::array<uint8_t, 16>& NewGuid) : Guid(NewGuid) {}

        // Constructor for initializing GUID from a Windows GUID
        explicit FGuid(const GUID& WinGuid)
        {
            // Copy the Windows GUID into our array representation
            memcpy(Guid.data(), &WinGuid, sizeof(GUID));
        }

        // Constructor for initializing GUID from a string (e.g., "60DBF646-773C-4B6D-AAD9-E9D51BE7E957")
        explicit FGuid(const std::string& GuidString)
        {
            FromString(GuidString);
        }

        // Generate a random 128-bit GUID
        static FGuid Generate()
        {
            GUID WinGuid;
            if (CoCreateGuid(&WinGuid) != S_OK)
            {
                throw std::runtime_error("Failed to generate a GUID.");
            }
            return FGuid(WinGuid);
        }

        // Getter for the internal GUID value (as a byte array)
        const std::array<uint8_t, 16>& Get() const { return Guid; }

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

            stream << std::setw(8) << *reinterpret_cast<const uint32_t*>(&Guid[0]) << '-';
            stream << std::setw(4) << *reinterpret_cast<const uint16_t*>(&Guid[4]) << '-';
            stream << std::setw(4) << *reinterpret_cast<const uint16_t*>(&Guid[6]) << '-';
            stream << std::setw(4) << *reinterpret_cast<const uint16_t*>(&Guid[8]) << '-';

            for (size_t i = 10; i < 16; ++i)
            {
                stream << std::setw(2) << static_cast<int>(Guid[i]);
            }

            return stream.str();
        }

        

        // Convert a string representation back to a GUID (if valid)
        void FromString(const std::string& GuidString)
        {
            if (GuidString.length() != 36)
            {
                throw std::invalid_argument("Invalid GUID string length.");
            }

            std::string cleanedGuid = GuidString;
            cleanedGuid.erase(std::ranges::remove(cleanedGuid, '-').begin(), cleanedGuid.end());

            if (cleanedGuid.length() != 32)
            {
                throw std::invalid_argument("Invalid GUID string format.");
            }

            std::istringstream stream(cleanedGuid);
            for (size_t i = 0; i < 16; ++i)
            {
                uint16_t byte;
                stream >> std::setw(2) >> std::hex >> byte;
                Guid[i] = static_cast<uint8_t>(byte);
            }
        }

        // Comparison operators
        bool operator==(const FGuid& Other) const { return Guid == Other.Guid; }
        bool operator!=(const FGuid& Other) const { return !(*this == Other); }

        // Serialization operator
        friend FArchive& operator<<(FArchive& Ar, FGuid& Data)
        {
            Ar.Serialize(Data.Guid.data(), Data.Guid.size());
            return Ar;
        }

    private:
        std::array<uint8_t, 16> Guid; // 128-bit GUID stored as 16 bytes
    };

    
    template<> struct TCanBulkSerialize<FGuid> { enum { Value = true }; };
    //template <> struct TIsPODType<FGuid> { enum { Value = true }; };
}


namespace std
{
    template <>
    struct hash<Lumina::FGuid>
    {
        std::size_t operator()(const Lumina::FGuid& Guid) const noexcept
        {
            return std::hash<std::string>{}(Guid.ToString());
        }
    };
}

namespace fmt
{
    template <>
    struct formatter<Lumina::FGuid>
    {
        // Parses the format specifier (not used here, so we ignore it)
        constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
        {
            return ctx.begin();
        }

        // Formats the FGuid instance
        template <typename FormatContext>
        auto format(const Lumina::FGuid& guid, FormatContext& ctx) -> decltype(ctx.out())
        {
            // Use FGuid's ToString method to get a string representation
            return fmt::format_to(ctx.out(), "{}", guid.ToString());
        }
    };
}
