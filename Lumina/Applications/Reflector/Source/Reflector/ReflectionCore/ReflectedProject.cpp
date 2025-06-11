#include "ReflectedProject.h"

#include <filesystem>
#include <fstream>

#include "ReflectedHeader.h"
#include "Reflector/Clang/Utils.h"

namespace Lumina::Reflection
{
    FReflectedProject::FReflectedProject(const eastl::string& SlnPath, const eastl::string& ProjectPath)
        : SolutionPath(SlnPath)
        , Path(ProjectPath)
        , ParentPath(std::filesystem::path(ProjectPath.c_str()).parent_path().string().c_str())
    {
    }

    bool FReflectedProject::Parse()
    {
        std::ifstream ProjectFile(Path.c_str());
        if (!ProjectFile.is_open())
        {
            return false;
        }

        std::filesystem::path FilesystemPath = Path.c_str();
        Name = FilesystemPath.stem().string().c_str();
        
        std::string ParseLine;
        while (std::getline(ProjectFile, ParseLine))
        {
            eastl::string Line(ParseLine.c_str());

            // Convert the line to lowercase to ensure case-insensitive search
            eastl::string LowerLine = Line;
            eastl::transform(LowerLine.begin(), LowerLine.end(), LowerLine.begin(), ::tolower);

            // Look for case-insensitive "<clinclude"
            eastl_size_t FirstIndex = LowerLine.find("<clinclude");  
            if (FirstIndex != eastl::string::npos)
            {
                FirstIndex = Line.find("Include=\"");
                if (FirstIndex != eastl::string::npos)
                {
                    FirstIndex += 9;

                    eastl_size_t SecondIndex = Line.find("\"", FirstIndex);
                    if (SecondIndex != eastl::string::npos)
                    {
                        const eastl::string HeaderPath = Line.substr(FirstIndex, SecondIndex - FirstIndex);
                        eastl::string HeaderFileFullPath = ParentPath + "\\" + HeaderPath;
                        
                        // Skip files that contain ".generated." in their name
                        if (HeaderFileFullPath.find(".generated.") != eastl::string::npos)
                        {
                            continue;
                        }

                        eastl::replace(HeaderFileFullPath.begin(), HeaderFileFullPath.end(), '\\', '/');
                        HeaderFileFullPath.make_lower();
                        FReflectedHeader Header(HeaderFileFullPath);

                        if (Header.Parse())
                        {
                            Headers.push_back(Header);
                            FStringHash Hash = FStringHash(Header.HeaderPath);
                            HeaderHashMap.insert_or_assign(Hash, Header);
                        }
                    }
                }
            }
        }

        ProjectFile.close();

        return !Headers.empty();
    }
}
