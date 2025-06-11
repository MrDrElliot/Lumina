#include "ReflectedHeader.h"

#include <filesystem>
#include <fstream>

#include "Reflector/ReflectionConfig.h"

namespace Lumina::Reflection
{
    
    FReflectedHeader::FReflectedHeader(const eastl::string& Path)
        : HeaderPath(Path)
        , bSkip(false)
    {
        std::filesystem::path FilesystemPath = Path.c_str();
        FileName = FilesystemPath.stem().string().c_str();
    }
    bool FReflectedHeader::Parse()
    {
        std::ifstream HeaderFile(HeaderPath.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
        if (!HeaderFile.is_open())
            return false;

        std::streamsize FileSize = HeaderFile.tellg();
        if (FileSize <= 0)
            return false;

        HeaderFile.seekg(0, std::ios::beg);
        eastl::string FileData(FileSize, '\0');
        if (!HeaderFile.read(&FileData[0], FileSize))
            return false;

        // Search for any of the macro strings directly in the buffer
        for (uint32_t i = 0; i < (uint32_t)EReflectionMacro::Size; ++i)
        {
            if (FileData.find(ReflectionEnumToString(EReflectionMacro(i))) != eastl::string::npos)
            {
                return true;
            }
        }

        return false;
    }

}
