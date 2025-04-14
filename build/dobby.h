// Stub implementation of dobby.h for CI builds
#pragma once

// Define basic stub functions
#ifdef __cplusplus
extern "C" {
#endif

// Stub for DobbyHook
void* DobbyHook(void* symbol_address, void* replace_call, void** origin_call);

// Stub for DobbyDestroy
int DobbyDestroy(void* symbol_address);

#ifdef __cplusplus
}
#endif
