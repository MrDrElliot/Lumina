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
#include "Tools/UI/ImGui/ImGuiX.h"

namespace Lumina
{
    
    static FPropertyRow* CreatePropertyRow(void* InOwner, FProperty* InProperty, FPropertyRow* InParentRow, const FPropertyChangedEventCallbacks& InCallbacks, SIZE_T ArrayElementIndex = INDEX_NONE)
    {
        FPropertyRow* NewRow = nullptr;
        if (FArrayProperty* ArrayProperty = dynamic_cast<FArrayProperty*>(InProperty))
        {
            NewRow = Memory::New<FArrayPropertyRow>(InOwner, ArrayProperty, InParentRow, InCallbacks);
        }
        else if (FStructProperty* StructProperty = dynamic_cast<FStructProperty*>(InProperty))
        {
            NewRow = Memory::New<FStructPropertyRow>(InOwner, StructProperty, InParentRow, InCallbacks);
        }
        else
        {
            NewRow = Memory::New<FPropertyPropertyRow>(InOwner, InProperty, InParentRow, ArrayElementIndex, InCallbacks);
        }

        return NewRow;
    }
    
    FPropertyRow::FPropertyRow(FProperty* InProperty, FPropertyRow* InParentRow, const FPropertyChangedEventCallbacks& InCallbacks)
        : ParentRow(InParentRow)
        , Callbacks(InCallbacks)
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

    void FPropertyRow::DestroyChildren()
    {
        for (FPropertyRow* Row : Children)
        {
            Row->DestroyChildren();
            Memory::Delete(Row);    
        }
        
        Children.clear();
    }

    void FPropertyRow::UpdateRow()
    {
        for (FPropertyRow* Child : Children)
        {
            Child->UpdateRow();
        }

        Update();
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
            constexpr ImGuiTableFlags Flags = ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_SizingFixedFit;
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(2, 0));
            if (ImGui::BeginTable("GridTable", HasExtraControls() ? 3 : 2, Flags))
            {
                ImGui::TableSetupColumn("##Editor", ImGuiTableColumnFlags_WidthStretch);

                if (HasExtraControls())
                {
                    ImGui::TableSetupColumn("##Extra", ImGuiTableColumnFlags_WidthFixed, GetExtraControlsSectionWidth());
                }
                
                ImGui::TableSetupColumn("##Reset", ImGuiTableColumnFlags_WidthFixed, 24);
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                
                DrawEditor();

                if (HasExtraControls())
                {
                    ImGui::TableNextColumn();
                    DrawExtraControlsSection();
                }
                
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

    FPropertyPropertyRow::FPropertyPropertyRow(void* InPropertyPointer, FProperty* InProperty, FPropertyRow* InParentRow, int64 InArrayElementIndex, const FPropertyChangedEventCallbacks& InCallbacks)
        : FPropertyRow(InProperty, InParentRow, InCallbacks)
        , ArrayElementIndex(InArrayElementIndex)
    {
        PropertyHandle = MakeSharedPtr<FPropertyHandle>(InPropertyPointer, InProperty);

        switch (Property->GetType())
        {
        case EPropertyTypeFlags::None:
            break;
        case EPropertyTypeFlags::Int8:
            {
                Customization = FNumericPropertyCustomization<int8, ImGuiDataType_S8>::MakeInstance();
            }
            break;
        case EPropertyTypeFlags::Int16:
            {
                Customization = FNumericPropertyCustomization<int16, ImGuiDataType_S16>::MakeInstance();
            }
            break;
        case EPropertyTypeFlags::Int32:
            {
                Customization = FNumericPropertyCustomization<int32, ImGuiDataType_S32>::MakeInstance();
            }
            break;
        case EPropertyTypeFlags::Int64:
            {
                Customization = FNumericPropertyCustomization<int64, ImGuiDataType_S64>::MakeInstance();
            }
            break;
        case EPropertyTypeFlags::UInt8:
            {
                Customization = FNumericPropertyCustomization<uint8, ImGuiDataType_U8>::MakeInstance();
            }
            break;
        case EPropertyTypeFlags::UInt16:
            {
                Customization = FNumericPropertyCustomization<uint16, ImGuiDataType_U16>::MakeInstance();
            }
            break;
        case EPropertyTypeFlags::UInt32:
            {
                Customization = FNumericPropertyCustomization<uint32, ImGuiDataType_U32>::MakeInstance();
            }
            break;
        case EPropertyTypeFlags::UInt64:
            {
                Customization = FNumericPropertyCustomization<uint64, ImGuiDataType_U64>::MakeInstance();
            }
            break;
        case EPropertyTypeFlags::Float:
            {
                Customization = FNumericPropertyCustomization<float, ImGuiDataType_Float>::MakeInstance();
            }
            break;
        case EPropertyTypeFlags::Double:
            {
                Customization = FNumericPropertyCustomization<double, ImGuiDataType_Double>::MakeInstance();
            }
            break;
        case EPropertyTypeFlags::Bool:
            {
                Customization = FBoolPropertyCustomization::MakeInstance();
            }
            break;
        case EPropertyTypeFlags::Object:
            {
                Customization = FCObjectPropertyCustomization::MakeInstance();
            }
            break;
        case EPropertyTypeFlags::Class:
            break;
        case EPropertyTypeFlags::Name:
            {
                Customization = FNamePropertyCustomization::MakeInstance();
            }
            break;
        case EPropertyTypeFlags::String:
            {
                Customization = FStringPropertyCustomization::MakeInstance();
            }
            break;
        case EPropertyTypeFlags::Enum:
            {
                Customization = FEnumPropertyCustomization::MakeInstance();
            }
            break;
        case EPropertyTypeFlags::Vector:
            break;
        case EPropertyTypeFlags::Struct:
            break;
        case EPropertyTypeFlags::Count:
            break;
        }
    }

    void FPropertyPropertyRow::Update()
    {
        switch (ChangeOp)
        {
        case EPropertyChangeOp::None:
            break;
        case EPropertyChangeOp::Updated:
            {
                if (Callbacks.PreChangeCallback)
                {
                    FPropertyChangedEvent Event(Callbacks.OwnerStruct, PropertyHandle->Property, PropertyHandle->Property->Name);
                    Callbacks.PreChangeCallback(Event);
                }
                
                Customization->UpdatePropertyValue(PropertyHandle);

                if (Callbacks.PostChangeCallback)
                {
                    FPropertyChangedEvent Event(Callbacks.OwnerStruct, PropertyHandle->Property, PropertyHandle->Property->Name);
                    Callbacks.PostChangeCallback(Event);
                }
            }
            break;
        }

        ChangeOp = EPropertyChangeOp::None;
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
            ChangeOp = Customization->UpdateAndDraw(PropertyHandle);
        }
        else
        {
            ImGui::Text(LE_ICON_EXCLAMATION "Missing Property Customization");
        }
        
        ImGui::EndDisabled();
    }

    bool FPropertyPropertyRow::HasExtraControls() const
    {
        return bArrayElement;
    }

    void FPropertyPropertyRow::DrawExtraControlsSection()
    {
        FArrayPropertyRow* ArrayRow = static_cast<FArrayPropertyRow*>(ParentRow);
        FArrayProperty* ArrayProperty = static_cast<FArrayPropertyRow*>(ParentRow)->ArrayProperty;
        FReflectArrayHelper Helper(ArrayProperty, ArrayRow->GetPropertyHandle()->PropertyPointer);
        
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 4));
        ImGuiX::FlatButton(LE_ICON_DOTS_HORIZONTAL, ImVec2(18, 24), 428768833);
        ImGui::PopStyleVar();

        if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonLeft))
        {
            if (ImGui::MenuItem(LE_ICON_PLUS" Insert New Element"))
            {
                
            }

            if (ArrayElementIndex > 0)
            {
                if (ImGui::MenuItem(LE_ICON_ARROW_UP" Move Element Up"))
                {
                    
                }
            }
            
            
            if (ArrayElementIndex < (Helper.Num() - 1))
            {
                if (ImGui::MenuItem(LE_ICON_ARROW_DOWN" Move Element Down"))
                {
                    
                }
            }

            if (ImGui::MenuItem(LE_ICON_TRASH_CAN" Remove Element"))
            {
                Helper.Remove(ArrayElementIndex);
                ArrayRow->RebuildChildren();
            }
            ImGui::EndPopup();
        }

        ImGuiX::ItemTooltip("Array Element Options");
    }

    FArrayPropertyRow::FArrayPropertyRow(void* InPropPointer, FArrayProperty* InProperty, FPropertyRow* InParentRow, const FPropertyChangedEventCallbacks& InCallbacks)
        : FPropertyRow(InProperty, InParentRow, InCallbacks)
        , ArrayProperty(InProperty)
    {
        PropertyHandle = MakeSharedPtr<FPropertyHandle>(InPropPointer, InProperty);
        RebuildChildren();
    }

    void FArrayPropertyRow::Update()
    {
        ChangeOp = EPropertyChangeOp::None;
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
        DestroyChildren();
        
        FReflectArrayHelper Helper(ArrayProperty, PropertyHandle->PropertyPointer);
        SIZE_T ElementCount = Helper.Num();

        for (SIZE_T i = 0; i < ElementCount; ++i)
        {
            void* Pointer = Helper.GetRawAt(i);
            FPropertyRow* NewRow = CreatePropertyRow(Pointer, ArrayProperty->GetInternalProperty(), this, Callbacks, i);
            NewRow->SetIsArrayElement(true);
            Children.push_back(NewRow);
        }
    }

    float FArrayPropertyRow::GetExtraControlsSectionWidth()
    {
        return 18 * 2 + 4;
    }

    void FArrayPropertyRow::DrawExtraControlsSection()
    {
        //ImGui::BeginDisabled();
        FReflectArrayHelper Helper(ArrayProperty, PropertyHandle->PropertyPointer);
        
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 4));
        if (ImGuiX::FlatButton(LE_ICON_PLUS, ImVec2(18, 24), 428768833))
        {
            Helper.Add(1);
            RebuildChildren();
        }
        ImGui::PopStyleVar();
        ImGuiX::ItemTooltip("Add array element");

        ImGui::SameLine();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 4));
        if (ImGuiX::FlatButton(LE_ICON_TRASH_CAN, ImVec2(18, 24), 428768833))
        {
            Helper.Clear();
            RebuildChildren();
        }
        ImGui::PopStyleVar();
        ImGuiX::ItemTooltip("Remove all array elements");

        //ImGui::EndDisabled();
    }

    FStructPropertyRow::FStructPropertyRow(void* InPropPointer, FStructProperty* InProperty, FPropertyRow* InParentRow, const FPropertyChangedEventCallbacks& InCallbacks)
        : FPropertyRow(InProperty, InParentRow, InCallbacks)
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

    void FStructPropertyRow::Update()
    {
        switch (ChangeOp)
        {
        case EPropertyChangeOp::None:
            break;
        case EPropertyChangeOp::Updated:
            {
                if (Callbacks.PreChangeCallback)
                {
                    FPropertyChangedEvent Event(Callbacks.OwnerStruct, PropertyHandle->Property, PropertyHandle->Property->Name);
                    Callbacks.PreChangeCallback(Event);
                }
                
                Customization->UpdatePropertyValue(PropertyHandle);

                if (Callbacks.PostChangeCallback)
                {
                    FPropertyChangedEvent Event(Callbacks.OwnerStruct, PropertyHandle->Property, PropertyHandle->Property->Name);
                    Callbacks.PostChangeCallback(Event);
                }
            }
            break;
        }

        ChangeOp = EPropertyChangeOp::None;
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
                ChangeOp = Customization->UpdateAndDraw(PropertyHandle);    
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

    FCategoryPropertyRow::FCategoryPropertyRow(void* InObj, const FName& InCategory, const FPropertyChangedEventCallbacks& InCallbacks)
        : FPropertyRow(InCallbacks)
        , Category(InCategory)
    {
        OwnerObject = InObj;
    }

    void FCategoryPropertyRow::AddProperty(FProperty* InProperty)
    {
        void* PropPointer = InProperty->GetValuePtr<void>(OwnerObject);
        FPropertyRow* NewRow = CreatePropertyRow(PropPointer, InProperty, this, Callbacks);
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
        : ChangeEventCallbacks()
        , Struct(InType)
        , Object(InObject)
    {
        ChangeEventCallbacks.OwnerStruct = Struct;
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
            Category->DestroyChildren();
            Memory::Delete(Category);
        }
        
        Categories.clear();
        CategoryMap.clear();
        
        FProperty* Current = Struct->LinkedProperty;
        while (Current != nullptr)
        {
            if (Current->Metadata.HasMetadata("Editable") || Current->Metadata.HasMetadata("ReadOnly"))
            {
                FName Category = "General";
                if (Current->Metadata.HasMetadata("Category"))
                {
                    Category = Current->Metadata.GetMetadata("Category");
                }
                
                FCategoryPropertyRow* CategoryRow = FindOrCreateCategoryRow(Category);
            
                CategoryRow->AddProperty(Current);
            }
            
            Current = static_cast<FProperty*>(Current->Next);
        }
    }

    void FPropertyTable::DrawTree()
    {
        constexpr ImGuiTableFlags Flags = 
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
                Category->UpdateRow();
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

        ChangeEventCallbacks.OwnerStruct = StructType;
        RebuildTree();
    }

    void FPropertyTable::SetPreEditCallback(const FPropertyChangedEventFn& Callback)
    {
        ChangeEventCallbacks.PreChangeCallback = Callback;
    }

    void FPropertyTable::SetPostEditCallback(const FPropertyChangedEventFn& Callback)
    {
        ChangeEventCallbacks.PostChangeCallback = Callback;
    }

    FCategoryPropertyRow* FPropertyTable::FindOrCreateCategoryRow(const FName& CategoryName)
    {
        if (CategoryMap.find(CategoryName) == CategoryMap.end())
        {
            FCategoryPropertyRow* NewRow = Memory::New<FCategoryPropertyRow>(Object, CategoryName, ChangeEventCallbacks);
            CategoryMap.emplace(CategoryName, NewRow);
            Categories.push_back(NewRow);
        }
        
        return CategoryMap[CategoryName];
    }
}
