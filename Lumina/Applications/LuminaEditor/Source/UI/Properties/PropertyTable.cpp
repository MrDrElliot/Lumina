#include "PropertyTable.h"

#include "Core/Engine/Engine.h"
#include "Core/Object/Class.h"
#include "Core/Object/Object.h"
#include "Core/Reflection/PropertyCustomization/PropertyCustomization.h"
#include "Core/Reflection/Type/LuminaTypes.h"
#include "Core/Reflection/Type/Properties/ArrayProperty.h"
#include "Core/Reflection/Type/Properties/ObjectProperty.h"
#include "Core/Reflection/Type/Properties/StructProperty.h"
#include "Customizations/CoreTypeCustomization.h"
#include "Tools/UI/DevelopmentToolUI.h"
#include "Tools/UI/ImGui/ImGuiDesignIcons.h"

namespace Lumina
{
    
    static FPropertyRow* CreatePropertyRow(void* InOwner, FProperty* InProperty, FPropertyRow* InParentRow, SIZE_T ArrayElementIndex = INDEX_NONE)
    {
        FPropertyRow* NewRow = nullptr;
        if (FArrayProperty* ArrayProperty = dynamic_cast<FArrayProperty*>(InProperty))
        {
            NewRow = Memory::New<FArrayPropertyRow>(InOwner, ArrayProperty, InParentRow);
        }
        else if (FStructProperty* StructProperty = dynamic_cast<FStructProperty*>(InProperty))
        {
            NewRow = Memory::New<FStructPropertyRow>(InOwner, StructProperty, InParentRow);
        }
        else
        {
            NewRow = Memory::New<FPropertyPropertyRow>(InOwner, InProperty, InParentRow, ArrayElementIndex);
        }

        return NewRow;
    }
    
    FPropertyRow::FPropertyRow(FProperty* InProperty, FPropertyRow* InParentRow)
        : ParentRow(InParentRow)
    {
        Property = InProperty;
    }

    FPropertyRow::~FPropertyRow()
    {
        for (FPropertyRow* Row : Children)
        {
            Memory::Delete(Row);
        }

        Children.clear();
    }

    void FPropertyRow::AddChild(FPropertyRow* InChild)
    {
        Children.push_back(InChild);
    }

    void FPropertyRow::DrawRow(float Offset)
    {
        ImGui::PushID(this);
        
        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        DrawHeader(Offset);
        
        ImGui::TableNextColumn();
        {
            const ImGuiTableFlags Flags = ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_SizingFixedFit;
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(2, 0));
            if (ImGui::BeginTable("GridTable", 2, Flags))
            {
                ImGui::TableSetupColumn("##Editor", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("##Reset", ImGuiTableColumnFlags_WidthFixed, 24);
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                
                DrawEditor();

                ImGui::TableNextColumn();

                ImGui::EndTable();
            }
            ImGui::PopStyleVar();
        }
        ImGui::PopID();

        if (bExpanded)
        {
            bool bIsReadOnly = Property == nullptr ? false : Property->Metadata.HasMetadata("ReadOnly");
            ImGui::BeginDisabled(bIsReadOnly);
            const float ChildHeaderOffset = Offset + 20;
            for (FPropertyRow* Row : Children)
            {
                Row->DrawRow(ChildHeaderOffset);
            }
            ImGui::EndDisabled();
        }
    }

    FPropertyPropertyRow::FPropertyPropertyRow(void* InPropertyPointer, FProperty* InProperty, FPropertyRow* InParentRow, int64 InArrayElementIndex)
        : FPropertyRow(InProperty, InParentRow)
        , ArrayElementIndex(InArrayElementIndex)
    {
        PropertyHandle = MakeSharedPtr<FPropertyHandle>(InPropertyPointer, InProperty);
        
        if (Property->GetType() == EPropertyTypeFlags::Object)
        {
            Customization = FCObjectPropertyCustomization::MakeInstance();
        }
        else if ((int)Property->GetType() <= (int)EPropertyTypeFlags::Double)
        {
            Customization = FNumericPropertyCustomization::MakeInstance();
        }
        else if (Property->GetType() == EPropertyTypeFlags::Bool)
        {
            Customization = FBoolPropertyCustomization::MakeInstance();
        }
        else if (Property->GetType() == EPropertyTypeFlags::Enum)
        {
            Customization = FEnumPropertyCustomization::MakeInstance();
        }
        else if (Property->GetType() == EPropertyTypeFlags::Name)
        {
            Customization = FNamePropertyCustomization::MakeInstance();
        }
    }

    void FPropertyPropertyRow::DrawHeader(float Offset)
    {
        ImGui::Dummy(ImVec2(Offset, 0));
        ImGui::SameLine();

        FString DisplayName = ArrayElementIndex == INDEX_NONE ? Property->Name.ToString() : eastl::to_string(ArrayElementIndex);
        ImGui::TextUnformatted(DisplayName.c_str());
    }

    void FPropertyPropertyRow::DrawEditor()
    {
        bool bIsReadOnly = Property->Metadata.HasMetadata("ReadOnly");
        ImGui::BeginDisabled(bIsReadOnly);
        
        if (Customization)
        {
            Customization->DrawProperty(PropertyHandle);
        }
        else
        {
            ImGui::Text(LE_ICON_EXCLAMATION "Missing Property Customization");
        }
        
        ImGui::EndDisabled();
    }

    FArrayPropertyRow::FArrayPropertyRow(void* InPropPointer, FArrayProperty* InProperty, FPropertyRow* InParentRow)
        : FPropertyRow(InProperty, InParentRow)
        , ArrayProperty(InProperty)
    {
        PropertyHandle = MakeSharedPtr<FPropertyHandle>(InPropPointer, InProperty);
        RebuildChildren();
    }

    void FArrayPropertyRow::DrawHeader(float Offset)
    {
        ImGui::Dummy(ImVec2(Offset, 0));
        ImGui::SameLine();

        ImGui::SetNextItemOpen(bExpanded);
        ImGui::PushStyleColor(ImGuiCol_Header, 0);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, 0);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, 0);
        bExpanded = ImGui::CollapsingHeader(ArrayProperty->Name.c_str());
        ImGui::PopStyleColor(3);
    }

    void FArrayPropertyRow::DrawEditor()
    {
        FReflectArrayHelper Helper(ArrayProperty, PropertyHandle->PropertyPointer);
        SIZE_T ElementCount = Helper.Num();
        
        ImGui::TextColored(ImVec4(0.24f, 0.24f, 0.24f, 1.0f), "%llu Elements", ElementCount);
    }

    void FArrayPropertyRow::RebuildChildren()
    {
        FReflectArrayHelper Helper(ArrayProperty, PropertyHandle->PropertyPointer);
        SIZE_T ElementCount = Helper.Num();

        for (SIZE_T i = 0; i < ElementCount; ++i)
        {
            void* Pointer = Helper.GetRawAt(i);
            Children.emplace_back(CreatePropertyRow(Pointer, ArrayProperty->GetInternalProperty(), this, i));
        }
    }

    FStructPropertyRow::FStructPropertyRow(void* InPropPointer, FStructProperty* InProperty, FPropertyRow* InParentRow)
        : FPropertyRow(InProperty, InParentRow)
        , StructProperty(InProperty)
    {
        PropertyHandle = MakeSharedPtr<FPropertyHandle>(InPropPointer, InProperty);

        FPropertyCustomizationRegistry* Registry = GEngine->GetDevelopmentToolsUI()->GetPropertyCustomizationRegistry();
        Customization = Registry->GetPropertyCustomizationForType(StructProperty->GetStruct()->GetName());
        
        if (!Customization)
        {
            RebuildChildren();
        }
    }

    FStructPropertyRow::~FStructPropertyRow()
    {
        if (PropertyTable)
        {
            Memory::Delete(PropertyTable);
            PropertyTable = nullptr;
        }
    }

    void FStructPropertyRow::DrawHeader(float Offset)
    {
        ImGui::Dummy(ImVec2(Offset, 0));
        ImGui::SameLine();

        ImGui::SetNextItemOpen(bExpanded);
        ImGui::PushStyleColor(ImGuiCol_Header, 0);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, 0);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, 0);
        bExpanded = ImGui::CollapsingHeader(StructProperty->Name.c_str());
        ImGui::PopStyleColor(3);
    }

    void FStructPropertyRow::DrawEditor()
    {
        bool bIsReadOnly = Property->Metadata.HasMetadata("ReadOnly");
        ImGui::BeginDisabled(bIsReadOnly);
        
        if (bExpanded)
        {
            if (Customization)
            {
                Customization->DrawProperty(PropertyHandle);    
            }
            else if (PropertyTable)
            {
                PropertyTable->DrawTree();
            }
        }
        
        ImGui::EndDisabled();
    }

    void FStructPropertyRow::RebuildChildren()
    {
        if (PropertyTable)
        {
            Memory::Delete(PropertyTable);
            PropertyTable = nullptr;
        }
        
        PropertyTable = Memory::New<FPropertyTable>(PropertyHandle->PropertyPointer, StructProperty->GetStruct());
        PropertyTable->RebuildTree();
    }

    FCategoryPropertyRow::FCategoryPropertyRow(void* InObj, const FName& InCategory)
        : Category(InCategory)
    {
        OwnerObject = InObj;
    }

    void FCategoryPropertyRow::AddProperty(FProperty* InProperty)
    {
        void* PropPointer = InProperty->GetValuePtr<void>(OwnerObject);
        FPropertyRow* NewRow = CreatePropertyRow(PropPointer, InProperty, this);
        Children.push_back(NewRow);
    }

    void FCategoryPropertyRow::DrawHeader(float Offset)
    {
        ImGui::Dummy(ImVec2(Offset, 0));
        ImGui::SameLine();

        ImGui::SetNextItemOpen(bExpanded);
        ImGui::PushStyleColor(ImGuiCol_Header, 0);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, 0);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, 0);
        bExpanded = ImGui::CollapsingHeader(Category.c_str());
        ImGui::PopStyleColor(3);

        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, 0xFF1C1C1C);
    }

    FPropertyTable::FPropertyTable(void* InObject, CStruct* InType)
        : Struct(InType)
        , Object(InObject)
    {
    }

    FPropertyTable::~FPropertyTable()
    {
        for (FCategoryPropertyRow* Row : Categories)
        {
            Memory::Delete(Row);
        }

        Categories.clear();
        CategoryMap.clear();
    }

    void FPropertyTable::RebuildTree()
    {
        for (FCategoryPropertyRow* Category : Categories)
        {
            Memory::Delete(Category);
        }
        
        Categories.clear();
        CategoryMap.clear();
        
        FProperty* Current = Struct->LinkedProperty;
        while (Current != nullptr)
        {
            FName Category = "General";
            if (Current->Metadata.HasMetadata("Category"))
            {
                Category = Current->Metadata.GetMetadata("Category");
            }

            FCategoryPropertyRow* CategoryRow = FindOrCreateCategoryRow(Category);

            if (Current->Metadata.HasMetadata("Editable") || Current->Metadata.HasMetadata("ReadOnly"))
            {
                CategoryRow->AddProperty(Current);
            }
            
            Current = static_cast<FProperty*>(Current->Next);
        }
    }

    void FPropertyTable::DrawTree()
    {
        const ImGuiTableFlags Flags = 
            ImGuiTableFlags_BordersOuter | 
            ImGuiTableFlags_BordersInnerH | 
            ImGuiTableFlags_NoBordersInBodyUntilResize | 
            ImGuiTableFlags_SizingFixedFit;

        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4, 8));
        ImGui::PushID(this);

        if (ImGui::BeginTable("GridTable", 2, Flags))
        {
            ImGui::TableSetupColumn("##Header", ImGuiTableColumnFlags_WidthFixed, 175);
            ImGui::TableSetupColumn("##Editor", ImGuiTableColumnFlags_WidthStretch);
            
            for (FCategoryPropertyRow* Category : Categories)
            {
                Category->DrawRow(0);
            }

            ImGui::EndTable();
        }

        ImGui::PopStyleVar();
        ImGui::PopID();
    }

    void FPropertyTable::SetObject(void* InObject, CStruct* StructType)
    {
        Object = InObject;
        Struct = StructType;
        
        RebuildTree();
    }
    

    FCategoryPropertyRow* FPropertyTable::FindOrCreateCategoryRow(const FName& CategoryName)
    {
        if (CategoryMap.find(CategoryName) == CategoryMap.end())
        {
            FCategoryPropertyRow* NewRow = Memory::New<FCategoryPropertyRow>(Object, CategoryName);
            CategoryMap.emplace(CategoryName, NewRow);
            Categories.push_back(NewRow);
        }
        
        return CategoryMap[CategoryName];
    }
}
