#include "ClangTranslationUnit.h"

#include "ClangVisitor_Structure.h"
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
        FString ParentCursorName = ClangUtils::GetCursorDisplayName(Parent);


        // Only parser valid headers under the solution directory.
        std::filesystem::path HeaderPath = ClangUtils::GetHeaderPathForCursor(Cursor);
        std::filesystem::path RelativePath = std::filesystem::relative(HeaderPath, ParserContext->ReflectedHeader.HeaderPath.c_str());
        if (!std::filesystem::exists(HeaderPath) || (RelativePath.empty() || RelativePath.string()[0] != '.'))
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
                {
                    ParserContext->PushNamespace(CursorName);
                    clang_visitChildren(Cursor, VisitTranslationUnit, ClientData);
                    ParserContext->PopNamespace();
                    
                    return Visitor::VisitClass(Cursor, Parent, ParserContext);
                }
            
            case(CXCursor_StructDecl):
                {
                    ParserContext->PushNamespace(CursorName);
                    clang_visitChildren(Cursor, VisitTranslationUnit, ClientData);
                    ParserContext->PopNamespace();
                    
                    return Visitor::VisitStructure(Cursor, Parent, ParserContext);
                }

            case(CXCursor_EnumDecl):
                {
                    return Visitor::VisitEnum(Cursor, Parent, ParserContext);
                }

            case(CXCursor_Namespace):
                {
                    ParserContext->PushNamespace(CursorName);
                    clang_visitChildren(Cursor, VisitTranslationUnit, ClientData);
                    ParserContext->PopNamespace();
                }
            
            return CXChildVisit_Continue;
            
            default:
                {
                    return CXChildVisit_Continue;
                }
        }
    }
}
