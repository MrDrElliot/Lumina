#include "ClangVisitor_Structure.h"

#include "Reflector/Clang/ClangParserContext.h"
#include "Reflector/Clang/Utils.h"
#include "Reflector/ReflectionCore/ReflectionMacro.h"

namespace Lumina::Reflection::Visitor
{
    static CXChildVisitResult VisitStructureContents(CXCursor Cursor, CXCursor parent, CXClientData pClientData)
    {
        FString CursorName = ClangUtils::GetCursorDisplayName(Cursor);
        LOG_INFO("Cursor Name Within Structure: {0}", CursorName);
        
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

        switch (Type)
        {
            case (CXCursor_ClassDecl):
                {
                    clang_visitChildren(Cursor, VisitStructureContents, Context);
                }
            break;

            case (CXCursor_StructDecl):
                {
                    clang_visitChildren(Cursor, VisitStructureContents, Context);
                }
            break;
        }
        
        
        return CXChildVisit_Recurse;
    }
}
