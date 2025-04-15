// mach/mach_vm.h stub for CI builds
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Types
typedef uint64_t mach_vm_address_t;
typedef uint64_t mach_vm_size_t;
typedef int kern_return_t;
typedef uint32_t vm_prot_t;
typedef uint32_t vm_inherit_t;
typedef uint32_t vm_behavior_t;
typedef int boolean_t;

// Constants
#define KERN_SUCCESS 0
#define VM_PROT_NONE 0x00
#define VM_PROT_READ 0x01
#define VM_PROT_WRITE 0x02
#define VM_PROT_EXECUTE 0x04
#define VM_PROT_ALL (VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE)

// Functions (stubbed for CI)
kern_return_t mach_vm_allocate(
    int task,
    mach_vm_address_t *addr,
    mach_vm_size_t size,
    int flags
);

kern_return_t mach_vm_deallocate(
    int task,
    mach_vm_address_t addr,
    mach_vm_size_t size
);

kern_return_t mach_vm_protect(
    int task,
    mach_vm_address_t addr,
    mach_vm_size_t size,
    boolean_t set_maximum,
    vm_prot_t new_protection
);

kern_return_t mach_vm_read(
    int task,
    mach_vm_address_t addr,
    mach_vm_size_t size,
    uint64_t *data,
    uint64_t *size_read
);

kern_return_t mach_vm_write(
    int task,
    mach_vm_address_t addr,
    uint64_t data,
    mach_vm_size_t size
);

#ifdef __cplusplus
}
#endif
