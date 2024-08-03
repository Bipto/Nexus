import os
import shutil

def build():
    current_dir = os.path.dirname(os.path.realpath(__file__))
    source_dir = os.path.dirname(current_dir)
    source_dir = os.path.dirname(source_dir)  
    build_dir = source_dir + "/build/Emscripten"    
    resource_dir = build_dir + "/resources"
    
    demo_asset_path = source_dir + "/Source/Demo/resources"
    engine_asset_path = source_dir + "/Source/NexusEngine/resources"
    
    #copy the html templates to the output directory
    debug_html_path = current_dir + "/resources/debug.html"
    fullscreen_html_path = current_dir + "/resources/fullscreen.html"
    demo_output_dir = build_dir + "/Source/Demo"
    
    if (not os.path.exists(demo_output_dir)):
        os.makedirs(demo_output_dir)
        
    if (not os.path.exists(demo_asset_path)):
        os.makedirs(demo_asset_path)
        
    if (not os.path.exists(engine_asset_path)):
        os.makedirs(engine_asset_path)
    
    shutil.copy2(debug_html_path, demo_output_dir)
    shutil.copy2(fullscreen_html_path, demo_output_dir)
    shutil.copytree(demo_asset_path, resource_dir, dirs_exist_ok=True)
    shutil.copytree(engine_asset_path, resource_dir, dirs_exist_ok=True)
        
    #build emscripten executable
    os.chdir(source_dir)    
    os.system('emsdk_env && emcmake cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -S . -B build/Emscripten')
    os.chdir(build_dir)
    os.system('ninja')

if (__name__ == "__main__"):
    build()