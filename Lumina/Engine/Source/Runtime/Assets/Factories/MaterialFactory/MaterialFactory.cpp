#include "MaterialFactory.h"

#include "Assets/AssetHeader.h"
#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Renderer/RHIIncl.h"
#include "Assets/AssetTypes/Material/Material.h"
#include "Core/Serialization/Package/PackageSaver.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"

namespace Lumina
{
    CObject* CMaterialFactory::CreateNew(const FName& Name, CPackage* Package)
    {
        return NewObject<CMaterial>(Package, Name);
    }
    
}
