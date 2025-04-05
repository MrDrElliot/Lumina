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

    bool FClangParserContext::TryFindMacroForCursor(FName HeaderID, const CXCursor& Cursor, FReflectionMacro& Macro)
    {
        auto headerIter = ReflectionMacros.find(HeaderID);
        if (headerIter == ReflectionMacros.end())
        {
            return false;
        }

        TVector<FReflectionMacro>& macrosForHeader = headerIter->second;

        CXSourceRange typeRange = clang_getCursorExtent(Cursor);
        CXSourceLocation startLoc = clang_getRangeStart(typeRange);

        CXFile cursorFile;
        unsigned cursorLine, cursorColumn;
        clang_getSpellingLocation(startLoc, &cursorFile, &cursorLine, &cursorColumn, nullptr);

        CXString FileName = clang_getFileName(cursorFile);
        FString FileNameChar = clang_getCString(FileName);
        FileNameChar.make_lower();

        clang_disposeString(FileName);

        FString PathString = HeaderID.c_str();
        PathString.make_lower();

        if (FileNameChar != PathString)
        {
            return false;
        }

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
