#pragma once
#include "Containers/Array.h"
#include "Containers/Function.h"


namespace Lumina
{
    template<typename... TArgs>
    class TMulticastDelegate
    {
    public:
        
        template<typename TFunc>
        requires(eastl::is_invocable_v<TFunc, TArgs...>)
        void AddStatic(TFunc&& Func)
        {
            InvocationList.push_back(std::forward<TFunc>(Func));
        }

        template<typename TObject, typename TMemFunc>
        void AddMember(TObject* Object, TMemFunc&& MemberFunc)
        {
            Add([Object, MemberFunc = std::forward<TMemFunc>(MemberFunc)](TArgs... Args)
            {
                (Object->*MemberFunc)(std::forward<TArgs>(Args)...);
            });
        }

        template<typename TFunc>
        requires(eastl::is_invocable_v<TFunc, TArgs...>)
        void AddLambda(TFunc&& Func)
        {
            Add<TFunc>(std::forward<TFunc>(Func));
        }
        

        template<typename... TCallArgs>
        void Broadcast(TCallArgs... Args)
        {
            TFixedVector<uint32, 2> RemovalList;

            for (int i = 0; i < InvocationList.size(); ++i)
            {
                if (InvocationList[i] != nullptr)
                {
                    InvocationList[i](std::forward<TCallArgs>(Args)...);
                }
                else
                {
                    RemovalList.push_back(i);
                }
            }

            for (SIZE_T i = RemovalList.size(); i-- > 0;)
            {
                uint32 Removal = RemovalList[i];
                InvocationList.erase(InvocationList.begin() + Removal);
            }
        }

    private:
        
        template<typename TCallable>
        requires(eastl::is_invocable_v<TCallable, TArgs...>)
        void Add(TCallable&& Callable)
        {
            InvocationList.emplace_back(std::forward<TCallable>(Callable));
        }
    
    private:

        TVector<TFunction<void(TArgs...)>> InvocationList;

    };
}

#define DECLARE_MULTICAST_DELEGATE(DelegateName, ...) \
struct DelegateName : public Lumina::TMulticastDelegate<__VA_ARGS__> {}