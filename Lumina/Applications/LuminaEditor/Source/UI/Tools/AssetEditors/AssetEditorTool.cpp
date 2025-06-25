#include "AssetEditorTool.h"

#include "Core/Object/Class.h"
#include "Core/Serialization/Structured/StructuredArchive.h"
#include "Core/Serialization/Package/PackageSaver.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"
#include <Assets/AssetHeader.h>

namespace Lumina
{
    void FAssetEditorTool::Deinitialize(const FUpdateContext& UpdateContext)
    {
        FEditorTool::Deinitialize(UpdateContext);
        
        Asset->MarkGarbage();
        Asset = nullptr;
    }

    void FAssetEditorTool::OnSave()
    {
        TVector<uint8> Buffer;
        FPackageSaver Saver(Buffer);

        FAssetHeader Header;
        Header.ClassName = Asset->GetClass()->GetName().c_str();
        Header.Version = 1;
        Header.Path = Asset->GetPathName();
        
        Saver << Header;

        FBinaryStructuredArchive StructuredArchive(Saver);
        FArchiveSlot Slot = StructuredArchive.Open();
        Asset->Serialize(Slot);

        FString FullPath = Paths::ResolveVirtualPath(Asset->GetPathName()) + ".lasset";
        FFileHelper::SaveArrayToFile(Buffer, FullPath);
        
    }
}
