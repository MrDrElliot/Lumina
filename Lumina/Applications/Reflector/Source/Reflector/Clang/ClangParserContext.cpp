#include "ClangParserContext.h"

namespace Lumina::Reflection
{

    FClangParserContext::~FClangParserContext()
    {
    }

    void FClangParserContext::AddReflectedMacro(const FReflectionMacro& Macro)
    {
        TVector<FReflectionMacro>& Macros = ReflectionMacros[Macro.HeaderID];
        Macros.push_back(Macro);
    }

    void FClangParserContext::AddGeneratedBodyMacro(const FReflectionMacro& Macro)
    {
        TQueue<FReflectionMacro>& Macros = GeneratedBodyMacros[Macro.HeaderID];
        Macros.push(Macro);
    }

    bool FClangParserContext::TryFindMacroForCursor(FName HeaderID, const CXCursor& Cursor, FReflectionMacro& Macro)
    {
        auto headerIter = ReflectionMacros.find(HeaderID);
        if (headerIter == ReflectionMacros.end())
        {
            return false;
        }

        CXSourceRange typeRange = clang_getCursorExtent(Cursor);
        CXSourceLocation startLoc = clang_getRangeStart(typeRange);

        CXFile cursorFile;
        uint32 cursorLine, cursorColumn;
        clang_getSpellingLocation(startLoc, &cursorFile, &cursorLine, &cursorColumn, nullptr);

        CXString FileName = clang_getFileName(cursorFile);

        if (FileName.data == nullptr)
        {
            LOG_ERROR("Failed to find a filename for cursor");
            return false;
        }

        FString FileNameChar = clang_getCString(FileName);
        FileNameChar.make_lower();

        clang_disposeString(FileName);

        FString PathString = HeaderID.c_str();
        PathString.make_lower();

        if (FileNameChar != PathString)
        {
            return false;
        }

        TVector<FReflectionMacro>& macrosForHeader = headerIter->second;
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

    bool FClangParserContext::TryFindGeneratedBodyMacro(FName HeaderID, const CXCursor& Cursor, FReflectionMacro& Macro)
    {
        auto headerIter = GeneratedBodyMacros.find(HeaderID);
        if (headerIter == GeneratedBodyMacros.end())
        {
            Macro = {};
            return false;
        }
        
        TQueue<FReflectionMacro>& MacrosForHeader = headerIter->second;

        if (MacrosForHeader.empty())
        {
            Macro = {};
            return false;
        }
        
        Macro = MacrosForHeader.front();
        MacrosForHeader.pop();

        return true;
    }


    void FClangParserContext::PushNamespace(const FString& Namespace)
    {
        NamespaceStack.push_back(Namespace);

        CurrentNamespace.clear();
        for (const FString& String : NamespaceStack)
        {
            CurrentNamespace.append(String);
            CurrentNamespace.append( "::" );
        }
    }

    void FClangParserContext::PopNamespace()
    {
        NamespaceStack.pop_back();

        CurrentNamespace.clear();
        for (const FString& String : NamespaceStack)
        {
            CurrentNamespace.append(String);
            CurrentNamespace.append( "::" );
        }
    }
}
