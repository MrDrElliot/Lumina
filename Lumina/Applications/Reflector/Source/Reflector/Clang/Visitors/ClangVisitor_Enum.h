#pragma once
#include "ClangVisitor_Structure.h"

namespace Lumina::Reflection::Visitor
{
    CXChildVisitResult VisitEnum(CXCursor Cursor, CXCursor Parent, FClangParserContext* Context);
}
