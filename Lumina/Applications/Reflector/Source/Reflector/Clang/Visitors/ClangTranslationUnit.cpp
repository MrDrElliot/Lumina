#include "ClangTranslationUnit.h"

#include <iostream>

#include "ClangVisitor_Class.h"
#include "ClangVisitor_Enum.h"
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
                std::cout << "Found Macro!: " << CursorName.c_str() << "\n";
            }

            case(CXCursor_ClassDecl):
            {
                Visitor::VisitClass(Cursor, Parent, ParserContext);
            }

            case(CXCursor_EnumDecl):
            {
                Visitor::VisitEnum(Cursor, Parent, ParserContext);
            }
            break;

        default: break;
        }

        return CXChildVisit_Continue;
    }
}
