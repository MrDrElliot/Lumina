#pragma once
#include <clang-c/Index.h>

namespace Lumina::Reflection
{
    class FClangParserContext;
}

namespace Lumina::Reflection::Visitor
{
    
    CXChildVisitResult VisitStructure(CXCursor Cursor, CXCursor Parent, FClangParserContext* Context);
    CXChildVisitResult VisitClass(CXCursor Cursor, CXCursor Parent, FClangParserContext* Context);

}
