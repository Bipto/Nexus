import os
import shutil

def clean():
    current_dir = os.path.dirname(os.path.realpath(__file__))
    source_dir = os.path.dirname(current_dir)
    source_dir = os.path.dirname(source_dir)
    os.chdir(source_dir)
    
    build_dir = os.path.join(source_dir, "build")
    if (os.path.exists(build_dir)):
        shutil.rmtree(build_dir)
        
    out_dir = os.path.join(source_dir, "out")
    if (os.path.exists(out_dir)):
        shutil.rmtree(out_dir)

if (__name__ == "__main__"):
    clean()