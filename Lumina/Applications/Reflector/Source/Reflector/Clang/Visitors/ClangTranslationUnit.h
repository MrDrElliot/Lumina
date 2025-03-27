#pragma once
#include <clang-c/Index.h>


namespace Lumina::Reflection
{
    CXChildVisitResult VisitTranslationUnit(CXCursor Cursor, CXCursor Parent, CXClientData ClientData);
}
