#pragma once
#include <typeindex>
#include <EASTL/unordered_map.h>

#include "Containers/Array.h"
#include "Core/Singleton/Singleton.h"
#include "Memory/RefCounted.h"
#include "Panels/ImGuiWindow.h"

namespace Lumina
{
    class EditorImGuiWindow;

    class PanelManager : public TSingleton<PanelManager>
    {
    public:

        template<typename Panel, typename... Args>
        void RegisterPanel(Args&&... args)
        {
            static_assert(std::is_base_of<EditorImGuiWindow, Panel>::value, "Panel does not inherit from EditorPanel");

            // Create a new panel instance
            TRefPtr<Panel> NewPanel = MakeRefPtr<Panel>(std::forward<Args>(args)...);

            // Attach the panel
            NewPanel->OnAttach();

            PanelMap[typeid(Panel).hash_code()] = NewPanel;
            EditorPanels.push_back(NewPanel);
            
        }

        template<typename T>
        static TRefPtr<T> GetPanel()
        {
            for (auto& Panel : Get()->EditorPanels)
            {
                uint32 index = typeid(T).hash_code();
                auto it = Get()->PanelMap.find(index);
                if(it != Get()->PanelMap.end())
                {
                    return RefPtrCast<T>(Get()->PanelMap.at(index));
                }
            }
            return TRefPtr<T>();
        }

        void Shutdown() override
        {
            for (auto& Panel : EditorPanels)
            {
                Panel->OnDetach();
            }
            EditorPanels.clear();
        }

        void GetPanels(TVector<TRefPtr<EditorImGuiWindow>>& Panels) { Panels = EditorPanels; }
        
    

    private:

        TVector<TRefPtr<EditorImGuiWindow>> EditorPanels;
        eastl::unordered_map<uint32, TRefPtr<EditorImGuiWindow>> PanelMap;

    };
}
