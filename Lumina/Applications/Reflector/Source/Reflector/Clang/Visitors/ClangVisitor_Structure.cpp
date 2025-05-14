#include "ClangVisitor_Structure.h"

#include "Reflector/Clang/ClangParserContext.h"
#include "Reflector/Clang/Utils.h"
#include "Reflector/ReflectionCore/ReflectionMacro.h"
#include "Reflector/Types/Properties/ReflectedNumericProperty.h"
#include "Reflector/Utils/StringUtils.h"

namespace Lumina::Reflection::Visitor
{

    static void CreatePropertyForType(FReflectedStruct* Struct, EPropertyTypeFlags Flags, const eastl::string& Name)
    {
        switch (Flags)
        {
        case EPropertyTypeFlags::UInt8:
            {
                auto* Prop = Struct->PushProperty<FReflectedUInt8Property>();
                Prop->Name = Name;
                Prop->Outer = Struct->DisplayName;
            }
            break;
        case EPropertyTypeFlags::UInt16:
            {
                auto* Prop = Struct->PushProperty<FReflectedUInt16Property>();
                Prop->Name = Name;
                Prop->Outer = Struct->DisplayName;
            }
            break;
        case EPropertyTypeFlags::UInt32:
            {
                auto* Prop = Struct->PushProperty<FReflectedUInt32Property>();
                Prop->Name = Name;
                Prop->Outer = Struct->DisplayName;
            }
            break;
        case EPropertyTypeFlags::UInt64:
            {
                auto* Prop = Struct->PushProperty<FReflectedUInt64Property>();
                Prop->Name = Name;
                Prop->Outer = Struct->DisplayName;
            }
            break;
        case EPropertyTypeFlags::Int8:
            {
                auto* Prop = Struct->PushProperty<FReflectedInt8Property>();
                Prop->Name = Name;
                Prop->Outer = Struct->DisplayName;
            }
            break;
        case EPropertyTypeFlags::Int16:
            {
                auto* Prop = Struct->PushProperty<FReflectedInt16Property>();
                Prop->Name = Name;
                Prop->Outer = Struct->DisplayName;
            }
            break;
        case EPropertyTypeFlags::Int32:
            {
                auto* Prop = Struct->PushProperty<FReflectedInt32Property>();
                Prop->Name = Name;
                Prop->Outer = Struct->DisplayName;
            }
            break;
        case EPropertyTypeFlags::Int64:
            {
                auto* Prop = Struct->PushProperty<FReflectedInt64Property>();
                Prop->Name = Name;
                Prop->Outer = Struct->DisplayName;
            }
            break;
        case EPropertyTypeFlags::Float:
            {
                auto* Prop = Struct->PushProperty<FReflectedFloatProperty>();
                Prop->Name = Name;
                Prop->Outer = Struct->DisplayName;
            }
            break;
        case EPropertyTypeFlags::Double:
            {
                auto* Prop = Struct->PushProperty<FReflectedDoubleProperty>();
                Prop->Name = Name;
                Prop->Outer = Struct->DisplayName;
            }
            break;
        default:
            {
                
            }
            break;
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
                if(!Context->TryFindMacroForCursor(Context->ReflectedHeader.HeaderID, Cursor, Macro))
                {
                    return CXChildVisit_Continue;
                }
                
                CXType FieldType = clang_getCursorType(Cursor);
                clang::QualType FieldQualType = ClangUtils::GetQualType(FieldType);
                eastl::string TypeSpelling = ClangUtils::GetString(clang_getTypeSpelling(FieldType));

                if (FieldQualType->isTemplateTypeParmType())
                {
                    std::cout << "Template\n";
                }
                
                EPropertyTypeFlags PropFlags = GetCoreTypeFromName(TypeSpelling.c_str());
                CreatePropertyForType(Struct, PropFlags, CursorName);
                
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
#if 0
        const clang::CXXBaseSpecifier* pBaseSpecifier = (const clang::CXXBaseSpecifier*)Cursor.data[0];
        if (pBaseSpecifier)
        {
            eastl::string FullyQualifiedName;
            if (!ClangUtils::GetQualifiedNameForType(clang::QualType::getFromOpaquePtr(pBaseSpecifier->getType().getTypePtr()), FullyQualifiedName))
            {
                return CXChildVisit_Break;
            }
        }
#endif    
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
                if(!Context->TryFindMacroForCursor(Context->ReflectedHeader.HeaderID, Cursor, Macro))
                {
                    return CXChildVisit_Continue;
                }
                
                CXType FieldType = clang_getCursorType(Cursor);
                clang::QualType FieldQualType = ClangUtils::GetQualType(FieldType);
                eastl::string TypeSpelling = ClangUtils::GetString(clang_getTypeSpelling(FieldType));
                EPropertyTypeFlags PropFlags = GetCoreTypeFromName(TypeSpelling.c_str());
                CreatePropertyForType(Class, PropFlags, CursorName);

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
        if (!Context->TryFindMacroForCursor(Context->ReflectedHeader.HeaderID, Cursor, Macro))
        {
            return CXChildVisit_Continue;
        }

        FReflectionMacro GeneratedBody;
        if (!Context->TryFindGeneratedBodyMacro(Context->ReflectedHeader.HeaderID, Cursor, GeneratedBody))
        {
            return CXChildVisit_Break;
        }
        
        FReflectedStruct* ReflectedClass = Context->ReflectionDatabase.CreateReflectedType<FReflectedClass>();
        ReflectedClass->Type = FReflectedType::EType::Structure;
        ReflectedClass->GeneratedBodyLineNumber = GeneratedBody.LineNumber;
        ReflectedClass->LineNumber = ClangUtils::GetCursorLineNumber(Cursor);
        ReflectedClass->DisplayName = CursorName;
        ReflectedClass->ID = eastl::string(CursorName);
        ReflectedClass->HeaderID = Context->ReflectedHeader.HeaderID;

        Context->LastReflectedType = ReflectedClass;
        
        FReflectedType* PreviousType = Context->ParentReflectedType;
        Context->ParentReflectedType = ReflectedClass;

        clang_visitChildren(Cursor, VisitStructureContents, Context);
        
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
        if (!Context->TryFindMacroForCursor(Context->ReflectedHeader.HeaderID, Cursor, Macro))
        {
            return CXChildVisit_Continue;
        }

        FReflectionMacro GeneratedBody;
        if (!Context->TryFindGeneratedBodyMacro(Context->ReflectedHeader.HeaderID, Cursor, GeneratedBody))
        {
            return CXChildVisit_Break;
        }
        
        FReflectedClass* ReflectedClass = Context->ReflectionDatabase.CreateReflectedType<FReflectedClass>();
        ReflectedClass->Type = FReflectedType::EType::Class;
        ReflectedClass->GeneratedBodyLineNumber = GeneratedBody.LineNumber;
        ReflectedClass->LineNumber = ClangUtils::GetCursorLineNumber(Cursor);
    
        if (!Context->CurrentNamespace.empty())
        {
            ReflectedClass->QualifiedName = Context->CurrentNamespace + CursorName;
            ReflectedClass->Namespace = Context->CurrentNamespace;
        }
    
        ReflectedClass->DisplayName = CursorName;
        ReflectedClass->ID = eastl::string(CursorName);
        ReflectedClass->HeaderID = Context->ReflectedHeader.HeaderID;

        FReflectedType* PreviousType = Context->ParentReflectedType;
        Context->ParentReflectedType = ReflectedClass;
        Context->LastReflectedType = ReflectedClass;
        
        clang_visitChildren(Cursor, VisitClassContents, Context);
        
        Context->ParentReflectedType = PreviousType;
        Context->ReflectionDatabase.AddReflectedType(ReflectedClass);
        
        return CXChildVisit_Recurse;
    }
}
