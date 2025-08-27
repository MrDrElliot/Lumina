import zipfile
import os
import time
import subprocess
try:
    from colorama import Fore, Style, init
except ImportError:
    import subprocess
    import sys
    subprocess.check_call([sys.executable, "-m", "pip", "install", "colorama"])
    from colorama import Fore, Style, init
def extract_zip(zip_filename, extract_to):
    init(autoreset=True)  # Initialize colorama for colored output

    if not os.path.exists(zip_filename):
        print(Fore.RED + f"[ERROR] {zip_filename} not found.")
        return

    print(Fore.CYAN + f"[INFO] Extracting {zip_filename} to {extract_to}...")

    try:
        with zipfile.ZipFile(zip_filename, 'r') as zip_ref:
            file_list = zip_ref.namelist()
            for file in file_list:
                print(Fore.YELLOW + f"[EXTRACTING] {file}")
            zip_ref.extractall(extract_to)
        print(Fore.GREEN + f"[SUCCESS] Extraction complete!")
    finally:
        print(Style.RESET_ALL)


def run_generate():
    init(autoreset=True)

    script_path = os.path.join("Scripts", "Win-GenProjects.py")

    if not os.path.exists(script_path):
        print(Fore.RED + f"[ERROR] Script not found: {script_path}")
        return

    print(Fore.CYAN + f"[INFO] Generating project files...")

    try:
        subprocess.run(["python", script_path], check=True)
    except subprocess.CalledProcessError as e:
        print(Fore.RED + f"[ERROR] Failed to run {script_path}: {e}")


if __name__ == '__main__':
    zip_filename = "External.zip"
    extract_to = ""  # Explicitly setting the extraction folder

    extract_zip(zip_filename, extract_to)
    time.sleep(1)
