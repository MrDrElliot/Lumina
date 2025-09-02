#pragma once

#include "Lumina.h"
#include "Core/Object/Class.h"
#include "Core/Object/Object.h"
#include "Memory/SmartPtr.h"

namespace Lumina
{
    struct FPackageThumbnail;
    class FSaveContext;
    class FPackageLoader;
}

#define PACKAGE_FILE_TAG 0x9E2A83C1

namespace Lumina
{
    //==================================================================================================
    //  CPackage
    //  ----------------------------------------------------------------------------------------------
    //  Represents the top-level container for one or more serialized or script-defined CObjects.
    //  
    //  In the Lumina engine, a package is the authoritative root for any CObject path. All objects 
    //  exist *within* a package, and all object references begin with their owning package.
    //
    //  Objects are generally lazy-loaded. That means when a package is loaded, all objects listed as
    //  exports inside of that package have their initial memory allocated and are referencable.
    //  The actual object still remains in a "needs loading" state, indicated by the "OF_NeedsLoad" flag.
    //
    //  When an object inside the package is requested, it starts a chain that can recursively load
    //  multiple other assets across different packages. Objects serialized inside of this package are
    //  stored within the export table, objects referenced that live outside of this package are stored
    //  in the import table.
    //
    //  There are two types of packages:
    //  
    //      - "script://"  → Indicates the package is defined in native C++ source code. These packages
    //                        typically hold engine classes and statically defined types.
    //
    //      - "project://" → Indicates the package is defined in a serialized asset file on disk 
    //                        (typically with a `.lasset` extension). These packages contain runtime or 
    //                        editor-authored asset data.
    //
    //  Each package maintains an internal Import Table and Export Table:
    //      - The **Import Table** lists all external CObject references used by this package.
    //      - The **Export Table** lists all CObjects defined in this package (name, class, offset, etc).
    //
    //  Import and export entries allow the system to resolve object dependencies across package
    //  boundaries in a lazy and memory-efficient way.
    //
    //  Object paths are formatted as:
    //      <package-path>.<object-name>
    //
    //  Where the dot (`.`) character separates the package name from the object name.
    //  For example:
    //      - project://Characters/HeroMesh.HeroMesh
    //      - script://Engine/Pipelines.DefaultPipeline
    //
    //==================================================================================================
    
    /*
    ====================================================================================================
    //  Lumina Serialized Package Layout
    //  --------------------------------------------------------------------------------------------
    //  Describes the binary structure of a `.lasset` package file used by the Lumina engine.
    //  This layout supports lazy object loading, import/export resolution, and efficient serialization.
    ====================================================================================================
    
        Offset 0
        +----------------------------+
        | FPackageHeader            |   // Version info + table offsets
        +----------------------------+
        | uint32  Version           |
        | uint32  ImportCount       |
        | uint32  ExportCount       |
        | uint64  ImportTableOffset |
        | uint64  ObjectDataOffset  |
        | uint64  ExportTableOffset |
        +----------------------------+
    
        // At ImportTableOffset
        +----------------------------------------+
        | TArray<FObjectImport>                  |   // Describes referenced external objects
        +----------------------------------------+
        | FObjectImport[0]                       |
        |   FName ObjectName                     |
        |   FName ClassName                      |
        | FObjectImport[1]                       |
        |   ...                                  |
        +----------------------------------------+
    
        // At ObjectDataOffset
        +----------------------------------------+
        | Serialized Object Data (Exports only)  |   // Raw serialized memory per object
        +----------------------------------------+
        | [Export 0]                             |
        |   Object->Serialize(Writer)           |
        | [Export 1]                             |
        |   Object->Serialize(Writer)           |
        | ...                                    |
        +----------------------------------------+
    
        // At ExportTableOffset
        +----------------------------------------+
        | TArray<FObjectExport>                  |   // Describes each serialized object in this package
        +----------------------------------------+
        | FObjectExport[0]                       |
        |   FName ObjectName                     |
        |   FName ClassName                      |
        |   uint64 Offset    // Into ObjectData  |
        |   uint64 Size                          |
        |   EObjectFlags Flags                   |
        | FObjectExport[1]                       |
        |   ...                                  |
        +----------------------------------------+
    
        // Notes:
        // - Objects are saved flat, not nested. Each export is serialized independently.
        // - Export and Import tables track cross-package dependencies and allow for lazy loading.
        // - Index 0 is always treated as "null object". ExportIndex = 1 maps to Export[0].
        // - FObjectPackageIndex uses positive for exports, negative for imports, 0 for null.
    
    ====================================================================================================
    */

    struct FObjectExport
    {
        FObjectExport() = default;
        FObjectExport(CObject* InObject);

        /** Object name within the package (e.g., "HeroMesh") */
        FName ObjectName;

        /** The class of the object (e.g., "CStaticMesh") */
        FName ClassName;

        /** Offset into the package data where the serialized object begins (for project packages) */
        int64 Offset;

        /** Size in bytes of the serialized object */
        int64 Size;

        /** The object which may have been loaded into the package */
        CObject* Object = nullptr;

        INLINE friend FArchive& operator << (FArchive& Ar, FObjectExport& Data)
        {
            Ar << Data.ObjectName;
            Ar << Data.ClassName;
            Ar << Data.Offset;
            Ar << Data.Size;
            
            return Ar;
        }
    };

    struct FObjectImport
    {
        FObjectImport() = default;
        FObjectImport(CObject* InObject);
        
        /** Full path to the package this object comes from */
        FName Package; // e.g., "project://Materials/Steel"

        /** Name of the object being imported from that package */
        FName ObjectName;    // e.g., "SteelMaterial"

        /** The class of the object (optional; may be validated on load) */
        FName ClassName;

        /** Runtime-resolved pointer after import is loaded */
        CObject* Object = nullptr;

        INLINE friend FArchive& operator << (FArchive& Ar, FObjectImport& Data)
        {
            Ar << Data.Package;
            Ar << Data.ObjectName;
            Ar << Data.ClassName;
            
            return Ar;
        }
    };

    struct FPackageHeader
    {

        /** Tag matching PACKAGE_FILE_TAG to make sure this file is a Lumina package */
        uint32 Tag;
        
        /** File format version (increment when the layout changes) */
        uint32 Version;

        /** Byte offset from file start to the import table section */
        uint64 ImportTableOffset;

        /** Number of entries in the import table */
        uint32 ImportCount;

        /** Byte offset from file start to the export table section */
        uint64 ExportTableOffset;

        /** Number of entries in the export table */
        uint32 ExportCount;

        /** Byte offset from file start to the raw object data block */
        uint64 ObjectDataOffset;

        /** Byte offset from the file start to the thumbnail */
        uint64 ThumbnailDataOffset;

        friend FArchive& operator << (FArchive& Ar, FPackageHeader& Data)
        {
            Ar << Data.Tag;
            Ar << Data.Version;
            Ar << Data.ImportTableOffset;
            Ar << Data.ImportCount;
            Ar << Data.ExportTableOffset;
            Ar << Data.ExportCount;
            Ar << Data.ObjectDataOffset;
            Ar << Data.ThumbnailDataOffset;

            return Ar;
        }
    };

    /**
     * Stores either a negative number to represent an import index,
     * or a positive number to represent an export index.
     * 0 represents null (no reference).
     * Use IsImport(), IsExport(), IsNull(), and GetArrayIndex() for safe access.
     */
    struct FObjectPackageIndex
    {
    public:
        
        FObjectPackageIndex() : Index(0) {}

        // Construct from raw index (Import: -(i+1), Export: i+1)
        explicit FObjectPackageIndex(int64 InIndex) : Index(InIndex) {}

        // Construct an import index from array index
        static FObjectPackageIndex FromImport(int64 ImportArrayIndex)
        {
            return FObjectPackageIndex(-(ImportArrayIndex + 1));
        }

        // Construct an export index from array index
        static FObjectPackageIndex FromExport(int64 ExportArrayIndex)
        {
            return FObjectPackageIndex(ExportArrayIndex + 1);
        }

        // Check if the index is null (no reference)
        bool IsNull() const
        {
            return Index == 0;
        }

        // Check if it's an import
        bool IsImport() const
        {
            return Index < 0;
        }

        // Check if it's an export
        bool IsExport() const
        {
            return Index > 0;
        }

        // Get the raw internal value
        int64 GetRaw() const
        {
            return Index;
        }

        // Returns the usable array index for ImportTable or ExportTable
        SIZE_T GetArrayIndex() const
        {
            if (IsNull())
            {
                return INDEX_NONE;
            }

            return IsExport() ? (Index - 1) : (-Index - 1);
        }

        bool operator==(const FObjectPackageIndex& Other) const { return Index == Other.Index; }
        bool operator!=(const FObjectPackageIndex& Other) const { return Index != Other.Index; }

        INLINE friend FArchive& operator << (FArchive& Ar, FObjectPackageIndex& Data)
        {
            Ar << Data.Index;
            
            return Ar;
        }
        
    private:
        int64 Index;
    };

    //---------------------------------------------------------------------------------
    
    
    /**
     * Packages are the only CObject without a package of their own.
     */
    class CPackage : public CObject
    {
    public:

        DECLARE_CLASS(Lumina, CPackage, CObject, "", LUMINA_API)
        DEFINE_DEFAULT_CONSTRUCTOR_CALL(CPackage)


        void OnDestroy() override;
        
        /**
         * 
         * @param FileName 
         * @return 
         */
        LUMINA_API static CPackage* CreatePackage(const FString& FileName);

        /**
         * 
         * @param PackageName 
         * @return 
         */
        LUMINA_API static bool DestroyPackage(const FString& PackageName);

        /**
         * Will load the package and create the package loader. If called twice, nothing will happen.
         * Objects loaded from this package are not yet serialized, and are essentially shells marked with OF_NeedsLoad.
         * @param FileName File name to load the linker from.
         * @return Loaded package.
         */
        LUMINA_API static CPackage* LoadPackage(const FName& FileName);
        
        /**
         * Saves one specific object to disk.
         * @param Package Package to save.
         * @param Asset Object to save into package.
         * @param FileName Full filename.
         *
         * @return true if package saved successfully.
         */
        LUMINA_API static bool SavePackage(CPackage* Package, CObject* Asset, const FName& FileName);

        
        LUMINA_API FPackageLoader* GetLoader() const;

        LUMINA_API void BuildSaveContext(FSaveContext& Context);

        LUMINA_API void CreateExports();
        LUMINA_API void CreateImports();
        
        
        bool Rename(const FName& NewName, CPackage* NewPackage = nullptr) override;
        
        /**
         * Actually serialize the object from this package. After this function is finished,
         * the object will be fully loaded. If it's called without OF_NeedsLoad, data serialization
         * will be skipped.
         * 
         * @param Object Object to load
         * @return If load was successful.
         */
        LUMINA_API void LoadObject(CObject* Object);

        /**
         * Load all the objects in this package (serialize).
         * @return If all object loads were successful.
         */
        LUMINA_API bool LoadObjects();
        

        
        LUMINA_API CObject* IndexToObject(const FObjectPackageIndex& Index);

        /** Returns the thumbnail data for this package */
        LUMINA_API TSharedPtr<FPackageThumbnail> GetPackageThumbnail() const { return PackageThumbnail; }

        LUMINA_API FString GetPackageFilename() const;

        
        LUMINA_API void MarkDirty() { bDirty = true; }
        LUMINA_API void ClearDirty() { bDirty = false; }
        LUMINA_API bool IsDirty() const { return bDirty; }
        
    public:

        uint32                           bDirty:1=0;
        
        TSharedPtr<FArchive>             Loader;
        TVector<FObjectImport>           ImportTable;
        TVector<FObjectExport>           ExportTable;
        
        TSharedPtr<FPackageThumbnail>    PackageThumbnail;
        
        int64       ExportIndex = 0;
    };
    
}
