﻿#include "ClangParserContext.h"

#include "xxhash.h"
#include "EASTL/queue.h"

namespace Lumina::Reflection
{

    FClangParserContext::~FClangParserContext()
    {
    }

    void FClangParserContext::AddReflectedMacro(FReflectionMacro&& Macro)
    {
        uint64 Hash = XXH64(Macro.HeaderID.c_str(), strlen(Macro.HeaderID.c_str()), 0);

        eastl::vector<FReflectionMacro>& Macros = ReflectionMacros[Hash];
        Macros.push_back(eastl::move(Macro));
    }

    void FClangParserContext::AddGeneratedBodyMacro(FReflectionMacro&& Macro)
    {
        uint64 Hash = XXH64(Macro.HeaderID.c_str(), strlen(Macro.HeaderID.c_str()), 0);
        
        eastl::queue<FReflectionMacro>& Macros = GeneratedBodyMacros[Hash];
        Macros.push(eastl::move(Macro));
    }

    bool FClangParserContext::TryFindMacroForCursor(eastl::string HeaderID, const CXCursor& Cursor, FReflectionMacro& Macro)
    {
        uint64 Hash = XXH64(HeaderID.c_str(), strlen(HeaderID.c_str()), 0);

        auto headerIter = ReflectionMacros.find(Hash);
        if (headerIter == ReflectionMacros.end())
        {
            return false;
        }

        CXSourceRange typeRange = clang_getCursorExtent(Cursor);
        CXSourceLocation startLoc = clang_getRangeStart(typeRange);

        CXFile cursorFile;
        uint32_t cursorLine, cursorColumn;
        clang_getSpellingLocation(startLoc, &cursorFile, &cursorLine, &cursorColumn, nullptr);

        CXString FileName = clang_getFileName(cursorFile);

        if (FileName.data == nullptr)
        {
            return false;
        }

        eastl::string FileNameChar = clang_getCString(FileName);
        FileNameChar.make_lower();

        clang_disposeString(FileName);

        if (FileNameChar != HeaderID)
        {
            return false;
        }

        eastl::vector<FReflectionMacro>& macrosForHeader = headerIter->second;
        for (auto iter = macrosForHeader.begin(); iter != macrosForHeader.end(); ++iter)
        {
            bool bValidMacro = (iter->LineNumber < cursorLine) && ((cursorLine - iter->LineNumber) <= 1);
        
            if (bValidMacro)
            {
                Macro = *iter;
                macrosForHeader.erase(iter);
                return true;
            }
        }

        return false;
    }

    bool FClangParserContext::TryFindGeneratedBodyMacro(eastl::string HeaderID, const CXCursor& Cursor, FReflectionMacro& Macro)
    {
        uint64 Hash = XXH64(HeaderID.c_str(), strlen(HeaderID.c_str()), 0);
        auto headerIter = GeneratedBodyMacros.find(Hash);
        if (headerIter == GeneratedBodyMacros.end())
        {
            Macro = {};
            return false;
        }

        
        eastl::queue<FReflectionMacro>& MacrosForHeader = headerIter->second;

        if (MacrosForHeader.empty())
        {
            Macro = {};
            return false;
        }
        
        Macro = MacrosForHeader.front();
        
        MacrosForHeader.pop();

        return true;
    }

    void FClangParserContext::LogError(char const* pErrorFormat, ...) const
    {
        char buffer[1024];

        va_list args;
        va_start( args, pErrorFormat );
        vsnprintf(buffer, 1024, pErrorFormat, args);
        va_end( args );

        ErrorMessage = buffer;
    }


    void FClangParserContext::PushNamespace(const eastl::string& Namespace)
    {
        NamespaceStack.push_back(Namespace);

        CurrentNamespace.clear();
        for (const eastl::string& String : NamespaceStack)
        {
            CurrentNamespace.append(String);
        }
    }

    void FClangParserContext::PopNamespace()
    {
        NamespaceStack.pop_back();

        CurrentNamespace.clear();
        for (const eastl::string& String : NamespaceStack)
        {
            CurrentNamespace.append(String);
        }
    }
}
