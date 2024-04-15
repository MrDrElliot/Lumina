-- Grab Vulkan SDK path
VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["spdlog"] = 	                "Engine/ThirdParty/spdlog/include/"
IncludeDir["glfw"]	 = 	                "Engine/ThirdParty/GLFW/include"
IncludeDir["glm"]	 = 	                "Engine/ThirdParty/GLM/"
IncludeDir["imgui"]	 = 	                "Engine/ThirdParty/imgui/"
IncludeDir["vkbootstrap"] =             "Engine/ThirdParty/vk-bootstrap/"
IncludeDir["fastgltf"] =                "%{prj.location}/Engine/ThirdParty/fastgltf/include/fastgltf"
IncludeDir["stb"] =                     "Engine/ThirdParty/stb/"
IncludeDir["vulkan"] =                  "%{VULKAN_SDK}/"
IncludeDir["vulkanmemoryallocator"] =   "Engine/THirdParty/VulkanMemoryAllocator/include/"