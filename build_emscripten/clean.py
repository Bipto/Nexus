import os
import shutil

def remove_files_from_directory(directory):
    for item in os.scandir(directory):
        _, file_extension = os.path.splitext(item.path)
        
        if (item.is_file()):
            if (file_extension != ".py" and file_extension != ".bat" and file_extension != ".html"):
                os.remove(item.path)
                
        if (item.is_dir()):
            if (item.name != "Demo"):
                shutil.rmtree(item.path)

    
def clean():
    current_dir = os.path.dirname(os.path.realpath(__file__))
    remove_files_from_directory(current_dir)

    demo_dir = os.path.join(current_dir, "Demo")
    if (os.path.exists(demo_dir)):
        remove_files_from_directory(demo_dir)
        
    return "Files cleaned"
        
if (__name__ == "__main__"):
    clean()