#include "Package.h"

#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Core/Engine/Engine.h"
#include "Core/Object/Class.h"
#include "Core/Profiler/Profile.h"
#include "Paths/Paths.h"
#include "Platform/Filesystem/FileHelper.h"

#include "Core/Serialization/Package/PackageSaver.h"
#include "Core/Serialization/Package/PackageLoader.h"


namespace Lumina
{
    IMPLEMENT_INTRINSIC_CLASS(CPackage, CObject, LUMINA_API)


    FObjectExport::FObjectExport(CObject* InObject)
    {
        ObjectName = InObject->GetFullyQualifiedName().c_str();
        ClassName = InObject->GetClass()->GetFullyQualifiedName();
        Offset = 0;
        Size = 0;
        Object = InObject;
    }

    FObjectImport::FObjectImport(CObject* InObject)
    {
        Package = InObject->GetPackage()->GetName();
        ObjectName = InObject->GetFullyQualifiedName().c_str();
        ClassName = InObject->GetClass()->GetFullyQualifiedName();
        Object = InObject;
    }

    
    void CPackage::OnDestroy()
    {
        if (Loader)
        {
            Memory::Delete(Loader);
        }
    }

    CPackage* CPackage::CreatePackage(const FString& InTopLevelClassName, const FString& FileName)
    {
        FString FullName = FileName;
        if (!Paths::HasExtension(FullName, ".lasset"))
        {
            Paths::AddPackageExtension(FullName);
        }
        
        FString VirtualPath = Paths::ConvertToVirtualPath(FileName.c_str());
        FName FileNameName = VirtualPath.c_str();
        
        CPackage* Package = FindObject<CPackage>(FileNameName);
        if (Package == nullptr)
        {
            FileHelper::CreateNewFile(FullName, true);
            Package = NewObject<CPackage>(nullptr, FileNameName);
        }

        Package->TopLevelClassName = InTopLevelClassName;
        LOG_INFO("Created Package: \"{}\"", VirtualPath);

        GEngine->GetEngineSubsystem<FAssetRegistry>()->BuildAssetDictionary();
        return Package;
    }

    CPackage* CPackage::LoadPackage(const FName& FileName)
    {
        LUMINA_PROFILE_SCOPE();

        FString FullName = FileName.ToString();
        if (!Paths::HasExtension(FullName, "lasset"))
        {
            Paths::AddPackageExtension(FullName);
        }

        FString VirtualPath = Paths::ConvertToVirtualPath(FullName);
        CPackage* Package = FindObject<CPackage>(VirtualPath.c_str());
        if (Package)
        {
            // Package is already loaded.
            return Package;
        }
        
        TVector<uint8> FileBinary;
        if (!FileHelper::LoadFileToArray(FileBinary, FullName))
        {
            return nullptr;
        }
        
        Package = NewObject<CPackage>(nullptr, VirtualPath.c_str());
        
        void* HeapData = Memory::Malloc(FileBinary.size());
        Memory::Memcpy(HeapData, FileBinary.data(), FileBinary.size());
        
        Package->Loader = Memory::New<FPackageLoader>(HeapData, FileBinary.size(), Package);
        FPackageLoader& Reader = *(FPackageLoader*)Package->Loader;

        
        FPackageHeader PackageHeader;
        Reader << PackageHeader;
        Package->TopLevelClassName = PackageHeader.ClassPath;

        Reader.Seek((int64)PackageHeader.ImportTableOffset);
        Reader << Package->ImportTable;
        
        Reader.Seek((int64)PackageHeader.ExportTableOffset);
        Reader << Package->ExportTable;

        for (const FObjectImport& Name : Package->ImportTable)
        {
            LOG_INFO("Import: {}", Name.ObjectName);
        }
        
        for (SIZE_T i = 0; i < Package->ExportTable.size(); ++i)
        {
            FObjectExport& Export = Package->ExportTable[i];
            
            FString ObjectName = GetObjectNameFromQualifiedName(Export.ObjectName.ToString());
            ObjectName = RemoveNumberSuffixFromObject(ObjectName);
            
            CClass* ObjectClass = FindObject<CClass>(Export.ClassName);
            CObject* Object = NewObject(ObjectClass, Package, FName(ObjectName));
            Object->SetFlag(OF_NeedsLoad);
            Object->LoaderIndex = FObjectPackageIndex::FromExport(i).GetRaw();

            Export.Object = Object;
        }

        LOG_INFO("Loaded Package: \"{}\" - ( [{}] Exports | [{}] Imports )", Package->GetName(), Package->ExportTable.size(), Package->ImportTable.size());

        return Package;
    }

    bool CPackage::SavePackage(CPackage* Package, CObject* Asset, const FName& FileName)
    {
        LUMINA_PROFILE_SCOPE();

        FString PathString = FileName.ToString();
        if (!Paths::HasExtension(PathString, "lasset"))
        {
            Paths::AddPackageExtension(PathString);
        }
        
        Package->ExportTable.clear();
        Package->ImportTable.clear();
        
        TVector<uint8> FileBinary;
        FPackageSaver Writer(FileBinary, Package);
        FPackageHeader PackageHeader;
        PackageHeader.Version = 1;
        PackageHeader.ClassPath = Package->TopLevelClassName;

        // Skip the header until we've built the tables.
        Writer.Seek(sizeof(FPackageHeader));
        
        // Build the save context (imports/exports)
        FSaveContext SaveContext(Package);
        Package->BuildSaveContext(SaveContext);

        for (CObject* Import : SaveContext.Imports)
        {
            Package->ImportTable.emplace_back(Import);
        }
        
        PackageHeader.ImportTableOffset = Writer.Tell();
        Writer << Package->ImportTable;


        
        for (CObject* Export : SaveContext.Exports)
        {
            Export->LoaderIndex = FObjectPackageIndex::FromExport(Package->ExportTable.size()).GetRaw();
            Package->ExportTable.emplace_back(Export);
        }
        
        PackageHeader.ObjectDataOffset = Writer.Tell();

        for (size_t i = 0; i < Package->ExportTable.size(); ++i)
        {
            FObjectExport& Export = Package->ExportTable[i];
            if (Export.Object == nullptr)
            {
                continue;
            }

            Export.Offset = Writer.Tell();
            
            Export.Object->Serialize(Writer);
            
            Export.Size = Writer.Tell() - Export.Offset;
            
        }
        
        PackageHeader.ExportTableOffset = Writer.Tell();
        Writer << Package->ExportTable;
        
        PackageHeader.ImportCount = (uint32)Package->ImportTable.size();
        PackageHeader.ExportCount = (uint32)Package->ExportTable.size();

        // Write the header.
        Writer.Seek(0);
        Writer << PackageHeader;

        // If the loader was not yet created, create it now.
        if (Package->Loader == nullptr)
        {
            void* HeapData = Memory::Malloc(FileBinary.size());
            Memory::Memcpy(HeapData, FileBinary.data(), FileBinary.size());
            Package->Loader = Memory::New<FPackageLoader>(HeapData, FileBinary.size(), Package);
        }

        if (!FileHelper::SaveArrayToFile(FileBinary, PathString))
        {
            return false;
        }
        
        LOG_INFO("Saved Package: \"{}\" - ( [{}] Exports | [{}] Imports )", Package->GetName(), Package->ExportTable.size(), Package->ImportTable.size());
        return true;
    }

    FPackageLoader* CPackage::GetLoader() const
    {
        return (FPackageLoader*)Loader;
    }

    void CPackage::BuildSaveContext(FSaveContext& Context)
    {
        TVector<CObject*> ExportObjects;
        ExportObjects.reserve(20);
        GetObjectsWithPackage(this, ExportObjects);

        FSaveReferenceBuilderArchive Builder(&Context);
        for (CObject* Object : ExportObjects)
        {
            Builder << Object;
        }
    }

    void CPackage::CreateExports()
    {
        while (ExportIndex < ExportTable.size())
        {
            

            ++ExportIndex;
        }
    }

    void CPackage::CreateImports()
    {
        
    }
    
    void CPackage::LoadObject(CObject* Object)
    {
        LUMINA_PROFILE_SCOPE();
        if (!Object || !Object->HasAnyFlag(OF_NeedsLoad))
        {
            return;
        }

        CPackage* ObjectPackage = Object->GetPackage();
        
        // If this object's package comes from somewhere else, load it through there.
        if (ObjectPackage != this)
        {
            ObjectPackage->LoadObject(Object);
            return;
        }

        int64 LoaderIndex = FObjectPackageIndex(Object->LoaderIndex).GetArrayIndex();

        // Validate index
        if (LoaderIndex < 0 || LoaderIndex >= (int64)ExportTable.size())
        {
            LOG_ERROR("Invalid loader index {} for object {}", LoaderIndex, Object->GetFullyQualifiedName());
            return;
        }

        FObjectExport& Export = ExportTable[LoaderIndex];

        if (!Loader)
        {
            LOG_ERROR("No loader set for package {}", GetName().ToString());
            return;
        }

        const int64 SavedPos = Loader->Tell();
        const int64 DataPos = Export.Offset;
        const int64 ExpectedSize = Export.Size;

        if (DataPos < 0 || ExpectedSize <= 0)
        {
            LOG_ERROR("Invalid export data for object {}. Offset: {}, Size: {}", Object->GetName().ToString(), DataPos, ExpectedSize);
            return;
        }
        
        // Seek to the data offset
        Loader->Seek(DataPos);

        
        Object->PreLoad();

        
        // Deserialize object
        Object->Serialize(*Loader);

        const int64 EndOffset = Loader->Tell();
        const int64 ActualSize = EndOffset - DataPos;
        
        if (ActualSize != ExpectedSize)
        {
            LOG_WARN("Mismatched size when loading object {}: expected {}, got {}", Object->GetName().ToString(), ExpectedSize, ActualSize);
        }


        Object->ClearFlags(OF_NeedsLoad);

        
        Object->PostLoad();
        
        // Reset the state of the loader to the previous object.
        Loader->Seek(SavedPos);
        
    }

    bool CPackage::LoadObjects()
    {
        for (const FObjectExport& Export : ExportTable)
        {
            LoadObject(Export.Object);
        }

        return true;
    }

    CObject* CPackage::IndexToObject(const FObjectPackageIndex& Index)
    {
        if (Index.IsNull())
        {
            return nullptr;
        }
        
        if (Index.IsImport())
        {
            SIZE_T ArrayIndex = Index.GetArrayIndex();
            if (ArrayIndex >= ImportTable.size())
            {
                LOG_WARN("Failed to find an object in the import table {}", ArrayIndex);
                return nullptr;
            }
            FObjectImport& Import = ImportTable[ArrayIndex];
            FString FullPath = Paths::ResolveVirtualPath(Import.Package.ToString());
            CPackage* Package = LoadPackage(FName(FullPath));
            Import.Object = FindObject<CObject>(Import.ObjectName);
            
            return ImportTable[ArrayIndex].Object;
        }

        if (Index.IsExport())
        {
            SIZE_T ArrayIndex = Index.GetArrayIndex();
            if (ArrayIndex >= ExportTable.size())
            {
                LOG_WARN("Failed to find an object in the export table {}", ArrayIndex);
                return nullptr;
            }
            
            return ExportTable[ArrayIndex].Object;
        }
        
        return nullptr;
    }
}
