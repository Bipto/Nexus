import os

def build():
    os.system("cd ../ && rmdir /s /q build")

if (__name__ == "__main__"):
    build()
    
    #wait for user to do something before closing
    print("Done!")
    input()