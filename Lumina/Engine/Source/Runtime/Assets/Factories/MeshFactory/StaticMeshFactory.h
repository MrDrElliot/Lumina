#pragma once

#include <filesystem>
#include <fastgltf/glm_element_traits.hpp>
#include "Containers/Array.h"
#include "Assets/Factories/Factory.h"
#include "Platform/GenericPlatform.h"



namespace Lumina
{
    
    class FStaticMeshFactory : public FFactory
    {
    public:

        FAssetPath CreateNew(const FString& Path) override;
        const FString& GetAssetName() const override { return "Static Mesh"; }
        
    };
}
