#include "ClangTranslationUnit.h"

#include <complex.h>

#include "ClangVisitor_Structure.h"
#include "ClangVisitor_Enum.h"
#include "ClangVisitor_Macro.h"
#include "Reflector/Clang/ClangParserContext.h"
#include "Reflector/Clang/Utils.h"

namespace Lumina::Reflection
{

    uint64_t GFilesLookedAt;
    
    eastl::hash_map<eastl::string, std::filesystem::path> CanonicalPathCache;
    
    CXChildVisitResult VisitTranslationUnit(CXCursor Cursor, CXCursor Parent, CXClientData ClientData)
    {
        
        GFilesLookedAt++;
        
        FClangParserContext* ParserContext = (FClangParserContext*)ClientData;
        
        eastl::string FilePath = ClangUtils::GetHeaderPathForCursor(Cursor);
        FStringHash Hash = FStringHash(FilePath);

        if (ParserContext->Project.HeaderHashMap.find(Hash) == ParserContext->Project.HeaderHashMap.end())
        {
            return CXChildVisit_Continue;
        }

        ParserContext->ReflectedHeader = ParserContext->Project.HeaderHashMap.at(Hash);

        
        CXCursorKind CursorKind = clang_getCursorKind(Cursor);
        eastl::string CursorName = ClangUtils::GetCursorDisplayName(Cursor);
        eastl::string ParentCursorName = ClangUtils::GetCursorDisplayName(Parent);

        // Get the source location of the cursor
        CXSourceLocation Location = clang_getCursorLocation(Cursor);
        CXFile CursorFile;
        unsigned Line, Column, Offset;
        clang_getSpellingLocation(Location, &CursorFile, &Line, &Column, &Offset);

        
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
