#include "ClangParser.h"

#include <clang-c/Index.h>

#include "Containers/Array.h"
#include "Visitors/ClangTranslationUnit.h"

namespace Lumina::Reflection
{

    static char const* const g_includePaths[] =
    {
        "\\Lumina\\Engine",
        "\\Lumina\\Engine\\Source",
        "\\Lumina\\Engine\\Source\\Runtime",
        "\\Lumina\\Engine\\ThirdParty\\",
        "\\Lumina\\Engine\\ThirdParty\\EA\\EABase\\include\\common\\",
        "\\Lumina\\Engine\\ThirdParty\\EA\\EASTL\\include\\",
    };

    
    FClangParser::FClangParser()
        : ParsingContext()
    {
    }

    bool FClangParser::Parse(const FString& SolutionPath, const FReflectedHeader& File, const FReflectedProject& Project)
    {
        ParsingContext.Solution = FProjectSolution(SolutionPath.c_str());
        ParsingContext.ReflectedHeader = File;
        ParsingContext.Project = Project;
        
        CXIndex ClangIndex = clang_createIndex(0, 1);
        constexpr uint32 ClangOptions = CXTranslationUnit_DetailedPreprocessingRecord | CXTranslationUnit_SkipFunctionBodies | CXTranslationUnit_IncludeBriefCommentsInCodeCompletion;
        
        TVector<FString> FullIncludePaths;
        TFixedVector<const char*, 10> clangArgs;
        
        for (const char* Path : g_includePaths)
        {
            FString SlnPath = std::filesystem::path(SolutionPath.c_str()).parent_path().string().c_str();
            FString FullPath = SlnPath + Path;
            FullIncludePaths.push_back("-I" + FullPath);
            clangArgs.push_back(FullIncludePaths.back().c_str());

            if (!std::filesystem::exists(FullPath.c_str()))
            {
                LOG_ERROR("Failed to find include path: {0}", FullPath);
                return false;
            }
        }
        
        clangArgs.push_back( "-x" );
        clangArgs.push_back( "c++" );
        clangArgs.push_back( "-std=c++17" );
        clangArgs.push_back( "-O0" );
        clangArgs.push_back( "-D" );
        clangArgs.push_back( "NDEBUG" );
        clangArgs.push_back( "-D" );
        clangArgs.push_back( "REFLECTION_PARSER" );
        clangArgs.push_back( "-Wno-return-type-c-linkage" );
        clangArgs.push_back( "-Wno-unknown-warning-option" );
        clangArgs.push_back( "-Wno-c++98-compat-pedantic" );
        clangArgs.push_back( "-Wno-gnu-folding-constant" );
        clangArgs.push_back( "-Wno-vla-extension-static-assert" );
        clangArgs.push_back( "-fparse-all-comments" );
        clangArgs.push_back( "-fms-extensions" );
        clangArgs.push_back( "-fms-compatibility" );
        clangArgs.push_back( "-fretain-comments-from-system-headers" );
        clangArgs.push_back( "-fno-spell-checking" );
        clangArgs.push_back( "-fmacro-backtrace-limit=0" );
        clangArgs.push_back("-Xclang");
        clangArgs.push_back("-ast-dump");


        CXTranslationUnit TranslationUnit;
        CXErrorCode Result = CXError_Failure;
        
        Result = clang_parseTranslationUnit2(ClangIndex, File.HeaderPath.c_str(), clangArgs.data(), clangArgs.size(), 0, 0, ClangOptions, &TranslationUnit);
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
