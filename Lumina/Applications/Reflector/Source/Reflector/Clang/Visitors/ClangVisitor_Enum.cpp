#include "ClangVisitor_Enum.h"

#include "Containers/String.h"
#include "Reflector/Clang/Utils.h"

namespace Lumina::Reflection::Visitor
{
    static CXChildVisitResult VisitEnumContents(CXCursor Cursor, CXCursor parent, CXClientData pClientData)
    {
        FClangParserContext* pContext = static_cast<FClangParserContext*>(pClientData);

        CXCursorKind kind = clang_getCursorKind(Cursor);
        if (kind == CXCursor_EnumConstantDecl)
        {
            clang::EnumConstantDecl* EnumConstantDecl = (clang::EnumConstantDecl*)Cursor.data[0];

            const auto& initVal = EnumConstantDecl->getInitVal();
            uint32 Value = (int32) initVal.getExtValue();
            
            std::cout << "Name: " << ClangUtils::GetCursorDisplayName(Cursor).c_str() << " Value: " << Value << "\n";
        }

        return CXChildVisit_Continue;
    }
    
    CXChildVisitResult VisitEnum(CXCursor Cursor, CXCursor Parent, FClangParserContext* Context)
    {
        FString CursorName = ClangUtils::GetCursorDisplayName(Cursor);

        FString FullyQualifiedName;
        void* Data = clang_getCursorType(Cursor).data[0];
        if(Data == nullptr)
        {
            return CXChildVisit_Break;
        }
        
        if (!ClangUtils::GetQualifiedNameForType(clang::QualType::getFromOpaquePtr(Data), FullyQualifiedName))
        {
            return CXChildVisit_Break;
        }

        const clang::EnumDecl* pEnumDecl = (const clang::EnumDecl*) Cursor.data[0];
        clang::QualType integerType = pEnumDecl->getIntegerType();

        if (integerType.isNull())
        {
            std::cerr << "Failed to get underlying integer type \n";
            return CXChildVisit_Break;
        }

        clang_visitChildren(Cursor, VisitEnumContents, Context);
        
        return CXChildVisit_Continue;

    }
}
