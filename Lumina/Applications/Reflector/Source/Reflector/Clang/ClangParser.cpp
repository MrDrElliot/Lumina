#include "ClangParser.h"

#include <iostream>
#include <clang-c/Index.h>

#include "Visitors/ClangTranslationUnit.h"

namespace Lumina::Reflection
{
    bool FClangParser::Parse(const FString& File)
    {
        std::cout << "Parsing: " << File.c_str() << "\n";
        
        CXIndex ClangIndex = clang_createIndex(0, 1);
        constexpr uint32 ClangOptions = CXTranslationUnit_DetailedPreprocessingRecord | CXTranslationUnit_SkipFunctionBodies | CXTranslationUnit_IncludeBriefCommentsInCodeCompletion;

        CXTranslationUnit TranslationUnit;
        CXErrorCode Result = CXError_Failure;

        Result = clang_parseTranslationUnit2(ClangIndex, File.c_str(), nullptr, 0, 0, 0, ClangOptions, &TranslationUnit);
        if (Result == CXError_Success)
        {
            CXCursor Cursor = clang_getTranslationUnitCursor(TranslationUnit);
            clang_visitChildren(Cursor, VisitTranslationUnit, &ParsingContext);
        }

        clang_disposeIndex(ClangIndex);

        return true;
    }
}
