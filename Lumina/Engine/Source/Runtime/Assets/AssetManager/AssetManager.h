#pragma once
#include <memory>

#include "Source/Runtime/ApplicationCore/Application.h"


namespace Lumina
{
    class FAssetRegistry;

    class FAssetManager
    {
    public:
        FAssetManager(const FApplicationSpecs& InAppSpecs);
        ~FAssetManager();
        
        static FAssetManager& Get();
        static FAssetRegistry& GetRegistry();
        

    private:

        static std::shared_ptr<FAssetRegistry> AssetRegistry;
    
    };
}
