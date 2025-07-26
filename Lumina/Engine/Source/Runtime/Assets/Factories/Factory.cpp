#include "Factory.h"

#include "Core/Object/Package/Package.h"
#include "Paths/Paths.h"
#include "TaskSystem/TaskSystem.h"

namespace Lumina
{
    CObject* CFactory::TryCreateNew(const FString& Path)
    {
        CPackage* Package = CPackage::CreatePackage(GetSupportedType()->GetName().ToString(), Path);
        FString FileName = Paths::FileName(Path);

        CObject* New = CreateNew(FileName.c_str(), Package);
        CPackage::SavePackage(Package, nullptr, Path.c_str());

        return New;
    }

    bool CFactory::ShowImportDialogue(CFactory* Factory, const FString& RawPath, const FString& DestinationPath)
    {
        bool bShouldClose = false;
        if (Factory->DrawImportDialogue(RawPath, DestinationPath, bShouldClose))
        {
            FTaskSystem::Get()->ScheduleLambda(1, [Factory, RawPath, DestinationPath](uint32 Start, uint32 End, uint32 Thread)
            {
                Factory->TryImport(RawPath, DestinationPath);
            });
        }

        return bShouldClose;
    }

    bool CFactory::ShowCreationDialogue(CFactory* Factory, const FString& Path)
    {
        bool bBah = false;
        return Factory->DrawCreationDialogue(Path, bBah);
    }
}
