#pragma once
/**
 * Dobby - A lightweight, multi-platform function hook framework
 * Implementation sourced from the full Dobby library
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Hook a function at the given address with a replacement function
 * 
 * @param function_address Address of the function to hook
 * @param replace_function Address of the replacement function
 * @param origin_function Pointer to store the original function address
 * @return int 0 on success, non-zero on failure
 */
int DobbyHook(void *function_address, void *replace_function, void **origin_function);

/**
 * @brief Unhook a previously hooked function
 * 
 * @param function_address Address of the function to unhook
 * @return int 0 on success, non-zero on failure
 */
int DobbyUnHook(void *function_address);

/**
 * @brief Initialize the Dobby hooking engine
 * 
 * @return int 0 on success, non-zero on failure
 */
int DobbyInit();

#ifdef __cplusplus
}
#endif
