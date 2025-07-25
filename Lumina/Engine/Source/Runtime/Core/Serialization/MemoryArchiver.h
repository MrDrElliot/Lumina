#pragma once

#include "Archiver.h"
#include "Containers/Array.h"
#include "Core/Assertions/Assert.h"
#include "Memory/Memory.h"

namespace Lumina
{

    class FMemoryArchiver : public FArchive
    {
    public:

        void Seek(int64 InPos) final
        {
            Offset = InPos;
        }
        
        int64 Tell() final
        {
            return Offset;
        }
        
    protected:

        FMemoryArchiver()
            : FArchive()
            , Offset(0)
        {}
        
        int64 Offset;
    };
    
    class FMemoryReader : public FMemoryArchiver
    {
    public:

        explicit FMemoryReader(const TVector<uint8>& InBytes, bool bIsPersistent = false)
            : Bytes    (InBytes)
            , LimitSize(INT64_MAX)
        {
            this->SetFlag(EArchiverFlags::Reading);
        }

        int64 TotalSize() override
        {
            return std::min((int64)Bytes.size(), LimitSize);   
        }

        void SetLimitSize(int64 NewLimitSize)
        {
            LimitSize = NewLimitSize;
        }

        void Serialize(void* V, int64 Length) override
        {
            if ((Length) && !HasError())
            {
                if (Length <= 0)
                {
                    SetHasError(true);
                    return;
                }
                
                // Only serialize if we have the requested amount of data
                if (Offset + Length <= TotalSize())
                {
                    memcpy(V, &Bytes[(int32)Offset], Length);
                    Offset += Length;
                }
                else
                {
                    SetHasError(true);
                }
            } 
        }

    private:

        const TVector<uint8>&   Bytes;
        int64                   LimitSize;
    
    };

    /**
     * Similar to FMemoryReader but controls the data.
     */
    class FBufferReader : public FMemoryArchiver
    {
    public:
        FBufferReader(void* InData, int64 InSize, bool bFreeAfterClose, bool bIsPersistent = false)
            : Data(static_cast<uint8*>(InData))
            , Size(InSize)
            , bFreeOnClose(bFreeAfterClose)
        {
            this->SetFlag(EArchiverFlags::Reading);
        }

        ~FBufferReader() override
        {
            if (bFreeOnClose && Data)
            {
                Memory::Free(Data);
                Data = nullptr;
            }
        }

        int64 TotalSize() override
        {
            return std::min(Size, LimitSize);
        }

        void SetLimitSize(int64 NewLimitSize)
        {
            LimitSize = NewLimitSize;
        }

        void Serialize(void* V, int64 Length) override
        {
            if ((Length > 0) && !HasError())
            {
                if (Offset + Length <= TotalSize())
                {
                    memcpy(V, &Data[Offset], Length);
                    Offset += Length;
                }
                else
                {
                    SetHasError(true);
                    LOG_ERROR("FBufferReader: Tried to read past end of buffer.");
                }
            }
            else if (Length <= 0)
            {
                SetHasError(true);
                LOG_ERROR("FBufferReader: Invalid length requested for serialize: {}", Length);
            }
        }

    private:
        
        uint8* Data = nullptr;
        int64 Size = 0;
        int64 LimitSize = INT64_MAX;
        bool bFreeOnClose = false;
    };
    

    class FMemoryWriter : public FMemoryArchiver
    {
    public:

        FMemoryWriter(TVector<uint8>& InBytes, bool bSetOffset = false)
            :Bytes(InBytes)
        {
            this->SetFlag(EArchiverFlags::Writing);
        }

        int64 TotalSize() override { return Bytes.size(); }
        
        virtual void Serialize(void* Data, int64 Num) override
        {
            if (!Data || Num <= 0)
            {
                LOG_ERROR("Invalid data pointer or size passed to Serialize.");
                SetHasError(true);
                return;
            }

            const int64 NumBytesToAdd = Offset + Num - Bytes.size();
            if (NumBytesToAdd > 0)
            {
                const int64 NewArrayCount = Bytes.size() + NumBytesToAdd;
                if (NewArrayCount > std::numeric_limits<int32>::max())
                {
                    LOG_ERROR("Cannot serialize, would overflow buffer limit!");
                    SetHasError(true);
                    return;
                }

                Bytes.resize(NewArrayCount, 0);
            }

            Assert((Offset + Num) <= (int64)Bytes.size())

            if (Num > 0)
            {
                memcpy(&Bytes[Offset], Data, Num);
                Offset += Num;
            }
        }



    private:
        
        TVector<uint8>&	Bytes;
    };
}
