#pragma once
#include "ClangVisitor_Class.h"

namespace Lumina::Reflection::Visitor
{
    CXChildVisitResult VisitEnum(CXCursor Cursor, CXCursor Parent, FClangParserContext* Context);
}
