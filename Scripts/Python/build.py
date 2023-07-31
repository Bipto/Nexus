import os
import platform

def build():
    current_dir = os.path.dirname(os.path.realpath(__file__))
    source_dir = os.path.dirname(current_dir)
    source_dir = os.path.dirname(source_dir)
    
    build_dir = source_dir + "/build/" + platform.system()
    
    os.chdir(source_dir)
    os.system("cmake -S . -B " + build_dir)
    os.system("cmake --build " + build_dir)

if (__name__ == "__main__"):
    build()