#include "ClangVisitor_Structure.h"

#include "Reflector/Clang/ClangParserContext.h"
#include "Reflector/Clang/Utils.h"
#include "Reflector/ReflectionCore/ReflectionMacro.h"

namespace Lumina::Reflection::Visitor
{
    CXChildVisitResult VisitStructure(CXCursor Cursor, CXCursor Parent, FClangParserContext* Context)
    {
        FString CursorName = ClangUtils::GetCursorDisplayName(Cursor);

        FString FullyQualifiedCursorName;
        void* Data = clang_getCursorType(Cursor).data[0];
        
        if (!ClangUtils::GetQualifiedNameForType(clang::QualType::getFromOpaquePtr(Data), FullyQualifiedCursorName))
        {
            LOG_ERROR("Failed to get fully qualified name for cursor");
            return CXChildVisit_Break;
        }
        
        FReflectionMacro Macro;
        if (Context->TryFindMacroForCursor(Context->ReflectedHeader.HeaderID, Cursor, Macro))
        {
            LOG_INFO("Found reflected structure: {0}", CursorName);
        }
        
        return CXChildVisit_Recurse;
    }
}
