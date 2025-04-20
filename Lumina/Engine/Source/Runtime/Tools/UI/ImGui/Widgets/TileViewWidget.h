#pragma once
#include "imgui.h"
#include "Containers/Array.h"
#include "Containers/Name.h"
#include "Core/Functional/Function.h"
#include "Core/Templates/Forward.h"
#include "Memory/Memory.h"

namespace Lumina
{

    class LUMINA_API FTileViewItem
    {

        friend class FTileViewWidget;
    public:
        
        virtual ~FTileViewItem() = default;

        FTileViewItem(FTileViewItem* InParent)
            : bExpanded(false)
            , bVisible(false)
            , bSelected(false)
        {}

        virtual FName GetName() const { return {}; }
        
        virtual const char* GetTooltipText() const { return nullptr; }

        virtual bool HasContextMenu() { return false; }

        virtual ImVec4 GetDisplayColor() const { return {};  }

        virtual void OnSelectionStateChanged() { }

        virtual FInlineString GetDisplayName() const
        {
            return GetName().c_str();
        }

    private:

        // Disable copies/moves
        FTileViewItem& operator=(FTileViewItem const&) = delete;
        FTileViewItem& operator=(FTileViewItem&&) = delete;


    protected:
        
        uint8                       bExpanded:1;
        uint8                       bVisible:1;
        uint8                       bSelected:1;
        
    };

    struct LUMINA_API FTileViewContext
    {
        /** Callback to draw any context menus this item may want */
        TFunction<void(const TVector<FTileViewItem*>&)>         DrawItemContextMenuFunction;

        /** Called when a rebuild of the widget tree is requested */
        TFunction<void(FTileViewWidget*)>                       RebuildTreeFunction;

        /** Called when an item has been selected in the tree */
        TFunction<void(FTileViewItem*)>                         ItemSelectedFunction;
    };

    class LUMINA_API FTileViewWidget
    {
    public:

        void Draw(FTileViewContext Context);

        void ClearTree();

        FORCEINLINE void MarkTreeDirty() { bDirty = true; }

        FORCEINLINE bool IsCurrentlyDrawing() const { return bCurrentlyDrawing; }
        FORCEINLINE bool IsDirty() const { return bDirty; }
        
        template<typename T, typename... Args>
        requires (std::is_base_of_v<FTileViewItem, T> && std::is_constructible_v<T, Args...>)
        void AddItemToTree(Args&&... args)
        {
            static_assert(std::is_base_of_v<FTileViewItem, T>, "T must inherit from FTreeListView");
            static_assert(std::is_constructible_v<T, Args...>, "T is not constructible using Args");

            T* New = FMemory::New<T>(TForward<Args>(args)...);
            ListItems.push_back(New);
        }


    private:

        
        void RebuildTree(FTileViewContext Context);
        
        void DrawItem(FTileViewItem* ItemToDraw, FTileViewContext Context);

        void SetSelection(FTileViewItem* Item, FTileViewContext Context);
        void ClearSelection();
    

    private:

        TVector<FTileViewItem*>                 Selections;

        /** Root nodes */
        TVector<FTileViewItem*>                 ListItems;

        uint8                                   bDirty:1;
        uint8                                   bCurrentlyDrawing:1;
    };
}
