import subprocess

def InstallPackage(package):
    print(f"Installing {package} module...")
    subprocess.check_call(['python', '-m', 'pip', 'install', package])

# Mandatory
# NOTE(Yan): pkg_resources is deprecated as an API. See https://setuptools.pypa.io/en/latest/pkg_resources.html
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