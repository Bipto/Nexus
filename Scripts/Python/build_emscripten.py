import os
import shutil

def build():
    current_dir = os.path.dirname(os.path.realpath(__file__))
    source_dir = os.path.dirname(current_dir)
    source_dir = os.path.dirname(source_dir)  
    build_dir = source_dir + "/build/Emscripten"        
    os.chdir(source_dir)
    
    os.system('emcmake cmake -D CMAKE_BUILD_TYPE=Release -G "Unix Makefiles" -S . -B build/Emscripten')
    os.chdir(build_dir)
    os.system('make')
    
    debug_html_path = current_dir + "/Resources/debug.html"
    fullscreen_html_path = current_dir + "/Resources/fullscreen.html"
    
    shutil.copy2(debug_html_path, build_dir + "/Source/Demo")
    shutil.copy2(fullscreen_html_path, build_dir + "/Source/Demo")

if (__name__ == "__main__"):
    build()