#pragma once

#include "Memory/Memory.h"


namespace Lumina
{

#define LUM_CLASS(...)
#define LUM_ENUM(...)
#define LUM_FIELD(...)
#define LUM_FUNCTION(...)
    
#define DECLARE_CLASS_NOBASE(ClassName)                                     \
private:                                                                    \
    using ThisClass = ClassName;                                            \
    static Class s_StaticClass;                                             \
public:                                                                     \
    static Class* StaticClass() { return &s_StaticClass; }                  \
    virtual Class* GetClass() const override { return StaticClass(); }      \
    static RegisterClassHelper _ClassRegisterHelper;                        \
                                                                            \

#define DECLARE_CLASS_ABSTRACT(ClassName, BaseClass)                              \
private:                                                                          \
    using Super = BaseClass;                                                      \
    using ThisClass = ClassName;                                                  \
    static Class s_StaticClass;                                                   \
public:                                                                           \
    static Class* StaticClass() { return &s_StaticClass; }                        \
    virtual Class* GetClass() const override { return StaticClass(); }            \
    static ClassRegistryHelper<ClassName> _ClassRegisterHelper;                   \
    static CObject* CreateInstance() { return nullptr; }

#define DECLARE_CLASS(ClassName, BaseClass)                                       \
private:                                                                          \
    using Super = BaseClass;                                                      \
    using ThisClass = ClassName;                                                  \
    static Class s_StaticClass;                                                   \
public:                                                                           \
    static Class* StaticClass() { return &s_StaticClass; }                        \
    virtual Class* GetClass() const override { return StaticClass(); }            \
    static ClassRegistryHelper<ClassName> _ClassRegisterHelper;                   \
    static CObject* CreateInstance() { return FMemory::New<ClassName>(); }        \

#define BEGIN_CLASS_DATA()                                                        \
    static FClassMemberData RegisterClassMembers() {                              \


#define END_CLASS_DATA()                \
    }                                   \
    

#define IMPLEMENT_CLASS(ClassName)                                                              \
    Class ClassName::s_StaticClass = Class();                                                   \
    ClassRegistryHelper<ClassName> ClassName::_ClassRegisterHelper(&ClassName::CreateInstance); \



    
    
}
