import os
import time
import shutil
import subprocess
from pathlib import Path
import tkinter as tk
from tkinter import filedialog
from colorama import Fore, Style, init


# Initialize colorama for cross-platform support
init()

# Setup Tkinter once and hide the main window
root = tk.Tk()
root.withdraw()

# Get the Lumina engine install directory.
lumina_engine_directory = os.getenv("LUMINA_DIR")


def sanitize_filename(filename):
    filename = filename.replace(" ", "_")
    filename = filename.strip()

    return filename


def create_project(folder, name):

    sanitize_filename(name)
    name += ".lproject"
    file_path = os.path.join(folder, name)


    try:
        with open(file_path, "w") as new_file:
            new_file.write("Testing")
        print(f"{Fore.GREEN}File '{name}' created successfully at {file_path}{Style.RESET_ALL}")
    except Exception as e:
        print(f"{Fore.RED} Error occurred: {e}{Style.RESET_ALL}")

    return file_path

def create_premake_files(folder, name):

    template_premake_solution_file_path = os.path.join(lumina_engine_directory, "Templates/Project/premake_solution_template.txt")
    premake_solution_file_path = os.path.join(folder, "premake5.lua")

    with open(template_premake_solution_file_path) as premake_file:
        template_content = premake_file.read()

    template_content = template_content.replace("$PROJECT_NAME", name)

    with open(premake_solution_file_path, "w") as premake_file:
        premake_file.write(template_content)


def create_python_files(folder):
    # Define the file path for GenerateProject.py
    generate_project_file_path = os.path.join(folder, "GenerateProject.py")

    # Python script content that will be written to GenerateProject.py
    python_script_content = '''
import subprocess
import os

def generate_project():

    # Call premake5 to generate Visual Studio solution
    subprocess.call(["Tools/premake5.exe", "vs2022"])

    input("Press Enter to continue...")

if __name__ == "__main__":
    generate_project()
'''

    try:
        # Create and write to GenerateProject.py
        with open(generate_project_file_path, "w") as py_file:
            py_file.write(python_script_content)
        print(f"Python file 'GenerateProject.py' created successfully at {generate_project_file_path}")
    except Exception as e:
        print(f"Error creating Python file: {e}")


def create_module_header(folder, name):
    name = sanitize_filename(name)
    source_directory = os.path.join(folder, f"Source/{name}/{name}.h")

    header_content = f'''    
#pragma once
#include "Core/Module/ModuleInterface.h"

// This is your core game module that the engine loads.
class F{name} : public Lumina::IModuleInterface
{{
    //...
}};
'''

    try:
        os.makedirs(os.path.dirname(source_directory), exist_ok=True)
        with open(source_directory, "w") as header_file:
            header_file.write(header_content)
        print(f"Header File created successfully at {source_directory}")
    except Exception as e:
        print(f"Error creating header file: {e}")



def create_module_source(folder, name):
    name = sanitize_filename(name)
    source_directory = os.path.join(folder, f"Source/{name}/{name}.cpp")

    source_content = f'''    
#include "{name}.h"
#include "Core/Module/ModuleManager.h"

// Boilerplate module discovery and implementation.
IMPLEMENT_MODULE(F{name}, "{name}")
'''

    try:
        os.makedirs(os.path.dirname(source_directory), exist_ok=True)
        with open(source_directory, "w") as source_file:
            source_file.write(source_content)
        print(f"Source File created successfully at {source_directory}")
    except Exception as e:
        print(f"Error creating source file: {e}")



def create_project_directories(directory, name):

    if not os.path.exists(directory):
        os.makedirs(directory)

    source_directory = os.path.join(directory, f"Source/{name}")
    content_directory = os.path.join(directory, "Game/Content")

    os.makedirs(source_directory, exist_ok=True)
    os.makedirs(content_directory, exist_ok=True)



def copy_directory(src, dest):
    try:
        # Copy the entire directory tree from 'src' to 'dest'
        shutil.copytree(src, dest)
        print(f"{Fore.GREEN}Directory copied successfully from {src} to {dest}{Style.RESET_ALL}")
    except Exception as e:
        print(f"{Fore.RED}Error occurred while copying directory: {e}{Style.RESET_ALL}")



def main():

    print(Fore.RED + "========================================")
    print(Fore.BLUE + "======== Lumina Project Creator ========")
    print(Fore.RED + "========================================\n")

    if not lumina_engine_directory:
        print(Fore.RED + "Failed to find Lumina engine directory")
        return
    else:
        print(Fore.GREEN + f"Lumina engine directory found at {lumina_engine_directory}{Style.RESET_ALL}")

    print(Fore.GREEN)

    input("Press enter to continue...")

    selected_folder = filedialog.askdirectory(title="Select a Project Directory")

    if not selected_folder:
        print("\nNo folder selected...")
        return

    print(f"\nYou selected: {selected_folder}")

    new_project_name = input("Please enter the new project name (without extension): ")

    create_project_directories(selected_folder, new_project_name)
    create_module_header(selected_folder, new_project_name)
    create_module_source(selected_folder, new_project_name)
    create_python_files(selected_folder)
    create_project(selected_folder, new_project_name)
    create_premake_files(selected_folder, new_project_name)

    source_directory = os.path.join(lumina_engine_directory, "Tools")
    destination_directory = os.path.join(selected_folder, "Tools")

    copy_directory(source_directory, destination_directory)


if __name__ == "__main__":
    main()
    input("\nPress Enter to exit...")