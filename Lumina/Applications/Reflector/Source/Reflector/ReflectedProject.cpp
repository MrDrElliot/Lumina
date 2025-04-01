#include "ReflectedProject.h"

#include <filesystem>
#include <fstream>

#include "ReflectedHeader.h"
#include "Log/Log.h"

namespace Lumina::Reflection
{
    FReflectedProject::FReflectedProject(const FString& ProjectPath)
        : Path(ProjectPath)
        , ParentPath(std::filesystem::path(ProjectPath.c_str()).parent_path().string().c_str())
    {
    }

    bool FReflectedProject::Parse()
    {
        std::ifstream ProjectFile(Path.c_str());
        if (!ProjectFile.is_open())
        {
            LOG_ERROR("Failed to open project for parsing");
            return false;
        }
        
        std::string ParseLine;
        while (std::getline(ProjectFile, ParseLine))
        {
            FString Line(ParseLine.c_str());

            // Convert the line to lowercase to ensure case-insensitive search
            FString LowerLine = Line;
            eastl::transform(LowerLine.begin(), LowerLine.end(), LowerLine.begin(), ::tolower);

            // Look for case-insensitive "<clinclude"
            eastl_size_t FirstIndex = LowerLine.find("<clinclude");  
            if (FirstIndex != FString::npos)
            {
                FirstIndex = Line.find("Include=\"");
                if (FirstIndex != FString::npos)
                {
                    FirstIndex += 9;

                    eastl_size_t SecondIndex = Line.find("\"", FirstIndex);
                    if (SecondIndex != FString::npos)
                    {
                        const FString HeaderPath = Line.substr(FirstIndex, SecondIndex - FirstIndex);
                        FString HeaderFileFullPath = ParentPath + "\\" + HeaderPath;
                        FReflectedHeader Header(HeaderFileFullPath);

                        if (Header.Parse())
                        {
                            Headers.push_back(Header);
                        }
                    }
                }
            }
        }

        ProjectFile.close();

        return !Headers.empty();
    }


}
