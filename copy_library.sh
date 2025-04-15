#!/bin/bash

# Create the output directory if it doesn't exist
mkdir -p output

# Copy the library to the expected location
cp build/lib/roblox_executor.dylib output/libmylibrary.dylib

echo "Copied library to expected location for workflow check"
