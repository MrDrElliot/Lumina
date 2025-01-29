#pragma once

#include <unordered_map>

#include "Containers/Array.h"
#include "ContentBrowser/ImGui_MeshImporter.h"
#include "Core/Singleton/Singleton.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{

    enum class ESelectionContext : uint8
    {
        ContentBrowser,
        SceneOutliner,
    };
    
    class FSelectionManager : public TSingleton<FSelectionManager>
    {
    public:

        void AddSelection(ESelectionContext Context, const FGuid& Guid);
        void RemoveSelection(ESelectionContext Context, const FGuid& Guid);
        bool IsSelected(ESelectionContext Context, const FGuid& Guid);
        void GetSelections(ESelectionContext Context, TVector<FGuid>& OutSelections);

        bool ClearSelectionList(ESelectionContext Context);
        bool CanMultiSelect(ESelectionContext Context, const FGuid& Guid);
        
    private:

        
    eastl::unordered_map<ESelectionContext, TVector<FGuid>> SelectionContexts;
        
    };
}
