#pragma once

#include "Containers/Name.h"

namespace Lumina
{
    struct FAssetData
    {
        /** Path of this asset in full form (project:://Package.ObjectName). */
        FName FullPath;

        /** Name of the package owning this asset (project://package) */
        FName PackageName;

        /** Name of the asset without its package */
        FName AssetName;

        /** Path of the asset's class (script://lumina.CStaticMesh) */
        FName AssetClass;


        LUMINA_API bool IsRedirector() const;
        
    };
}
