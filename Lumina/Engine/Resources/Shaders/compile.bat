@echo off
setlocal

set VULKAN_SDK_PATH=C:/VulkanSDK/1.3.280.0/

echo Compiling shaders...

%VULKAN_SDK_PATH%/Bin/glslc.exe  Mesh.vert -o Mesh.vert.spv
%VULKAN_SDK_PATH%/Bin/glslc.exe  Mesh.frag -o Mesh.frag.spv

PAUSE

endlocal