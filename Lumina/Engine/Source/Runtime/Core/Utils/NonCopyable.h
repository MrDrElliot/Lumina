#pragma once


namespace Lumina
{

    class INonCopyable
    {
    public:
        INonCopyable() = default;
        INonCopyable(const INonCopyable&) = delete;
        INonCopyable& operator = (const INonCopyable&) = delete;
        
    };
    
}