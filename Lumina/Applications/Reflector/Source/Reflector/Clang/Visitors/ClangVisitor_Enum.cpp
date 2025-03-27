#include "ClangVisitor_Enum.h"

#include "Containers/String.h"
#include "Reflector/Clang/Utils.h"

namespace Lumina::Reflection::Visitor
{
    CXChildVisitResult VisitEnum(CXCursor Cursor, CXCursor Parent, FClangParserContext* Context)
    {
        FString CursorName = ClangUtils::GetCursorDisplayName(Cursor);

        FString FullyQualifiedName;
        if (!ClangUtils::GetQualifiedNameForType(clang::QualType::getFromOpaquePtr(clang_getCursorType(Cursor).data[0]), FullyQualifiedName))
        {
            return CXChildVisit_Break;
        }

        const clang::EnumDecl* pEnumDecl = (const clang::EnumDecl*) Cursor.data[0];
        clang::QualType integerType = pEnumDecl->getIntegerType();

        if (integerType.isNull())
        {
            std::cerr << "Failed to get underlying integer type \n";
        }

        return CXChildVisit_Continue;

    }
}
