import os

def build():
    os.system('emsdk_env && emcmake cmake -S ../ -B ../build/Emscripten -DCMAKE_BUILD_TYPE=Release && cmake --build ../build/Emscripten --config Release')

if (__name__ == "__main__"):
    build()
    
    #wait for user to do something before closing
    print("Done!")
    input()