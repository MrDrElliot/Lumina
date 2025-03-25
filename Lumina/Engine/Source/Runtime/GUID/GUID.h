#pragma once

#include <objbase.h> // For CoCreateGuid and related functions
#include "Containers/Array.h"
#include "Core/Serialization/Archiver.h"
#include "Core/Templates/CanBulkSerialize.h"

namespace Lumina
{
    class FGuid
    {
    public:
        
        explicit FGuid(const TArray<unsigned char, 16> &bytes);
        explicit FGuid(TArray<unsigned char, 16> &&bytes);

        explicit FGuid(eastl::string_view fromString);
        FGuid();
	
        FGuid(const FGuid &other) = default;
        FGuid &operator=(const FGuid &other) = default;
        FGuid(FGuid &&other) = default;
        FGuid &operator=(FGuid &&other) = default;

        bool operator==(const FGuid &other) const;
        bool operator!=(const FGuid &other) const;

        FString String() const;
        operator FString() const;
        const TArray<unsigned char, 16>& Data() const;
        void Swap(FGuid &other);
        bool IsValid() const;

        FGuid static Generate();

        friend FArchive& operator << (FArchive& Ar, FGuid& Guid)
        {
            Ar.Serialize(Guid.Bytes.data(), Guid.Bytes.size());
            return Ar;
        }

    private:
        
        void Invalidate();

        TArray<unsigned char, 16> Bytes;

        friend std::ostream &operator<<(std::ostream &s, const FGuid &guid);
        friend bool operator<(const FGuid &lhs, const FGuid &rhs);
    };

    
    template<> struct TCanBulkSerialize<FGuid> { enum { Value = true }; };
    //template <> struct TIsPODType<FGuid> { enum { Value = true }; };
}


namespace eastl
{
    template <>
    struct hash<Lumina::FGuid>
    {
        std::size_t operator()(const Lumina::FGuid& Guid) const noexcept
        {
            const auto* data = reinterpret_cast<const uint8*>(Guid.Data().data());
            std::size_t hashValue = 0;

            for (size_t i = 0; i < 16; ++i)
            {
                hashValue = (hashValue << 8) | data[i];
            }

            return hashValue;
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
            return fmt::format_to(ctx.out(), "{}", guid.String());
        }
    };
}
