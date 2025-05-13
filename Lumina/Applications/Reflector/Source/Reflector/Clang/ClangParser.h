#pragma once
#include "ClangParserContext.h"

namespace Lumina::Reflection
{
    class FClangParser
    {
    public:

        FClangParser();

        bool Parse(const eastl::string& SolutionPath, const FReflectedHeader& File, const FReflectedProject& Project);

        FClangParserContext ParsingContext;
    };
}
