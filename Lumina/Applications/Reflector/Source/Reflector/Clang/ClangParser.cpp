#include "ClangParser.h"

#include <iostream>
#include <clang-c/Index.h>

#include "Containers/Array.h"
#include "Visitors/ClangTranslationUnit.h"

namespace Lumina::Reflection
{
    bool FClangParser::Parse(const FReflectedHeader& File)
    {
        std::cout << "Parsing: " << File.HeaderPath.c_str() << "\n";
        ParsingContext.SolutionPath = File;
        
        CXIndex ClangIndex = clang_createIndex(0, 1);
        constexpr uint32 ClangOptions = CXTranslationUnit_DetailedPreprocessingRecord | CXTranslationUnit_SkipFunctionBodies | CXTranslationUnit_IncludeBriefCommentsInCodeCompletion;

        CXTranslationUnit TranslationUnit;
        CXErrorCode Result = CXError_Failure;

        TInlineVector<char const*, 10> clangArgs;

        clangArgs.push_back( "-x" );
        clangArgs.push_back( "c++" );
        clangArgs.push_back( "-std=c++20" );
        clangArgs.push_back( "-O0" );
        clangArgs.push_back( "-D NDEBUG" );
        clangArgs.push_back( "-Werror" );
        clangArgs.push_back( "-Wno-multichar" );
        clangArgs.push_back( "-Wno-deprecated-builtins" );
        clangArgs.push_back( "-fparse-all-comments" );
        clangArgs.push_back( "-fms-extensions" );
        clangArgs.push_back( "-fms-compatibility" );
        clangArgs.push_back( "-Wno-unknown-warning-option" );
        clangArgs.push_back( "-Wno-return-type-c-linkage" );
        clangArgs.push_back( "-Wno-gnu-folding-constant" );
        clangArgs.push_back( "-Wno-vla-extension-static-assert" );


        Result = clang_parseTranslationUnit2(ClangIndex, File.c_str(), clangArgs.data(), clangArgs.size(), 0, 0, ClangOptions, &TranslationUnit);
        if (Result == CXError_Success)
        {
            CXCursor Cursor = clang_getTranslationUnitCursor(TranslationUnit);
            clang_visitChildren(Cursor, VisitTranslationUnit, &ParsingContext);
        }

        clang_disposeTranslationUnit(TranslationUnit);
        clang_disposeIndex(ClangIndex);

        return true;
    }
}
