-- Grab Vulkan SDK path

VULKAN_SDK = os.getenv("VULKAN_SDK")
LuminaEngineDirectory = os.getenv("LUMINA_DIR")

IncludeDir = {}
IncludeDir["spdlog"] = 	                "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/spdlog/include/"
IncludeDir["glfw"]	 = 	                "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/GLFW/include"
IncludeDir["glm"]	 = 	                "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/GLM/"
IncludeDir["imgui"]	 = 	                "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/imgui/"
IncludeDir["imnodes"]	 = 	            "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/imnodes/"
IncludeDir["vkbootstrap"] =             "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/vk-bootstrap/"
IncludeDir["fastgltf"] =                "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/fastgltf/include"
IncludeDir["stb"] =                     "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/stb/"
IncludeDir["vulkan"] =                  "%{VULKAN_SDK}/Include/"
IncludeDir["EnkiTS"] =                  "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/EnkiTS/src/"
IncludeDir["SPIRV_Reflect"] =           "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/SPIRV-Reflect/"
IncludeDir["json"] =                    "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/json/include/"
IncludeDir["entt"] =					"%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/entt/single_include/"
IncludeDir["yaml"] =                    "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/yaml-cpp/include/"
IncludeDir["ImGuizmo"] =                "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/ImGuizmo/"
IncludeDir["EASTL"] =                   "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/EA/EASTL/include/"
IncludeDir["EABase"] =                  "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/EA/EABase/include/Common/"
IncludeDir["rpmalloc"] =                "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/rpmalloc/"
IncludeDir["xxhash"] =                  "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/xxhash/"



function reflection_directory()
    return "%{wks.location}/Intermediates/Reflection/%{prj.name}/"
end


function includedependencies()
    local includes = {}
    for _, dir in pairs(IncludeDir) do
        table.insert(includes, dir)
    end
    return includes
end