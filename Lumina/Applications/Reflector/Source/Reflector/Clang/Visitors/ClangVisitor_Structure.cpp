#include "ClangVisitor_Structure.h"

#include "EASTL/shared_ptr.h"
#include "Reflector/Clang/ClangParserContext.h"
#include "Reflector/Clang/Utils.h"
#include "Reflector/ReflectionCore/ReflectionMacro.h"
#include "Reflector/Types/FieldInfo.h"
#include "Reflector/Types/Properties/ReflectedArrayProperty.h"
#include "Reflector/Types/Properties/ReflectedEnumProperty.h"
#include "Reflector/Types/Properties/ReflectedNumericProperty.h"
#include "Reflector/Types/Properties/ReflectedObjectProperty.h"
#include "Reflector/Types/Properties/ReflectedStringProperty.h"
#include "Reflector/Types/Properties/ReflectedStructProperty.h"

namespace Lumina::Reflection::Visitor
{

    static FFieldInfo CreateFieldInfo(FClangParserContext* Context, const CXCursor& Cursor)
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
            // Try finding it with the current namespace scope.
            FStringHash TestHash = FStringHash(Context->CurrentNamespace + "::" + TypeSpelling);
            if (Context->ReflectionDatabase.IsTypeRegistered(TestHash))
            {
                TypeSpelling = (Context->CurrentNamespace + "::" + TypeSpelling);
            }
            
            if (FieldQualType->isEnumeralType())
            {
                PropFlags = EPropertyTypeFlags::Enum;
            }
            else if (FieldQualType->isStructureType())
            {
                PropFlags = EPropertyTypeFlags::Struct;
            }
            else if (FieldQualType->isPointerType())
            {
                Context->LogError("[%s] Object properties as pointers are *NOT* supported, use TObjectHandle instead", CursorName.c_str());
                Context->FlushLogs();
                return FFieldInfo();
            }
        }

        FFieldInfo Info;
        Info.Flags = PropFlags;
        Info.Type = FieldType;
        Info.Name = CursorName;
        Info.TypeName = TypeSpelling;
        Info.RawFieldType = FieldQualType.getAsString().c_str();

        return Info;
    }

    static FFieldInfo CreateSubFieldInfo(FClangParserContext* Context, const CXType& FieldType, const FFieldInfo& ParentField)
    {
        clang::QualType FieldQualType = ClangUtils::GetQualType(FieldType);
        
        eastl::string FieldName; 
        ClangUtils::GetQualifiedNameForType(FieldQualType, FieldName);
        
        EPropertyTypeFlags PropFlags = GetCoreTypeFromName(FieldName.c_str());

        // Is not a core type.
        if (PropFlags == EPropertyTypeFlags::None)
        {
            // Try finding it with the current namespace scope.
            FStringHash TestHash = FStringHash(Context->CurrentNamespace + "::" + FieldName);
            if (Context->ReflectionDatabase.IsTypeRegistered(TestHash))
            {
                FieldName = (Context->CurrentNamespace + "::" + FieldName);
            }

            if (FieldQualType->isEnumeralType())
            {
                PropFlags = EPropertyTypeFlags::Enum;
            }
            else if (FieldQualType->isStructureType())
            {
                PropFlags = EPropertyTypeFlags::Struct;
            }
            else if (FieldQualType->isPointerType())
            {
                Context->LogError("[%s] Object properties as pointers are *NOT* supported, use TObjectHandle instead", FieldName.c_str());
                Context->FlushLogs();
            }
        }
        
        FFieldInfo Info;
        Info.Flags = PropFlags;
        Info.Type = FieldType;
        Info.TypeName = FieldName;
        Info.RawFieldType = ParentField.RawFieldType;
        return Info;
    }
    
    template<typename T>
    eastl::shared_ptr<T> CreateProperty(const FFieldInfo& Info)
    {
        eastl::shared_ptr<T> New = eastl::make_shared<T>();
        New->Name = Info.Name;
        New->TypeName = Info.TypeName;
        New->RawTypeName = Info.RawFieldType;
        return New;
    }

    static bool CreatePropertyForType(FClangParserContext* Context, FReflectedStruct* Struct, eastl::shared_ptr<FReflectedProperty>& NewProperty, const FFieldInfo& FieldInfo)
    {
        switch (FieldInfo.Flags)
        {
        case EPropertyTypeFlags::UInt8:
            {
                NewProperty = CreateProperty<FReflectedUInt8Property>(FieldInfo);
            }
            break;
        case EPropertyTypeFlags::UInt16:
            {
                NewProperty = CreateProperty<FReflectedUInt16Property>(FieldInfo);
            }
            break;
        case EPropertyTypeFlags::UInt32:
            {
                NewProperty = CreateProperty<FReflectedUInt32Property>(FieldInfo);
            }
            break;
        case EPropertyTypeFlags::UInt64:
            {
                NewProperty = CreateProperty<FReflectedUInt64Property>(FieldInfo);
            }
            break;
        case EPropertyTypeFlags::Int8:
            {
                NewProperty = CreateProperty<FReflectedInt8Property>(FieldInfo);
            }
            break;
        case EPropertyTypeFlags::Int16:
            {
                NewProperty = CreateProperty<FReflectedInt16Property>(FieldInfo);
            }
            break;
        case EPropertyTypeFlags::Int32:
            {
                NewProperty = CreateProperty<FReflectedInt32Property>(FieldInfo);
            }
            break;
        case EPropertyTypeFlags::Int64:
            {
                NewProperty = CreateProperty<FReflectedInt64Property>(FieldInfo);
            }
            break;
        case EPropertyTypeFlags::Float:
            {
                NewProperty = CreateProperty<FReflectedFloatProperty>(FieldInfo);
            }
            break;
        case EPropertyTypeFlags::Double:
            {
                NewProperty = CreateProperty<FReflectedDoubleProperty>(FieldInfo);
            }
            break;
        case EPropertyTypeFlags::Bool:
            {
                NewProperty = CreateProperty<FReflectedBoolProperty>(FieldInfo);
            }
            break;
        case EPropertyTypeFlags::String:
            {
                NewProperty = CreateProperty<FReflectedStringProperty>(FieldInfo);
            }
            break;
        case EPropertyTypeFlags::Name:
            {
                NewProperty = CreateProperty<FReflectedNameProperty>(FieldInfo);
            }
            break;
        case EPropertyTypeFlags::Struct:
            {
                NewProperty = CreateProperty<FReflectedStructProperty>(FieldInfo);
            }
            break;
        case EPropertyTypeFlags::Enum:
            {
                NewProperty = CreateProperty<FReflectedEnumProperty>(FieldInfo);
                const CXCursor EnumCursor = clang_getTypeDeclaration(FieldInfo.Type);

                if (clang_getCursorKind(EnumCursor) == CXCursor_EnumDecl)
                {
                    CXType UnderlyingType = clang_getEnumDeclIntegerType(EnumCursor);
                    FFieldInfo SubType = CreateSubFieldInfo(Context, UnderlyingType, FieldInfo);
                    if (!SubType.Validate(Context))
                    {
                        return false;
                    }

                    SubType.Name = FieldInfo.Name + "_Inner";
                    SubType.PropertyFlags.emplace_back("PF_SubField");

                    eastl::shared_ptr<FReflectedProperty> FieldProperty;
                    CreatePropertyForType(Context, Struct, FieldProperty, SubType);
                    FieldProperty->bInner = true;
                }
            }
            break;
        case EPropertyTypeFlags::Object:
            {
                const CXType ArgType = clang_Type_getTemplateArgumentAsType(FieldInfo.Type, 0);
                FFieldInfo ParamFieldInfo = CreateSubFieldInfo(Context, ArgType, FieldInfo);
                ParamFieldInfo.Name = FieldInfo.Name; // Replace the empty template property name with the parent.
                if (!ParamFieldInfo.Validate(Context))
                {
                    return false;
                }
                
                NewProperty = CreateProperty<FReflectedObjectProperty>(ParamFieldInfo);
            }
            break;
        case EPropertyTypeFlags::Vector:
            {
                auto ArrayProperty = CreateProperty<FReflectedArrayProperty>(FieldInfo);
                NewProperty = ArrayProperty;

                const CXType ArgType = clang_Type_getTemplateArgumentAsType(FieldInfo.Type, 0);
                FFieldInfo ParamFieldInfo = CreateSubFieldInfo(Context, ArgType, FieldInfo);
                if (!ParamFieldInfo.Validate(Context))
                {
                    return false;
                }
                
                ParamFieldInfo.Name = FieldInfo.Name + "_Inner";
                ParamFieldInfo.PropertyFlags.emplace_back("PF_SubField");
                
                eastl::shared_ptr<FReflectedProperty> FieldProperty;
                CreatePropertyForType(Context, Struct, FieldProperty, ParamFieldInfo);
                if (FieldProperty == nullptr)
                {
                    Context->LogError("Failed to create property for array. %s [%s]", FieldInfo.Name.c_str(), ParamFieldInfo.Name.c_str());
                    Context->FlushLogs();
                    return false;
                }

                ArrayProperty->ElementTypeName = clang_getCString(clang_getTypeSpelling(ArgType));
                
                FieldProperty->bInner = true; // This property "belongs" to the array.
            }
            break;
        default:
            {
                
            }
            break;
        }

        if (NewProperty != nullptr)
        {
            Struct->PushProperty(NewProperty);
        }

        return NewProperty != nullptr;
        
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

                FFieldInfo FieldInfo = CreateFieldInfo(Context, Cursor);
                if (!FieldInfo.Validate(Context))
                {
                    return CXChildVisit_Continue;
                }
                
                eastl::shared_ptr<FReflectedProperty> NewProperty;
                CreatePropertyForType(Context, Struct, NewProperty, FieldInfo);
                NewProperty->GenerateMetadata(Macro.MacroContents);
                
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
                
                FFieldInfo FieldInfo = CreateFieldInfo(Context, Cursor);
                if (!FieldInfo.Validate(Context))
                {
                    return CXChildVisit_Continue;
                }
                
                eastl::shared_ptr<FReflectedProperty> NewProperty;
                CreatePropertyForType(Context, Class, NewProperty, FieldInfo);
                NewProperty->GenerateMetadata(Macro.MacroContents);

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
        
        FReflectedStruct* ReflectedStruct = Context->ReflectionDatabase.GetOrCreateReflectedType<FReflectedStruct>(FStringHash(FullyQualifiedCursorName));
        ReflectedStruct->DisplayName = CursorName;
        ReflectedStruct->Project = Context->Project.Name;
        ReflectedStruct->Type = FReflectedType::EType::Structure;
        ReflectedStruct->GeneratedBodyLineNumber = GeneratedBody.LineNumber;
        ReflectedStruct->LineNumber = ClangUtils::GetCursorLineNumber(Cursor);
        ReflectedStruct->HeaderID = Context->ReflectedHeader.HeaderPath;
        ReflectedStruct->GenerateMetadata(Macro.MacroContents);
        
        if (!Context->CurrentNamespace.empty())
        {
            ReflectedStruct->Namespace = Context->CurrentNamespace;
        }
        
        Context->LastReflectedType = ReflectedStruct;
        
        FReflectedType* PreviousType = Context->ParentReflectedType;
        Context->ParentReflectedType = ReflectedStruct;
        Context->LastReflectedType = ReflectedStruct;

        if (!Context->bInitialPass)
        {
            clang_visitChildren(Cursor, VisitStructureContents, Context);
        }
        
        Context->ParentReflectedType = PreviousType;
        Context->ReflectionDatabase.AddReflectedType(ReflectedStruct);
        
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
        
        FReflectedClass* ReflectedClass = Context->ReflectionDatabase.GetOrCreateReflectedType<FReflectedClass>(FStringHash(FullyQualifiedCursorName));
        ReflectedClass->DisplayName = CursorName;
        ReflectedClass->Project = Context->Project.Name;
        ReflectedClass->Type = FReflectedType::EType::Class;
        ReflectedClass->GeneratedBodyLineNumber = GeneratedBody.LineNumber;
        ReflectedClass->LineNumber = ClangUtils::GetCursorLineNumber(Cursor);
        ReflectedClass->HeaderID = Context->ReflectedHeader.HeaderPath;
        ReflectedClass->GenerateMetadata(Macro.MacroContents);
        
        if (!Context->CurrentNamespace.empty())
        {
            ReflectedClass->Namespace = Context->CurrentNamespace;
        }
    

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
