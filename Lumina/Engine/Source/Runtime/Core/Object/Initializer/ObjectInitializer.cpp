#include "ObjectInitializer.h"

#include "Containers/Array.h"
#include "Core/Threading/Thread.h"

namespace Lumina
{
    FMutex StackMutex;
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
        FScopeLock Lock(StackMutex);
        InitializerStack.pop_back();
    }

    FObjectInitializer* FObjectInitializer::Get()
    {
        return InitializerStack.empty() ? nullptr : InitializerStack.back();
    }

    void FObjectInitializer::Construct()
    {
        FScopeLock Lock(StackMutex);
        InitializerStack.push_back(this);
    }
}
