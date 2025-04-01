#include "ReflectionMacro.h"

#include "Clang/Utils.h"

namespace Lumina::Reflection
{
    FReflectionMacro::FReflectionMacro(FReflectedHeader* ReflectedHeader, CXCursor Cursor, CXSourceRange Range, EReflectionMacro InType)
        : HeaderID(ReflectedHeader->HeaderID)
        , Type(InType)
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
        clang_disposeTokens( translationUnit, tokens, numTokens );

        size_t const startIdx = MacroContents.find_first_of("(");
        size_t const endIdx = MacroContents.find_last_of( ')');
        if ( startIdx != FString::npos && endIdx != FString::npos && endIdx > startIdx )
        {
            
        }
        else
        {
            MacroContents.clear();
        }
    }
}
