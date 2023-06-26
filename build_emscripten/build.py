import os
import subprocess



def build():
    current_dir = os.path.dirname(os.path.realpath(__file__))
    batch_file = os.path.join(current_dir, "build.bat")

    os.chdir(current_dir)

    process = subprocess.Popen(batch_file)
    #stdout, stderr = process.communicate()    
    #return stdout, stderr
    
    while (True):
        retcode = process.poll()
        line = process.stdout.readline()
        yield line
        if (retcode is not None):
            break
    
if (__name__ == "__main__"):
    build()