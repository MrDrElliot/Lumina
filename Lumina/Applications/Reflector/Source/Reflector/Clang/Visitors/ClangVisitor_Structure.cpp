#include "ClangVisitor_Structure.h"

#include "Reflector/Clang/ClangParserContext.h"
#include "Reflector/Clang/Utils.h"
#include "Reflector/ReflectionCore/ReflectionMacro.h"

namespace Lumina::Reflection::Visitor
{
    static CXChildVisitResult VisitStructureContents(CXCursor Cursor, CXCursor parent, CXClientData pClientData)
    {
        FClangParserContext* Context = (FClangParserContext*)pClientData;
        FString CursorName = ClangUtils::GetCursorDisplayName(Cursor);
        FString ParentCursorName = ClangUtils::GetCursorDisplayName(parent);

        uint32 LineNumber = ClangUtils::GetLineNumberForCursor(Cursor);
        CXCursorKind Kind = clang_getCursorKind(Cursor);

        switch (Kind)
        {

        case CXCursor_CXXBaseSpecifier:
            {
                
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
                FString TypeSpelling = ClangUtils::GetString(clang_getTypeSpelling(FieldType));

                LOG_INFO("Found Reflected Field: {0} - In Structure - {1}", CursorName, ParentCursorName);
                
            }
            break;
        }
        
        return CXChildVisit_Continue;

    }

    static CXChildVisitResult VisitClassContents(CXCursor Cursor, CXCursor parent, CXClientData pClientData)
    {
        FClangParserContext* Context = (FClangParserContext*)pClientData;
        FString CursorName = ClangUtils::GetCursorDisplayName(Cursor);
        
        return CXChildVisit_Continue;
    }
    
    CXChildVisitResult VisitStructure(CXCursor Cursor, CXCursor Parent, FClangParserContext* Context)
    {
        FString CursorName = ClangUtils::GetCursorDisplayName(Cursor);

        FString FullyQualifiedCursorName;
        CXType Type = clang_getCursorType(Cursor);
        void* Data = Type.data[0];
        
        if (!ClangUtils::GetQualifiedNameForType(clang::QualType::getFromOpaquePtr(Data), FullyQualifiedCursorName))
        {
            LOG_ERROR("Failed to get fully qualified name for cursor");
            return CXChildVisit_Break;
        }
        
        FReflectionMacro Macro;
        if (!Context->TryFindMacroForCursor(Context->ReflectedHeader.HeaderID, Cursor, Macro))
        {
            return CXChildVisit_Continue;
        }

        
        FReflectedStruct* ReflectedClass = Context->ReflectionDatabase.CreateReflectedType<FReflectedClass>();
        ReflectedClass->DisplayName = CursorName;
        ReflectedClass->ID = FName(CursorName);
        ReflectedClass->HeaderID = Context->ReflectedHeader.HeaderID;

        FReflectedType* PreviousType = Context->ParentReflectedType;
        Context->ParentReflectedType = ReflectedClass;
        
        clang_visitChildren(Cursor, VisitStructureContents, Context);
        
        Context->ParentReflectedType = PreviousType;
        Context->ReflectionDatabase.AddReflectedType(ReflectedClass);
        
        return CXChildVisit_Recurse;
    }

    CXChildVisitResult VisitClass(CXCursor Cursor, CXCursor Parent, FClangParserContext* Context)
    {
        FString CursorName = ClangUtils::GetCursorDisplayName(Cursor);

        FString FullyQualifiedCursorName;
        CXType Type = clang_getCursorType(Cursor);
        void* Data = Type.data[0];
        
        if (!ClangUtils::GetQualifiedNameForType(clang::QualType::getFromOpaquePtr(Data), FullyQualifiedCursorName))
        {
            LOG_ERROR("Failed to get fully qualified name for cursor");
            return CXChildVisit_Break;
        }
        
        FReflectionMacro Macro;
        if (!Context->TryFindMacroForCursor(Context->ReflectedHeader.HeaderID, Cursor, Macro))
        {
            return CXChildVisit_Continue;
        }

        
        FReflectedClass* ReflectedClass = Context->ReflectionDatabase.CreateReflectedType<FReflectedClass>();
        ReflectedClass->DisplayName = CursorName;
        ReflectedClass->ID = FName(CursorName);
        ReflectedClass->HeaderID = Context->ReflectedHeader.HeaderID;

        FReflectedType* PreviousType = Context->ParentReflectedType;
        Context->ParentReflectedType = ReflectedClass;
        
        clang_visitChildren(Cursor, VisitClassContents, Context);
        
        Context->ParentReflectedType = PreviousType;
        Context->ReflectionDatabase.AddReflectedType(ReflectedClass);
        
        return CXChildVisit_Recurse;
    }
}
