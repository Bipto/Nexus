import glob
import subprocess
import os

#function to perform the formatting
def format():
    #filters to use when searching files
    excluded_dirs = ["external", "binaries"]
    source_file_extensions = [".hpp", ".cpp"]

    #go back a directory from where this file is located
    working_dir = os.path.dirname(os.path.realpath(__file__))
    working_dir = os.path.dirname(working_dir)

    #change the working directory so that we can use the .clang-format file
    os.chdir(working_dir)

    #we want to begin searching in the src directory
    source_dir = os.path.join(working_dir, "src")

    #the filter to use to search files
    #i.e any files
    filter = source_dir + "**/**"

    #iterate through all files from the base source directory
    for file in glob.iglob(filter, recursive=True):
        file_name, file_extension = os.path.splitext(file)

        #lambda function to check whether a string contains a given value from an array
        contains_word = lambda s, l : any(map(lambda x: x in s, l))
        
        #skip files within any excluded directories
        if (contains_word(file, excluded_dirs)):
            continue
        
        #skip files that are not the correct extension
        if (not contains_word(file_extension, source_file_extensions)):
            continue;
        
        #open a subprocess to run the clang format command
        subprocess.Popen("clang-format -i " + file)
        print("Processing file: " + file)

#do not support using this python file as a module
if (__name__ == "__main__"):
    format()
    
    #wait for user to do something before closing
    print("Done!")
    input()