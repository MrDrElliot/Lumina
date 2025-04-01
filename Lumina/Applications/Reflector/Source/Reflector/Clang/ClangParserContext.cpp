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
        // Try get macros for this header
        //-------------------------------------------------------------------------

        auto headerIter = ReflectionMacros.find(HeaderID);
        if (headerIter == ReflectionMacros.end())
        {
            return false;
        }

        TVector<FReflectionMacro>& macrosForHeader = headerIter->second;

        // Check the header macros
        //-------------------------------------------------------------------------

        const CXSourceRange typeRange = clang_getCursorExtent(Cursor);

        for ( auto iter = macrosForHeader.begin(); iter != macrosForHeader.end(); ++iter )
        {
            bool const macroWithinCursorExtents = iter->Position > typeRange.begin_int_data && iter->Position < typeRange.end_int_data;
            if ( macroWithinCursorExtents )
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
