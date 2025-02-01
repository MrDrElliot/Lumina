#pragma once
#include "Containers/String.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{
    static FString GenericRequester = "Generic";

    class FAssetRequester
    {
    public:
        
        FAssetRequester() = default;
        FAssetRequester(const FString& InID) :Requester(InID) { }

        FORCEINLINE FString GetRequester() const { return Requester; }

        FORCEINLINE bool operator == (const FAssetRequester& Other) { return Requester == Other.Requester; }
        FORCEINLINE bool operator != (const FAssetRequester& Other) { return Requester != Other.Requester; }

    private:

        FString Requester = GenericRequester;
    };


    
}
