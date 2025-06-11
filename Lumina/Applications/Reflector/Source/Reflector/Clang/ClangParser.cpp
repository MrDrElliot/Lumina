#include "ClangParser.h"

#include <fstream>
#include <iostream>
#include <clang-c/Index.h>

#include "EASTL/fixed_vector.h"
#include "Visitors/ClangTranslationUnit.h"


#define OPTIMIZE_HEADER_WRITES 0

namespace Lumina::Reflection
{

    static char const* const GIncludePaths[] =
    {
        "/Lumina/Engine",
        "/Lumina/Engine/Source",
        "/Lumina/Engine/Source/Runtime",
        "/Lumina/Engine/ThirdParty",
        "/Lumina/Engine/ThirdParty/spdlog/include",
        "/Lumina/Engine/ThirdParty/EA/EABase/include/common",
        "/Lumina/Engine/ThirdParty/EA/EASTL/include",
    };

    
    FClangParser::FClangParser()
        : ParsingContext()
    {
    }

    bool FClangParser::Parse(const eastl::string& SolutionPath, eastl::vector<FReflectedHeader>& Headers, const FReflectedProject& Project)
    {
        ParsingContext.Solution = FProjectSolution(SolutionPath.c_str());
        ParsingContext.Project = Project;

        const eastl::string ProjectReflectionDirectory = ParsingContext.Solution.GetParentPath() + "/Intermediates/Reflection/" + Project.Name;
        const eastl::string AmalgamationPath = ProjectReflectionDirectory + "/ReflectHeaders.h";
        
        std::ofstream AmalgamationFile(AmalgamationPath.c_str());
        AmalgamationFile << "#pragma once\n\n";

#if OPTIMIZE_HEADER_WRITES
        
        for (const FReflectedHeader& Header : Headers)
        {
            const eastl::string ReflectionCounterpart = ProjectReflectionDirectory + "/" + Header.FileName + ".generated.h";

            bool bNeedsReflection = false;

            if (!std::filesystem::exists(ReflectionCounterpart.c_str()))
            {
                bNeedsReflection = true;
            }
            else
            {
                auto HeaderTime = std::filesystem::last_write_time(Header.HeaderPath.c_str());
                auto GeneratedTime = std::filesystem::last_write_time(ReflectionCounterpart.c_str());

                if (HeaderTime > GeneratedTime)
                {
                    bNeedsReflection = true;
                }
            }

            
            if (bNeedsReflection)
            {
                AmalgamationFile << "#include \"" << Header.HeaderPath.c_str() << "\"\n";
                ParsingContext.NumHeadersReflected++;
            }
            else
            {
                for (auto& DatabaseProject : ParsingContext.ReflectionDatabase.ReflectedProjects)
                {
                    if (DatabaseProject.Name == Project.Name)
                    {
                        auto& headers = DatabaseProject.Headers;
                        headers.erase(
                            eastl::remove_if(headers.begin(), headers.end(), [&](const FReflectedHeader& DbHeader)
                            {
                                return DbHeader.HeaderPath == Header.HeaderPath;
                            }), headers.end()
                        );
                    }
                }
            }
        }
        
#else
        for (const FReflectedHeader& Header : Headers)
        {
            AmalgamationFile << "#include \"" << Header.HeaderPath.c_str() << "\"\n";
            ParsingContext.NumHeadersReflected++;
        }
        
#endif
        AmalgamationFile.close();

        
        eastl::vector<eastl::string> FullIncludePaths;
        eastl::fixed_vector<const char*, 10> clangArgs;

        eastl::string PrjPath = Project.ParentPath + "/Source/";
        FullIncludePaths.push_back("-I" + PrjPath);
        clangArgs.push_back(FullIncludePaths.back().c_str());
        
        for (const char* Path : GIncludePaths)
        {
            eastl::string SlnPath = std::filesystem::path(SolutionPath.c_str()).parent_path().string().c_str();
            eastl::string FullPath = SlnPath + Path;
            FullIncludePaths.push_back("-I" + FullPath);
            clangArgs.push_back(FullIncludePaths.back().c_str());
            if (!std::filesystem::exists(FullPath.c_str()))
            {
                ParsingContext.LogError("Invalid include path: %s", FullPath.c_str());
                return false;
            }
        }

#if 0
        clangArgs.push_back( "-x" );
        clangArgs.push_back( "c++" );
        clangArgs.push_back( "-std=c++20" );
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
#endif

        clangArgs.push_back("-x");
        clangArgs.push_back("c++");
        clangArgs.push_back("-std=c++20");
        clangArgs.push_back( "-O0" );
        
        
        clangArgs.push_back("-D REFLECTION_PARSER");
        clangArgs.push_back("-D NDEBUG");
        
        clangArgs.push_back("-fsyntax-only");
        clangArgs.push_back("-fparse-all-comments");
        clangArgs.push_back("-fms-extensions");
        clangArgs.push_back("-fms-compatibility");

        clangArgs.push_back("-w"); 
        
        clangArgs.push_back("-Wno-multichar");
        clangArgs.push_back("-Wno-deprecated-builtins");
        clangArgs.push_back("-Wno-unknown-warning-option");
        clangArgs.push_back("-Wno-return-type-c-linkage");
        clangArgs.push_back("-Wno-c++98-compat-pedantic");
        clangArgs.push_back("-Wno-gnu-folding-constant");
        clangArgs.push_back("-Wno-vla-extension-static-assert");


        CXTranslationUnit TranslationUnit;

        CXIndex ClangIndex = clang_createIndex(0, 0);
        constexpr uint32_t ClangOptions = CXTranslationUnit_DetailedPreprocessingRecord | CXTranslationUnit_SkipFunctionBodies | CXTranslationUnit_IncludeBriefCommentsInCodeCompletion | CXTranslationUnit_KeepGoing;
        
        CXErrorCode Result = CXError_Failure;
        Result = clang_parseTranslationUnit2(ClangIndex, AmalgamationPath.c_str(), clangArgs.data(), clangArgs.size(), 0, 0, ClangOptions, &TranslationUnit);
        if (Result == CXError_Success)
        {
            CXCursor Cursor = clang_getTranslationUnitCursor(TranslationUnit);
            clang_visitChildren(Cursor, VisitTranslationUnit, &ParsingContext);
        }
        else
        {
            switch (Result)
            {
            case CXError_Failure:
                ParsingContext.LogError("Clang Unknown failure");
                break;

            case CXError_Crashed:
                ParsingContext.LogError("Clang crashed");
                break;

            case CXError_InvalidArguments:
                ParsingContext.LogError("Clang Invalid arguments");
                break;

            case CXError_ASTReadError:
                ParsingContext.LogError("Clang AST read error");
                break;
            }
        }

        clang_disposeTranslationUnit(TranslationUnit);
        clang_disposeIndex(ClangIndex);
        

        return true;
    }
    
}
