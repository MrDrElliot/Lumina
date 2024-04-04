#pragma once
#include <memory>


namespace Lumina
{
    class FRendererContext
    {
    public:

        static std::shared_ptr<FRendererContext> Create();
        
        FRendererContext() = default;
        virtual ~FRendererContext() = default;
        virtual void Init() = 0;


        template<typename T>
        std::shared_ptr<T> GetAs()
        {
            return static_cast<T>(*this);
        }
        
    };
}
