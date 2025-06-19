#pragma once
#include "Containers/Array.h"
#include "Core/Assertions/Assert.h"
#include "Core/Serialization/Archiver.h"

namespace Lumina
{
    class FName;

    class FArchiveElement
    {
    public:

        FArchiveElement() = default;

        FArchiveElement(uint32 InPos)
            :Position(InPos)
        {}

        uint32 Position = 0;
        
    };

    class FArchiveSlot
    {
        friend class IStructuredArchive;

    public:

        FArchiveSlot(IStructuredArchive* InAr, FArchiveElement InElem)
            : StructuredArchive(InAr)
            , Element(InElem)
        {}

        FArchiveElement GetElement() const { return Element; }

        IStructuredArchive* GetStructuredArchive() const { return StructuredArchive; }
        
        void Serialize(uint8& Value);
        void Serialize(uint16& Value);
        void Serialize(uint32& Value);
        void Serialize(uint64& Value);
        void Serialize(int8& Value);
        void Serialize(int16& Value);
        void Serialize(int32& Value);
        void Serialize(int64& Value);
        void Serialize(float& Value);
        void Serialize(double& Value);
        void Serialize(bool& Value);
        void Serialize(FString& Value);
        void Serialize(FName& Value);
        void Serialize(CObject*& Value);
        void Serialize(void* Data, uint64 DataSize);

    protected:

        IStructuredArchive*     StructuredArchive = nullptr;
        FArchiveElement         Element;
    };

    class IStructuredArchive
    {
        friend class FArchiveSlot;
        
    public:

        using FSlot = FArchiveSlot;
        
        virtual ~IStructuredArchive() = default;

        IStructuredArchive(FArchive& InInnerAr)
            :InnerAr(&InInnerAr)
        {
            Assert(InnerAr)
        }

        LUMINA_API FArchiveSlot Open();

        virtual void EnterRecord() const = 0;
        virtual void LeaveRecord() const = 0;
        
        virtual void EnterSlot(FSlot Slot) = 0;
        virtual void LeaveSlot() = 0;

        virtual void EnterField(FName& FieldName) = 0;
        virtual void LeaveField() = 0;

        FArchive* GetInnerAr() const { return InnerAr; }

    protected:

        FArchiveElement RootElement;
        TVector<FArchiveElement> SlotStack;
        FArchive* InnerAr = nullptr;
    
    };

    class FBinaryStructuredArchive final : public IStructuredArchive
    {
    public:

        LUMINA_API FBinaryStructuredArchive(FArchive& InAr)
            :IStructuredArchive(InAr)
        {}
        
        LUMINA_API void EnterRecord() const override;
        LUMINA_API void LeaveRecord() const override;
        
        LUMINA_API void EnterSlot(FSlot Slot) override;
        LUMINA_API void LeaveSlot() override;

        LUMINA_API void EnterField(FName& FieldName) override;
        LUMINA_API void LeaveField() override;

    private:
        
    };
}
