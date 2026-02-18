#generate runtime folder name with given version
RUNTIME_VERSION = "1.0.0"

def runtime_folder_name():
    return f"runtime_v{RUNTIME_VERSION.replace('.', '_')}"
