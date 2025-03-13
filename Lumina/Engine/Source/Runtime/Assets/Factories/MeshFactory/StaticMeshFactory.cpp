#include "StaticMeshFactory.h"

#include <random>
#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include "Renderer/RHIIncl.h"
#include "Log/Log.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"
#include "Core/Serialization/MemoryArchiver.h"
#include "Platform/Filesystem/FileHelper.h"


namespace Lumina
{
    ELoadResult FStaticMeshFactory::CreateNew(FAssetRecord* InRecord)
    {
        AStaticMesh* NewMesh = new AStaticMesh(InRecord->GetAssetPath());

        TVector<uint8> Buffer;
        if (!FFileHelper::LoadFileToArray(Buffer, InRecord->GetAssetPath().GetPathAsString()))
        {
            return ELoadResult::Failed;
        }

        FMemoryReader Reader(Buffer);

        FAssetHeader Header;
        Reader << Header;

        if (Header.Type != InRecord->GetAssetType())
        {
            return ELoadResult::Failed;
        }

        InRecord->SetDependencies(eastl::move(Header.Dependencies));
        
        NewMesh->Serialize(Reader);

        InRecord->SetAssetPtr(NewMesh);
        
        return ELoadResult::Succeeded;
    }

}
