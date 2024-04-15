#include <iostream>
#include <string>
#include <random>
#include <sstream>
#include <iomanip>

#if defined(_WIN32)
#include <objbase.h>
#elif defined(__linux__) || defined(__unix__)
#include <uuid/uuid.h>
#endif

class FGuid
{
public:
    static FGuid Generate();
    static FGuid Invalidate();

    bool IsValid() const;
    std::string ToString() const;


    bool operator==(const FGuid& other) const
    {
        return std::equal(std::begin(Bytes), std::end(Bytes), std::begin(other.Bytes));
    }

    bool operator!=(const FGuid& other) const
    {
        return !(*this == other);
    }

private:
    FGuid() = default;

    union
    {
        struct
        {
            uint32_t Data1;
            uint16_t Data2;
            uint16_t Data3;
            uint8_t  Data4[8];
        };
        uint8_t Bytes[16] = {0};
    };
};

inline FGuid FGuid::Generate()
{
    FGuid guid;
#if defined(_WIN32)
    ::CoCreateGuid(reinterpret_cast<GUID*>(&guid));
#elif defined(__linux__) || defined(__unix__)
    uuid_generate(reinterpret_cast<unsigned char*>(&guid));
#else
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    for (int i = 0; i < 16; ++i) {
        guid.Bytes[i] = static_cast<uint8_t>(dis(gen));
    }
#endif
    return guid;
}

inline FGuid FGuid::Invalidate()
{
    return FGuid();
}

inline bool FGuid::IsValid() const
{
    for (int i = 0; i < 16; ++i) {
        if (Bytes[i] != 0) return true;
    }
    return false;
}

inline std::string FGuid::ToString() const
{
    std::stringstream ss;
    ss << std::hex << std::setfill('0')
       << std::setw(8) << Data1 << '-'
       << std::setw(4) << Data2 << '-'
       << std::setw(4) << Data3 << '-'
       << std::setw(2) << static_cast<int>(Data4[0]) << std::setw(2) << static_cast<int>(Data4[1]) << '-'
       << std::setw(2) << static_cast<int>(Data4[2]) << std::setw(2) << static_cast<int>(Data4[3])
       << std::setw(2) << static_cast<int>(Data4[4]) << std::setw(2) << static_cast<int>(Data4[5])
       << std::setw(2) << static_cast<int>(Data4[6]) << std::setw(2) << static_cast<int>(Data4[7]);
    return ss.str();
}