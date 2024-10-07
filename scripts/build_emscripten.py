import os

def build():
    os.system('emsdk_env && emcmake cmake -S ../ -B ../build/Emscripten && cmake --build ../build/Emscripten')

if (__name__ == "__main__"):
    build()
    
    #wait for user to do something before closing
    print("Done!")
    input()