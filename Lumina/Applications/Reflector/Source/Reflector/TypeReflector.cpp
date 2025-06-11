#include "TypeReflector.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "Clang/ClangParser.h"
#include "CodeGeneration/CodeGenerator.h"
#include "EASTL/sort.h"
#include "ReflectionCore/ReflectedProject.h"

#define VS_PROJECT_ID "{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}"  // VS Project UID



namespace Lumina::Reflection
{
    FTypeReflector::FTypeReflector(const eastl::string& InSolutionPath)
        :Solution(InSolutionPath.c_str())
    {
    }

    bool FTypeReflector::ParseSolution()
    {
        std::ifstream slnFile(Solution.GetPath().c_str());

        eastl::vector<eastl::string> ProjectFilePaths;

        std::string CurrentLine;
        while (std::getline(slnFile, CurrentLine))
        {
            eastl::string Line(CurrentLine.c_str());
            if (Line.find(VS_PROJECT_ID) != eastl::string::npos)
            {
                auto projectNameStartIdx = Line.find(" = \"");
                projectNameStartIdx += 4;
                auto projectNameEndIdx = Line.find("\", \"", projectNameStartIdx);
                auto projectPathStartIdx = projectNameEndIdx + 4;
                auto projectPathEndIdx = Line.find("\"", projectPathStartIdx);
                const eastl::string projectPathString = Line.substr(projectPathStartIdx, projectPathEndIdx - projectPathStartIdx);

                eastl::string ProjectPath = Solution.GetParentPath() + "\\" + projectPathString;
                ProjectFilePaths.push_back(std::move(ProjectPath));
            }
        }

        slnFile.close();

        eastl::sort(ProjectFilePaths.begin(), ProjectFilePaths.end(), [](const eastl::string& A, const eastl::string& B)
        {
            return A < B;
        });

        for (const eastl::string& FilePath : ProjectFilePaths)
        {
            FReflectedProject Project(Solution.GetPath(), FilePath);
            
            
            if (Project.Parse())
            {
                Projects.push_back(Project);
            }
        }
        
        return !Projects.empty();
    }

    bool FTypeReflector::Clean()
    {
        return false;
    }

    bool FTypeReflector::Build(FClangParser& Parser)
    {
        for (FReflectedProject& Project : Projects)
        {
            Parser.ParsingContext.ReflectionDatabase.AddReflectedProject(Project);

            // Initial Pass. Register Types and create types only.
            Parser.ParsingContext.bInitialPass = true;
            
            if (!Parser.Parse(Project.SolutionPath, Project.Headers, Project))
            {
                std::cout << "Failed to parse\n";
            }

            Parser.ParsingContext.bInitialPass = false;

            // Second Pass. Traverse children and finish building types.
            if (!Parser.Parse(Project.SolutionPath, Project.Headers, Project))
            {
                std::cout << "Failed to parse\n";
            }
        }

        std::cout << "[Reflection] Number of headers reflected: " << Parser.ParsingContext.NumHeadersReflected << "\n";
        std::cout << "[Reflection] Number of files looked at: " << GFilesLookedAt << "\n";

        if (Parser.ParsingContext.HasError())
        {
            std::cout << Parser.ParsingContext.ErrorMessage.c_str() << "\n";
        }
        
        return true;
    }

    bool FTypeReflector::Generate(FClangParser& Parser)
    {
        WriteGeneratedFiles(Parser);

        return true;
    }


    bool FTypeReflector::WriteGeneratedFiles(const FClangParser& Parser)
    {
        FCodeGenerator Generator(Solution, Parser.ParsingContext.ReflectionDatabase);

        Generator.GenerateCodeForSolution();
        
        return true;
    }
}
