#pragma once
#include "RenderResource.h"
#include "Memory/SmartPtr.h"

namespace Lumina
{

    class FDescriptorHandle
    {
        friend class FDescriptorTableManager;
    public:

        FDescriptorHandle(const FDescriptorHandle&) = delete;
        FDescriptorHandle(FDescriptorHandle&&) = default;
        FDescriptorHandle& operator=(const FDescriptorHandle&) = delete;
        FDescriptorHandle& operator=(FDescriptorHandle&&) = default;

        FDescriptorHandle();
        FDescriptorHandle(const TSharedPtr<FDescriptorTableManager>& InManager, int64 InIndex)
            : Manager(InManager)
            , DescriptorIndex(InIndex)
        {}
        
        ~FDescriptorHandle();

        NODISCARD bool IsValid() const { return DescriptorIndex >= 0 && !Manager.expired(); }
        NODISCARD int64 Get() const { if (DescriptorIndex >= 0) Assert(!Manager.expired()) return DescriptorIndex; }

        NODISCARD int64 GetIndexInHeap() const;
        void Reset() { DescriptorIndex = -1; Manager.reset(); }
        
        TWeakPtr<FDescriptorTableManager> Manager;
        int64 DescriptorIndex = -1;
    };
    
    class FDescriptorTableManager : public TSharedFromThis<FDescriptorTableManager>
    {
    public:
        
        FDescriptorTableManager(IRenderContext* InContext, FRHIBindingLayout* BindingLayout);
        ~FDescriptorTableManager();
        
        // Custom hasher that doesn't look at the binding slot
        struct FBindingSetItemHasher
        {
            std::size_t operator()(const FBindingSetItem& Item) const
            {
                size_t hash = 0;
                Hash::HashCombine(hash, Item.ResourceHandle);
                Hash::HashCombine(hash, Item.Type);
                Hash::HashCombine(hash, Item.RawData[0]);
                Hash::HashCombine(hash, Item.RawData[1]);
                return hash;
            }
        };

        // Custom equality tester that doesn't look at the binding slot
        struct FBindingSetItemsEqual
        {
            bool operator()(const FBindingSetItem& a, const FBindingSetItem& b) const 
            {
                return a.ResourceHandle == b.ResourceHandle && a.Type == b.Type;
            }
        };

        FRHIDescriptorTable* GetDescriptorTable() const { return DescriptorTable; }
        int64 CreateDescriptor(FBindingSetItem Item);
        FDescriptorHandle CreateDescriptorHandle(const FBindingSetItem& Item);
        FBindingSetItem GetDescriptor(int64 Index);
        void ReleaseDescriptor(int64 DescriptorIndex);


    private:
        IRenderContext* Context = nullptr;
        FRHIDescriptorTableRef DescriptorTable;
        TVector<FBindingSetItem> Descriptors;
        THashMap<FBindingSetItem, int64, FBindingSetItemHasher, FBindingSetItemsEqual> DescriptorIndexMap;
        TVector<bool> AllocatedDescriptors;
        int64 SearchStart = 0;
    
    };
}
