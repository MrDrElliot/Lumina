#include "ReflectionMacro.h"

#include "Reflector/Clang/Utils.h"


namespace Lumina::Reflection
{
    FReflectionMacro::FReflectionMacro(FReflectedHeader* ReflectedHeader, const CXCursor& Cursor, const CXSourceRange& Range, EReflectionMacro InType)
        : Type(InType)
        , HeaderID(ReflectedHeader->HeaderID)
        , Position(Range.begin_int_data)
    {
        clang_getExpansionLocation(clang_getRangeStart(Range), nullptr, &LineNumber, nullptr, nullptr);
        
        CXToken* tokens = nullptr;
        uint32 numTokens = 0;
        CXTranslationUnit translationUnit = clang_Cursor_getTranslationUnit(Cursor);
        clang_tokenize(translationUnit, Range, &tokens, &numTokens);
        for (uint32 n = 0; n < numTokens; n++)
        {
            MacroContents += ClangUtils::GetString(clang_getTokenSpelling(translationUnit, tokens[n]));
        }
        clang_disposeTokens(translationUnit, tokens, numTokens);

        const size_t startIdx = MacroContents.find_first_of("(");
        const size_t endIdx = MacroContents.find_last_of(')');
        if (startIdx != FString::npos && endIdx != FString::npos && endIdx > startIdx)
        {
            
        }
        else
        {
            MacroContents.clear();
        }
    }
}
