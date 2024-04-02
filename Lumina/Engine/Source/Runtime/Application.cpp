#include "Application.h"

#include <iostream>

namespace Lumina
{
    FApplication::FApplication(const FApplicationSpecs& InAppSpecs)
    {
        AppSpecs = InAppSpecs;
    }

    FApplication::~FApplication()
    {
    }

    void FApplication::Run()
    {
        OnInit();
        
        while(!ShouldExit())
        {
            
        }
    }

    void FApplication::Close()
    {
    }

    void FApplication::OnInit()
    {
    }

    void FApplication::OnShutdown()
    {
    }
}
