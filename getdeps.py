#!/usr/bin/env python

# This script will clone all of the relevant development branches of the
# INANE project if you would like to build a bleeding-edge development
# version. It also simplifies the entire process of cloning and building.
#
# To clone all the directories:
#   $ python getdeps.py


import  sys, os, getopt, multiprocessing, platform, shutil
from subprocess import call

def pullAll():
    os.system("git -C ./external/ncl pull")

def cloneAllDeps():
    os.chdir("./external")
    os.system("git clone https://github.com/mtholder/ncl.git")
    pullAll()
    os.chdir("..")

def buildLibs():
    os.chdir("./external/ncl")
    os.system("mkdir build")
    os.chdir("build")
    os.system("cmake ..")
    os.system("make")

def main(argv):
    args = ["update"]
    print("Fetching all INANE dependencies.")
    if (len(argv) > 0):
        if (argv[0] == "update"):
            pullAll()
        elif (argv[0] == "build"):
            buildLibs()
        else:
            print("Unrecognised argument")
    else:
        cloneAllDeps()

if __name__ == "__main__":
    main(sys.argv[1:])
