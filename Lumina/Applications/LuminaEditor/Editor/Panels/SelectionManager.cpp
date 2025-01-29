#include "SelectionManager.h"

namespace Lumina
{
    void FSelectionManager::AddSelection(ESelectionContext Context, const FGuid& Guid)
    {

        if ((!SelectionContexts.empty()) && (!CanMultiSelect(Context, Guid)))
        {
            ClearSelectionList(Context);
        }
        
        // Check if the context exists, if not, create an empty vector
        if (SelectionContexts.find(Context) == SelectionContexts.end())
        {
            SelectionContexts[Context] = TVector<FGuid>();
        }

        // Only add the Guid if it is not already selected
        auto& selectionList = SelectionContexts[Context];
        if (std::find(selectionList.begin(), selectionList.end(), Guid) == selectionList.end())
        {
            selectionList.push_back(Guid);
        }
    }

    void FSelectionManager::RemoveSelection(ESelectionContext Context, const FGuid& Guid)
    {
        // Check if the context exists and the Guid is selected
        auto it = SelectionContexts.find(Context);
        if (it != SelectionContexts.end())
        {
            auto& selectionList = it->second;
            selectionList.erase(std::remove_if(selectionList.begin(), selectionList.end(), [&] (const FGuid& Other)
            {
                return Other == Guid;
            }));

        }
    }

    bool FSelectionManager::IsSelected(ESelectionContext Context, const FGuid& Guid)
    {
        // Check if the context exists and if the Guid is in the selection
        auto it = SelectionContexts.find(Context);
        if (it != SelectionContexts.end())
        {
            auto& selectionList = it->second;
            return std::find(selectionList.begin(), selectionList.end(), Guid) != selectionList.end();
        }
        return false;
    }

    void FSelectionManager::GetSelections(ESelectionContext Context, TVector<FGuid>& OutSelections)
    {
        // Check if the context exists and the Guid is selected
        auto it = SelectionContexts.find(Context);
        if (it != SelectionContexts.end())
        {
            OutSelections = SelectionContexts[Context];
        }
    }

    bool FSelectionManager::ClearSelectionList(ESelectionContext Context)
    {
        return SelectionContexts.erase(Context);
    }

    bool FSelectionManager::CanMultiSelect(ESelectionContext Context, const FGuid& Guid)
    {
        return Context != ESelectionContext::SceneOutliner ? true : false;
    }
}
