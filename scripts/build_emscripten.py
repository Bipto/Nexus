import os

def build():
    os.system('emsdk_env && emcmake cmake -S ../ -B ../build/Emscripten -G "Ninja" && cd ../build/Emscripten && ninja')

if (__name__ == "__main__"):
    build()