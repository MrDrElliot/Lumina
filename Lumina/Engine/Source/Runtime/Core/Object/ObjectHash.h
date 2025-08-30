#pragma once
#include "Containers/Array.h"
#include "Containers/Name.h"
#include "Core/Assertions/Assert.h"
#include "Core/Profiler/Profile.h"
#include "Core/Singleton/Singleton.h"
#include "Core/Threading/Thread.h"


namespace Lumina
{
    class CClass;
    class CPackage;
    class CObjectBase;
}

namespace Lumina
{

    using FObjectHashBucket = THashSet<CObjectBase*>;

    class LUMINA_API FObjectHashTables : public TSingleton<FObjectHashTables>
    {
    public:

        void AddObject(CObjectBase* Object);

        void RemoveObject(CObjectBase* Object);

        CObjectBase* FindObject(CClass* ObjectClass, CPackage* Package, const FName& ObjectName, bool bExactClass = false);

        void Clear();

        mutable FMutex Mutex;
        THashMap<FName,     FObjectHashBucket>  ObjectNameHash;
        THashMap<CPackage*, FObjectHashBucket>  ObjectPackageHash;
        THashMap<CClass*,   FObjectHashBucket>  ObjectClassHash;
    };
}
