#pragma once

#include "Assets/AssetHandle.h"
#include "Assets/AssetTypes.h"



namespace Lumina
{
    class FFactory
    {
    public:
        
        virtual ~FFactory() = default;
        virtual FAssetPath CreateNew(const FString& Path) = 0;
        
        virtual const FString& GetAssetName() const = 0;
        
    };
}
