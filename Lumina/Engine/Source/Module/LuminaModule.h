#pragma once
#include "Core/Module/ModuleInterface.h"

namespace Lumina
{
    class LUMINA_API FLuminaModule : public IModuleInterface
    {
    public:

        void StartupModule() override;
        void ShutdownModule() override;
    
    };
}
