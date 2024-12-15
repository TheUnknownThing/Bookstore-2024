import os
import glob

# Get a list of all .dat files in the current directory
dat_files = glob.glob("*.dat")

# Loop through the list and delete each file
for file in dat_files:
    try:
        os.remove(file)
        print(f"Deleted {file}")
    except Exception as e:
        print(f"Error deleting {file}: {e}")

# delete *.dat in build/ directory
dat_files = glob.glob("build/*.dat")
for file in dat_files:
    try:
        os.remove(file)
        print(f"Deleted {file}")
    except Exception as e:
        print(f"Error deleting {file}: {e}")
        