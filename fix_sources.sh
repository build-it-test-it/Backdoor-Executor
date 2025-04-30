#!/bin/bash
# Script to fix issues in source files

set -e

# Fix unused variables in VM source files
echo "Fixing unused variables in VM source files..."
if [ -f VM/src/lfunc.cpp ]; then
    sed -i 's/GCObject\* o = obj2gco(uv);/(void)obj2gco(uv); \/\/ Prevent unused variable warning/g' VM/src/lfunc.cpp
    sed -i 's/global_State\* g = L->global;/(void)L->global; \/\/ Prevent unused variable warning/g' VM/src/lfunc.cpp
fi

if [ -f VM/src/lvmexecute.cpp ]; then
    sed -i 's/Instruction insn = \*pc++;/Instruction insn = \*pc++; (void)insn; \/\/ Prevent unused variable warning/g' VM/src/lvmexecute.cpp
fi

if [ -f VM/src/lvmroblox.cpp ]; then
    sed -i 's/VMMetrics metrics = {};/VMMetrics metrics = {0, 0, 0}; \/\/ Initialize all fields/g' VM/src/lvmroblox.cpp
fi

# Fix include paths in hooks.hpp
if [ -f source/cpp/hooks/hooks.hpp ]; then
    sed -i 's/#include "..\/..\/include\/objc\/runtime.h"/#include <objc\/runtime.h>/g' source/cpp/hooks/hooks.hpp
fi

echo "Source files fixed."
echo "Run ./build.sh on macOS with Xcode to build the project."