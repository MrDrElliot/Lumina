﻿#include "ModuleManager.h"

#include "ModuleInterface.h"
#include "Paths/Paths.h"
#include "Platform/Process/PlatformProcess.h"


namespace Lumina
{
    IModuleInterface* FModuleManager::LoadModule(const FString& ModuleName)
    {
        void* ModuleHandle = Platform::GetDLLHandle(StringUtils::ToWideString(ModuleName).c_str());

        if (!ModuleHandle)
        {
            LOG_WARN("Failed to load module: {}", ModuleName);
            return nullptr;
        }

        ModuleInitFunc InitFunctionPtr = reinterpret_cast<ModuleInitFunc>(
            Platform::GetDLLExport(ModuleHandle, L"InitializeModule"));

        if (!InitFunctionPtr)
        {
            LOG_WARN("Failed to get InitializeModule export: {}", ModuleName);
            return nullptr;
        }

        IModuleInterface* ModuleInterface = InitFunctionPtr();

        if (!ModuleInterface)
        {
            LOG_WARN("Module returned null from InitializeModule(): {}", ModuleName);
            return nullptr;
        }

        FModuleInfo* ModuleInfo = GetOrCreateModuleInfo(ModuleName);
        ModuleInfo->Module = ModuleHandle;
        ModuleInfo->ModuleInterface.reset(ModuleInterface);

        ModuleInterface->StartupModule();
        return ModuleInterface;
    }

    bool FModuleManager::UnloadModule(const FString& ModuleName)
    {
        FName ModuleFName = FName(ModuleName);
        auto it = ModuleHashMap.find(ModuleFName);

        if (it == ModuleHashMap.end())
        {
            LOG_WARN("Tried to unload module that isn't loaded: {}", ModuleName);
            return false;
        }

        FModuleInfo& Info = it->second;
        ModuleHashMap.erase(it);

        if (Info.ModuleInterface)
        {
            Info.ModuleInterface->ShutdownModule();
        }

        bool freed = Platform::FreeDLLHandle(Info.Module);
        if (!freed)
        {
            LOG_ERROR("Failed to free DLL handle for module: {}", ModuleName);
            return false;
        }

        return true;
    }

    void FModuleManager::UnloadAllModules()
    {
        TVector<FName> Keys;
        for (const auto& Pair : ModuleHashMap)
        {
            Keys.push_back(Pair.first);
        }

        for (const FName& Key : Keys)
        {
            UnloadModule(Key.ToString());
        }
    }

    FModuleInfo* FModuleManager::GetOrCreateModuleInfo(const FString& ModuleName)
    {
        FName ModuleFName = FName(ModuleName);
        auto it = ModuleHashMap.find(ModuleFName);

        if (it != ModuleHashMap.end())
        {
            return &it->second;
        }

        FModuleInfo NewInfo;
        NewInfo.ModuleName = ModuleFName;

        auto [insertIt, inserted] = ModuleHashMap.insert(eastl::make_pair(ModuleFName, Memory::Move(NewInfo)));
        return &insertIt->second;
    }

}
