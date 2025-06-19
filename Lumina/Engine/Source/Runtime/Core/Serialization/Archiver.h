#pragma once

#include "Containers/Name.h"
#include "Core/Templates/IsSigned.h"
#include "Core/Versioning/CoreVersion.h"
#include "Log/Log.h"
#include "Platform/WindowsPlatform.h"
#include "Containers/String.h"

namespace Lumina
{
    class FField;
}

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

namespace Lumina
{
    class CObject;

    class FArchive
    {
    public:
    
        FArchive() = default;
        FArchive(const FArchive&) = default;
        FArchive& operator=(const FArchive& ArchiveToCopy) = default;
        virtual ~FArchive() = default;

    
        virtual void Seek(int64 InPos) { }
        virtual int64 Tell() { return 0; }
        virtual int64 TotalSize() { return 0; }

        virtual void Serialize(void* V, int64 Length) {}

        // Set, remove, and check flags
        FORCEINLINE void SetFlag(EArchiverFlags flag) { Flags = Flags | flag; }
        FORCEINLINE void RemoveFlag(EArchiverFlags flag) { Flags = Flags & ~flag; }
        FORCEINLINE bool HasFlag(EArchiverFlags flag) const { return (Flags & flag) != EArchiverFlags::None; }

        /** Is this archiver writing to a buffer */
        FORCEINLINE bool IsWriting() const { return HasFlag(EArchiverFlags::Writing); }

        /** Is this archiver reading from a buffer, not to be mistaken with reading from a class.
         * As this is used to write to a class, whilst reading from a buffer.
         * */
        FORCEINLINE bool IsReading() const { return HasFlag(EArchiverFlags::Reading); }

        FORCEINLINE void SetHasError(bool bIsError) { bHasError = bIsError; }
        FORCEINLINE bool HasError() const { return bHasError; }

        FORCEINLINE static FPackageFileVersion GetEngineVersion()
        {
            return GPackageFileLuminaVersion;
        }
    
        /** Returns the maximum size of data that this archive is allowed to serialize. */
        FORCEINLINE int64 GetMaxSerializeSize() const { return ArMaxSerializeSize; }

        
        virtual FArchive& operator<<(CObject*& Value)
        {
            LOG_ERROR("Serializing objects is not supported by this archive.");
            return *this;
        }

        virtual FArchive& operator<<(FField*& Value)
        {
            LOG_ERROR("Serializing fields is not supported by this archive.");
            return *this;
        }
    
        FORCEINLINE FArchive& operator<<(uint8& Value)
        {
            Serialize(&Value, 1);
            return *this;
        }

        FORCEINLINE FArchive& operator<<(int8& Value)
        {
            Serialize(&Value, 1);
            return *this;
        }
    
        FORCEINLINE FArchive& operator<<(uint16& Value)
        {
            ByteOrderSerialize(Value);
            return *this;
        }
    
        FORCEINLINE FArchive& operator<<(int16& Value)
        {
            ByteOrderSerialize(reinterpret_cast<uint16&>(Value));
            return *this;
        }
    
        FORCEINLINE FArchive& operator<<(uint32& Value)
        {
            ByteOrderSerialize(Value);
            return *this;
        }
    
        FORCEINLINE FArchive& operator<<(int32& Value)
        {
            ByteOrderSerialize(reinterpret_cast<uint32&>(Value));
            return *this;
        }

        FORCEINLINE FArchive& operator<<( bool& D)
        {
            SerializeBool(D);
            return *this;
        }

        FORCEINLINE FArchive& operator<<(float& Value)
        {
            static_assert(sizeof(float) == sizeof(uint32_t), "Unexpected float size");
            uint32 Temp;
            std::memcpy(&Temp, &Value, sizeof(Temp));
            ByteOrderSerialize(Temp);
            std::memcpy(&Value, &Temp, sizeof(Value));
            return *this;
        }

        FORCEINLINE FArchive& operator<<(double& Value)
        {
            static_assert(sizeof(double) == sizeof(uint64), "Unexpected double size");
            uint64 Temp;
            std::memcpy(&Temp, &Value, sizeof(Temp));
            ByteOrderSerialize(Temp);
            std::memcpy(&Value, &Temp, sizeof(Value));
            return *this;
        }


        FORCEINLINE FArchive& operator<<(uint64& Value)
        {
            ByteOrderSerialize(Value);
            return *this;
        }

        FORCEINLINE FArchive& operator<<(int64& Value)
        {
            ByteOrderSerialize(reinterpret_cast<uint64&>(Value));
            return *this;
        }

        virtual void SerializeBool(bool& D);
    
        FORCEINLINE FArchive& operator<<(Lumina::FString& str)
        {
            if (IsReading())
            {
                int32 SaveNum = 0;
                *this << SaveNum;

                if (SaveNum < 0)
                {
                    SetHasError(true);
                    LOG_ERROR("Archive is corrupt!");
                    return *this;
                }
            
                int64 MaxSerializeSize = GetMaxSerializeSize();
                if ((MaxSerializeSize > 0) && (SaveNum > MaxSerializeSize))
                {
                    SetHasError(true);
                    LOG_ERROR("String is too large! (Size: {0}, Max: {1})", SaveNum, MaxSerializeSize);
                    return *this;
                }
            
                if (SaveNum)
                {
                    str.clear();
                    str.resize(SaveNum);
                    Serialize(str.data(), SaveNum);
                }
                else
                {
                    str.clear();
                }
            }
            else
            {
                int32 SaveNum = str.size();
                *this << SaveNum;

                if (SaveNum)
                {
                    Serialize(str.data(), SaveNum);
                }
            }
        
            return *this;
        }

        FORCEINLINE FArchive& operator<<(Lumina::FName& str)
        {
            FString String(str.c_str());
            *this << String;
            str = FName(String);

            return *this;
        }

        

        template<typename EnumType>
        FORCEINLINE FArchive& operator<<(EnumType& Value)
        requires (std::is_enum_v<EnumType>)
        {
            return *this << reinterpret_cast<std::underlying_type_t<EnumType>&>(Value);

        }

    private:
    
        template<typename T>
        FArchive& ByteOrderSerialize(T& Value)
        {
            static_assert(!TIsSigned<T>::Value, "To reduce the number of template instances, cast 'Value' to a uint16&, uint32& or uint64& prior to the call or use ByteOrderSerialize(void*, int32).");
        
            Serialize(&Value, sizeof(T));
            return *this;
        }



    private:

        EArchiverFlags Flags;
        uint8 bHasError:1;
        int64 ArMaxSerializeSize = INT32_MAX;

    };

    inline void FArchive::SerializeBool(bool& D)
    {
        uint32 OldUBoolValue;

        OldUBoolValue = D ? 1 : 0;
        Serialize(&OldUBoolValue, sizeof(OldUBoolValue));

        if (OldUBoolValue > 1)
        {
            LOG_ERROR("Invalid boolean encountered while reading archive - stream is most likely corrupted.");

            SetHasError(true);
        }
        D = !!OldUBoolValue;
    }
}

// Example usage for a custom struct
#if 0
struct MyStruct
{
    int x;
    float y;
    FString name;

    friend FArchive& operator<<(FArchive& archive, MyStruct& s)
    {
        archive << s.x << s.y << s.name;
        return archive;
    }
};
#endif