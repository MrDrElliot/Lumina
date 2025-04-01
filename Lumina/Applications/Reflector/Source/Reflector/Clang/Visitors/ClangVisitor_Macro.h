#pragma once
#include "ClangVisitor_Class.h"

namespace Lumina::Reflection
{
    class FReflectedHeader;
}

namespace Lumina::Reflection::Visitor
{
    CXChildVisitResult VisitMacro(CXCursor Cursor, FReflectedHeader* Header, FClangParserContext* Context);
}
