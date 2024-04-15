#include "StaticMesh.h"

namespace Lumina
{
    FStaticMesh::FStaticMesh(const std::filesystem::path& InPath)
    {
    }

    FStaticMesh::~FStaticMesh()
    {
    }

    void FStaticMesh::SetName(const std::string& NewName)
    {
        Name = NewName;
    }
}
