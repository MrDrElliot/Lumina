#pragma once
#include "Core/Assertions/Assert.h"
#include "Core/Serialization/Archiver.h"

namespace Lumina
{
    class FName;

    class IStructuredArchive
    {
    public:
        
        virtual ~IStructuredArchive() = default;

        IStructuredArchive(FArchive& InInnerAr)
            :InnerAr(&InInnerAr)
        {
            Assert(InnerAr)
        }


        virtual void EnterRecord() const = 0;
        virtual void LeaveRecord() const = 0;
        
        virtual void EnterSlot() const = 0;
        virtual void LeaveSlot() const = 0;


        

        virtual void Serialize(uint8& Value) = 0;
        virtual void Serialize(uint16& Value) = 0;
        virtual void Serialize(uint32& Value) = 0;
        virtual void Serialize(uint64& Value) = 0;
        virtual void Serialize(int8& Value) = 0;
        virtual void Serialize(int16& Value) = 0;
        virtual void Serialize(int32& Value) = 0;
        virtual void Serialize(int64& Value) = 0;
        virtual void Serialize(float& Value) = 0;
        virtual void Serialize(double& Value) = 0;
        virtual void Serialize(bool& Value) = 0;
        virtual void Serialize(FString& Value) = 0;
        virtual void Serialize(FName& Value) = 0;
        virtual void Serialize(CObject*& Value) = 0;
        virtual void Serialize(void* Data, uint64 DataSize) = 0;

    protected:
        
        FArchive* InnerAr = nullptr;
    
    };

    class FBinaryStructuredArchive final : public IStructuredArchive
    {
    public:

        FBinaryStructuredArchive(FArchive& InAr)
            :IStructuredArchive(InAr)
        {}
        
        void EnterRecord() const override;
        void LeaveRecord() const override;
        
        void EnterSlot() const override;
        void LeaveSlot() const override;
        
        void Serialize(uint8& Value) override;
        void Serialize(uint16& Value) override;
        void Serialize(uint32& Value) override;
        void Serialize(uint64& Value) override;
        void Serialize(int8& Value) override;
        void Serialize(int16& Value) override;
        void Serialize(int32& Value) override;
        void Serialize(int64& Value) override;
        void Serialize(float& Value) override;
        void Serialize(double& Value) override;
        void Serialize(bool& Value) override;
        void Serialize(FString& Value) override;
        void Serialize(FName& Value) override;
        void Serialize(CObject*& Value) override;
        void Serialize(void* Data, uint64 DataSize) override;
    };
}
