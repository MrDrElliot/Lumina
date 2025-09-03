#include "Factory.h"

#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Core/Engine/Engine.h"
#include "Core/Object/Package/Package.h"
#include "Paths/Paths.h"
#include "TaskSystem/TaskSystem.h"

namespace Lumina
{
    CObject* CFactory::TryCreateNew(const FString& Path)
    {
        CPackage* Package = CPackage::CreatePackage(Path);
        FString FileName = Paths::FileName(Path);

        CObject* New = CreateNew(FileName.c_str(), Package);
        New->SetFlag(OF_Public);
        GEngine->GetEngineSubsystem<FAssetRegistry>()->AssetCreated(New);

        return New;
    }

    bool CFactory::ShowImportDialogue(CFactory* Factory, const FString& RawPath, const FString& DestinationPath)
    {
        bool bShouldClose = false;
        if (Factory->DrawImportDialogue(RawPath, DestinationPath, bShouldClose))
        {
            Task::AsyncTask(1, [Factory, RawPath, DestinationPath](uint32 Start, uint32 End, uint32 Thread)
            {
                Factory->TryImport(RawPath, DestinationPath);
            });
        }

        return bShouldClose;
    }

    bool CFactory::ShowCreationDialogue(CFactory* Factory, const FString& Path)
    {
        bool bShouldClose = false;
        if (Factory->DrawCreationDialogue(Path, bShouldClose))
        {
            Task::AsyncTask(1, [Factory, Path](uint32 Start, uint32 End, uint32 Thread)
            {
                Factory->TryCreateNew(Path);    
            });
        }

        return bShouldClose;
    }
}
