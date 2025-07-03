#pragma once
#include <filesystem>
#include <iostream>
#include <clang/AST/Ast.h>
#include <clang-c/Index.h>

#include "xxhash.h"


namespace Lumina::ClangUtils
{
    inline eastl::string GetString(const CXString& string)
    {
        eastl::string str = clang_getCString(string);
        clang_disposeString(string);
        return str;
    }

    inline uint32_t GetCursorLineNumber(const CXCursor& Cr)
    {
        uint32_t Line = 0;
        uint32_t Column = 0;
        uint32_t Offset = 0;

        CXSourceLocation Location = clang_getCursorLocation(Cr);
        clang_getSpellingLocation(Location, nullptr, &Line, &Column, &Offset);

        return Line;
    }

    inline eastl::string StripNamespace(const eastl::string& Input)
    {
        size_t Pos = Input.rfind("::");
        if (Pos != eastl::string::npos)
        {
            return Input.substr(Pos + 2); // skip past the last "::"
        }
        return Input; // return unchanged if no "::" found
    }

    inline eastl::string MakeCodeFriendlyNamespace(eastl::string Input)
    {
        const eastl::string from = "::";
        const eastl::string to = "_";

        size_t start_pos = 0;
        while ((start_pos = Input.find(from, start_pos)) != eastl::string::npos)
        {
            Input.replace(start_pos, from.length(), to);
            start_pos += to.length(); // Move past the replacement
        }

        return Input;
    }

    
    inline eastl::string GetCursorDisplayName(const CXCursor& cr)
    {
        CXString displayName = clang_getCursorDisplayName(cr);
        eastl::string str = clang_getCString(displayName);
        clang_disposeString(displayName);
        return str;
    }

    inline eastl::string GetHeaderPathForCursor(CXCursor cr)
    {
        CXFile pFile;
        const CXSourceRange cursorRange = clang_getCursorExtent(cr);
        clang_getExpansionLocation(clang_getRangeStart(cursorRange), &pFile, nullptr, nullptr, nullptr);

        eastl::string HeaderFilePath;
        if (pFile != nullptr)
        {
            CXString clangFilePath = clang_File_tryGetRealPathName(pFile);
            HeaderFilePath = eastl::string(clang_getCString(clangFilePath));
            clang_disposeString(clangFilePath);
        }

        eastl::replace(HeaderFilePath.begin(), HeaderFilePath.end(), '\\', '/');
        HeaderFilePath.make_lower();
        
        return HeaderFilePath;
    }

    inline uint32_t GetLineNumberForCursor(const CXCursor& cr)
    {
        uint32_t line, column, offset;
        CXSourceRange range = clang_getCursorExtent(cr);
        CXSourceLocation start = clang_getRangeStart(range);
        clang_getExpansionLocation( start, nullptr, &line, &column, &offset);
        return line;
    }

    inline clang::QualType GetQualType(CXType type) 
    {
        return clang::QualType::getFromOpaquePtr(type.data[0]); 
    }
    
    inline bool GetQualifiedNameForType(clang::QualType Type, eastl::string& QualifiedName)
    {
        const clang::Type* pType = Type.getTypePtr();

        if (pType->isArrayType())
        {
            auto ElementType = pType->castAsArrayTypeUnsafe()->getElementType();
            if (!GetQualifiedNameForType(ElementType, QualifiedName))
            {
                return false;
            }
        }
        else if (pType->isBooleanType())
        {
            QualifiedName = "bool";
        }
        else if (pType->isBuiltinType())
        {
            const clang::BuiltinType* pBT = pType->getAs<clang::BuiltinType>();
            
            switch (pBT->getKind())
            {
                case clang::BuiltinType::Char_S:
                    QualifiedName = "int8";
                    break;
                
                case clang::BuiltinType::Char_U:
                    QualifiedName = "uint8";
                    break;
                
                case clang::BuiltinType::UChar:
                    QualifiedName = "uint8";
                    break;
                
                case clang::BuiltinType::SChar:
                    QualifiedName = "int8";
                    break;
                
                case clang::BuiltinType::Char16:
                    QualifiedName = "uint16";
                    break;
                
                case clang::BuiltinType::Char32:
                    QualifiedName = "uint32";
                    break;
                
                case clang::BuiltinType::UShort:
                    QualifiedName = "uint16";
                    break;
                
                case clang::BuiltinType::Short:
                    QualifiedName = "int16";
                    break;
                
                case clang::BuiltinType::UInt:
                    QualifiedName = "uint32";
                    break;
                
                case clang::BuiltinType::Int:
                    QualifiedName = "int32";
                    break;
                
                case clang::BuiltinType::ULongLong:
                    QualifiedName = "uint64";
                    break;
                
                case clang::BuiltinType::LongLong:
                    QualifiedName = "int64";
                    break;
                
                case clang::BuiltinType::Float:
                    QualifiedName = "float";
                    break;
                
                case clang::BuiltinType::Double:
                    QualifiedName = "double";
                    break;
                default:
                {
                    return false;
                }
            }
        }
        else if (pType->isPointerType())
        {
            const clang::QualType QualType = pType->getAs<clang::PointerType>()->getPointeeType();
            QualifiedName = QualType.getAsString().c_str();
        }
        else if (pType->isRecordType())
        {
            const clang::RecordDecl* pRecordDecl = pType->getAs<clang::RecordType>()->getDecl();
            QualifiedName = pRecordDecl->getQualifiedNameAsString().c_str();
        }
        else if (pType->isEnumeralType())
        {
            const clang::NamedDecl* pNamedDecl = pType->getAs<clang::EnumType>()->getDecl();
            QualifiedName = pNamedDecl->getQualifiedNameAsString().c_str();
        }
        else if (pType->getTypeClass() == clang::Type::Typedef || pType->getTypeClass() == clang::Type::Using)
        {
            const clang::NamedDecl* pNamedDecl = pType->getAs<clang::TypedefType>()->getDecl();
            QualifiedName = pNamedDecl->getQualifiedNameAsString().c_str();
        }

        if (QualifiedName == "eastl::vector")
        {
            QualifiedName = "Lumina::TVector";
        }

        if (QualifiedName == "eastl::basic_string")
        {
            QualifiedName = "Lumina::FString";
        }
        
        if (QualifiedName == "eastl::fixed_string")
        {
            QualifiedName = "Lumina::FString";
        }

        if (QualifiedName == "FString")
        {
            QualifiedName = "Lumina::FString";
        }

        if (QualifiedName == "TObjectPtr")
        {
            QualifiedName = "Lumina::TObjectPtr";
        }

        if (QualifiedName == "CObject")
        {
            QualifiedName = "Lumina::CObject";
        }

        if (QualifiedName == "CMaterial")
        {
            QualifiedName = "Lumina::CMaterial";
        }

        return !QualifiedName.empty();
    }
    

    inline uint64_t HashString(const eastl::string& str)
    {
        return XXH64(str.data(), strlen(str.c_str()), 0);
    }
}
