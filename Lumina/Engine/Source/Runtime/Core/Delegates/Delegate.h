#pragma once
#include <stack>

#include "Containers/Array.h"
#include "Core/Functional/Function.h"


namespace Lumina
{
    template<typename Args = void>
    class TMulticastDelegate
    {
    public:

        // Add a static function to the invocation list
        template<typename Func, typename... FuncArgs, std::enable_if_t<std::is_invocable<Func, Args>::value>* = nullptr>
        void AddStatic(Func&& func)
        {
            InvokationList.push_back(std::forward<Func>(func));
        }
        
        void AddTFunction(TFunction<void(Args)> Functor)
        {
            InvokationList.push_back(std::move(Functor));
        }

        void RemoveTFunction(TFunction<void(Args)> Functor)
        {
            VectorRemove(InvokationList, Functor);
        }
        
        template<typename... InvokeArgs>
        void Broadcast(InvokeArgs... args)
        {
            TFixedVector<uint32, 2> RemovalList;

            for (int i = 0; i < InvokationList.size(); ++i)
            {
                if (InvokationList[i] != nullptr)
                {
                    InvokationList[i](std::forward<InvokeArgs>(args)...);
                }
                else
                {
                    RemovalList.push_back(i);
                }
            }

            for (int i = RemovalList.size() - 1; i >= 0; --i)
            {
                uint32 Removal = RemovalList[i];
                InvokationList.erase(InvokationList.begin() + Removal);
            }
        }

    
    
    private:

        TVector<TFunction<void(Args)>> InvokationList;

    };
}

#define DECLARE_MULTICAST_DELEGATE(DelegateName, ...) \
struct DelegateName : public Lumina::TMulticastDelegate<__VA_ARGS__> {}