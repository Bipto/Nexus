import os
import shutil

def clean():
    #go back a directory from where this file is located
    working_dir = os.path.dirname(os.path.realpath(__file__))
    working_dir = os.path.dirname(working_dir)
    
    os.chdir(working_dir)
    
    build_dir = os.path.join(working_dir, "build")
    if (os.path.isdir(build_dir)):
        shutil.rmtree(build_dir)

if (__name__ == "__main__"):
    clean()
    
    #wait for user to do something before closing
    print("Done!")
    input()