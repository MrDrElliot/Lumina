#pragma once
#include "Memory/SmartPtr.h"

namespace Lumina
{
    class FPropertyTable;
    class FStructProperty;
    class CStruct;
    class FPropertyHandle;
}

namespace Lumina
{
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


        void AddChild(FPropertyRow* InChild);
        void DrawRow(float Offset);

    protected:

        TSharedPtr<IPropertyTypeCustomization>  Customization;
        FProperty*                              Property = nullptr;
        FPropertyRow*                           ParentRow = nullptr;
        TVector<FPropertyRow*>                  Children;

        bool                                    bExpanded = true;
    };

    class FPropertyPropertyRow : public FPropertyRow
    {
    public:

        FPropertyPropertyRow(void* InPropertyPointer, FProperty* InProperty, FPropertyRow* InParentRow, int64 InArrayElementIndex);
        
        void DrawHeader(float Offset) override;
        void DrawEditor() override;

    private:

        int64                       ArrayElementIndex;
        TSharedPtr<FPropertyHandle> PropertyHandle;
    };

    class FArrayPropertyRow : public FPropertyRow
    {
    public:
        
        FArrayPropertyRow(void* InPropPointer, FArrayProperty* InProperty, FPropertyRow* InParentRow);
        void DrawHeader(float Offset) override;
        void DrawEditor() override;

        void RebuildChildren();
        
    private:

        FArrayProperty*             ArrayProperty = nullptr;
        TSharedPtr<FPropertyHandle> PropertyHandle;

    };

    class FStructPropertyRow : public FPropertyRow
    {
    public:
        
        FStructPropertyRow(void* InPropPointer, FStructProperty* InProperty, FPropertyRow* InParentRow);
        ~FStructPropertyRow();
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
            
        FCategoryPropertyRow* FindOrCreateCategoryRow(const FName& CategoryName);

    private:

        CStruct*                                Struct = nullptr;
        void*                                   Object = nullptr;
        TVector<FCategoryPropertyRow*>          Categories;
        THashMap<FName, FCategoryPropertyRow*>  CategoryMap;
    };
}
