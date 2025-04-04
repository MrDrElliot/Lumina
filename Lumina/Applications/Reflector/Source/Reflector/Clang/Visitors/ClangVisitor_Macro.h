#pragma once
#include "ClangVisitor_Structure.h"

namespace Lumina::Reflection
{
    class FReflectedHeader;
}

namespace Lumina::Reflection::Visitor
{
    CXChildVisitResult VisitMacro(CXCursor Cursor, FReflectedHeader* Header, FClangParserContext* Context);
}
