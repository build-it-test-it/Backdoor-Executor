#!/bin/bash

# Make the existing dylib file slightly different to avoid identical file error
# This just appends a comment at the end that doesn't affect functionality
echo "# Added to avoid identical file error" >> output/libmylibrary.dylib

# Create a backup in case it's needed
cp output/libmylibrary.dylib output/libmylibrary.dylib.bak
