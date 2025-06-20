#include "ReflectionMacro.h"

#include "Reflector/Clang/Utils.h"


namespace Lumina::Reflection
{
    FReflectionMacro::FReflectionMacro(FReflectedHeader* ReflectedHeader, const CXCursor& Cursor, const CXSourceRange& Range, EReflectionMacro InType)
        : Type(InType)
        , HeaderID(ReflectedHeader->HeaderPath)
        , Position(Range.begin_int_data)
    {
        clang_getExpansionLocation(clang_getRangeStart(Range), nullptr, &LineNumber, nullptr, nullptr);
        
        CXToken* tokens = nullptr;
        uint32_t numTokens = 0;
        CXTranslationUnit translationUnit = clang_Cursor_getTranslationUnit(Cursor);
        clang_tokenize(translationUnit, Range, &tokens, &numTokens);
        for (uint32_t n = 0; n < numTokens; n++)
        {
            MacroContents += ClangUtils::GetString(clang_getTokenSpelling(translationUnit, tokens[n]));
        }
        clang_disposeTokens(translationUnit, tokens, numTokens);

        const size_t StartIdx = MacroContents.find_first_of("(");
        const size_t EndIdx = MacroContents.find_last_of(')');
        if (StartIdx != eastl::string::npos && EndIdx != eastl::string::npos && EndIdx > StartIdx)
        {
            MacroContents = MacroContents.substr(StartIdx + 1, EndIdx - StartIdx - 1);
        }
        else
        {
            MacroContents.clear();
        }
    }
}
