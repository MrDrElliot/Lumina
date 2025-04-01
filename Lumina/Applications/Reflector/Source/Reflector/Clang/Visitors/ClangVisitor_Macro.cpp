#include "ClangVisitor_Macro.h"

#include "Containers/String.h"
#include "Reflector/Clang/ClangParserContext.h"
#include "Reflector/Clang/Utils.h"

#define ADD_REFLECTION_MACRO(Type) \
    Context->AddReflectedMacro(FReflectionMacro(Header, Cursor, Range, EReflectionMacro::Type));

namespace Lumina::Reflection::Visitor
{
    CXChildVisitResult VisitMacro(CXCursor Cursor, FReflectedHeader* Header, FClangParserContext* Context)
    {
        FString CursorName = ClangUtils::GetCursorDisplayName(Cursor);
        CXSourceRange Range = clang_getCursorExtent(Cursor);

        if (CursorName == ReflectionEnumToString(EReflectionMacro::Class))
        {
            ADD_REFLECTION_MACRO(Class)
        }
        else if (CursorName == ReflectionEnumToString(EReflectionMacro::Struct))
        {
            ADD_REFLECTION_MACRO(Struct)
        }
        else if (CursorName == ReflectionEnumToString(EReflectionMacro::Enum))
        {
            ADD_REFLECTION_MACRO(Enum)
        }
        else if (CursorName == ReflectionEnumToString(EReflectionMacro::Field))
        {
            ADD_REFLECTION_MACRO(Field)
        }
        else if (CursorName == ReflectionEnumToString(EReflectionMacro::Method))
        {
            ADD_REFLECTION_MACRO(Method)
        }
        
        return CXChildVisit_Continue;
    }
}

#undef ADD_REFLECTION_MACRO