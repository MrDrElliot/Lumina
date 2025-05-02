#include "ObjectInitializer.h"

namespace Lumina
{
    static TVector<FObjectInitializer*> InitializerStack;

    FObjectInitializer::FObjectInitializer(CObject* Obj, const FConstructCObjectParams& InParams)
        : Object(Obj)
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
