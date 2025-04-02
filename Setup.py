import zipfile
import os
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
        
        # Prompt user to delete the zip file
        delete_zip = input(Fore.MAGENTA + "[PROMPT] Do you want to delete the ZIP file? (Y/N): ")
        if delete_zip.strip().lower() == 'y':
            os.remove(zip_filename)
            print(Fore.GREEN + f"[INFO] {zip_filename} deleted.")
        else:
            print(Fore.CYAN + f"[INFO] {zip_filename} was not deleted.")
    except zipfile.BadZipFile:
        print(Fore.RED + "[ERROR] Invalid ZIP file.")
    except Exception as e:
        print(Fore.RED + f"[ERROR] {str(e)}")

if __name__ == "__main__":
    zip_filename = "External.zip"
    extract_to = "External"  # Extracts to a folder named External
    
    extract_zip(zip_filename, extract_to)