#pragma once
#include "Core/Object/ObjectMacros.h"
#include "Core/Object/Object.h"
#include "Core/Object/ObjectHandleTyped.h"
#include "ThumbnailManager.generated.h"

namespace Lumina
{
    class CStaticMesh;
}

namespace Lumina
{
    LUM_CLASS()
    class CThumbnailManager : public CObject
    {
        GENERATED_BODY()
    public:

        CThumbnailManager();

        void Initialize();
        
        static CThumbnailManager& Get();
        


        LUM_PROPERTY(NotSerialized)
        TObjectHandle<CStaticMesh> CubeMesh;
        
        
        static CThumbnailManager* ThumbnailManagerSingleton;
    };
}
