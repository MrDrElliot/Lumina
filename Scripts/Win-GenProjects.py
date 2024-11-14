import os
import subprocess
import CheckPython

# Make sure everything we need is installed
CheckPython.ValidatePackages()

import Utils
import colorama

from colorama import Fore
from colorama import Back
from colorama import Style

colorama.init()

# Change from Scripts directory to root
os.chdir('../')

# Set LUMINA_DIR environment variable to current Hazel root directory
print(f"{Style.BRIGHT}{Back.GREEN}Setting LUMINA_DIR to {os.getcwd()}{Style.RESET_ALL}")
subprocess.call(["setx", "LUMINA_DIR", os.getcwd()])
os.environ['LUMINA_DIR'] = os.getcwd()

print(f"{Style.BRIGHT}{Back.GREEN}Generating Visual Studio 2022 solution.{Style.RESET_ALL}")
subprocess.call(["Tools/premake5.exe", "vs2022"])
