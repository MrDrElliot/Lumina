#pragma once

#include <functional>


template<typename... Args>
class TDelegate {
public:
    using FuncType = std::function<void(Args...)>;

private:
    FuncType func;

public:
    TDelegate() = default;

    template<typename F, typename... Us>
    void Bind(F&& f, Us&&... us)
    {
        func = [f = std::forward<F>(f), ...us = std::forward<Us>(us)](Args... args) mutable
        {
            std::invoke(f, us..., std::forward<Args>(args)...);
        };
    }

    void Execute(Args... args) const {
        if (func) {
            func(std::forward<Args>(args)...);
        }
    }

    bool IsBound() const {
        return static_cast<bool>(func);
    }
};


template<typename... Args>
class TMulticastDelegate
{
public:
    using FuncType = std::function<void(Args...)>;

private:
    std::vector<FuncType> funcs;

public:
    TMulticastDelegate() = default;

    template<typename F, typename... Us>
    void Add(F&& f, Us&&... us)
    {
        funcs.emplace_back([f = std::forward<F>(f), ...us = std::forward<Us>(us)](Args... args) mutable
        {
            std::invoke(f, us..., std::forward<Args>(args)...);
        });
    }

    void Broadcast(Args... args) const
    {
        for (const auto& func : funcs)
        {
            func(std::forward<Args>(args)...);
        }
    }
};