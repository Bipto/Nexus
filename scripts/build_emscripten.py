import os

def build():
    overlay_path = os.path.abspath("../overlay-ports")

    os.system('emsdk_env && cmake -S ../ -B ../build/emscripten -G "Ninja" -DEMSCRIPTEN=1 -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake -DVCPKG_OVERLAY_PORTS=' + overlay_path + '  -DVCPKG_TARGET_TRIPLET=wasm32-emscripten -DCMAKE_BUILD_TYPE=Release -DNDEBUG=1 -DNX_PLATFORM_OPENGL=1 -DNX_PLATFORM_WEBGL=1 -DNX_PLATFORM_OPENAL=1 -DNX_BUILD_DEMO=1 && cmake --build ../build/emscripten --config Release')
    #os.system('emsdk_env && emcmake cmake -S ../ -B ../build/emscripten -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=wasm32-emscripten -DCMAKE_BUILD_TYPE=Release -DNDEBUG=1 -DNX_PLATFORM_OPENGL=1 -DNX_PLATFORM_WEBGL=1 -DNX_PLATFORM_OPENAL=1 -DNX_BUILD_DEMO=1 && cmake --build ../build/emscripten --config Release')
   
if (__name__ == "__main__"):
    build()
    
    #wait for user to do something before closing
    print("Done!")
    input()