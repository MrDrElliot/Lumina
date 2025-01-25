#pragma once
#include <stack>
#include <functional>

template<typename Args>
class TMulticastDelegate
{
public:

    // Add a static function to the invocation list
    template<typename Func, typename... FuncArgs, std::enable_if_t<std::is_invocable<Func, Args>::value>* = nullptr>
    void AddStatic(Func&& func)
    {
        InvokationList.push(std::forward<Func>(func));
    }

    template<typename Obj, typename Func, typename... FuncArgs, std::enable_if_t<std::is_member_function_pointer<Func>::value>* = nullptr>
    void AddMember(Obj* obj, Func func)
    {
        InvokationList.push([obj, func](FuncArgs&&... args)
        {
            (obj->*func)(std::forward<FuncArgs>(args)...);
        });
    }
    
    template<typename...InvokeArgs>
    void Broadcast(InvokeArgs... args)
    {
        while (!InvokationList.empty())
        {
            auto& func = InvokationList.top();
            func(std::forward<InvokeArgs>(args)...);
            InvokationList.pop();
        }
    }
    
    
private:

    std::stack<std::move_only_function<void(Args)>> InvokationList;
};
