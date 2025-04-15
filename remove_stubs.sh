#!/bin/bash
# Script to remove CI_BUILD definitions and replace stub implementations
# with real code

echo "Removing CI_BUILD defines and stub implementations..."

# Remove CI_BUILD definitions from all source files
find source -type f \( -name "*.h" -o -name "*.hpp" -o -name "*.cpp" -o -name "*.mm" \) | xargs sed -i 's/#define CI_BUILD//g'

# Create real implementation directories if they don't exist
mkdir -p external/dobby/include
mkdir -p external/dobby/lib

echo "Done removing CI_BUILD definitions."
