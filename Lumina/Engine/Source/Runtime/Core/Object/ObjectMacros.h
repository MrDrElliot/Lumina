#pragma once

#include "Memory/Memory.h"
#include "Core/Object/ObjectCore.h"


class Lumina::CClass;

enum EInternal { EC_InternalUseOnlyConstructor };


#define NO_API



#define LUM_CLASS(...)
#define LUM_STRUCT(...)
#define LUM_ENUM(...)
#define LUM_PROPERTY(...)
#define LUM_FUNCTION(...)

#define CONCAT_INNER(a, b) a##b
#define CONCAT(a, b) CONCAT_INNER(a, b)

#define CONCAT3_INNER(a, b, c) a##b##c
#define CONCAT3(a, b, c) CONCAT3_INNER(a, b, c)

#define CONCAT4_INNER(a, b, c, d) a##b##c##d
#define CONCAT4(a, b, c, d) CONCAT4_INNER(a, b, c, d)

#define CONCAT_WITH_UNDERSCORE(a, b) CONCAT3(a, _, b)
#define FRIEND_STRUCT_NAME(ns, cls) CONCAT3(Construct_CClass_, CONCAT_WITH_UNDERSCORE(ns, cls), _Statics)


// =====================================================================================================
// NOTE: Clang has a known limitation where it struggles to properly expand variadic macros
// during parsing (especially when using libclang or Clang tooling).
// 
// When REFLECTION_PARSER is defined (i.e., when running our custom reflection parser),
// we disable the GENERATED_BODY macro expansion to prevent parsing errors and allow
// correct type discovery.
//
// This workaround ensures that Clang sees an empty GENERATED_BODY(...) instead of trying
// to expand it and breaking the AST.
//
// IMPORTANT: This does not affect normal compilation; only parsing for reflection!
// =====================================================================================================
#if defined(REFLECTION_PARSER)

    #define GENERATED_BODY(...)

#else

    #define GENERATED_BODY(...) CONCAT4(CURRENT_FILE_ID, _, __LINE__, _GENERATED_BODY)

#endif



#define DECLARE_CLASS(TNamespace, TClass, TBaseClass, TPackage, TAPI) \
private: \
    friend struct FRIEND_STRUCT_NAME(TNamespace, TClass); \
    TNamespace::TClass& operator=(TNamespace::TClass&&); \
    TNamespace::TClass& operator=(const TNamespace::TClass&); \
    TAPI static Lumina::CClass* GetPrivateStaticClass(); \
public: \
    using ThisClass = TNamespace::TClass; \
    using Super = TBaseClass; \
    inline static Lumina::CClass* StaticClass() { return GetPrivateStaticClass(); } \
    inline static const TCHAR* StaticPackage() { return TEXT(#TPackage); } \
    inline void* operator new(const size_t InSize, EInternal InMem, Lumina::FName InName = Lumina::NAME_None, Lumina::EObjectFlags InSetFlags = Lumina::OF_None) \
    { \
        Lumina::FConstructCObjectParams Params(StaticClass()); \
        Params.Name = InName; \
        Params.Flags = InSetFlags; \
        Params.Package = TEXT(#TPackage); \
        return Lumina::StaticAllocateObject(Params); \
    } \
    inline void* operator new(const size_t InSize, EInternal* InMem) \
    { \
        return (void*)InMem; \
    }


#define DECLARE_SERIALIZER(TNamespace, TClass) \
    friend FArchive& operator << (FArchive& Ar, TNamespace::TClass*& Res) \
    { \
        return Ar << (CObject*&)Res; \
    } \


#define DEFINE_DEFAULT_CONSTRUCTOR_CALL(TClass) \
    static void __DefaultConstructor(const Lumina::FObjectInitializer& OI) { new ((EInternal*)OI.GetObj())TClass; }


#define IMPLEMENT_CLASS(TNamespace, TClass) \
    Lumina::FClassRegistrationInfo CONCAT3(Registration_Info_CClass_, TNamespace, _##TClass); \
    NO_API Lumina::CClass* ##TNamespace::##TClass::GetPrivateStaticClass() \
    { \
        if (CONCAT3(Registration_Info_CClass_, TNamespace, _##TClass).Singleton == nullptr) \
        { \
            Lumina::AllocateStaticClass( \
                TClass::StaticPackage(), \
                TEXT(#TClass), \
                &CONCAT3(Registration_Info_CClass_, TNamespace, _##TClass).Singleton, \
                sizeof(TNamespace::TClass), \
                alignof(TNamespace::TClass), \
                &TNamespace::TClass::Super::StaticClass, \
                (Lumina::CClass::ClassConstructorType)Lumina::InternalConstructor<TNamespace::TClass>); \
        } \
        return CONCAT4(Registration_Info_CClass_, TNamespace, _, ##TClass).Singleton; \
    }


/** Intrinsic classic auto register. */
#define IMPLEMENT_INTRINSIC_CLASS(TClass, TBaseClass, TAPI) \
    TAPI CClass* Construct_CClass_Lumina_##TClass(); \
    extern FClassRegistrationInfo Registration_Info_CClass_Lumina_##TClass; \
    struct Construct_CClass_Lumina_##TClass##_Statics \
    { \
        static CClass* Construct() \
        { \
            extern TAPI CClass* Construct_CClass_Lumina_##TBaseClass(); \
            CClass* SuperClass = Construct_CClass_Lumina_##TBaseClass(); \
            CClass* Class = TClass::StaticClass(); \
            CObjectForceRegistration(Class); \
            return Class; \
        } \
    }; \
    CClass* Construct_CClass_Lumina_##TClass() \
    { \
        if(!Registration_Info_CClass_Lumina_##TClass.Singleton) \
        { \
            Registration_Info_CClass_Lumina_##TClass.Singleton = Construct_CClass_Lumina_##TClass##_Statics::Construct(); \
        } \
        return Registration_Info_CClass_Lumina_##TClass.Singleton; \
    } \
    IMPLEMENT_CLASS(Lumina, TClass) \
    static FRegisterCompiledInInfo AutoInitialize_##TClass(&Construct_CClass_Lumina_##TClass, TClass::StaticPackage(), TEXT(#TClass));


namespace LuminaAsserts_Private
{
    // A junk function to allow us to use sizeof on a member variable which is potentially a bitfield
    template <typename T>
    bool GetMemberNameCheckedJunk(const T&);
    template <typename T>
    bool GetMemberNameCheckedJunk(const volatile T&);
    template <typename R, typename ...Args>
    bool GetMemberNameCheckedJunk(R(*)(Args...));
}

// Returns FName(TEXT("MemberName")), while statically verifying that the member exists in ClassName
#define GET_MEMBER_NAME_CHECKED(ClassName, MemberName) \
((void)sizeof(LuminaAsserts_Private::GetMemberNameCheckedJunk(((ClassName*)0)->MemberName)), FName(TEXT(#MemberName)))
