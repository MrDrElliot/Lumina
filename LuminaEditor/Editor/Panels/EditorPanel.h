#pragma once
#include <string>

namespace Lumina
{
    class FEvent;

    class FEditorPanel
    {
    public:
        virtual ~FEditorPanel() = default;

        const std::string& GetName() { return Name; }

        virtual void OnAdded() = 0;
        virtual void OnRemoved() = 0;
        virtual void OnEvent(FEvent& Event) = 0;
        virtual void OnNewScene() = 0;
        virtual void OnRender(double DeltaTime) = 0;

    protected:

        std::string Name;
    };
}
