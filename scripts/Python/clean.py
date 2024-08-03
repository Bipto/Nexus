import os
import shutil

def clean():
    current_dir = os.path.dirname(os.path.realpath(__file__))
    base_dir = os.path.dirname(current_dir)
    base_dir = os.path.dirname(base_dir)
    os.chdir(base_dir)
    
    build_dir = os.path.join(base_dir, "build")
    if (os.path.exists(build_dir)):
        shutil.rmtree(build_dir)
        
    out_dir = os.path.join(base_dir, "out")
    if (os.path.exists(out_dir)):
        shutil.rmtree(out_dir)

if (__name__ == "__main__"):
    clean()