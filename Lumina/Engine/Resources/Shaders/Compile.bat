@echo off
setlocal

set VULKAN_SDK_PATH=C:/VulkanSDK/1.3.280.0/

echo Compiling shaders...

%VULKAN_SDK_PATH%/Bin/glslc.exe  CookTorrance.vert -o CookTorrance.vert.spv
echo Compiling CookTorrance.vert

%VULKAN_SDK_PATH%/Bin/glslc.exe  CookTorrance.frag -o CookTorrance.frag.spv
echo Compiling CookTorrance.frag

%VULKAN_SDK_PATH%/Bin/glslc.exe  InfiniteGrid.vert -o InfiniteGrid.vert.spv
echo Compiling InfiniteGrid.vert

%VULKAN_SDK_PATH%/Bin/glslc.exe  InfiniteGrid.frag -o InfiniteGrid.frag.spv
echo Compiling InfiniteGrid.frag

%VULKAN_SDK_PATH%/Bin/glslc.exe  TAA.vert -o TAA.vert.spv
echo Compiling TAA.vert

%VULKAN_SDK_PATH%/Bin/glslc.exe  TAA.frag -o TAA.frag.spv
echo Compiling TAA.frag



PAUSE

endlocal