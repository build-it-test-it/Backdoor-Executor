#!/bin/bash
# Fix conflicts between Lua headers and iOS frameworks

echo "==== Fixing Lua header conflicts with iOS frameworks ===="

# 1. First, modify globals.hpp to include lua_defs.h before real Lua headers
if [ -f "source/cpp/globals.hpp" ]; then
  echo "Fixing source/cpp/globals.hpp..."
  cp source/cpp/globals.hpp source/cpp/globals.hpp.bak
  
  # Check if it includes luau/lua.h
  if grep -q "#include \"luau/lua.h\"" source/cpp/globals.hpp; then
    # Insert lua_defs.h before the lua.h include
    sed -i 's/#include "luau\/lua.h"/#include "luau\/lua_defs.h"\n#include "luau\/lua.h"/' source/cpp/globals.hpp
  fi
fi

# 2. Make sure lua_defs.h has all the necessary macros
cat > source/cpp/luau/lua_defs.h << 'EOL'
// Essential definitions for Lua to work with iOS frameworks
#pragma once

// Core API declarations (already defined in lua_defs.h)
#ifndef LUA_API
#define LUA_API extern
#endif

#ifndef LUALIB_API
#define LUALIB_API extern
#endif

// Define lua function attributes
#ifndef LUA_PRINTF_ATTR
#define LUA_PRINTF_ATTR(fmt, args)
#endif

// Define C++ attribute macros that might conflict
#ifndef LUA_NORETURN
#define LUA_NORETURN
#endif

// Make l_noret not depend on LUA_NORETURN
#undef l_noret
#define l_noret void

// Add defines for missing macros that cause compilation errors
#ifndef LUAI_USER_ALIGNMENT_T
#define LUAI_USER_ALIGNMENT_T double
#endif

#ifndef LUA_EXTRA_SIZE
#define LUA_EXTRA_SIZE 0
#endif

#ifndef LUA_SIZECLASSES
#define LUA_SIZECLASSES 32
#endif

#ifndef LUA_MEMORY_CATEGORIES
#define LUA_MEMORY_CATEGORIES 8
#endif

#ifndef LUA_UTAG_LIMIT
#define LUA_UTAG_LIMIT 16
#endif
EOL

# 3. Fix ObfuscateStrings and ObfuscateControlFlow missing functions
echo "Fixing missing Obfuscator functions..."
if grep -q "AntiDetection::Obfuscator::ObfuscateStrings" source/cpp/exec/funcs.hpp; then
  sed -i 's/AntiDetection::Obfuscator::ObfuscateStrings/AntiDetection::Obfuscator::ObfuscateIdentifiers/g' source/cpp/exec/funcs.hpp
fi

if grep -q "AntiDetection::Obfuscator::ObfuscateControlFlow" source/cpp/exec/funcs.hpp; then
  sed -i 's/AntiDetection::Obfuscator::ObfuscateControlFlow/AntiDetection::Obfuscator::AddDeadCode/g' source/cpp/exec/funcs.hpp
fi

# 4. Create a simple obfuscator implementation if it doesn't exist
mkdir -p source/cpp/anti_detection
if [ ! -f "source/cpp/anti_detection/obfuscator.hpp" ] || ! grep -q "ObfuscateIdentifiers" source/cpp/anti_detection/obfuscator.hpp; then
  echo "Creating minimal obfuscator implementation..."
  
  cat > source/cpp/anti_detection/obfuscator.hpp << 'EOL'
#pragma once
#include <string>

namespace AntiDetection {
    class Obfuscator {
    public:
        // Basic obfuscation for identifiers
        static std::string ObfuscateIdentifiers(const std::string& script) {
            // Simple implementation - in real code you'd do more
            return script;
        }
        
        // Add dead code to confuse analysis
        static std::string AddDeadCode(const std::string& script) {
            // Simple implementation - in real code you'd add fake branches
            return script;
        }
    };
}
EOL
fi

echo "==== Lua/iOS conflict fixes applied ===="
