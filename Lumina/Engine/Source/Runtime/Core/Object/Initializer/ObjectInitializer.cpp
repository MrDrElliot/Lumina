#include "ObjectInitializer.h"

#include "Containers/Array.h"

namespace Lumina
{
    static TVector<FObjectInitializer*> InitializerStack;

    FObjectInitializer::FObjectInitializer(CObject* Obj, CPackage* InPackage, const FConstructCObjectParams& InParams)
        : Object(Obj)
        , Package(InPackage)
        , Params(InParams)
    {
        Construct();
    }

    FObjectInitializer::~FObjectInitializer()
    {
        InitializerStack.pop_back();
    }

    FObjectInitializer* FObjectInitializer::Get()
    {
        return InitializerStack.empty() ? nullptr : InitializerStack.back();
    }

    void FObjectInitializer::Construct()
    {
        InitializerStack.push_back(this);
        
        
    }
}
