import subprocess
import sys
import platform
import shutil
stlfilename = ""
if len(sys.argv) > 1:
    stlfilename = sys.argv[1]
else: 
    stlfilename = input("Enter the name of the STL file: ")

# generate stl_data
print("Generating stl_data...")
proc = subprocess.run(["python", "stl_generate_array.py", stlfilename])
if proc.returncode != 0:
    print("\033[0;31mError:\033[0m in generating stl_data",file=sys.stderr)
    exit(1)
# build new flash
print("Building new flash...")
result = subprocess.run(
    ["make"],    
    stdout = subprocess.PIPE,
    stderr = subprocess.STDOUT,
    universal_newlines = True
    )
if "Error" in result.stdout:
    print("\033[0;31mError:\033[0m in building new flash",file=sys.stderr)
    if "`FLASH' overflowed" in result.stdout:
        print("\033[0;31mError:\033[0m Flash overflowed",file=sys.stderr)
    exit(1)
else:
    print("Build successful")
# flash new firmware
print("Flashing new firmware...") 
print("Note: for windows, windows defender may block wlink. Please allow it to run. It is safe to run.")
print("Note: for linux-arm64 or other error occurs in this step, you may need to install wlink from https://github.com/ch32-rs/wlink manually")
if shutil.which("wlink"):
    proc = subprocess.run(["wlink", "flash", "rotate_image.bin"])
    if proc.returncode != 0:
        print("\033[0;31mError:\033[0m in flashing new firmware",file=sys.stderr)
        exit(1)
else:
    print("wlink not found. Trying to run wlink from wlink_bin")
    is_windows = 'Windows' in platform.uname().system
    is_linux = 'Linux' in platform.uname().system
    is_mac = 'Darwin' in platform.uname().system
    if is_windows:
        proc = subprocess.run(["./wlink_bin/wlink.exe", "flash", "rotate_image.bin"])
    elif is_linux:
        proc = subprocess.run(["./wlink_bin/wlink-linux", "flash", "rotate_image.bin"])
    elif is_mac:
        is_arm = 'ARM' in platform.uname().version
        if is_arm:
            proc = subprocess.run(["./wlink_bin/wlink-mac-arm64", "flash", "rotate_image.bin"])
        else:
            proc = subprocess.run(["./wlink_bin/wlink-mac-x86", "flash", "rotate_image.bin"])
    else:
        print("\033[0;31mError:\033[0m Unsupported platform. Please install wlink from https://github.com/ch32-rs/wlink manually",file=sys.stderr)
        exit(1)
    if proc.returncode != 0:
        print("\033[0;31mError:\033[0m in flashing new firmware",file=sys.stderr)
        exit(1)
print("Done")