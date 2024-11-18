#include "SelectionManager.h"

namespace Lumina
{
    void FSelectionManager::AddSelection(ESelectionContext Context, const FGuid& Guid)
    {
        // Check if the context exists, if not, create an empty vector
        if (SelectionContexts.find(Context) == SelectionContexts.end())
        {
            SelectionContexts[Context] = TFastVector<FGuid>();
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
            auto pos = std::find(selectionList.begin(), selectionList.end(), Guid);
            if (pos != selectionList.end())
            {
                selectionList.erase(pos);
            }
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

    void FSelectionManager::GetSelections(ESelectionContext Context, TFastVector<FGuid>& OutSelections)
    {
        // Check if the context exists and the Guid is selected
        auto it = SelectionContexts.find(Context);
        if (it != SelectionContexts.end())
        {
            OutSelections = SelectionContexts[Context];
        }
    }
}
