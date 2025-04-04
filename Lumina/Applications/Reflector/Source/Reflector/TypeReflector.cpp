﻿#include "TypeReflector.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "Clang/ClangParser.h"
#include "CodeGeneration/CodeGenerator.h"
#include "Containers/Array.h"
#include "EASTL/sort.h"
#include "Memory/Memory.h"
#include "ReflectionCore/ReflectedProject.h"

#define VS_PROJECT_ID "{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}"  // VS Project UID



namespace Lumina::Reflection
{
    FTypeReflector::FTypeReflector(const FString& InSolutionPath)
        :Solution(InSolutionPath.c_str())
    {
    }

    bool FTypeReflector::ParseSolution()
    {
        std::cout << "Parsing Solution: " << Solution.GetPath().c_str() << "\n";
        std::ifstream slnFile(Solution.GetPath().c_str());

        TVector<FString> ProjectFilePaths;

        std::string CurrentLine;
        while (std::getline(slnFile, CurrentLine))
        {
            FString Line(CurrentLine.c_str());
            if (Line.find(VS_PROJECT_ID) != FString::npos)
            {
                auto projectNameStartIdx = Line.find(" = \"");
                projectNameStartIdx += 4;
                auto projectNameEndIdx = Line.find("\", \"", projectNameStartIdx);
                auto projectPathStartIdx = projectNameEndIdx + 4;
                auto projectPathEndIdx = Line.find("\"", projectPathStartIdx);
                const FString projectPathString = Line.substr(projectPathStartIdx, projectPathEndIdx - projectPathStartIdx);

                FString ProjectPath = Solution.GetParentPath() + "\\" + projectPathString;
                ProjectFilePaths.push_back(FMemory::Move(ProjectPath));
            }
        }

        slnFile.close();

        eastl::sort(ProjectFilePaths.begin(), ProjectFilePaths.end(), [](const FString& A, const FString& B)
        {
            return A < B;
        });

        for (const FString& FilePath : ProjectFilePaths)
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

    bool FTypeReflector::Build()
    {
        FClangParser Parser;
        
        for (FReflectedProject& Project : Projects)
        {
            Parser.ParsingContext.ReflectionDatabase.AddReflectedProject(Project);
            
            for (FReflectedHeader& Header : Project.Headers)
            {
                LOG_ERROR("Reflecting Header: {0}", Header.HeaderPath);

                if (!Parser.Parse(Project.SolutionPath, Header.HeaderPath))
                {
                    LOG_ERROR("Failed to parse header file! {0}", Header.HeaderPath);
                }
            }
        }

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
