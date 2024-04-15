#pragma once
#include <filesystem>
#include <vector>

#include <iostream>
#include <optional>



namespace Lumina
{
    class FStaticMesh;

    std::optional<std::vector<std::shared_ptr<FStaticMesh>>> LoadGltfMeshes(std::filesystem::path FilePath);

}
