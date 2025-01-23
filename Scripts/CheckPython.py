import subprocess

def InstallPackage(package):
    print(f"Installing {package} module...")
    subprocess.check_call(['python', '-m', 'pip', 'install', package])

# Mandatory
InstallPackage('setuptools')

import pkg_resources

def ValidatePackage(package):
    required = { package }
    installed = {pkg.key for pkg in pkg_resources.working_set}
    missing = required - installed

    if missing:
        InstallPackage(package)

def ValidatePackages():
    ValidatePackage('requests')
    ValidatePackage('fake-useragent')
    ValidatePackage('colorama')