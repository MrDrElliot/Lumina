#include "ReflectedHeader.h"

#include <fstream>

#include "Log/Log.h"
#include "Reflector/ReflectionConfig.h"

namespace Lumina::Reflection
{
    
    FReflectedHeader::FReflectedHeader(const FString& Path)
        : HeaderPath(Path)
    {
        FString LowercasePath = HeaderPath;
        LowercasePath.make_lower();
        HeaderID = LowercasePath.c_str();
    }

    bool FReflectedHeader::Parse()
    {
        std::ifstream HeaderFile(HeaderPath.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
        if (!HeaderFile.is_open())
        {
            LOG_ERROR("Failed to parse header file: {0}", HeaderPath);
            return false;
        }

        if (HeaderFile.tellg() == 0)
        {
            HeaderFile.close();
            return false;
        }
        
        HeaderFile.seekg(0, std::ios::beg);
        {
            std::string FileLine;
            while (std::getline(HeaderFile, FileLine))
            {
                Contents.emplace_back(FileLine.c_str());
            }
        
            HeaderFile.close();
        }

        for (const FString& FileLine : Contents)
        {
            for (uint32 i = 0; i < (uint32)EReflectionMacro::Size; ++i)
            {
                eastl_size_t MacroIndex = FileLine.find(ReflectionEnumToString(EReflectionMacro(i)));
                if (MacroIndex != FString::npos)
                {
                    return true;
                }   
            }
        }

        return false;
    }
}
