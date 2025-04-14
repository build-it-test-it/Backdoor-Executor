// Compatibility header for mach types in CI builds
#pragma once

#include <cstdint>

// Define common mach VM types for CI builds
typedef uint64_t mach_vm_address_t;
typedef uint64_t mach_vm_size_t;
typedef int kern_return_t;

// Define some common constants
#define KERN_SUCCESS 0

// Add other mach-related types as needed
#ifdef __cplusplus
extern "C" {
#endif

// Stub functions if needed
int stub_mach_vm_read(uint64_t task, mach_vm_address_t addr, mach_vm_size_t size, void** data, uint64_t* outsize);

#ifdef __cplusplus
}
#endif
