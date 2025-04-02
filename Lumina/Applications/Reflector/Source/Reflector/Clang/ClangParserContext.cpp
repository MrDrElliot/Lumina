#include "ClangParserContext.h"

namespace Lumina::Reflection
{
    void FClangParserContext::AddReflectedMacro(const FReflectionMacro& Macro)
    {
        TVector<FReflectionMacro>& Macros = ReflectionMacros[Macro.HeaderID];
        Macros.push_back(Macro);
    }

    bool FClangParserContext::GetMacroForType(FName HeaderID, const CXCursor& Cursor, FReflectionMacro& Macro)
    {
        auto headerIter = ReflectionMacros.find(HeaderID);
        if (headerIter == ReflectionMacros.end())
        {
            return false;
        }

        TVector<FReflectionMacro>& macrosForHeader = headerIter->second;

        CXSourceRange typeRange = clang_getCursorExtent(Cursor);
    
        uint32 enumLineStart, enumColumnStart;
        clang_getSpellingLocation(clang_getRangeStart(typeRange), nullptr, &enumLineStart, &enumColumnStart, nullptr);

        for (auto iter = macrosForHeader.begin(); iter != macrosForHeader.end(); ++iter)
        {
            uint32 MacroLineStart = iter->LineNumber;
            
            bool bValidMacro = (MacroLineStart < enumLineStart) && (enumLineStart - MacroLineStart <= 1);
        
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

    void FClangParserContext::Popnamespace()
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
