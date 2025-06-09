#pragma once
#include <string>

#include "EASTL/string.h"
#include "EASTL/vector.h"


namespace Lumina::Reflection
{
    class FReflectedHeader
    {
    public:

        FReflectedHeader()
            :bSkip(false)
        {}
        
        FReflectedHeader(const eastl::string& Path);

        bool Parse();
        

        eastl::string             FileName;
        eastl::string             HeaderID;
        eastl::string             HeaderPath;
        uint8_t                   bSkip:1;


        
    };
}
