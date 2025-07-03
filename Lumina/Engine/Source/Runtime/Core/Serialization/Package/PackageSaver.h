#pragma once
#include "Core/Object/Object.h"
#include "Core/Serialization/MemoryArchiver.h"
#include "Core/Serialization/Archiver.h"

namespace Lumina
{

    class CPackage;

    class FSaveContext
    {
    public:

        FSaveContext() = delete;
        FSaveContext(CPackage* InPackage)
            :CurrentPackage(InPackage)
        {}
        
        friend class FSaveReferenceBuilderArchive;

        void AddImport(CObject* Import);
        void AddExport(CObject* Export);

        
        THashSet<CObject*> SeenExports;
        THashSet<CObject*> SeenImports;
        
        TVector<CObject*> Imports;
        TVector<CObject*> Exports;
        
        CPackage* CurrentPackage;
    };

    //---------------------------------------------------------------------------------

    /** This archiver will traverse an object hierarchy and find any references and build a save context. */
    class FSaveReferenceBuilderArchive : public FArchive
    {
    public:
        
        using FArchive::operator<<;

        FSaveReferenceBuilderArchive() = delete;
        FSaveReferenceBuilderArchive(FSaveContext* SaveContext)
            : SaveContext(SaveContext)
        {
            this->SetFlag(EArchiverFlags::Writing);
        }

        virtual FArchive& operator<<(CObject*& Value) override;

    private:

        FSaveContext* SaveContext;
    };
    
    class FPackageSaver : public FMemoryWriter
    {
    public:
        
        using FArchive::operator<<;

        explicit FPackageSaver(TVector<uint8>& InBytes, CPackage* InPackage)
            : FMemoryWriter(InBytes, false)
            , Package(InPackage)
        {}

        virtual FArchive& operator<<(CObject*& Value) override;

    private:

        CPackage*   Package;

        uint32      CurrentExportIndex = 0;
        uint32      CurrentImportIndex = 0;
    };
}
