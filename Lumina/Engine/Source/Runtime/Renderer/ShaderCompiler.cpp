#include "ShaderCompiler.h"

#include <fstream>

#include "Source/Runtime/Log/Log.h"

namespace Lumina
{

    shaderc_shader_kind convert(const EShaderStage& stage)
    {
        switch (stage)
        {
        case EShaderStage::VERTEX:		return shaderc_vertex_shader;
        case EShaderStage::FRAGMENT:	return shaderc_fragment_shader;
        case EShaderStage::COMPUTE:		return shaderc_compute_shader;
        default:						std::unreachable();
        }
    }

    
    FShaderCompiler::FShaderCompiler()
    {
        GlobalOptions.SetOptimizationLevel(shaderc_optimization_level_performance);
    }

    bool FShaderCompiler::ReadShaderFile(std::filesystem::path path, std::stringstream* out)
    {

        std::ifstream Reader(path, std::ifstream::ate | std::ifstream::binary);

        std::byte* Data;
        glm::uint64 FileSize = Reader.tellg();
        Data = new std::byte[FileSize];
		
        Reader.seekg(0);
        Reader.read((char*)Data, FileSize);
        Reader.close();

        out->write((const char*)Data, FileSize);

        delete[] Data;

        return true;
    }
	
    void FShaderCompiler::AddGlobalMacro(const std::string& key, const std::string& value)
    {
        GlobalMacros.emplace(key, value);
        GlobalOptions.AddMacroDefinition(key, value);
    }

    FShaderCompilationResult FShaderCompiler::Compile(std::string& source, const std::string& filename)
    {
    	FShaderCompilationResult CompilationResult = { .bValid = true };
		shaderc::CompileOptions local_options = GlobalOptions;

		std::map<EShaderStage, std::string> separated_sources;
		std::map<EShaderStage, std::vector<glm::uint32>> binaries;

		std::istringstream input_stream(source);
		std::string current_parsing_line;
		glm::uint32 current_line_number = 1;
		EShaderStage current_parsing_stage = EShaderStage::UNKNOWN;

		bool pragma_lang_found = false;

		/* Taken from Omni-Force Engine */
		while (std::getline(input_stream, current_parsing_line))
		{
			if (current_parsing_line.find("#pragma") != std::string::npos)
			{
				if (current_parsing_line.find(" lang") != std::string::npos)
				{
					if (current_parsing_line.find("glsl") != std::string::npos)
					{
						local_options.SetSourceLanguage(shaderc_source_language_glsl);
						pragma_lang_found = true;
					}
					else if (current_parsing_line.find("hlsl") != std::string::npos)
					{
						local_options.SetSourceLanguage(shaderc_source_language_hlsl);
						pragma_lang_found = true;
					}
				}
				else if (current_parsing_line.find("stage") != std::string::npos)
				{
					if (current_parsing_line.find("vertex") != std::string::npos)
					{
						current_parsing_stage = EShaderStage::VERTEX;
						// \n to ensure line similarity between shader file and actual parsed source
						separated_sources.emplace(EShaderStage::VERTEX, "\n"); 
					}
					else if (current_parsing_line.find("fragment") != std::string::npos)
					{
						current_parsing_stage = EShaderStage::FRAGMENT;
						separated_sources.emplace(EShaderStage::FRAGMENT, "\n");
					}
					else if (current_parsing_line.find("compute") != std::string::npos)
					{
						current_parsing_stage = EShaderStage::COMPUTE;
						separated_sources.emplace(EShaderStage::COMPUTE, "\n");
					}
					else {
						CompilationResult.bValid = false;
						LE_LOG_ERROR("Failed to parse shader, invalid stage at line {0}", current_line_number);
						return CompilationResult;
					}
				}
				else {
					CompilationResult.bValid = false;
					LE_LOG_ERROR("Failed to parse shader, invalid #pragma at line {0}", current_line_number);
					return CompilationResult;
				}
			}
			else {
				separated_sources[current_parsing_stage].append(current_parsing_line + '\n');
				current_line_number++;
			}
		};

		if (!pragma_lang_found)
		{
			LE_LOG_ERROR("#pragma lang in {0} was not found", filename);
			CompilationResult.bValid = false;
			return CompilationResult;
		}

		// Compilation stage
		for (auto& stage_source : separated_sources)
		{
			shaderc::CompilationResult result = Compiler.CompileGlslToSpv(stage_source.second, convert(stage_source.first), filename.c_str(), local_options);
			if (result.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				LE_LOG_ERROR("Failed to compile shader {0}.{1}:\n{2}", filename, 1, result.GetErrorMessage());
				CompilationResult.bValid = false;
			}

			std::vector<glm::uint32> stage_compilation_result(result.begin(), result.end());
			binaries.emplace(stage_source.first, std::move(stage_compilation_result));
		}

		CompilationResult.Bytecode = std::move(binaries);
			
		return CompilationResult;
    }
}
