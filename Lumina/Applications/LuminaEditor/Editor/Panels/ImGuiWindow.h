#pragma once

#include "Memory/RefCounted.h"
#include "Containers/String.h"

namespace Lumina
{
    class FEvent;

    class EditorImGuiWindow : public FRefCounted
    {
    public:

        EditorImGuiWindow()
        {
            bVisible = true;
            bShowInWindows = true;
        }
        
        virtual void OnAttach() = 0;
        virtual void OnDetach() = 0;
        virtual void OnUpdate(double DeltaTime) = 0;
        virtual void OnEvent(FEvent& InEvent) = 0;
        
        bool IsVisible() { return bVisible; }
        void SetVisible(bool bNewVisibility) { bVisible = bNewVisibility; }

        FString GetName() { return Name; }
        void SetName(const FString& NewName) { Name = NewName; }
        bool ShouldShowInWindows() { return bShowInWindows; }
        

    protected:

        FString Name;
        uint8 bVisible:1;
        uint8 bShowInWindows:1;
    
    };
}
