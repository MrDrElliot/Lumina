#pragma once
#include "ClangParserContext.h"
#include "Containers/String.h"

namespace Lumina::Reflection
{
    class FClangParser
    {
    public:

        FClangParser();

        bool Parse(const FString& SolutionPath, const FReflectedHeader& File, const FReflectedProject& Project);

        FClangParserContext ParsingContext;
    };
}
