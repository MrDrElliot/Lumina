#include "ClangTranslationUnit.h"

#include <iostream>

#include "ClangVisitor_Class.h"
#include "ClangVisitor_Enum.h"
#include "ClangVisitor_Macro.h"
#include "Containers/String.h"
#include "Reflector/Clang/ClangParserContext.h"
#include "Reflector/Clang/Utils.h"

namespace Lumina::Reflection
{
    CXChildVisitResult VisitTranslationUnit(CXCursor Cursor, CXCursor Parent, CXClientData ClientData)
    {
        FClangParserContext* ParserContext = (FClangParserContext*)ClientData;
        CXCursorKind CursorKind = clang_getCursorKind(Cursor);
        FString CursorName = ClangUtils::GetCursorDisplayName(Cursor);

        
        std::filesystem::path HeaderPath = ClangUtils::GetHeaderPathForCursor(Cursor);
        if (!exists(HeaderPath))
        {
            return CXChildVisit_Continue;
        }
        
        switch (CursorKind)
        {
            case (CXCursor_MacroExpansion):
                {
                    return Visitor::VisitMacro(Cursor, &ParserContext->ReflectedHeader, ParserContext);
                }
            
            case(CXCursor_ClassDecl):
            case(CXCursor_StructDecl):
                {
                    return Visitor::VisitClass(Cursor, Parent, ParserContext);
                }

            case(CXCursor_EnumDecl):
                {
                    return Visitor::VisitEnum(Cursor, Parent, ParserContext);
                }

            case(CXCursor_Namespace):
                {
                    clang_visitChildren(Cursor, VisitTranslationUnit, ClientData);
                }
            
            default: return CXChildVisit_Continue;
        }
    }
}
