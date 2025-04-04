#include "CodeGenerator.h"
#include <fstream>
#include <sstream>
#include <string>
#include "Platform/Filesystem/FileHelper.h"
#include "Reflector/ReflectionCore/ReflectedHeader.h"
#include "Reflector/ReflectionCore/ReflectedProject.h"

namespace Lumina::Reflection
{
    FCodeGenerator::FCodeGenerator(const FProjectSolution& SlnPath, const FReflectionDatabase& Database)
        : CurrentProject("", "")
        , Solution(SlnPath)
        , ReflectionDatabase(&Database)
    {
        
    }

    void FCodeGenerator::GenerateCodeForSolution()
    {
        for (const FReflectedProject& Project : ReflectionDatabase->ReflectedProjects)
        {
            SetProject(Project);
            GenerateCodeForProject(Project);    
        }
    }

    void FCodeGenerator::GenerateCodeForProject(const FReflectedProject& Project)
    {
        for (const FReflectedHeader& Header : Project.Headers)
        {
            GenerateReflectionCodeForHeader(Header);
        }
    }

    void FCodeGenerator::GenerateReflectionCodeForHeader(const FReflectedHeader& Header)
    {
        std::stringstream stream;
        stream.str(std::string());
        stream.clear();

        GenerateCodeHeader(stream, Header);
    


        FString ReflectionDataPath = Solution.GetParentPath() + R"(\Intermediates\Reflection\)" + CurrentProject.Name + R"(\)" + Header.FileName + ".generated.h";
        std::filesystem::path outputPath(ReflectionDataPath.c_str());
        std::filesystem::create_directories(outputPath.parent_path());
        
        std::ofstream outputFile(ReflectionDataPath.c_str());

        if (outputFile.is_open())
        {
            outputFile << stream.str();
            outputFile.close();
        }
        else
        {
            LOG_ERROR("Failed to open file: {0}", ReflectionDataPath);
        }
    }

    void FCodeGenerator::GenerateCodeHeader(std::stringstream& SS, const FReflectedHeader& Header)
    {
        SS << "#pragma once\n\n";
        SS << "//*************************************************************************\n";
        SS << "// Generated by Lumina Reflection Tool. \n";
        SS << "// This is an auto-generated file - DO NOT EDIT.\n";
        SS << "//*************************************************************************\n\n";
        SS << "#include \"" << Header.HeaderPath.c_str() << "\"\n";
    }
}
