import os
import subprocess
import urllib.request
import zipfile
import platform
import CheckPython

# Make sure everything we need is installed
CheckPython.ValidatePackages()

import Utils
import colorama

from colorama import Fore, Back, Style

colorama.init()

# Change from Scripts directory to root
os.chdir('../')

# Set LUMINA_DIR environment variable to current root directory
print(f"{Style.BRIGHT}{Back.GREEN}Setting LUMINA_DIR to {os.getcwd()}{Style.RESET_ALL}")
subprocess.call(["setx", "LUMINA_DIR", os.getcwd()])
os.environ['LUMINA_DIR'] = os.getcwd()


### 🚀 Vulkan SDK Auto-Installer ###
VULKAN_SDK_URL = "https://sdk.lunarg.com/sdk/home"
VULKAN_INSTALL_DIR = os.path.join(os.getcwd(), "Dependencies", "Vulkan")
VULKAN_ENV_VAR = "VULKAN_SDK"

def is_vulkan_installed():
    return os.environ.get(VULKAN_ENV_VAR) is not None or os.path.exists(VULKAN_INSTALL_DIR)

def download_and_install_vulkan():
    print(f"{Style.BRIGHT}{Back.YELLOW}Vulkan SDK not found! Downloading...{Style.RESET_ALL}")

    system = platform.system()

    if system == "Windows":
        vulkan_download_url = "https://sdk.lunarg.com/sdk/download/1.4.304.1/windows/VulkanSDK-1.4.304.1-Installer.exe"
        vulkan_installer_path = os.path.join(VULKAN_INSTALL_DIR, "VulkanSDK-Installer.exe")

        os.makedirs(VULKAN_INSTALL_DIR, exist_ok=True)
        urllib.request.urlretrieve(vulkan_download_url, vulkan_installer_path)

        print(f"{Style.BRIGHT}{Fore.CYAN}Installing Vulkan SDK...{Style.RESET_ALL}")
        subprocess.run([vulkan_installer_path, "/S"], check=True)  # Silent installation

    elif system == "Linux":
        print(f"{Style.BRIGHT}{Fore.RED}Vulkan installation must be done manually on Linux!{Style.RESET_ALL}")
        print(f"{Fore.CYAN}Visit {VULKAN_SDK_URL} to install Vulkan.{Style.RESET_ALL}")
        return

    elif system == "Darwin":
        print(f"{Style.BRIGHT}{Fore.RED}Vulkan for macOS requires MoltenVK. Please install it manually.{Style.RESET_ALL}")
        return

    print(f"{Style.BRIGHT}{Back.GREEN}Vulkan SDK installed successfully! Restart your terminal.{Style.RESET_ALL}")

# Check Vulkan SDK
if not is_vulkan_installed():
    download_and_install_vulkan()
else:
    print(f"{Style.BRIGHT}{Back.GREEN}Vulkan SDK is already installed!{Style.RESET_ALL}")

### 🚀 Generate Visual Studio Solution ###
print(f"{Style.BRIGHT}{Back.GREEN}Generating Visual Studio 2022 solution.{Style.RESET_ALL}")
subprocess.call(["Tools/premake5.exe", "vs2022"])
