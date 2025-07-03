#pragma once

#include "Containers/String.h"

namespace Lumina
{
    class CObject;
    class CFactory;
    
    class FAssetRequest
    {
    public:

        friend class FAssetManager;

        
        FAssetRequest(const FString& InPath)
            : AssetPath(InPath)
            , PendingObject(nullptr)
        {
        }

        

        FORCEINLINE FStringView GetAssetPath() const { return AssetPath; }
        FORCEINLINE CObject* GetPendingObject() const { return PendingObject; }

    private:

        bool Process();
        
        
    private:

        FString                         AssetPath;
        CObject*                        PendingObject;
    };
    
}
