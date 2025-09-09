#include "ObjectHash.h"

#include "ObjectBase.h"
#include "Package/Package.h"

namespace Lumina
{
    
    void FObjectHashTables::AddObject(CObjectBase* Object)
    {
        LUMINA_PROFILE_SCOPE();

        FName Name = Object->GetName();
        if (Name != NAME_None)
        {
            FScopeLock Lock(Mutex);
            
            THashSet<CObjectBase*>& NameHashSet = ObjectNameHash[Name];
            if (NameHashSet.find(Object) != NameHashSet.end())
            {
                LOG_CRITICAL("Object {} already exists in hash! [Double Add]", Name);
                LUM_ASSERT(0)
            }

            NameHashSet.emplace(Object);

            if (Object->IsA(CPackage::StaticClass()))
            {
                ObjectPackageHash.insert((CPackage*)Object);
            }

            if (CPackage* Package = Object->GetPackage())
            {
                THashSet<CObjectBase*>& PackageHashSet = ObjectPackageHash[Package];
                if (PackageHashSet.find(Object) != PackageHashSet.end())
                {
                    LOG_CRITICAL("Object {} already exists in package hash! [Double Add]", Name);
                    LUM_ASSERT(0)
                }

                PackageHashSet.emplace(Object);
            }

            THashSet<CObjectBase*>& ClassHashSet = ObjectClassHash[Object->GetClass()];
            if (ClassHashSet.find(Object) != ClassHashSet.end())
            {
                LOG_CRITICAL("Object {} already exists in class hash! [Double Add]", Name);
                LUM_ASSERT(0)
            }

            ClassHashSet.emplace(Object);
        }
    }

    void FObjectHashTables::RemoveObject(CObjectBase* Object)
    {
        LUMINA_PROFILE_SCOPE();

        FName Name = Object->GetName();
        if (Name != NAME_None)
        {
            FScopeLock Lock(Mutex);
            
            LUM_ASSERT(ObjectNameHash.find(Name) != ObjectNameHash.end())
            
            FObjectHashBucket& NameHashSet = ObjectNameHash.at(Name);
            NameHashSet.erase(Object);
            if (NameHashSet.empty())
            {
                ObjectNameHash.erase(Name);
            }

            if (CPackage* Package = Object->GetPackage())
            {
                LUM_ASSERT(ObjectPackageHash.find(Package) != ObjectPackageHash.end())
                FObjectHashBucket& PackageHashSet = ObjectPackageHash[Package];
                if (PackageHashSet.find(Object) == PackageHashSet.end())
                {
                    LOG_CRITICAL("Object {} does not exist in package hash {}! [Hash Failure]", Name, Package->GetName());
                    LUM_ASSERT(0)
                }

                PackageHashSet.erase(Object);
            }
            
            LUM_ASSERT(ObjectClassHash.find(Object->GetClass()) != ObjectClassHash.end())
            FObjectHashBucket& ClassHashSet = ObjectClassHash[Object->GetClass()];
            if (ClassHashSet.find(Object) == ClassHashSet.end())
            {
                LOG_CRITICAL("Object {} does not exist in class hash {}! [Hash Failure]", Name, Object->GetClass()->GetName());
                LUM_ASSERT(0)
            }

            ClassHashSet.erase(Object);
        }
    }

    CObjectBase* FObjectHashTables::FindObject(CClass* ObjectClass, CPackage* Package, const FName& ObjectName, bool bExactClass)
    {
        LUMINA_PROFILE_SCOPE();
        FScopeLock Lock(Mutex);
    
        FObjectHashBucket* Bucket = nullptr;
    
        if (Package != nullptr)
        {
            LUM_ASSERT(ObjectPackageHash.find(Package) != ObjectPackageHash.end())
            Bucket = &ObjectPackageHash[Package];
        }
        else
        {
            Bucket = &ObjectNameHash[ObjectName];
        }
    
        for (CObjectBase* Object : *Bucket)
        {
            if (Package != nullptr && Object->GetName() != ObjectName)
            {
                continue;
            }
            
            if (Package == nullptr && Object->GetPackage() != nullptr)
            {
                continue;
            }
            
            if (ObjectClass == nullptr || (bExactClass ? Object->GetClass() == ObjectClass : Object->IsA(ObjectClass)))
            {
                return Object;
            }
        }
    
        return nullptr;
    }
    

    void FObjectHashTables::Clear()
    {
        FScopeLock Lock(Mutex);
        
        ObjectNameHash.clear();
        ObjectPackageHash.clear();
        ObjectClassHash.clear();
    }
}
