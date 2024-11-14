#pragma once

#include <cstring>
#include <ostream>
#include <string>
#include "Platform/GenericPlatform.h"
#include <spdlog/fmt/fmt.h>

#include "Core/Serialization/Archiver.h"


class LString
{
public:
    
    LString() = default;

    // Destructor
    ~LString()
    {
        CleanUp();
    }

    // Constructor from C-string
    LString(const char* InChar)
    {
        Size = strlen(InChar);
        Buffer = new char[Size + 1];
        strncpy_s(Buffer, Size + 1, InChar, Size);
    }

    // Copy constructor
    LString(const LString& Other)
    {
        Size = Other.Size;
        Buffer = new char[Size + 1];
        strncpy_s(Buffer, Size + 1, Other.Buffer, Size);
    }

    // Constructor from std::string
    LString(const std::string& Str)
    {
        Size = Str.size();
        Buffer = new char[Size + 1];
        strncpy_s(Buffer, Size + 1, Str.c_str(), Size);
    }

    // Equality operator
    bool operator==(const LString& Other) const
    {
        if (Size != Other.Size) {
            return false;
        }
        return std::strcmp(Buffer, Other.Buffer) == 0;
    }

    bool operator==(const char* Other) const
    {
        if (!Buffer || !Other)
        {
            return Buffer == Other; // Both are nullptr or both are not
        }
        return std::strcmp(Buffer, Other) == 0;
    }

    // Inequality operator
    bool operator!=(const LString& Other) const
    {
        return !(*this == Other);
    }

    bool operator!=(const char* Other) const
    {
        return !(*this == Other);
    }

    // Assignment operator for const char*
    LString& operator=(const char* Other)
    {
        if (Buffer != Other)
        {
            CleanUp();
            if (Other != nullptr)
            {
                Size = strlen(Other);
                Buffer = new char[Size + 1];
                strncpy_s(Buffer, Size + 1, Other, Size);
            }
        }
        return *this;
    }
    
    // Copy assignment
    LString& operator=(const LString& Other)
    {
        if (this != &Other)
        {
            CleanUp();
            Size = Other.Size;
            Buffer = new char[Size + 1];
            strncpy_s(Buffer, Size + 1, Other.Buffer, Size);
        }
        return *this;
    }

    // Assignment from std::string
    LString& operator=(const std::string& Str)
    {
        CleanUp();
        Size = Str.size();
        Buffer = new char[Size + 1];
        strncpy_s(Buffer, Size + 1, Str.c_str(), Size);
        return *this;
    }

    // Move constructor
    LString(LString&& Other) noexcept
    {
        Size = Other.Size;
        Buffer = Other.Buffer;
        Other.Buffer = nullptr;
        Other.Size = 0;
    }

    // Move assignment
    LString& operator=(LString&& Other) noexcept
    {
        if (this != &Other)
        {
            CleanUp();
            Size = Other.Size;
            Buffer = Other.Buffer;
            Other.Buffer = nullptr;
            Other.Size = 0;
        }
        return *this;
    }

    // + operator for concatenation
    LString operator+(const LString& Other) const
    {
        LString String;
        String.Size = this->Size + Other.Size;
        String.Buffer = new char[String.Size + 1];
        strncpy_s(String.Buffer, this->Size + 1, this->Buffer, this->Size);
        strncpy_s(String.Buffer + this->Size, Other.Size + 1, Other.Buffer, Other.Size);
        return String;
    }

    // Friend operator<< for ostream output
    friend std::ostream& operator<<(std::ostream& cout, const LString& obj)
    {
        cout << obj.CStr();
        return cout;
    }
    
    // Length accessor
    uint64 Length() const { return Size; }

    // C-string accessor
    const char* CStr() const { return Buffer; }
    char* CStr() { return Buffer; }


private:
    // Memory cleanup helper
    void CleanUp()
    {
        if (Buffer != nullptr)
        {
            delete[] Buffer;
            Buffer = nullptr;
        }
        Size = 0;
    }

    char* Buffer = nullptr;
    uint64 Size = 0;
};

// Formatter specialization for LString
template <>
struct fmt::formatter<LString>
{
    // Parses the format specifications (we'll ignore them here)
    constexpr auto parse(fmt::format_parse_context& ctx) -> decltype(ctx.begin())
    {
        return ctx.begin();
    }

    // Formats an LString using fmt's API
    template <typename FormatContext>
    auto format(const LString& str, FormatContext& ctx) -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "{}", str.CStr());
    }
};

namespace std
{
    template <>
    struct hash<LString>
    {
        std::size_t operator()(const LString& str) const noexcept
        {
            if (str.CStr() == nullptr) {
                return 0; // Handle empty string or nullptr
            }
            // Use a hash function for C-strings
            return std::hash<std::string_view>{}(std::string_view(str.CStr(), str.Length()));
        }
    };
}