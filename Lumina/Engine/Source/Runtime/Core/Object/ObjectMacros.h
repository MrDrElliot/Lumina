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


#define MACRO_BODY_COMBINE(A, B, C, D) A##B##C##D
#define MACRO_BODY_COMBINE_EXPAND(A, B, C, D) MACRO_BODY_COMBINE(A, B, C, D)


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
#define GENERATED_BODY(...) MACRO_BODY_COMBINE_EXPAND(CURRENT_FILE_ID, _, __LINE__, _GENERATED_BODY)
#endif




#define DECLARE_CLASS(TClass, TBaseClass, TAPI) \
private: \
    friend struct Construct_CClass_##TClass##_Statics; \
    TClass& operator=(TClass&&); \
    TClass& operator=(const TClass&); \
    TAPI static CClass* GetPrivateStaticClass(); \
public: \
    /** Simple helper typedefs */ \
    using ThisClass = TClass; \
    using Super = TBaseClass; \
    /** Returns CClass object representing this class */ \
    inline static CClass* StaticClass() \
    { \
        return GetPrivateStaticClass(); \
    } \
	inline void* operator new(const size_t InSize, EInternal InMem, FName InName = NAME_None, EObjectFlags InSetFlags = OF_None) \
	{ \
        FConstructCObjectParams Params(StaticClass()); \
        Params.Name = InName; \
        Params.Flags = InSetFlags; \
		return StaticAllocateObject(Params); \
	} \
    inline void* operator new(const size_t InSize, EInternal* InMem) \
    { \
        return (void*)InMem; \
    } \

#define DECLARE_SERIALIZER(TClass) \
    friend FArchive& operator << (FArchive& Ar, TClass*& Res) \
    { \
        return Ar << (CObject*&)Res; \
    } \


#define DEFINE_DEFAULT_CONSTRUCTOR_CALL(TClass) \
    static void __DefaultConstructor(const FObjectInitializer& OI) { new ((EInternal*)OI.GetObj())TClass; }

#define IMPLEMENT_CLASS(TClass) \
    FClassRegistrationInfo Registration_Info_CClass_##TClass; \
    CClass* TClass::GetPrivateStaticClass() \
    { \
        if (Registration_Info_CClass_##TClass.Singleton == nullptr) \
        { \
            AllocateStaticClass( \
            TEXT(#TClass), \
            &Registration_Info_CClass_##TClass.Singleton, \
            sizeof(TClass), \
            alignof(TClass), \
            &TClass::Super::StaticClass, \
            (CClass::ClassConstructorType)InternalConstructor<TClass>); \
        } \
        return Registration_Info_CClass_##TClass.Singleton; \
    }

/** Intrinsic classic auto register. */
#define IMPLEMENT_INTRINSIC_CLASS(TClass, TBaseClass, TAPI) \
    TAPI CClass* Construct_CClass_##TClass(); \
    extern FClassRegistrationInfo Registration_Info_CClass_##TClass; \
    struct Construct_CClass_##TClass##_Statics \
    { \
        static CClass* Construct() \
        { \
            extern TAPI CClass* Construct_CClass_##TBaseClass(); \
            CClass* SuperClass = Construct_CClass_##TBaseClass(); \
            CClass* Class = TClass::StaticClass(); \
            CObjectForceRegistration(Class); \
            return Class; \
        } \
    }; \
    CClass* Construct_CClass_##TClass() \
    { \
        if(!Registration_Info_CClass_##TClass.Singleton) \
        { \
            Registration_Info_CClass_##TClass.Singleton = Construct_CClass_##TClass##_Statics::Construct(); \
        } \
        return Registration_Info_CClass_##TClass.Singleton; \
    } \
    IMPLEMENT_CLASS(TClass) \
    static FRegisterCompiledInInfo AutoInitialize_##TClass(&Construct_CClass_##TClass, TEXT(#TClass));


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
