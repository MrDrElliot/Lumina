

-- Grab Vulkan SDK path
VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["spdlog"] = 	"Engine/Source/ThirdParty/spdlog/include/"
IncludeDir["glfw"]	 = 	"Engine/Source/ThirdParty/GLFW/include"
IncludeDir["glm"]	 = 	"Engine/Source/ThirdParty/GLM/"