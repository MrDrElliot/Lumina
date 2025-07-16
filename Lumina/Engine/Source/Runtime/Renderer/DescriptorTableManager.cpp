#include "DescriptorTableManager.h"

#include "RenderContext.h"

namespace Lumina
{
    FDescriptorHandle::FDescriptorHandle()
    {
    }

    FDescriptorHandle::~FDescriptorHandle()
    {
        if (DescriptorIndex >= 0)
        {
            if (auto TableManager = Manager.lock())
            {
                TableManager->ReleaseDescriptor(DescriptorIndex);
            }
            DescriptorIndex = -1;
        }
    }

    int64 FDescriptorHandle::GetIndexInHeap() const
    {
        if (DescriptorIndex >= 0)
        {
            Assert(!Manager.expired())
            if (auto LockedManager = Manager.lock())
            {
                return LockedManager->GetDescriptorTable()->GetFirstDescriptorIndexInHeap() + DescriptorIndex;
            }
        }
        
        return -1;
    }

    FDescriptorTableManager::FDescriptorTableManager(IRenderContext* InContext, FRHIBindingLayout* BindingLayout)
        : Context(InContext)
    {
        DescriptorTable = InContext->CreateDescriptorTable(BindingLayout);
        SIZE_T Capacity = DescriptorTable->GetCapacity();
        AllocatedDescriptors.resize(Capacity);
        Descriptors.resize(Capacity);
        Memory::Memzero(Descriptors.data(), sizeof(FBindingSetItem) * Capacity);
    }

    FDescriptorTableManager::~FDescriptorTableManager()
    {
        for (auto& Descriptor : Descriptors)
        {
            if (Descriptor.ResourceHandle)
            {
                Descriptor.ResourceHandle->Release();
                Descriptor.ResourceHandle = nullptr;
            }
        }
    }

    int64 FDescriptorTableManager::CreateDescriptor(FBindingSetItem Item)
    {
        const auto& Found = DescriptorIndexMap.find(Item);
        if (Found != DescriptorIndexMap.end())
        {
            return Found->second;
        }

        SIZE_T Capacity = DescriptorTable->GetCapacity();
        bool bFoundFreeSlot = false;
        uint32 Index = 0;
        for (Index = SearchStart; Index < Capacity; Index++)
        {
            if (!AllocatedDescriptors[Index])
            {
                bFoundFreeSlot = true;
                break;
            }
        }

        if (!bFoundFreeSlot)
        {
            uint32 NewCapacity = Math::Max<uint32>(64u, Capacity * 2);
            Context->ResizeDescriptorTable(DescriptorTable, NewCapacity, true);
            AllocatedDescriptors.resize(NewCapacity);
            Descriptors.resize(NewCapacity);

            Memory::Memzero(&Descriptors[Capacity], sizeof(FBindingSetItem) * (NewCapacity - Capacity));

            Index = Capacity;
            Capacity = NewCapacity;
        }

        Item.Slot = Index;
        SearchStart = Index + 1;
        AllocatedDescriptors[Index] = true;
        Descriptors[Index] = Item;
        DescriptorIndexMap[Item] = Index;
        Context->WriteDescriptorTable(DescriptorTable, Item);

        if (Item.ResourceHandle)
        {
            Item.ResourceHandle->AddRef();
        }

        return Index;
    }

    FDescriptorHandle FDescriptorTableManager::CreateDescriptorHandle(const FBindingSetItem& Item)
    {
        int64 Index = CreateDescriptor(Item);
        return FDescriptorHandle(shared_from_this(), Index);
    }

    FBindingSetItem FDescriptorTableManager::GetDescriptor(int64 Index)
    {
        if ((SIZE_T)Index >= Descriptors.size())
        {
            return FBindingSetItem();
        }

        return Descriptors[Index];
    }

    void FDescriptorTableManager::ReleaseDescriptor(int64 DescriptorIndex)
    {
        FBindingSetItem& Descriptor = Descriptors[DescriptorIndex];
        if (Descriptor.ResourceHandle)
        {
            Descriptor.ResourceHandle->Release();
        }

        const auto IndexMapEntry = DescriptorIndexMap.find(Descriptors[DescriptorIndex]);
        if (IndexMapEntry != DescriptorIndexMap.end())
        {
            DescriptorIndexMap.erase(IndexMapEntry);
        }

        Descriptor = FBindingSetItem();
        Context->WriteDescriptorTable(DescriptorTable, Descriptor);
        AllocatedDescriptors[DescriptorIndex] = false;
        SearchStart = Math::Min<int64>(SearchStart, DescriptorIndex);
    }
}
