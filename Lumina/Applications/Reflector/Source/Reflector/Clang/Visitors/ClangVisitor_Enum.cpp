#include "ClangVisitor_Enum.h"

#include "Containers/String.h"
#include "Memory/Memory.h"
#include "Reflector/Clang/ClangParserContext.h"
#include "Reflector/Clang/Utils.h"
#include "Reflector/Types/ReflectedType.h"

namespace Lumina::Reflection::Visitor
{
    static CXChildVisitResult VisitEnumContents(CXCursor Cursor, CXCursor parent, CXClientData pClientData)
    {
        FClangParserContext* pContext = static_cast<FClangParserContext*>(pClientData);
        FReflectedEnum* Enum = (FReflectedEnum*)pContext->ParentReflectedType;
        
        CXCursorKind kind = clang_getCursorKind(Cursor);
        
        if (kind == CXCursor_EnumConstantDecl)
        {
            FString DisplayName = ClangUtils::GetCursorDisplayName(Cursor);
            const clang::EnumConstantDecl* EnumConstantDecl = (const clang::EnumConstantDecl*)Cursor.data[0];

            const llvm::APSInt& initVal = EnumConstantDecl->getInitVal();
            uint32 Value = (int32)initVal.getExtValue();
            
            FReflectedEnum::FConstant Constant;
            Constant.Label = DisplayName;
            Constant.ID = FName(DisplayName);
            Constant.Value = Value;
            
            const CXString CommentString = clang_Cursor_getBriefCommentText(Cursor);
            if (CommentString.data != nullptr)
            {
                Constant.Description = clang_getCString(CommentString);
            }
            clang_disposeString(CommentString);

            Enum->AddConstant(Constant);
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

        
        FReflectionMacro Macro;
        if(!Context->TryFindMacroForCursor(Context->ReflectedHeader.HeaderID, Cursor, Macro))
        {
            return CXChildVisit_Continue;
        }

        if(Macro.Type != EReflectionMacro::Enum)
        {
            return CXChildVisit_Continue;
        }
        
        const clang::EnumDecl* pEnumDecl = (const clang::EnumDecl*) Cursor.data[0];
        clang::QualType integerType = pEnumDecl->getIntegerType();

        if (integerType.isNull())
        {
            return CXChildVisit_Break;
        }

        FReflectedEnum* Enum = Context->ReflectionDatabase.CreateReflectedType<FReflectedEnum>();
        Enum->DisplayName = CursorName;
        Enum->ID = FName(FullyQualifiedName);
        Enum->HeaderID = Context->ReflectedHeader.HeaderID;

        FReflectedType* PreviousParentType = Context->ParentReflectedType;
        Context->ParentReflectedType = Enum;
        {
            clang_visitChildren(Cursor, VisitEnumContents, Context);
        }
        Context->ParentReflectedType = PreviousParentType;
        
        Context->ReflectionDatabase.AddReflectedType(Enum);
        
        return CXChildVisit_Continue;

    }
}
