#include "StructuredArchive.h"


namespace Lumina
{
    void FArchiveSlot::Serialize(uint8& Value)
    {
        StructuredArchive->EnterSlot(*this);
        *StructuredArchive->InnerAr << Value;
        StructuredArchive->LeaveSlot();
    }

    void FArchiveSlot::Serialize(uint16& Value)
    {
        StructuredArchive->EnterSlot(*this);
        *StructuredArchive->InnerAr << Value;
        StructuredArchive->LeaveSlot();
    }

    void FArchiveSlot::Serialize(uint32& Value)
    {
        StructuredArchive->EnterSlot(*this);
        *StructuredArchive->InnerAr << Value;
        StructuredArchive->LeaveSlot();
    }

    void FArchiveSlot::Serialize(uint64& Value)
    {
        StructuredArchive->EnterSlot(*this);
        *StructuredArchive->InnerAr << Value;
        StructuredArchive->LeaveSlot();
    }

    void FArchiveSlot::Serialize(int8& Value)
    {
        StructuredArchive->EnterSlot(*this);
        *StructuredArchive->InnerAr << Value;
        StructuredArchive->LeaveSlot();
    }

    void FArchiveSlot::Serialize(int16& Value)
    {
        StructuredArchive->EnterSlot(*this);
        *StructuredArchive->InnerAr << Value;
        StructuredArchive->LeaveSlot();
    }

    void FArchiveSlot::Serialize(int32& Value)
    {
        StructuredArchive->EnterSlot(*this);
        *StructuredArchive->InnerAr << Value;
        StructuredArchive->LeaveSlot();
    }

    void FArchiveSlot::Serialize(int64& Value)
    {
        StructuredArchive->EnterSlot(*this);
        *StructuredArchive->InnerAr << Value;
        StructuredArchive->LeaveSlot();
    }

    void FArchiveSlot::Serialize(float& Value)
    {
        StructuredArchive->EnterSlot(*this);
        *StructuredArchive->InnerAr << Value;
        StructuredArchive->LeaveSlot();
    }

    void FArchiveSlot::Serialize(double& Value)
    {
        StructuredArchive->EnterSlot(*this);
        *StructuredArchive->InnerAr << Value;
        StructuredArchive->LeaveSlot();
    }

    void FArchiveSlot::Serialize(bool& Value)
    {
        StructuredArchive->EnterSlot(*this);
        *StructuredArchive->InnerAr << Value;
        StructuredArchive->LeaveSlot();
    }

    void FArchiveSlot::Serialize(FString& Value)
    {
        StructuredArchive->EnterSlot(*this);
        *StructuredArchive->InnerAr << Value;
        StructuredArchive->LeaveSlot();
    }

    void FArchiveSlot::Serialize(FName& Value)
    {
        StructuredArchive->EnterSlot(*this);
        *StructuredArchive->InnerAr << Value;
        StructuredArchive->LeaveSlot();
    }

    void FArchiveSlot::Serialize(CObject*& Value)
    {
        StructuredArchive->EnterSlot(*this);
        *StructuredArchive->InnerAr << Value;
        StructuredArchive->LeaveSlot();
    }

    void FArchiveSlot::Serialize(void* Data, uint64 DataSize)
    {
        StructuredArchive->EnterSlot(*this);
        StructuredArchive->InnerAr->Serialize(Data, (int64)DataSize);
        StructuredArchive->LeaveSlot();
    }

    FArchiveSlot IStructuredArchive::Open()
    {
        Assert(SlotStack.empty())
        
        RootElement.Position++;
        SlotStack.emplace_back(RootElement);

        FArchiveSlot Slot(this, RootElement);

        return Slot;
    }

    //------------------------------------------------------------------------------------------------------

    void FBinaryStructuredArchive::EnterRecord() const
    {
        
    }

    void FBinaryStructuredArchive::LeaveRecord() const
    {
        
    }

    void FBinaryStructuredArchive::EnterSlot(FSlot Slot)
    {
        FArchiveElement ParentElement = Slot.GetElement();
        ParentElement.Position++;
        
        SlotStack.push_back(ParentElement);
        
    }

    void FBinaryStructuredArchive::LeaveSlot()
    {
        SlotStack.pop_back();
    }

    void FBinaryStructuredArchive::EnterField(FName& FieldName)
    {
        *InnerAr << FieldName;  
    }

    void FBinaryStructuredArchive::LeaveField()
    {
    }
}
