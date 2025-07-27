#include "ProjectSolution.h"

#include <fstream>
#include <iostream>

#include "ReflectionCore/ReflectedProject.h"

namespace Lumina::Reflection
{
    FProjectSolution::FProjectSolution(const std::filesystem::path& SlnPath)
        : Path(SlnPath.string().c_str())
        , ParentPath(SlnPath.parent_path().string().c_str())
    {
    }
    
    bool FProjectSolution::DirtyProjectFiles()
    {
        for (FReflectedProject& Project : ReflectedProjects)
        {
            std::string ProjectPath = Project.Path.c_str();

            std::ifstream Input(ProjectPath);
            if (!Input)
            {
                std::cout << "Failed to open project file: " << ProjectPath << "\n";
                continue;
            }

            std::ostringstream Buffer;
            Buffer << Input.rdbuf();
            Input.close();

            std::ofstream Output(ProjectPath, std::ios::trunc);
            if (!Output)
            {
                std::cout << "Failed to reopen project file for writing: " << ProjectPath << "\n";
                continue;
            }

            Output << Buffer.str();
            Output.close();
        }

        return true;
    }
}
