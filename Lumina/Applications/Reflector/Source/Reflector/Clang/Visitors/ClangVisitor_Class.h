#pragma once
#include <clang-c/Index.h>

namespace Lumina::Reflection
{
    class FClangParserContext;
}

namespace Lumina::Reflection::Visitor
{

    
    CXChildVisitResult VisitClass(CXCursor Cursor, CXCursor Parent, FClangParserContext* Context);
}
