import os
import subprocess
import platform
import CheckPython
import colorama

from colorama import Fore, Back, Style

# === Initialization ===
colorama.init()
os.chdir('../')  # Move from Scripts/ to root
print(f"{Style.BRIGHT}Generating Project Files for Lumina\n{Style.RESET_ALL}")

# === Constants ===
VULKAN_SDK_URL = "https://vulkan.lunarg.com/sdk/home"
VULKAN_ENV_VAR = "VULKAN_SDK"
VULKAN_INSTALL_DIR = os.path.join(os.getcwd(), "Dependencies", "Vulkan")

# === Set Environment Variable ===
current_dir = os.getcwd()
print(f"{Style.BRIGHT}Setting LUMINA_DIR to {current_dir}{Style.RESET_ALL}")
subprocess.call(["setx", "LUMINA_DIR", current_dir])
os.environ['LUMINA_DIR'] = current_dir

# === Check Python Requirements ===
try:
    CheckPython.ValidatePackages()
except Exception as e:
    print(f"{Fore.RED}Error: Required Python packages not installed. {e}{Style.RESET_ALL}")
    input("\nPress Enter to exit...")
    exit(1)

# === Check Vulkan SDK ===
def is_vulkan_installed():
    return os.environ.get(VULKAN_ENV_VAR) is not None or os.path.exists(VULKAN_INSTALL_DIR)

if not is_vulkan_installed():
    print(f"{Fore.RED}Vulkan SDK not found!{Style.RESET_ALL}")
    print(f"{Fore.CYAN}Please install it from: {VULKAN_SDK_URL}{Style.RESET_ALL}")
    input("\nPress Enter to exit...")
    exit(1)
else:
    vk_path = os.environ.get(VULKAN_ENV_VAR, VULKAN_INSTALL_DIR)
    print(f"{Fore.GREEN}Vulkan SDK found at: {vk_path}{Style.RESET_ALL}")

# === Generate Visual Studio Solution ===
print(f"\n{Style.BRIGHT}{Back.GREEN}Generating Visual Studio 2022 solution...{Style.RESET_ALL}")
premake_result = subprocess.call(["Tools/premake5.exe", "vs2022"])

if premake_result != 0:
    print(f"{Fore.RED}Premake generation failed. Please check your setup.{Style.RESET_ALL}")
else:
    print(f"{Fore.GREEN}Solution generated successfully!{Style.RESET_ALL}")

input("\nPress Enter to exit...")
