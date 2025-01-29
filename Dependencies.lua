-- Grab Vulkan SDK path
VULKAN_SDK = os.getenv("VULKAN_SDK")
LuminaEngineDirectory = os.getenv("LUMINA_DIR")

IncludeDir = {}
IncludeDir["spdlog"] = 	                "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/spdlog/include/"
IncludeDir["glfw"]	 = 	                "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/GLFW/include"
IncludeDir["glm"]	 = 	                "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/GLM/"
IncludeDir["imgui"]	 = 	                "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/imgui/"
IncludeDir["vkbootstrap"] =             "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/vk-bootstrap/"
IncludeDir["fastgltf"] =                "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/fastgltf/include"
IncludeDir["stb"] =                     "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/stb/"
IncludeDir["vulkan"] =                  "%{VULKAN_SDK}/Include/"
IncludeDir["SPIRV_Reflect"] =           "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/SPIRV-Reflect/"
IncludeDir["shaderc"] =                 "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/shaderc/libshaderc_util/include"
IncludeDir["json"] =                    "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/json/include/"
IncludeDir["nvrhi"] =                   "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/nvrhi-main/include/"
IncludeDir["entt"] =					"%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/entt/single_include/"
IncludeDir["vulkanmemoryallocator"] =   "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/VulkanMemoryAllocator/include/"
IncludeDir["yaml"] =                    "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/yaml-cpp/include/"
IncludeDir["ImGuizmo"] =                "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/ImGuizmo/"
IncludeDir["EASTL"] =                   "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/EA/EASTL/include/"
IncludeDir["EABase"] =                  "%{LuminaEngineDirectory}/Lumina/Engine/ThirdParty/EA/EABase/include/Common/"



function includedependencies()
    local includes = {}
    for _, dir in pairs(IncludeDir) do
        table.insert(includes, dir)
    end
    return includes
end