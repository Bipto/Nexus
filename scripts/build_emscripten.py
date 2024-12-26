import os

def build():
    os.system('emsdk_env && emcmake cmake -S ../ -B ../build/Emscripten -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DNDEBUG=1 -DNX_PLATFORM_OPENGL=1 -DNX_PLATFORM_WEBGL=1 -DNX_PLATFORM_OPENAL=1 && cmake --build ../build/Emscripten --config Release')

if (__name__ == "__main__"):
    build()
    
    #wait for user to do something before closing
    print("Done!")
    input()