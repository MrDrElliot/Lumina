#pragma once
#include "Subsystems/Subsystem.h"


namespace Lumina
{
    class FWindow;
    struct FWindowSpecs;

    class WindowSubsystem : public ISubsystem
    {
    public:

        void Initialize() override;
        void Update(double DeltaTime);
        void Deinitialize() override;

        void InitializeWindow(const FWindowSpecs& Specs);

        FWindow* GetWindow() const { return Window; }


    private:

        FWindow* Window = nullptr;
    
    };
}
