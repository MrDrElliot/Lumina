#pragma once

#include "Containers/Array.h"
#include "Containers/Name.h"
#include "Containers/String.h"
#include "Core/Singleton/Singleton.h"
#include "ModuleInterface.h"
#include "Memory/Memory.h"
#include "Memory/SmartPtr.h"


#define IMPLEMENT_MODULE(ModuleClass, ModuleName) \
    DECLARE_MODULE_ALLOCATOR_OVERRIDES() \
    extern "C" __declspec(dllexport) Lumina::IModuleInterface* InitializeModule() \
    { \
        Lumina::Memory::InitializeThreadHeap(); \
        return Lumina::Memory::New<ModuleClass>(); \
    } \


namespace Lumina
{
    struct FModuleInfo
    {
        FName ModuleName;
        TUniquePtr<IModuleInterface> ModuleInterface;
        void* Module;
    };
    
    using ModuleInitFunc = IModuleInterface* (*)();
    
    class FModuleManager : public TSingleton<FModuleManager>
    {
    public:
        

        IModuleInterface* LoadModule(const FString& ModuleName);
        bool UnloadModule(const FString& ModuleName);

        void UnloadAllModules();


    private:

        FModuleInfo* GetOrCreateModuleInfo(const FString& ModuleName);


    private:

        THashMap<FName, FModuleInfo> ModuleHashMap;
        
    };
}
