// Include needed system headers here in the implementation file
#ifdef __APPLE__
#include <sys/types.h>
#include <sys/sysctl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <mach/mach_init.h>
#include <mach/mach_error.h>
#include <mach/mach_traps.h>
#include <mach/task.h>
#include <mach/mach_port.h>
#include <dlfcn.h>
#include <mach-o/dyld.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#endif

// Now include our own header which uses forward declarations
#include "anti_tamper.hpp"

namespace Security {
    // Empty implementation file - static members defined in anti_tamper.cpp
    // This file is needed to avoid linker errors when the header is included multiple times
}
