import os
import platform

def build():
    
    build_dir = "../build/" + platform.system()
    os.system("cmake ../ -B " + build_dir)
    os.system("cmake --build " + build_dir)

if (__name__ == "__main__"):
    build()
    
    #wait for user to do something before closing
    print("Done!")
    input()