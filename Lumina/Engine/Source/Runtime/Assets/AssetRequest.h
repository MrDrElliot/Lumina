#pragma once

#include "Core/Object/ObjectPtr.h"

namespace Lumina
{
    class CFactory;
    
    class FAssetRequest
    {
    public:

        friend class FAssetManager;

        
        FAssetRequest(const FString& InPath)
            : AssetPath(InPath)
            , PendingObject(nullptr)
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
