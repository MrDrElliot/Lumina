#pragma once

#include <EASTL/atomic.h>
#include "AssetHandle.h"
#include "Core/Object/ObjectPtr.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    class CFactory;
    
    class FAssetRequest
    {
    public:

        friend class FAssetManager;

        
        FAssetRequest(const FString& InPath)
            : AssetPath(InPath)
            , bFailed(false)
        {
        }

        

        FORCEINLINE FStringView GetAssetPath() const { return AssetPath; }
        FORCEINLINE CObject* GetPendingObject() const { return PendingObject; }

    private:

        bool Process();
        
        
    private:

        FString                         AssetPath;
        CObject*                        PendingObject;
        bool                            bFailed;
    };
    
}
