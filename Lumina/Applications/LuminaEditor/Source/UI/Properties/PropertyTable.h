#pragma once
#include "Core/Functional/Function.h"
#include "Core/Reflection/PropertyCustomization/PropertyCustomization.h"
#include "Memory/SmartPtr.h"

namespace Lumina
{
    enum class EPropertyChangeOp : uint8;
    class FPropertyTable;
    class FStructProperty;
    class CStruct;
    class FPropertyHandle;
    class FArrayProperty;
    class FProperty;
    struct IPropertyTypeCustomization;
    class CObject;
}

namespace Lumina
{
    class FPropertyRow
    {
    public:

        FPropertyRow() = default;
        FPropertyRow(FProperty* InProperty, FPropertyRow* InParentRow);
        virtual ~FPropertyRow();

        virtual void DrawHeader(float Offset) { }
        virtual void DrawEditor() { }

        virtual bool HasExtraControls() const { return false; }
        virtual void DrawExtraControlsSection() { }
        virtual float GetExtraControlsSectionWidth() { return 0; }
        
        void AddChild(FPropertyRow* InChild);
        void DestroyChildren();

        virtual void Update() { }
        void UpdateRow();
        void DrawRow(float Offset);

        void SetIsArrayElement(bool bTrue) { bArrayElement = bTrue; }
        bool IsArrayElementProperty() const { return bArrayElement; }
        
    protected:

        EPropertyChangeOp                       ChangeOp = EPropertyChangeOp::None;
        TSharedPtr<IPropertyTypeCustomization>  Customization;
        FProperty*                              Property = nullptr;
        FPropertyTable*                         Table = nullptr;
        FPropertyRow*                           ParentRow = nullptr;
        TVector<FPropertyRow*>                  Children;

        bool                                    bArrayElement = false;
        bool                                    bExpanded = true;
    };

    class FPropertyPropertyRow : public FPropertyRow
    {
    public:

        FPropertyPropertyRow(void* InPropertyPointer, FProperty* InProperty, FPropertyRow* InParentRow, int64 InArrayElementIndex);
        void Update() override;
        void DrawHeader(float Offset) override;
        void DrawEditor() override;
        bool HasExtraControls() const override;
        void DrawExtraControlsSection() override;

        TSharedPtr<FPropertyHandle> GetPropertyHandle() const { return PropertyHandle; }
        
    private:

        int64                       ArrayElementIndex;
        TSharedPtr<FPropertyHandle> PropertyHandle;
    };

    class FArrayPropertyRow : public FPropertyRow
    {
    public:
        
        FArrayPropertyRow(void* InPropPointer, FArrayProperty* InProperty, FPropertyRow* InParentRow);
        void Update() override;
        void DrawHeader(float Offset) override;
        void DrawEditor() override;
        void RebuildChildren();
        bool HasExtraControls() const override { return true; }
        float GetExtraControlsSectionWidth() override;
        void DrawExtraControlsSection() override;
        TSharedPtr<FPropertyHandle> GetPropertyHandle() const { return PropertyHandle; }


        FArrayProperty*             ArrayProperty = nullptr;
        TSharedPtr<FPropertyHandle> PropertyHandle;

    };

    class FStructPropertyRow : public FPropertyRow
    {
    public:
        
        FStructPropertyRow(void* InPropPointer, FStructProperty* InProperty, FPropertyRow* InParentRow);
        ~FStructPropertyRow() override;
        void Update() override;
        void DrawHeader(float Offset) override;
        void DrawEditor() override;

        void RebuildChildren();

    private:
        
        FStructProperty*            StructProperty = nullptr;
        TSharedPtr<FPropertyHandle> PropertyHandle;
        FPropertyTable*             PropertyTable = nullptr;
    };
    
    class FCategoryPropertyRow : public FPropertyRow
    {
    public:
        
        FCategoryPropertyRow(void* InObj, const FName& InCategory);

        void AddProperty(FProperty* InProperty);
        FName GetCategoryName() const { return Category; }

        void DrawHeader(float Offset) override;
        
    private:

        void* OwnerObject = nullptr;
        FName Category;
    };
    
    class FPropertyTable
    {
    public:

        FPropertyTable() = default;
        FPropertyTable(void* InObject, CStruct* InType);
        FPropertyTable(const FPropertyTable&) = delete;
        FPropertyTable(FPropertyTable&&) = delete;
        ~FPropertyTable();
        
        bool operator = (const FPropertyTable&) const = delete;
        bool operator = (FPropertyTable&&) = delete;

        void RebuildTree();
        void DrawTree();

        CStruct* GetType() const { return Struct; }

        void SetObject(void* InObject, CStruct* StructType);
        void SetPreEditCallback(const TFunction<void()>& Callback);
        void SetPostEditCallback(const TFunction<void()>& Callback);
            
        FCategoryPropertyRow* FindOrCreateCategoryRow(const FName& CategoryName);

        TFunction<void()>                       PrePropertyChange;
        TFunction<void()>                       PostPropertyChange;
    private:

        CStruct*                                Struct = nullptr;
        void*                                   Object = nullptr;
        TVector<FCategoryPropertyRow*>          Categories;
        THashMap<FName, FCategoryPropertyRow*>  CategoryMap;
        
    };
}
