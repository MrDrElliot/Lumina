

-- Grab Vulkan SDK path
VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["spdlog"] = 	"Engine/ThirdParty/spdlog/include/"
IncludeDir["glfw"]	 = 	"Engine/ThirdParty/GLFW/include"
IncludeDir["glm"]	 = 	"Engine/ThirdParty/GLM/"
IncludeDir["vulkan"] = "%{VULKAN_SDK}/"