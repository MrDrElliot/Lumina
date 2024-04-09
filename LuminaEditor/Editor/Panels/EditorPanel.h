#pragma once
#include <string>

namespace Lumina
{
    class FEditorPanel
    {
    public:

        const std::string& GetName() { return Name; }

        virtual void OnAdded() = 0;
        virtual void OnRemoved() = 0;
        virtual void OnImGui() = 0;
        virtual void OnNewScene() = 0;
        virtual void OnRender() = 0;

    protected:

        std::string Name;
    };
}
