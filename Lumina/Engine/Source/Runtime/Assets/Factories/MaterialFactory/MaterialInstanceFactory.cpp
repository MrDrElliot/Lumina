#include "MaterialInstanceFactory.h"

#include "assets/assettypes/material/material.h"
#include "Tools/UI/ImGui/ImGuiX.h"


namespace Lumina
{
    bool CMaterialInstanceFactory::HasCreationDialogue() const
    {
        return true;
    }

    bool CMaterialInstanceFactory::DrawCreationDialogue(const FString& Path, bool& bShouldClose)
    {
        FARFilter Filter;
        Filter.ClassNames.push_back(CMaterial::StaticClass()->GetName().ToString());

        bShouldClose = ImGuiX::ObjectSelector(Filter, SelectedMaterial);

        return SelectedMaterial != nullptr;
    }

    CObject* CMaterialInstanceFactory::CreateNew(const FName& Name, CPackage* Package)
    {
        CMaterialInstance* NewInstance =  NewObject<CMaterialInstance>(Package, Name);
        NewInstance->Material = SelectedMaterial;

        SelectedMaterial = nullptr;
        return NewInstance;
    }
}
