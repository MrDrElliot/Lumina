﻿#pragma once
#include <typeindex>

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

            PanelMap[typeid(Panel)] = NewPanel;
            EditorPanels.push_back(NewPanel);
            
        }

        template<typename T>
        static TRefPtr<T> GetPanel()
        {
            for (auto& Panel : Get()->EditorPanels)
            {
                std::type_index index = typeid(T);
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

        void GetPanels(TFastVector<TRefPtr<EditorImGuiWindow>>& Panels) { Panels = EditorPanels; }
        
    

    private:

        TFastVector<TRefPtr<EditorImGuiWindow>> EditorPanels;
        std::unordered_map<std::type_index, TRefPtr<EditorImGuiWindow>> PanelMap;

    };
}
