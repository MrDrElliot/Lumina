#include "ClangVisitor_Macro.h"

#include "Reflector/Clang/ClangParserContext.h"
#include "Reflector/Clang/Utils.h"

namespace Lumina::Reflection::Visitor
{
    CXChildVisitResult VisitMacro(CXCursor Cursor, FReflectedHeader* Header, FClangParserContext* Context)
    {
        eastl::string CursorName = ClangUtils::GetCursorDisplayName(Cursor);
        CXSourceRange Range = clang_getCursorExtent(Cursor);

        
        if (CursorName == ReflectionEnumToString(EReflectionMacro::Class))
        {
            Context->AddReflectedMacro(FReflectionMacro(Header, Cursor, Range, EReflectionMacro::Class));
        }
        else if (CursorName == ReflectionEnumToString(EReflectionMacro::Struct))
        {
            Context->AddReflectedMacro(FReflectionMacro(Header, Cursor, Range, EReflectionMacro::Struct));

        }
        else if (CursorName == ReflectionEnumToString(EReflectionMacro::Enum))
        {
            Context->AddReflectedMacro(FReflectionMacro(Header, Cursor, Range, EReflectionMacro::Enum));

        }
        else if (CursorName == ReflectionEnumToString(EReflectionMacro::Field))
        {
            Context->AddReflectedMacro(FReflectionMacro(Header, Cursor, Range, EReflectionMacro::Field));

        }
        else if (CursorName == ReflectionEnumToString(EReflectionMacro::Method))
        {
            Context->AddReflectedMacro(FReflectionMacro(Header, Cursor, Range, EReflectionMacro::Method));

        }
        else if (CursorName == ReflectionEnumToString(EReflectionMacro::GeneratedBody))
        {
            Context->AddGeneratedBodyMacro(FReflectionMacro(Header, Cursor, Range, EReflectionMacro::GeneratedBody));
        }
        
        return CXChildVisit_Continue;
    }
}
