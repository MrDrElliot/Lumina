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
        void GetSelections(ESelectionContext Context, TFastVector<FGuid>& OutSelections);


    private:

        
    std::unordered_map<ESelectionContext, TFastVector<FGuid>> SelectionContexts;
        
    };
}
