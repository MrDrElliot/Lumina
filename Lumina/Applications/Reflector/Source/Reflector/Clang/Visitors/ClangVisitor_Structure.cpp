#include "ClangVisitor_Structure.h"

#include "Reflector/Clang/ClangParserContext.h"
#include "Reflector/Clang/Utils.h"
#include "Reflector/ReflectionCore/ReflectionMacro.h"
#include "Reflector/Types/FieldInfo.h"
#include "Reflector/Types/Properties/ReflectedArrayProperty.h"
#include "Reflector/Types/Properties/ReflectedEnumProperty.h"
#include "Reflector/Types/Properties/ReflectedNumericProperty.h"
#include "Reflector/Types/Properties/ReflectedObjectProperty.h"

namespace Lumina::Reflection::Visitor
{

    static FFieldInfo CreateFieldInfo(CXCursor Cursor)
    {
        eastl::string CursorName = ClangUtils::GetCursorDisplayName(Cursor);

        CXType FieldType = clang_getCursorType(Cursor);
        clang::QualType FieldQualType = ClangUtils::GetQualType(FieldType);


        eastl::string TypeSpelling;
        ClangUtils::GetQualifiedNameForType(FieldQualType, TypeSpelling);
        
        EPropertyTypeFlags PropFlags = GetCoreTypeFromName(TypeSpelling.c_str());

        // Is not a core type.
        if (PropFlags == EPropertyTypeFlags::None)
        {
            if (FieldQualType->isEnumeralType())
            {
                PropFlags = EPropertyTypeFlags::Enum;
            }
        }

        FFieldInfo Info;
        Info.Flags = PropFlags;
        Info.Type = FieldType;
        Info.Name = CursorName;
        Info.TypeName = TypeSpelling;

        return Info;
    }

    static FFieldInfo CreateSubFieldInfo(CXType FieldType)
    {
        clang::QualType FieldQualType = ClangUtils::GetQualType(FieldType);
        eastl::string TypeSpelling = ClangUtils::GetString(clang_getTypeSpelling(FieldType));

        eastl::string FieldName; 
        ClangUtils::GetQualifiedNameForType(FieldQualType, FieldName);
        EPropertyTypeFlags PropFlags = GetCoreTypeFromName(FieldName.c_str());

        // Is not a core type.
        if (PropFlags == EPropertyTypeFlags::None)
        {
            if (FieldQualType->isEnumeralType())
            {
                PropFlags = EPropertyTypeFlags::Enum;
            }
        }
        

        FFieldInfo Info;
        Info.Flags = PropFlags;
        Info.Type = FieldType;
        Info.TypeName = FieldName;

        return Info;
    }
    
    template<typename T>
    T* CreateProperty(const eastl::string& Name, const eastl::string& TypeName)
    {
        T* New = new T;
        New->Name = Name;
        New->TypeName = TypeName;
        return New;
    }

    static void CreatePropertyForType(FReflectedStruct* Struct, FReflectedProperty** NewProperty, const FFieldInfo& FieldInfo)
    {
        if (NewProperty == nullptr)
        {
            return;
        }

        if (*NewProperty != nullptr)
        {
            return;
        }
        
        switch (FieldInfo.Flags)
        {
        case EPropertyTypeFlags::UInt8:
            {
                *NewProperty = CreateProperty<FReflectedUInt8Property>(FieldInfo.Name, FieldInfo.TypeName);
            }
            break;
        case EPropertyTypeFlags::UInt16:
            {
                *NewProperty = CreateProperty<FReflectedUInt16Property>(FieldInfo.Name, FieldInfo.TypeName);
            }
            break;
        case EPropertyTypeFlags::UInt32:
            {
                *NewProperty = CreateProperty<FReflectedUInt32Property>(FieldInfo.Name, FieldInfo.TypeName);
            }
            break;
        case EPropertyTypeFlags::UInt64:
            {
                *NewProperty = CreateProperty<FReflectedUInt64Property>(FieldInfo.Name, FieldInfo.TypeName);
            }
            break;
        case EPropertyTypeFlags::Int8:
            {
                *NewProperty = CreateProperty<FReflectedInt8Property>(FieldInfo.Name, FieldInfo.TypeName);
            }
            break;
        case EPropertyTypeFlags::Int16:
            {
                *NewProperty = CreateProperty<FReflectedInt16Property>(FieldInfo.Name, FieldInfo.TypeName);
            }
            break;
        case EPropertyTypeFlags::Int32:
            {
                *NewProperty = CreateProperty<FReflectedInt32Property>(FieldInfo.Name, FieldInfo.TypeName);
            }
            break;
        case EPropertyTypeFlags::Int64:
            {
                *NewProperty = CreateProperty<FReflectedInt64Property>(FieldInfo.Name, FieldInfo.TypeName);
            }
            break;
        case EPropertyTypeFlags::Float:
            {
                *NewProperty = CreateProperty<FReflectedFloatProperty>(FieldInfo.Name, FieldInfo.TypeName);
            }
            break;
        case EPropertyTypeFlags::Double:
            {
                *NewProperty = CreateProperty<FReflectedDoubleProperty>(FieldInfo.Name, FieldInfo.TypeName);
            }
            break;
        case EPropertyTypeFlags::Enum:
            {
                *NewProperty = CreateProperty<FReflectedEnumProperty>(FieldInfo.Name, FieldInfo.TypeName);
            }
            break;
        case EPropertyTypeFlags::Object:
            {
                const CXType ArgType = clang_Type_getTemplateArgumentAsType(FieldInfo.Type, 0);
                FFieldInfo ParamFieldInfo = CreateSubFieldInfo(ArgType);
                
                *NewProperty = CreateProperty<FReflectedObjectProperty>(FieldInfo.Name, ParamFieldInfo.TypeName);
            }
            break;
        case EPropertyTypeFlags::Vector:
            {
                auto ArrayProperty = CreateProperty<FReflectedArrayProperty>(FieldInfo.Name, FieldInfo.TypeName);
                const CXType ArgType = clang_Type_getTemplateArgumentAsType(FieldInfo.Type, 0);
                FFieldInfo ParamFieldInfo = CreateSubFieldInfo(ArgType);
                ParamFieldInfo.Name = FieldInfo.Name + "_Inner";
                ParamFieldInfo.PropertyFlags.emplace_back("PF_SubField");
                
                FReflectedProperty* FieldProperty = nullptr;
                CreatePropertyForType(Struct, &FieldProperty, ParamFieldInfo);
                FieldProperty->bInner = true; // This property "belongs" to the array.
                *NewProperty = ArrayProperty;
            }
            break;
        default:
            {
                
            }
            break;
        }

        if (*NewProperty != nullptr)
        {
            Struct->PushProperty(*NewProperty);
        }
    }
    
    static CXChildVisitResult VisitStructureContents(CXCursor Cursor, CXCursor parent, CXClientData pClientData)
    {
        FClangParserContext* Context = (FClangParserContext*)pClientData;
        eastl::string CursorName = ClangUtils::GetCursorDisplayName(Cursor);
        eastl::string ParentCursorName = ClangUtils::GetCursorDisplayName(parent);

        CXCursorKind Kind = clang_getCursorKind(Cursor);

        FReflectedStruct* Struct = Context->GetParentReflectedType<FReflectedStruct>();
        
        switch (Kind)
        {
        case(CXCursor_CXXBaseSpecifier):
            {
                Struct->Parent = CursorName;
            }
            break;
        case(CXCursor_FieldDecl):
            {
                FReflectionMacro Macro;
                if(!Context->TryFindMacroForCursor(Context->ReflectedHeader.HeaderPath, Cursor, Macro))
                {
                    return CXChildVisit_Continue;
                }

                FFieldInfo FieldInfo = CreateFieldInfo(Cursor);

                if (!Context->ReflectionDatabase.IsTypeRegistered(FStringHash(FieldInfo.TypeName)))
                {
                    std::cout << "Type not yet registered: " << FieldInfo.TypeName.c_str() << "\n";
                }
                
                FReflectedProperty* NewProperty = nullptr;
                CreatePropertyForType(Struct, &NewProperty, FieldInfo);
                
            }
            break;
        }
        
        return CXChildVisit_Continue;

    }

    static CXChildVisitResult VisitClassContents(CXCursor Cursor, CXCursor parent, CXClientData pClientData)
    {
        FClangParserContext* Context = (FClangParserContext*)pClientData;
        eastl::string CursorName = ClangUtils::GetCursorDisplayName(Cursor);
        eastl::string ParentCursorName = ClangUtils::GetCursorDisplayName(parent);

        CXCursorKind Kind = clang_getCursorKind(Cursor);

        FReflectedClass* Class = Context->GetParentReflectedType<FReflectedClass>();
        
        switch (Kind)
        {
        case(CXCursor_CXXBaseSpecifier):
            {
                Class->Parent = CursorName;
            }
            break;
        case(CXCursor_FieldDecl):
            {
                
                FReflectionMacro Macro;
                if(!Context->TryFindMacroForCursor(Context->ReflectedHeader.HeaderPath, Cursor, Macro))
                {
                    return CXChildVisit_Continue;
                }

                FFieldInfo FieldInfo = CreateFieldInfo(Cursor);
                FReflectedProperty* NewProperty = nullptr;
                CreatePropertyForType(Class, &NewProperty, FieldInfo);
                
            }
            break;
        }
        
        return CXChildVisit_Continue;
    }
    
    CXChildVisitResult VisitStructure(CXCursor Cursor, CXCursor Parent, FClangParserContext* Context)
    {
        eastl::string CursorName = ClangUtils::GetCursorDisplayName(Cursor);

        eastl::string FullyQualifiedCursorName;
        CXType Type = clang_getCursorType(Cursor);
        void* Data = Type.data[0];
        
        if (!ClangUtils::GetQualifiedNameForType(clang::QualType::getFromOpaquePtr(Data), FullyQualifiedCursorName))
        {
            return CXChildVisit_Break;
        }
        
        FReflectionMacro Macro;
        if (!Context->TryFindMacroForCursor(Context->ReflectedHeader.HeaderPath, Cursor, Macro))
        {
            return CXChildVisit_Continue;
        }

        FReflectionMacro GeneratedBody;
        if (!Context->TryFindGeneratedBodyMacro(Context->ReflectedHeader.HeaderPath, Cursor, GeneratedBody))
        {
            return CXChildVisit_Break;
        }

        FReflectedStruct* ReflectedClass = Context->ReflectionDatabase.GetOrCreateReflectedType<FReflectedClass>(FStringHash(CursorName));
        ReflectedClass->Project = Context->Project.Name;
        ReflectedClass->Type = FReflectedType::EType::Structure;
        ReflectedClass->GeneratedBodyLineNumber = GeneratedBody.LineNumber;
        ReflectedClass->LineNumber = ClangUtils::GetCursorLineNumber(Cursor);
        ReflectedClass->HeaderID = Context->ReflectedHeader.HeaderPath;

        Context->LastReflectedType = ReflectedClass;
        
        FReflectedType* PreviousType = Context->ParentReflectedType;
        Context->ParentReflectedType = ReflectedClass;

        if (!Context->bInitialPass)
        {
            clang_visitChildren(Cursor, VisitStructureContents, Context);
        }
        
        Context->ParentReflectedType = PreviousType;
        Context->ReflectionDatabase.AddReflectedType(ReflectedClass);
        
        return CXChildVisit_Recurse;
    }

    CXChildVisitResult VisitClass(CXCursor Cursor, CXCursor Parent, FClangParserContext* Context)
    {
        eastl::string CursorName = ClangUtils::GetCursorDisplayName(Cursor);

        eastl::string FullyQualifiedCursorName;
        CXType Type = clang_getCursorType(Cursor);
        void* Data = Type.data[0];
        
        if (!ClangUtils::GetQualifiedNameForType(clang::QualType::getFromOpaquePtr(Data), FullyQualifiedCursorName))
        {
            return CXChildVisit_Break;
        }
        
        FReflectionMacro Macro;
        if (!Context->TryFindMacroForCursor(Context->ReflectedHeader.HeaderPath, Cursor, Macro))
        {
            return CXChildVisit_Continue;
        }

        FReflectionMacro GeneratedBody;
        if (!Context->TryFindGeneratedBodyMacro(Context->ReflectedHeader.HeaderPath, Cursor, GeneratedBody))
        {
            return CXChildVisit_Break;
        }
        
        FReflectedClass* ReflectedClass = Context->ReflectionDatabase.GetOrCreateReflectedType<FReflectedClass>(FStringHash(CursorName));
        ReflectedClass->Project = Context->Project.Name;
        ReflectedClass->Type = FReflectedType::EType::Class;
        ReflectedClass->GeneratedBodyLineNumber = GeneratedBody.LineNumber;
        ReflectedClass->LineNumber = ClangUtils::GetCursorLineNumber(Cursor);
    
        if (!Context->CurrentNamespace.empty())
        {
            ReflectedClass->QualifiedName = Context->CurrentNamespace + CursorName;
            ReflectedClass->Namespace = Context->CurrentNamespace;
        }
    
        ReflectedClass->HeaderID = Context->ReflectedHeader.HeaderPath;

        FReflectedType* PreviousType = Context->ParentReflectedType;
        Context->ParentReflectedType = ReflectedClass;
        Context->LastReflectedType = ReflectedClass;

        if (!Context->bInitialPass)
        {
            clang_visitChildren(Cursor, VisitClassContents, Context);
        }
        
        Context->ParentReflectedType = PreviousType;
        Context->ReflectionDatabase.AddReflectedType(ReflectedClass);
        
        return CXChildVisit_Recurse;
    }
}
