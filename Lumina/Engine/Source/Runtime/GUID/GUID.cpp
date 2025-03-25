#include "GUID.h"

namespace Lumina
{

    // converts a single hex char to a number (0 - 15)
    unsigned char hexDigitToChar(char ch)
    {
        // 0-9
        if (ch > 47 && ch < 58)
            return ch - 48;

        // a-f
        if (ch > 96 && ch < 103)
            return ch - 87;

        // A-F
        if (ch > 64 && ch < 71)
            return ch - 55;

        return 0;
    }

    bool isValidHexChar(char ch)
    {
        // 0-9
        if (ch > 47 && ch < 58)
            return true;

        // a-f
        if (ch > 96 && ch < 103)
            return true;

        // A-F
        if (ch > 64 && ch < 71)
            return true;

        return false;
    }

    // converts the two hexadecimal characters to an unsigned char (a byte)
    unsigned char hexPairToChar(char a, char b)
    {
        return hexDigitToChar(a) * 16 + hexDigitToChar(b);
    }
    
    
    FGuid::FGuid(const TArray<unsigned char, 16>& bytes)
        :Bytes(Bytes)
    {
    }

    FGuid::FGuid(TArray<unsigned char, 16>&& bytes)
        :Bytes(eastl::move(bytes))
    {
    }

    FGuid::FGuid(eastl::string_view fromString)
    {
        char charOne = '\0';
        char charTwo = '\0';
        bool lookingForFirstChar = true;
        unsigned nextByte = 0;

        for (const char &ch : fromString)
        {
            if (ch == '-')
                continue;

            if (nextByte >= 16 || !isValidHexChar(ch))
            {
                Invalidate();
                return;
            }

            if (lookingForFirstChar)
            {
                charOne = ch;
                lookingForFirstChar = false;
            }
            else
            {
                charTwo = ch;
                auto byte = hexPairToChar(charOne, charTwo);
                Bytes[nextByte++] = byte;
                lookingForFirstChar = true;
            }
        }

        // If there were fewer than 16 bytes in the string then guid is bad
        if (nextByte < 16)
        {
            Invalidate();
            return;
        }
    }

    FGuid::FGuid()
        :Bytes({0})
    {
    }

    bool FGuid::operator==(const FGuid& other) const
    {
        return Bytes == other.Bytes;
    }

    bool FGuid::operator!=(const FGuid& other) const
    {
        return !((*this) == other);
    }

    FString FGuid::String() const
    {
        char one[10], two[6], three[6], four[6], five[14];

        snprintf(one, 10, "%02X%02X%02X%02X", Bytes[0], Bytes[1], Bytes[2], Bytes[3]);
        snprintf(two, 6, "%02X%02X", Bytes[4], Bytes[5]);
        snprintf(three, 6, "%02X%02X", Bytes[6], Bytes[7]);
        snprintf(four, 6, "%02X%02X", Bytes[8], Bytes[9]);
        snprintf(five, 14, "%02X%02X%02X%02X%02X%02X", Bytes[10], Bytes[11], Bytes[12], Bytes[13], Bytes[14], Bytes[15]);

        const std::string sep("-");
        std::string out(one);

        out += sep + two;
        out += sep + three;
        out += sep + four;
        out += sep + five;

        return out.c_str();
    }

    FGuid::operator eastl::basic_string<char>() const
    {
        return String();
    }

    const TArray<unsigned char, 16>& FGuid::Data() const
    {
        return Bytes;
    }

    void FGuid::Swap(FGuid& other)
    {
        Bytes.swap(other.Bytes);
    }

    bool FGuid::IsValid() const
    {
        FGuid Empty;
        return *this != Empty;
    }
    
#if LE_PLATFORM_WINDOWS
    FGuid FGuid::Generate()
    {
        GUID WinGUID;
        CoCreateGuid(&WinGUID);

        TArray<unsigned char, 16> bytes =
        {
            (unsigned char)((WinGUID.Data1 >> 24) & 0xFF),
            (unsigned char)((WinGUID.Data1 >> 16) & 0xFF),
            (unsigned char)((WinGUID.Data1 >> 8) & 0xFF),
            (unsigned char)((WinGUID.Data1) & 0xff),

            (unsigned char)((WinGUID.Data2 >> 8) & 0xFF),
            (unsigned char)((WinGUID.Data2) & 0xff),

            (unsigned char)((WinGUID.Data3 >> 8) & 0xFF),
            (unsigned char)((WinGUID.Data3) & 0xFF),

            (unsigned char)WinGUID.Data4[0],
            (unsigned char)WinGUID.Data4[1],
            (unsigned char)WinGUID.Data4[2],
            (unsigned char)WinGUID.Data4[3],
            (unsigned char)WinGUID.Data4[4],
            (unsigned char)WinGUID.Data4[5],
            (unsigned char)WinGUID.Data4[6],
            (unsigned char)WinGUID.Data4[7]
        };

        return FGuid{eastl::move(bytes)};
    }
#endif

    void FGuid::Invalidate()
    {
        eastl::fill(Bytes.begin(), Bytes.end(), static_cast<unsigned char>(0));
    }

    std::ostream& operator<<(std::ostream& s, const FGuid& guid)
    {
        std::ios_base::fmtflags f(s.flags());
        s << std::hex << std::setfill('0')
            << std::setw(2) << (int)guid.Bytes[0]
            << std::setw(2) << (int)guid.Bytes[1]
            << std::setw(2) << (int)guid.Bytes[2]
            << std::setw(2) << (int)guid.Bytes[3]
            << "-"
            << std::setw(2) << (int)guid.Bytes[4]
            << std::setw(2) << (int)guid.Bytes[5]
            << "-"
            << std::setw(2) << (int)guid.Bytes[6]
            << std::setw(2) << (int)guid.Bytes[7]
            << "-"
            << std::setw(2) << (int)guid.Bytes[8]
            << std::setw(2) << (int)guid.Bytes[9]
            << "-"
            << std::setw(2) << (int)guid.Bytes[10]
            << std::setw(2) << (int)guid.Bytes[11]
            << std::setw(2) << (int)guid.Bytes[12]
            << std::setw(2) << (int)guid.Bytes[13]
            << std::setw(2) << (int)guid.Bytes[14]
            << std::setw(2) << (int)guid.Bytes[15];
        s.flags(f);
        return s;
    }

    bool operator<(const FGuid& lhs, const FGuid& rhs)
    {
        return lhs.Data() < rhs.Data();
    }
}
