
import subprocess
import sys

def install_package(package_name):
    subprocess.check_call([sys.executable, "-m", "pip", "install", package_name])
def uninstall_package(package_name):
    subprocess.check_call([sys.executable, "-m", "pip", "uninstall", package_name])
def purge_cache():
    subprocess.check_call([sys.executable, "-m", "pip", "cache", "purge"])

# Example usage:
install_package("ffmpeg")
install_package("git+https://github.com/facebookresearch/audiocraft.git")
#uninstall_package("torch")
#purge_cache()
#install_package("torch==2.1.0")
